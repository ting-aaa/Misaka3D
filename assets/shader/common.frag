#version 330 core
out vec4 FragColor;

in vec3 v_FragPos;
in vec2 v_TexCoords;
in mat3 v_TBN;

// =======================
// 材质定义
// =======================
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

uniform vec3 u_ViewPos;

// =======================
// 灯光结构定义
// =======================
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

// 预设最大数量 (必须与 C++ Renderer::UploadLights 逻辑一致)
#define MAX_DIR_LIGHTS 2
#define MAX_POINT_LIGHTS 4

uniform DirLight u_DirLights[MAX_DIR_LIGHTS];
uniform int u_DirLightCount;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

// =======================
// 光照计算函数
// =======================
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metallic);

void main() {
    // 1. 采样材质属性
    vec4 baseColor = u_HasAlbedoMap ? texture(u_AlbedoMap, v_TexCoords) : u_AlbedoColor;
    if(baseColor.a < 0.1) discard;

    float roughness = u_HasRoughnessMap ? texture(u_RoughnessMap, v_TexCoords).r : u_RoughnessVal;
    float metallic = u_HasMetallicMap ? texture(u_MetallicMap, v_TexCoords).r : u_MetallicVal;

    vec3 N;
    if (u_HasNormalMap) {
        vec3 normalMapVal = texture(u_NormalMap, v_TexCoords).rgb;
        N = normalize(v_TBN * (normalMapVal * 2.0 - 1.0));
    } else {
        N = normalize(v_TBN[2]);
    }

    vec3 V = normalize(u_ViewPos - v_FragPos);
    vec3 result = vec3(0.0);

    // 2. 累加方向光
    for(int i = 0; i < u_DirLightCount; i++) {
        result += CalcDirLight(u_DirLights[i], N, V, baseColor.rgb, roughness, metallic);
    }

    // 3. 累加点光源
    for(int i = 0; i < u_PointLightCount; i++) {
        result += CalcPointLight(u_PointLights[i], N, v_FragPos, V, baseColor.rgb, roughness, metallic);
    }

    // 环境光 (简单处理)
    vec3 ambient = vec3(0.03) * baseColor.rgb;
    
    FragColor = vec4(ambient + result, baseColor.a);
}

// 半兰伯特 + Blinn-Phong 计算方向光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(-light.direction); // 光照方向取反 (指向光源)
    vec3 H = normalize(L + viewDir);

    // Diffuse (Half-Lambert)
    float diff = pow(dot(normal, L) * 0.5 + 0.5, 2.0);
    vec3 diffuse = light.color * light.intensity * diff * albedo;

    // Specular
    float shininess = (1.0 - roughness) * 256.0 + 1.0;
    float spec = pow(max(dot(normal, H), 0.0), shininess);
    
    vec3 F0 = vec3(0.04); 
    vec3 specColor = mix(F0, albedo, metallic);
    vec3 specular = light.color * light.intensity * spec * specColor;

    return diffuse + specular;
}

// 点光源计算 (带衰减)
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(L + viewDir);
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Diffuse
    float diff = pow(max(dot(normal, L), 0.0), 2.0); // 点光源通常不用半兰伯特，这里用标准 Lambert 平方让光感更好
    vec3 diffuse = light.color * light.intensity * diff * albedo;

    // Specular
    float shininess = (1.0 - roughness) * 256.0 + 1.0;
    float spec = pow(max(dot(normal, H), 0.0), shininess);
    
    vec3 F0 = vec3(0.04); 
    vec3 specColor = mix(F0, albedo, metallic);
    vec3 specular = light.color * light.intensity * spec * specColor;

    return (diffuse + specular) * attenuation;
}