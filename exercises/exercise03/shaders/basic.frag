#version 330 core

// (todo) 03.X: Add the in variables here
in vec3 Normal;

out vec4 FragColor;

uniform vec3 Color = vec3(1);

void main()
{
	FragColor = vec4(Color * dot(normalize(Normal), vec3(1)), 1);
}
