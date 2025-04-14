#version 460

layout (location = 0) in vec3 VertexPosition;

out vec3 Vec;
out vec3 Pos;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    vec4 Position = ModelMatrix * vec4(VertexPosition, 1.0);
    Pos = Position.xyz;

    Vec = VertexPosition;

    gl_Position = ProjectionMatrix * ViewMatrix * Position;
}
