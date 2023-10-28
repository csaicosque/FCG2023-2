#include "Sprite.h"

void Sprite::initialize(int nAnimations, int nFrames)
{
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;

	ds = 1.0 / (float)nFrames;
	dt = 1.0 / (float)nAnimations;
	

	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//Primeiro triangulo
		//x    y     z     r     g    b   s    t
		-0.5,  0.5, 0.0, 1.0, 0.0, 0.0, 0.0, dt, //v0
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v1
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, ds,  dt, //v2 
		 //Segundo triangulo
		//x    y     z     r     g    b   s    t
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v1
		 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, ds,  0.0, //v3
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, ds,  dt  //v2 
	};

	GLuint VBO;
	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 


	//Atributo layout 0 - Posi��o
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo layout 1 - Cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo layout 2 - Coordenada de textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	//Inicializando outros atributos
	vel = 3.0;
	iAnimation = 0;
	iFrame = 0;
	state = IDLE;
	

}

void Sprite::update()
{
	shader->Use();
	glm::mat4 model = glm::mat4(1); //matriz identidade
	model = glm::translate(model, position);
	//model = glm::rotate(model, /*glm::radians(45.0f)*/angle, glm::vec3(0.0, 0.0, 1.0));
	
	
	/*if (state == WALKING_LEFT)
	{
		scaleFactor.x = -dimensions.x;
	}
	if (state == WALKING_RIGHT)
	{
		scaleFactor.x = dimensions.x;
	}*/
	model = glm::scale(model, dimensions);
	
	
	shader->setMat4("model", glm::value_ptr(model));

	//Atualizando o deslocamento das coordenadas de textura baseado nos �ndices de frame e anima��o
	shader->setVec2("offset", 0, 0);
	//shader->setVec2("offset", iFrame * ds, iAnimation * dt);

	//Para tocar as anima��es de forma c�clica
	/*if (state != IDLE)
	{
		iFrame = (iFrame + 1) % nFrames;
	}*/

}

void Sprite::draw()
{
	shader->Use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0); //Desconectando o buffer de geometria
	glBindTexture(GL_TEXTURE_2D, 0);
}
