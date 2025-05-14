#version 330 core

in vec2 fragUV;
out vec4 fragColor;

uniform float Time;

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

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

float flameShape(vec2 uv)
{

    // Center the UV at the bottom middle
    uv -= vec2(0.5, 0.0);


    // Vertical scale pulsing over time
    float pulse = 1.0 + 0.05 * sin(Time); // Pulse between 1.0 and 1.2
    uv.y *= pulse;

    // Inverted parabola shape
    float maxWidth = 0.6 * (1.0 - uv.y * uv.y);
    float r = abs(uv.x);
    float body = smoothstep(maxWidth, maxWidth - 0.5, r);

    // Fade top and bottom
    float bottomFade = smoothstep(-0.1, 0.1, uv.y);

    float topFade = 1.0 - smoothstep(1.0, 1.1, uv.y);

    return body * bottomFade * topFade;
}

void main()
{
    vec2 uv = fragUV;
    float t = Time * 0.5;

    float sinFlicker = sin((uv.y + Time*0.5) * 10.0) * 0.03;
    float noiseFlicker = (noise(vec2(uv.y * 5.0, Time * 0.5)) - 0.5) * 0.1;
    uv.x += (sinFlicker + noiseFlicker) * uv.y;

    float intensity = 0.0;
    vec2 fireUV = uv * vec2(1.0, 1.0);
    fireUV.y -= t * 1.2;

    for (int i = 0; i < 3; ++i)
    {
        float scale = pow(2.0, float(i));
        float weight = 1.0 / scale;
        intensity += noise(fireUV * scale) * weight;
        intensity = clamp(intensity, 0.0, 1.0);
    }

    float blend = smoothstep(0.0, 1.0, intensity);
    vec3 base = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.3, 0.0), blend);
    float glow = exp(-4.0 * distance(uv, vec2(0.5, 0.2))) * intensity;
    float mask = flameShape(uv);
    vec3 color = base + vec3(1.0, 0.5, 0.0) * glow * 0.7;

    float alpha = mask * intensity;
    alpha = pow(alpha, 1.5); // fades out faster toward the edge

    vec3 finalColor = mix(vec3(0.0), color, alpha);

    vec3 premultiplied = finalColor * alpha;
    fragColor = vec4(premultiplied, alpha);
}