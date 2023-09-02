#version 400

layout (location = 0) in vec3 position;

void main()
{
	//...pode ter mais linhas de código aqui!
	gl_Position = vec4(position, 1.0);
}