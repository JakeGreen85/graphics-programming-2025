#version 330 core

layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in float ParticleSize;
layout (location = 2) in float ParticleBirth;
layout (location = 3) in float ParticleDuration;
layout (location = 4) in vec4 ParticleColor;
layout (location = 5) in vec2 ParticleVelocity;

out vec4 Color;
out float Age;

uniform float CurrentTime;
uniform float Gravity;

void main()
{
    float age = CurrentTime - ParticleBirth;
    float progress = clamp(age / ParticleDuration, 0.0, 1.0);

    // Check if particle is smoke based on the color values
    bool isSmoke = (ParticleColor.r < 0.5 && ParticleColor.g < 0.5 && ParticleColor.b < 0.5);

    // Add wobble to smoke particles
    float wobbleX = isSmoke ? sin(age * 10.0) * 0.1 * progress : 0.0;

    // Calculate new position for the particle (If smoke particle, ignore gravity)
    vec2 position = ParticlePosition;
    position += ParticleVelocity * age;
    position += isSmoke ? 0.2f * vec2(0, 1) * age * age :  0.5f * vec2(0, Gravity) * age * age;
    position.x += wobbleX; // smoke curl

    // Remove sparks after their duration
    gl_PointSize = isSmoke ? ParticleSize : age < ParticleDuration ? ParticleSize : 0.0;
    gl_Position = vec4(position, 0.0, 1.0);

    // Set out variables
    Color = ParticleColor;
    Age = age;
}
