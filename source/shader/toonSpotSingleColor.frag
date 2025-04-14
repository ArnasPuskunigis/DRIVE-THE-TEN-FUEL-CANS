#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D carTex;

layout (location = 0) out vec4 FragColor;

uniform struct SpotLightInfo {
    vec3 Position;
    vec3 La;
    vec3 L;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spot;

uniform struct FogInfo{
    float MaxDist;
    float MinDist;
    vec3 Color;
} Fog;

const int levels = 4;
const int specLevels = 3;
const float scaleFactor = 1.0 / levels;

uniform struct MaterialInfo {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
} Material;

vec3 blinnPhong(vec3 position, vec3 n, vec3 texColor) {
    vec3 diffuse = vec3(0), spec = vec3(0);
    vec3 ambient = Spot.La * texColor;
    vec3 s = normalize(Spot.Position - position);

    float cosAng = dot(-s, normalize(Spot.Direction));
    float angle = acos(cosAng);
    float spotScale = 0.0;

    if (angle >= 0.0 && angle < Spot.Cutoff) {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN = max(dot(s, n), 0.0);
        diffuse = texColor * floor(sDotN * levels) * scaleFactor;
        if (sDotN > 0.0) {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            float specToon = pow(max(dot(h, n), 0.0), Material.Shininess);
            specToon = floor(specToon * specLevels) * scaleFactor;
            spec = Material.Ks * specToon;
        }
    }

    return ambient + spotScale * (diffuse + spec) * Spot.L;
}

void main() {
    vec3 texColor = texture(carTex, TexCoord).rgb;
    float dist=abs(Position.z);
    float fogFactor=(Fog.MaxDist - dist)/(Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp (fogFactor, 0.0, 1.0);
    vec3 shadeColor = blinnPhong(Position, normalize(Normal), texColor);
    vec3 color = mix(Fog.Color, shadeColor, fogFactor);
    FragColor = vec4(color, 1.0f);
}
