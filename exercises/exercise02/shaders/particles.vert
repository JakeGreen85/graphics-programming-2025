#version 330 core

layout (location = 0) in vec2 ParticlePosition;
// (todo) 02.X: Add more vertex attributes
layout (location = 1) in float ParticleSize;
layout (location = 2) in float ParticleBirth;
layout (location = 3) in float ParticleDuration;
layout (location = 4) in vec4 ParticleColor;
layout (location = 5) in vec2 ParticleVelocity;


// (todo) 02.5: Add Color output variable here
out vec4 color;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;
uniform float Gravity;


void main()
{
	float age = CurrentTime - ParticleBirth;

	color = ParticleColor;

	gl_PointSize = age < ParticleDuration ? ParticleSize : 0;

	vec2 position = ParticlePosition;
	position += ParticleVelocity * age;
	position += 0.5f * vec2(0, Gravity) * age * age;
	gl_Position = vec4(position, 0.0, 1.0);
}
