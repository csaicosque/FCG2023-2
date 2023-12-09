//-------------------------------------------INCLUDE-------------------------------------------
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Classe gerenciadora dos shaders
#include "Shader.h"

//------------------------------------------PROTÓTIPOS-DE-FUNÇÕES-------------------------------------------

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Protótipos das funções
int setupGeometry();
void initializeGrid();
void drawGrid(Shader *shader);
void drawAlphaGrid(Shader* shader); //nem precisaria ter duas funções, feito por questões didáticas
void drawWireGrid(Shader* shader);
void drawGround(Shader* shader);
void drawCursor(Shader* shader);
int setupGroundGeometry();

//------------------------------------------VARIÁVEIS-E-CONSTS-------------------------------------------

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;
const GLfloat cameraSpeed = 0.05;

const int GRID_W = 5, GRID_H = 5, GRID_D = 5;

bool rotateX=false, rotateY=false, rotateZ=false;


//Variáveis de controle da câmeera
glm::vec3 cameraPos = glm::vec3(0.0, GRID_H/2.0, GRID_D + 5.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0; //calculo do mouse
float yaw = -90.0, pitch = 0.0; //rotação em x e y

float fov = glm::radians(60.0);

int grid3D[GRID_H][GRID_W][GRID_D];

glm::vec3 cursorPos = glm::vec3(0, 0, 0);


glm::vec3 cursorColor = glm::vec3(0.3, 0.3, 0.3);

//---------------------------------------------PALETA-DE-CORES-----------------------------------------

glm::vec3 palette[5] = {
	glm::vec3(1.0, 0.0, 1.0), // Magenta
	glm::vec3(0.0, 1.0, 1.0), // Ciano
	glm::vec3(1.0, 1.0, 0.0), // Amarelo
	glm::vec3(0.0, 1.0, 0.0), // Verde
	glm::vec3(1.0, 0.0, 0.0)  // Vermelho
};

int paletteColor = 0; // Inicia no Magenta

//-------------------------------------------FUNÇÃO-SALVAR-------------------------------------------

void saveScene(const std::string& filename) {
	std::ofstream outFile(filename, std::ios::out | std::ios::binary);

	if (!outFile) {
		std::cerr << "Erro ao abrir o arquivo para salvar a cena." << std::endl;
		return;
	}

	// Salva as dimensões da matriz
	outFile.write(reinterpret_cast<const char*>(&GRID_H), sizeof(int));
	outFile.write(reinterpret_cast<const char*>(&GRID_W), sizeof(int));
	outFile.write(reinterpret_cast<const char*>(&GRID_D), sizeof(int));

	// Salva os dados da matriz
	outFile.write(reinterpret_cast<const char*>(&grid3D[0][0][0]), sizeof(int) * GRID_H * GRID_W * GRID_D);

	outFile.close();

	std::cout << "Cena salva com sucesso!" << std::endl;
}

//-------------------------------------------FUNÇÃO-CARREGAR-------------------------------------------

void loadScene(const std::string& filename) {
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);

	if (!inFile) {
		std::cerr << "Erro ao abrir o arquivo para carregar a cena." << std::endl;
		return;
	}

	int loaded_GRID_H, loaded_GRID_W, loaded_GRID_D;

	// Lê as dimensões da matriz
	inFile.read(reinterpret_cast<char*>(&loaded_GRID_H), sizeof(int));
	inFile.read(reinterpret_cast<char*>(&loaded_GRID_W), sizeof(int));
	inFile.read(reinterpret_cast<char*>(&loaded_GRID_D), sizeof(int));

	// Verifica se as dimensões são compatíveis
	if (loaded_GRID_H == GRID_H && loaded_GRID_W == GRID_W && loaded_GRID_D == GRID_D) {
		// Lê os dados da matriz
		inFile.read(reinterpret_cast<char*>(&grid3D[0][0][0]), sizeof(int) * GRID_H * GRID_W * GRID_D);

		std::cout << "Cena carregada com sucesso!" << std::endl;
	}
	else {
		std::cerr << "As dimensões da cena no arquivo não coincidem com as dimensões atuais." << std::endl;
	}

	inFile.close();
}


//-------------------------------------------FUNÇÃO-MAIN-------------------------------------------

int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Magica Voxel - Cindi Saicosque", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando oa programaa de shader
	Shader shader("../shaders/HelloPyramid.vs", "../shaders/HelloPyramid.fs");
	Shader shaderAlpha("../shaders/HelloPyramid - alphaChannel.vs", "../shaders/HelloPyramid - alphaChannel.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();
	GLuint groundVAO = setupGroundGeometry();

	//Criando a matriz de modelo
	glm::mat4 model = glm::mat4(1); //matriz identidade;
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	//Criando a matriz de projeção
	glm::mat4 projection = glm::mat4(1); //matriz identidade;
	//projection = glm::ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	//Criando a matriz de view
	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(cameraPos, glm::vec3(0.0, GRID_H / 2.0, 0.0), cameraUp);
	
	//Enviando essas infos para os dois shaders
	glUseProgram(shader.ID);
	shader.setMat4("model", glm::value_ptr(model));
	shader.setMat4("projection", glm::value_ptr(projection));
	shader.setMat4("view", glm::value_ptr(view));
	shader.setBool("override", true);
	shader.setVec3("overrideColor", cursorColor.r, cursorColor.g, cursorColor.b);

	glUseProgram(shaderAlpha.ID);
	shaderAlpha.setMat4("model", glm::value_ptr(model));
	shaderAlpha.setMat4("projection", glm::value_ptr(projection));
	shaderAlpha.setMat4("view", glm::value_ptr(view));
	//Enviando a intensidade de transparência
	shaderAlpha.setFloat("alpha", 0.1);
	shaderAlpha.setBool("override", true); //quero substituir a cor
	shaderAlpha.setVec3("overrideColor", 0.0, 0.0, 0.0); //cor nova

	//Habilitando o teste de profundidade
	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Habilitando o culling
	glEnable(GL_CULL_FACE);  // Habilita o culling
	glCullFace(GL_BACK);     // Descarta as faces traseiras


	initializeGrid();

	//---------------------------------------------------LOOP-PRINCIPAL-------------------------------------------------------

	while (!glfwWindowShouldClose(window))
	{
		//Checa os inputs
		glfwPollEvents();


		//Limpa os buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(2);
		glPointSize(20);


		//Atualizando o shader com a nova posição e orientação da camera
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		
		// Chamadas  de desenho
		glUseProgram(shaderAlpha.ID);
		shaderAlpha.setMat4("view", glm::value_ptr(view));
		shaderAlpha.setFloat("alpha", 0.1);
		//Desenhando o chão
		glBindVertexArray(groundVAO);
		drawGround(&shaderAlpha);

		
		//Desenhando a grid transparente
		glBindVertexArray(VAO);
		shaderAlpha.setFloat("alpha", 0.08);
		drawAlphaGrid(&shaderAlpha);

		//Desenhando a grid de voxels
		glUseProgram(shader.ID);
		shader.setMat4("view", glm::value_ptr(view));
		glBindVertexArray(VAO);
		glEnable(GL_DEPTH_TEST);
		drawGrid(&shader);
		
		drawCursor(&shader);
		
		glDisable(GL_DEPTH_TEST);
		glUseProgram(shaderAlpha.ID);

		//Desenhando a grid só contorno
		glBindVertexArray(groundVAO);
		drawWireGrid(&shaderAlpha);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &groundVAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

//-------------------------------------------NAVEGAÇÃO-------------------------------------------

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

	// para frente
	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	// para trás
	if (key == GLFW_KEY_Q)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	// para cima
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	// para baixo
	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}


	//Navegação do cursor do mouse para direita
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		/*if (cursorPos.x < GRID_W - 1)
		{
			cursorPos.x = cursorPos.x + 1;
		}
		else
		{
			cursorPos.x = 0;
		}*/

		cursorPos.x = cursorPos.x < GRID_W - 1 ? cursorPos.x + 1 : 0;
	}

	//Navegação do cursor do mouse para esquerda
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		/*if (cursorPos.x > 0)
		{
			cursorPos.x = cursorPos.x - 1;
		}
		else
		{
			cursorPos.x = GRID_W - 1;
		}*/
		cursorPos.x = cursorPos.x > 0 ? cursorPos.x - 1 : GRID_W - 1;
	}

	//Navegação do cursor do mouse para cima
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
	/*	if (cursorPos.y < GRID_H - 1)
		{
			cursorPos.y = cursorPos.y + 1;
		}
		else
		{
			cursorPos.y = 0;
		}*/
		cursorPos.y = cursorPos.y < GRID_H - 1 ? cursorPos.y + 1 : 0;
	}

	//Navegação do cursor do mouse para baixo
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		/*if (cursorPos.y > 0)
		{
			cursorPos.y = cursorPos.y - 1;
		}
		else
		{
			cursorPos.y = GRID_H - 1;
		}*/
		cursorPos.y = cursorPos.y > 0 ? cursorPos.y - 1 : GRID_H - 1;
	}

	// grid para frente
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
	/*	if (cursorPos.z < GRID_D - 1)
		{
			cursorPos.z = cursorPos.z + 1;
		}
		else
		{
			cursorPos.z = 0;
		}*/
		cursorPos.z = cursorPos.z < GRID_D - 1 ? cursorPos.z + 1 : 0;
	}

	// grid para trás
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		/*if (cursorPos.z > 0)
		{
			cursorPos.z = cursorPos.z - 1;
		}
		else
		{
			cursorPos.z = GRID_D - 1;
		}*/
		cursorPos.z = cursorPos.z > 0 ? cursorPos.z - 1 : GRID_D - 1;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		int x = (int)cursorPos.x;
		int y = (int)cursorPos.y;
		int z = (int)cursorPos.z;
		if (grid3D[y][x][z] == -1) //vazio
		{
			grid3D[y][x][z] = paletteColor; //adiciona voxel na grid
		}
		else
		{
			grid3D[y][x][z] = -1; //remove voxel na grid
		}
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		paletteColor = 0; // Magenta
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		paletteColor = 1; // Ciano
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		paletteColor = 2; // Amarelo
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		paletteColor = 3; // Verde
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		paletteColor = 4; // Vermelho
	}

	void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
	{
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_S:
				saveScene("scene.txt");
				break;
			case GLFW_KEY_L:
				loadScene("scene.txt");
				break;
			default:
				break;
			}
		}
	}


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//cout << xpos << " " << ypos << endl;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	//Precisamos também atualizar o cameraUp!! Pra isso, usamos o Up do  
	//mundo (y), recalculamos Right e depois o Up
	glm::vec3 right = glm::normalize(glm::cross(cameraFront,
	glm::vec3(0.0, 1.0, 0.0)));
	cameraUp = glm::normalize(glm::cross(right, cameraFront));

}

//-------------------------------------------BUFFERS-------------------------------------------

int setupGeometry()
{
	GLfloat vertices[] = {

		//Base do cubo: 2 triângulos
		//x    y    z    r    g    b
		-0.5, -0.5, -0.5, 0.88, 0.15, 0.07, //v0
		-0.5, -0.5,  0.5, 0.88, 0.15, 0.07, //v1
		 0.5, -0.5, -0.5, 0.88, 0.15, 0.07, //v2

		 -0.5, -0.5, 0.5,  0.88, 0.15, 0.07, //v3
		  0.5, -0.5,  0.5, 0.88, 0.15, 0.07, //v4
		  0.5, -0.5, -0.5, 0.88, 0.15, 0.07, //v5

		  //Face de cima: 2 triângulos
		  -0.5, 0.5,  0.5, 1.0, 0.41, 0.03, //v6
		   0.5, 0.5,  0.5, 1.0, 0.41, 0.03, //v7
		  -0.5, 0.5, -0.5, 1.0, 0.41, 0.03, //v8

		  0.5, 0.5,  0.5, 1.0, 0.41, 0.03, //v9
		  0.5, 0.5, -0.5, 1.0, 0.41, 0.03, //v10
		 -0.5, 0.5, -0.5, 1.0, 0.41, 0.03, //v11

		 //Face de frente: 2 triângulos
		 -0.5, -0.5, -0.5, 0.94, 1.0, 0.03, //v12
		 -0.5,  0.5, -0.5, 0.94, 1.0, 0.03, //v13
		  0.5, -0.5, -0.5, 0.94, 1.0, 0.03, //v14

		  0.5, -0.5, -0.5, 0.94, 1.0, 0.03, //v15
		 -0.5,  0.5, -0.5, 0.94, 1.0, 0.03, //v16
		  0.5,  0.5, -0.5, 0.94, 1.0, 0.03, //v17

		  //Face de trás: 2 triângulos
		  -0.5,  0.5,  0.5, 0.09, 0.49, 0.12, //v18
		  -0.5, -0.5,  0.5, 0.09, 0.49, 0.12, //v19
		   0.5,  0.5,  0.5, 0.09, 0.49, 0.12, //v20

		   0.5,  0.5,  0.5, 0.09, 0.49, 0.12, //v21
		  -0.5, -0.5,  0.5, 0.09, 0.49, 0.12, //v22
		   0.5, -0.5,  0.5, 0.09, 0.49, 0.12, //v23

		   //Face da esquerda: 2 triângulos
		  -0.5,  0.5, -0.5, 0.28, 0.28, 1.0, //v24
		  -0.5, -0.5, -0.5, 0.28, 0.28, 1.0, //v25
		  -0.5, -0.5,  0.5, 0.28, 0.28, 1.0, //v26

		  -0.5, -0.5,  0.5, 0.28, 0.28, 1.0, //v27
		  -0.5,  0.5,  0.5, 0.28, 0.28, 1.0, //v28
		  -0.5,  0.5, -0.5, 0.28, 0.28, 1.0, //v29

		  //Face da direita: 2 triângulos
		   0.5,  0.5,  0.5, 0.47, 0.18, 0.54, //v30
		   0.5, -0.5,  0.5, 0.47, 0.18, 0.54, //v31
		   0.5, -0.5, -0.5, 0.47, 0.18, 0.54, //v32

		   0.5, -0.5, -0.5, 0.47, 0.18, 0.54, //v33
		   0.5,  0.5, -0.5, 0.47, 0.18, 0.54, //v34
		   0.5,  0.5,  0.5, 0.47, 0.18, 0.54, //v35
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void initializeGrid()
{
	for (int y = 0; y < GRID_H; y++)
	{
		for (int x = 0; x < GRID_W; x++)
		{
			for (int z = 0; z < GRID_D; z++)
			{
				grid3D[y][x][z] = -1;
			}
		}
	}
}

void drawGrid(Shader* shader)
{
	glm::mat4 model;

	float xi = -GRID_W / 2.0;
	float zi = -GRID_D / 2.0;
	
	for (int h = 0; h < GRID_H; h++)
	{
		for (int w = 0; w < GRID_W; w++)
		{
			for (int d = 0; d < GRID_D; d++)
			{
				if (grid3D[h][w][d] != -1)
				{
					int idColor = grid3D[h][w][d];
					shader->setVec3("overrideColor", palette[idColor].r, palette[idColor].g, palette[idColor].b);
					float x = xi + w; //w * cubeWidth
					float y = h;
					float z = zi + d;
					model = glm::mat4(1);
					model = glm::translate(model, glm::vec3(x, y, z));
					shader->setMat4("model", glm::value_ptr(model));

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
	}	
}


void drawAlphaGrid(Shader* shader)
{
	glm::mat4 model;

	float xi = -GRID_W / 2.0;
	float zi = -GRID_D / 2.0;

	for (int h = 0; h < GRID_H; h++)
	{
		for (int w = 0; w < GRID_W; w++)
		{
			for (int d = 0; d < GRID_D; d++)
			{
				float x = xi + w; //w * cubeWidth
				float y = h;
				float z = zi + d;
				model = glm::mat4(1);
				model = glm::translate(model, glm::vec3(x, y, z));
				shader->setMat4("model", glm::value_ptr(model));		
				
				glDrawArrays(GL_TRIANGLES, 0, 36);
				
			}
		}
	}
}

void drawWireGrid(Shader* shader)
{
	glm::mat4 model;

	float xi = -GRID_W / 2.0;
	float zi = -GRID_D / 2.0;

	for (int h = 0; h < GRID_H; h++)
	{
		for (int w = 0; w < GRID_W; w++)
		{
			for (int d = 0; d < GRID_D; d++)
			{
				float x = xi + w; //w * cubeWidth
				float y = h;
				float z = zi + d;
				model = glm::mat4(1);
				model = glm::translate(model, glm::vec3(x, y, z));
				shader->setMat4("model", glm::value_ptr(model));
				glDrawArrays(GL_LINE_LOOP, 0, 4);
				glDrawArrays(GL_LINE_LOOP, 4, 4);
				glDrawArrays(GL_LINE_LOOP, 8, 4);
				glDrawArrays(GL_LINE_LOOP, 12, 4);

			}
		}
	}
}

void drawGround(Shader* shader)
{
	//Vamos desenhar o chão a partir de chamadas de desenho
	//utilizando os primeiros 2 triângulos do cubo
	glm::mat4 model;
	float xi = -(GRID_W + 10) / 2.0;
	float zi = -(GRID_D + 10) / 2.0;
	for (int d = 0; d < GRID_D + 10; d++)
	{
		for (int w = 0; w < GRID_W + 10; w++)
		{
			float x = xi + w; //w * cubeWidth
			float y = 0.0;
			float z = zi + d;
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(x, y, z));
			shader->setMat4("model", glm::value_ptr(model));
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		}
	}
}

int setupGroundGeometry()
{
	//pode virar parametro isso
	float gray = 0.0;
	
	GLfloat vertices[] = {

		//4 vértices para os quadrados do chão (que serão desenhados com LINE_LOOP)
		//x    y    z    r    g    b
		-0.5, -0.5, -0.5, gray, gray, gray, //v0
		 0.5, -0.5, -0.5, gray, gray, gray, //v1
		 0.5, -0.5,  0.5, gray, gray, gray, //v2
		-0.5, -0.5,  0.5, gray, gray, gray, //v3

		//face de cima
		-0.5, 0.5, -0.5, gray, gray, gray, //v4
		 0.5, 0.5, -0.5, gray, gray, gray, //v5
		 0.5, 0.5,  0.5, gray, gray, gray, //v6
		-0.5, 0.5,  0.5, gray, gray, gray, //v7

		//Face da frente
		-0.5, -0.5,  0.5, gray, gray, gray, //v8
		 0.5, -0.5,  0.5, gray, gray, gray, //v8
		 0.5,  0.5,  0.5, gray, gray, gray, //v10
		-0.5,  0.5,  0.5, gray, gray, gray, //v11

		//Face de trás
		-0.5, -0.5, -0.5, gray, gray, gray, //v12
		 0.5, -0.5, -0.5, gray, gray, gray, //v13
		 0.5,  0.5, -0.5, gray, gray, gray, //v14
		-0.5,  0.5, -0.5, gray, gray, gray, //v15
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);


	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void drawCursor(Shader* shader)
{
	shader->setVec3("overrideColor", cursorColor.r, cursorColor.g, cursorColor.b);

	glm::mat4 model;

	float xi = -GRID_W / 2.0;
	float zi = -GRID_D / 2.0;

	float x = xi + cursorPos.x; //w * cubeWidth
	float y = cursorPos.y;
	float z = zi + cursorPos.z;
	model = glm::mat4(1);
	model = glm::translate(model, glm::vec3(x, y, z));
	shader->setMat4("model", glm::value_ptr(model));

	glDrawArrays(GL_TRIANGLES, 0, 36);
}
