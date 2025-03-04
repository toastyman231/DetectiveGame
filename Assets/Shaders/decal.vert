#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 uvs;
out vec3 fragNormal;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec4 clipPos;
out vec4 mvPos;
out mat3 TBN;
out mat4 modelOut;

#include "globals.glsl"
uniform mat4 model = mat4(1.0);
uniform mat4 normalMat = mat4(1.0);
uniform mat4 lightSpaceMatrix;

void main()
{
    uvs = texcoords;
    fragNormal = normalize(mat3(normalMat) * normal);
    fragPos = vec3(model * vec4(position, 1.0));
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    //vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);
    modelOut = model;
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    clipPos = proj * view * model * vec4(position, 1.0);
    mvPos = model * vec4(position, 1.0);
    mvPos = view * mvPos;
    gl_Position = clipPos;
}