#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

out vec2 fragUV;

void main()
{
    fragUV = inUV;
    gl_Position = ViewProjMatrix * WorldMatrix * vec4(inPosition, 1.0);
}
