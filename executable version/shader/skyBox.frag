#version 460

layout (binding = 0) uniform samplerCube SkyBoxTex;
in vec3 Vec;
in vec3 Pos;
layout (location = 0) out vec4 FragColor;
uniform vec3 CameraPos;


uniform struct FogInfo{
    float MaxDist;
    float MinDist;
    vec3 Color;
} Fog;

void main() {
	vec3 texColor=texture(SkyBoxTex, normalize(Vec)).rgb;
	texColor=pow(texColor,vec3(1.0/2.2));

	float dist = length(Pos - CameraPos);
    float fogFactor=(Fog.MaxDist - dist)/(Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp (fogFactor, 0.0, 1.0);
    vec3 shadeColor = texColor;
    vec3 color = mix(Fog.Color, shadeColor, fogFactor);
    FragColor = vec4(color, 1.0f);
}