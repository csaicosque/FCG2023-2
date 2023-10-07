#version 400

out vec4 color;

in vec3 vertexColor;
in vec2 texcoord;

uniform sampler2D texbuffer;

uniform vec2 offset;

void main()
{
	color = texture(texbuffer,texcoord + offset);//vec4(vertexColor, 1.0);
}