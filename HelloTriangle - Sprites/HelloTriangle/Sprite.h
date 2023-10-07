#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

enum states {IDLE, WALKING_LEFT, WALKING_RIGHT};

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
	inline void setDimensions(glm::vec3 dimensions) { this->dimensions.x = dimensions.x / nFrames; this->dimensions.y = dimensions.y / nAnimations; scaleFactor = this->dimensions; }
	inline void setShader(Shader *shader) { this->shader = shader; }
	inline void setState(int state) { this->state = state; }

	void initialize(int nAnimations = 1, int nFrames = 1);
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
	
	int nAnimations, nFrames;
	int iAnimation, iFrame;
	float ds, dt;

	int state;
	glm::vec3 scaleFactor;

};

