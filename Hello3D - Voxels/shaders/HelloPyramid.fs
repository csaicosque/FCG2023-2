#version 400

in vec4 finalColor;
out vec4 color;

uniform vec3 overrideColor;
uniform bool override;

void main()
{
	if (override)
	{
		color = vec4(overrideColor,1.0);
	}
	else
	{
		color = finalColor;
	}
	
}