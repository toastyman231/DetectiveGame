#version 410 core
out vec4 outColor;
in vec2 uvs;

uniform sampler2D SceneColor;
uniform sampler2D SceneDepth;
uniform sampler2D ShadowMap;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depth = LinearizeDepth(texture(SceneDepth, uvs).r) / far;
    outColor = vec4(vec3(depth), 1.0);
}