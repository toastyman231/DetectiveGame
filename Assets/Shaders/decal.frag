#version 410 core

out vec4 outColor;

in vec2 uvs;
in vec3 fragNormal;
in vec3 fragPos;
in vec4 clipPos;
in vec4 mvPos;
in vec4 fragPosLightSpace;
in mat3 TBN;
in mat4 modelOut;

#include "material.glsl"
#include "globals.glsl"
#include "lighting.glsl"

//uniform sampler2D SceneColor;
uniform sampler2D SceneDepth;
//uniform sampler2D ShadowMap;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec4 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(uvs * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(proj) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(view) * viewSpacePosition;

    return worldSpacePosition;
}

vec4 reconstruct_pos(float z, vec2 uv_f)
{
    vec4 sPos = vec4((uv_f * 2.0) - 1.0, z * 2.0 - 1.0, 1.0);
    sPos = inverse(proj) * sPos;
    return vec4(sPos / sPos.w);
}

void clip(vec3 v){
    vec3 tr = vec3(1280, 720, 1.0);
    vec3 bl = vec3(0, 0, 1.0);
   if (v.x > tr.x || v.x < bl.x ) { discard; }
   if (v.y > tr.y || v.y < bl.y ) { discard; }
   if (v.z > tr.z || v.z < bl.z ) { discard; }
}

vec2 postProjToScreen(vec4 position)
{
    vec2 screenPos = position.xy / position.w;
    return 0.5 * (vec2(screenPos.x, screenPos.y) + 1);
}

void main()
{
    vec2 ndc = clipPos.xy / clipPos.w;

    vec2 screenPos = (ndc.xy * 0.5) + 0.5;

    float sceneDepth = texture(SceneDepth, screenPos).r;

    /*vec4 worldSpacePos = WorldPosFromDepth(sceneDepth);

    vec4 objectSpacePos = inverse(modelOut) * worldSpacePos;

    outColor = texture(material.diffuseMat.tex, objectSpacePos.xy);*/

    vec4 ScreenPosition;
    ScreenPosition.xy = screenPos * 2.0 - 1.0;
    ScreenPosition.z = (sceneDepth);
    ScreenPosition.w = 1.0;

    vec4 WorldPosition = inverse(proj) * ScreenPosition;
    WorldPosition.xyz /= WorldPosition.w;
    WorldPosition.w = 1.0;

    WorldPosition = inverse(modelOut) * WorldPosition;
    //clip(WorldPosition.xyz);

    WorldPosition.xy += 0.5;
    WorldPosition.y *= -1.0;

    outColor = textureProj(material.diffuseMat.tex, WorldPosition);

    /*if (renderMode == 0) {
        outColor = texture2DProj(material.diffuseMat.tex, uvs);//texture(material.diffuseMat.tex, objectPos.xy * 10);
    } else if (renderMode == 1) {
        outColor = texture(material.diffuseMat.tex, objectPos.xy);
    } else {
        outColor = vec4(1.0, 0.0, 1.0, 1.0);
    }*/
}