#version 330 core
out vec4 FragColor;

in vec3 v_FragPos;
in vec2 v_TexCoords;
in mat3 v_TBN;

// ==========================================
// Uniforms (对应 CommonMaterial)
// ==========================================

// 1. Albedo
uniform bool u_HasAlbedoMap;
uniform sampler2D u_AlbedoMap;
uniform vec4 u_AlbedoColor; // 备用

// 2. Normal
uniform bool u_HasNormalMap;
uniform sampler2D u_NormalMap;

// 3. Roughness
uniform bool u_HasRoughnessMap;
uniform sampler2D u_RoughnessMap;
uniform float u_RoughnessVal; // 备用

// 4. Metallic
uniform bool u_HasMetallicMap;
uniform sampler2D u_MetallicMap;
uniform float u_MetallicVal; // 备用

// Lighting
uniform vec3 u_LightDir;
uniform vec3 u_ViewPos; // 来自 Renderer

void main() {
    // ------------------------------------------
    // 1. 数据采集 (采样贴图 或 使用缺省值)
    // ------------------------------------------
    
    // Albedo
    vec4 baseColor = u_HasAlbedoMap ? texture(u_AlbedoMap, v_TexCoords) : u_AlbedoColor;
    //float gray = baseColor.r*0.3+ baseColor.g*0.5+ baseColor.b*0.2;
    //gray = pow(gray, 2);
    // 如果是 PNG 透明贴图，这里可以做 alpha 剔除
    if(baseColor.a < 0.1) discard;

    // Roughness (通常存储在红色通道)
    float roughness = u_HasRoughnessMap ? texture(u_RoughnessMap, v_TexCoords).r : u_RoughnessVal;

    // Metallic (通常存储在红色通道)
    float metallic = u_HasMetallicMap ? texture(u_MetallicMap, v_TexCoords).r : u_MetallicVal;

    // Normal
    vec3 N;
    if (u_HasNormalMap) {
        // 从贴图采样 [0,1] -> [-1,1]
        vec3 normalMapVal = texture(u_NormalMap, v_TexCoords).rgb;
        normalMapVal = normalize(normalMapVal * 2.0 - 1.0);
        N = normalize(v_TBN * normalMapVal); // 转换到世界空间
    } else {
        // 无贴图时，直接使用几何体法线 (TBN 的 Z 轴)
        N = normalize(v_TBN[2]); 
    }

    // ------------------------------------------
    // 2. 光照计算 (半兰伯特 + PBR风格高光)
    // ------------------------------------------

    vec3 L = normalize(u_LightDir);
    vec3 V = normalize(u_ViewPos - v_FragPos);
    vec3 H = normalize(L + V);

    // [Diffuse] 半兰伯特 (Half-Lambert)
    // 公式: pow(dot * 0.5 + 0.5, 2.0)
    float NdotL = dot(N, L);
    float halfLambert = NdotL * 0.5 + 0.5;
    float diffuseTerm = pow(halfLambert, 2.0);
    vec3 diffuse = baseColor.rgb * diffuseTerm;

    // [Specular] PBR 风格的高光 (Blinn-Phong 模拟)
    // 粗糙度越低(0)，光泽度越高；粗糙度越高(1)，高光越散
    float shininess = (1.0 - roughness) * 256.0 + 1.0; 
    float NdotH = max(dot(N, H), 0.0);
    float specTerm = pow(NdotH, shininess);

    // 金属度影响高光颜色
    // 非金属(metallic=0)的高光是白色的
    // 金属(metallic=1)的高光是它本身的颜色(Albedo)
    vec3 F0 = vec3(0.04); // 绝缘体基础反射率
    vec3 specColor = mix(F0, baseColor.rgb, metallic);
    vec3 specular = specColor * specTerm;

    // ------------------------------------------
    // 3. 合成输出
    // ------------------------------------------
    
    // 简单的环境光
    vec3 ambient = vec3(0.1) * baseColor.rgb;

    vec3 finalColor = ambient + diffuse + specular;
    
    FragColor = vec4(finalColor, baseColor.a);
}