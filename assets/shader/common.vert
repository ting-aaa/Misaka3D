#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec3 v_FragPos;
out vec2 v_TexCoords;
out mat3 v_TBN; 

void main() {
    v_FragPos = vec3(u_Model * vec4(aPos, 1.0));
    v_TexCoords = aTexCoords;

    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

    T = normalize(T - dot(T, N) * N);
    
    vec3 B = cross(N, T);

    v_TBN = mat3(T, B, N);
    
    gl_Position = u_Proj * u_View * vec4(v_FragPos, 1.0);
}