#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Sprite
{
public:
	Sprite() {}
	~Sprite()
	{
		glDeleteVertexArrays(1, &VAO);
	}

	//M�todos setters de alguns atributos
	inline void setTexID(GLuint texID) { this->texID = texID; }
	inline void setPosition(glm::vec3 position) { this->position = position; }
	inline void setDimensions(glm::vec3 dimensions) { this->dimensions = dimensions; }
	inline void setShader(Shader *shader) { this->shader = shader; }

	void initialize();
	void update();
	void draw();

	inline void moveLeft() { position.x -= vel; }
	inline void moveRight() { position.x += vel; }

protected:
	GLuint VAO; //id do buffer de geometria
	GLuint texID; //id do buffer de textura

	//Informa��es para a matriz de transforma��es
	glm::vec3 position;
	glm::vec3 dimensions;
	glm::vec3 axis;
	float angle;
	float vel;

	//Armazena o endere�o do Shader
	Shader* shader;
	


};

