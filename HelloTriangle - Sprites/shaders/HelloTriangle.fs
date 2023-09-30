#version 400

out vec4 color;

in vec3 vertexColor;
in vec2 texcoord;

uniform sampler2D texbuffer;

void main()
{
	color = texture(texbuffer,texcoord);//vec4(vertexColor, 1.0);
}