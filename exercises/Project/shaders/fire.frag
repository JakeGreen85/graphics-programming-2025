#version 330 core

in vec2 fragUV;
out vec4 fragColor;

uniform float Time;

// Generates a repeatable pseudo-random float in the range [0, 1] based on a 2D input vector.
float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); // Randomly chosen large number to produce visual randomness without patterns
}

// 2D value noise based on interpolated hashed grid corners.
// Produces smooth, continuous pseudo-random values for a given position.
float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f*f*(3.0 - 2.0*f);
    return mix(a, b, u.x) + (c - a)*u.y*(1.0 - u.x) + (d - b)*u.x*u.y;
}

// Returns a mask value defining the shape of the flame.
// Based on a vertically-scaled, inverted parabola for a teardrop profile,
// with smooth fades at the top and bottom for blending.
float flameShape(vec2 uv)
{
    // Center the UV at the bottom middle
    uv -= vec2(0.5, 0.0);

    // Add slight vertical pulsing over time to make the flame "breathe"
    float pulse = 1.0 + 0.05 * sin(Time); // Pulse between 1.0 and 1.2
    uv.y *= pulse;

    // Define maximum horizontal width using an inverted parabola shape
    float maxWidth = 0.6 * (1.0 - uv.y * uv.y);
    float r = abs(uv.x);
    float body = smoothstep(maxWidth, maxWidth - 0.5, r);

    // Fade top and bottom
    float bottomFade = smoothstep(-0.1, 0.1, uv.y);
    float topFade = 1.0 - smoothstep(1.0, 1.1, uv.y);

    // Combine shape with vertical fades to get final mask
    return body * bottomFade * topFade;
}

// Combines procedural shape, animated noise, and color blending
// to render a stylized, animated flame with glowing and flickering effects.
void main()
{
    vec2 uv = fragUV;

    // Scaled time for slower motion
    float t = Time * 0.5;

    // Horizontal flickering using sine wave based on vertical position and time + additional random flicker using noise
    float sinFlicker = sin((uv.y + Time*0.5) * 10.0) * 0.03;
    float noiseFlicker = (noise(vec2(uv.y * 5.0, Time * 0.5)) - 0.5) * 0.1;
    uv.x += (sinFlicker + noiseFlicker) * uv.y;

    // Accumulate layered noise to drive flame intensity
    float intensity = 0.0;
    vec2 fireUV = uv * vec2(1.0, 1.0);

    // Scroll noise upward over time to simulate motion
    fireUV.y -= t * 1.2;

    for (int i = 0; i < 3; ++i)
    {
        float scale = pow(2.0, float(i));
        float weight = 1.0 / scale;
        intensity += noise(fireUV * scale) * weight;
        intensity = clamp(intensity, 0.0, 1.0);
    }

    // Blend between yellow and orange based on intensity
    float blend = smoothstep(0.0, 1.0, intensity);
    vec3 base = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.3, 0.0), blend);

    // Add local glow near the bottom-center of the flame
    float glow = exp(-4.0 * distance(uv, vec2(0.5, 0.2))) * intensity;

    // Compute shape mask using custom flame profile
    float mask = flameShape(uv);

    // Combine base flame color with orange glow tint
    vec3 color = base + vec3(1.0, 0.5, 0.0) * glow * 0.7;

    // Final alpha based on shape and intensity
    float alpha = mask * intensity;
    alpha = pow(alpha, 1.5);

    // Mix black background with color based on alpha for smoother edges
    vec3 finalColor = mix(vec3(0.0), color, alpha);

    // Premultiplied alpha to ensure proper blending with additive mode
    vec3 premultiplied = finalColor * alpha;
    fragColor = vec4(premultiplied, alpha);
}