#version 330 core

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here
in vec4 color;

void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord
	float alpha = 1 - length((gl_PointCoord * 2) - 1);

	FragColor = vec4(color.rgb, color.a * alpha);
}
