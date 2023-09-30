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

	//Métodos setters de alguns atributos
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

	//Informações para a matriz de transformações
	glm::vec3 position;
	glm::vec3 dimensions;
	glm::vec3 axis;
	float angle;
	float vel;

	//Armazena o endereço do Shader
	Shader* shader;
	


};

