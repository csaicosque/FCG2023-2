#version 400

in vec3 finalColor;
out vec4 color;
uniform float alpha;
uniform vec3 overrideColor;
uniform bool override;

void main()
{
	if (override)
	{
		color = vec4(overrideColor,alpha);
	}
	else
	{
		color = vec4(finalColor,alpha);
	}
}