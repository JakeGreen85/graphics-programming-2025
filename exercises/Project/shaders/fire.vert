#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

out vec2 fragUV;

void main()
{
    // Forward UV coordinates to fragment shader
    fragUV = inUV;

    // Compute final clip-space position by applying world, view, and projection transforms
    gl_Position = ViewProjMatrix * WorldMatrix * vec4(inPosition, 1.0);
}
