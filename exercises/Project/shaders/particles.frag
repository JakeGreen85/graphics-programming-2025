#version 330 core

out vec4 FragColor;

in vec4 Color;
in float Age;

void main()
{
	float alpha = 1 - length(gl_PointCoord * 2 - 1);

    // Check if particle is smoke based on color values
    bool isSmoke = (Color.r < 0.5 && Color.g < 0.5 && Color.b < 0.5);

    // If particle is smoke, then fade the alpha over time
	if (isSmoke)
    {
        alpha *= (1.0 - Age * 0.1);
        alpha = pow(alpha, 1.5);
    }

    // Set out variable
	FragColor = vec4(Color.rgb, Color.a * alpha);
}
