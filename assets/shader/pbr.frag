#version 330 core
out vec4 FragColor;

in vec3 v_FragPos;
in vec2 v_TexCoords;
in mat3 v_TBN;

uniform bool u_HasAlbedoMap;
uniform sampler2D u_AlbedoMap;
uniform vec4 u_AlbedoColor;

uniform bool u_HasNormalMap;
uniform sampler2D u_NormalMap;

uniform bool u_HasRoughnessMap;
uniform sampler2D u_RoughnessMap;
uniform float u_RoughnessVal;

uniform bool u_HasMetallicMap;
uniform sampler2D u_MetallicMap;
uniform float u_MetallicVal;

uniform bool u_HasAoMap;
uniform sampler2D u_AoMap;
uniform float u_AoVal;

uniform bool u_HasEmissiveMap;
uniform sampler2D u_EmissiveMap;
uniform vec3 u_EmissiveColor;

uniform vec3 u_ViewPos;

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_DIR_LIGHTS 2
#define MAX_POINT_LIGHTS 4

uniform DirLight u_DirLights[MAX_DIR_LIGHTS];
uniform int u_DirLightCount;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, 0.000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, 0.000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 EvaluatePBR(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float roughness, float metallic) {
    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.0001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() {
    vec4 albedoRGBA = u_HasAlbedoMap ? texture(u_AlbedoMap, v_TexCoords) : u_AlbedoColor;
    if (albedoRGBA.a < 0.1) discard;

    vec3 albedo = albedoRGBA.rgb;
    float roughness = u_HasRoughnessMap ? texture(u_RoughnessMap, v_TexCoords).r : u_RoughnessVal;
    float metallic = u_HasMetallicMap ? texture(u_MetallicMap, v_TexCoords).r : u_MetallicVal;
    float ao = u_HasAoMap ? texture(u_AoMap, v_TexCoords).r : u_AoVal;
    vec3 emissive = u_HasEmissiveMap ? texture(u_EmissiveMap, v_TexCoords).rgb : u_EmissiveColor;

    roughness = clamp(roughness, 0.045, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    ao = clamp(ao, 0.0, 1.0);

    vec3 N;
    if (u_HasNormalMap) {
        vec3 normalTex = texture(u_NormalMap, v_TexCoords).rgb;
        normalTex = normalTex * 2.0 - 1.0;
        N = normalize(v_TBN * normalTex);
    } else {
        N = normalize(v_TBN[2]);
    }

    vec3 V = normalize(u_ViewPos - v_FragPos);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < u_DirLightCount; ++i) {
        vec3 L = normalize(-u_DirLights[i].direction);
        vec3 radiance = u_DirLights[i].color * u_DirLights[i].intensity;
        Lo += EvaluatePBR(N, V, L, radiance, albedo, roughness, metallic);
    }

    for (int i = 0; i < u_PointLightCount; ++i) {
        vec3 toLight = u_PointLights[i].position - v_FragPos;
        float distance = length(toLight);
        vec3 L = normalize(toLight);

        float attenuation = 1.0 / (u_PointLights[i].constant + u_PointLights[i].linear * distance + u_PointLights[i].quadratic * distance * distance);
        vec3 radiance = u_PointLights[i].color * u_PointLights[i].intensity * attenuation;
        Lo += EvaluatePBR(N, V, L, radiance, albedo, roughness, metallic);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo + emissive;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, albedoRGBA.a);
}
