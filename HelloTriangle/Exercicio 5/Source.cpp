#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>


//Dimensões da janela
const GLuint width = 800, height = 600;

//Cria a janela GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


//Função principal
int main() {
    //Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        return -1;
    }

    //cria a janela
    GLFWwindow* window = glfwCreateWindow(width, height, "Exercicio 5 - A", nullptr, nullptr);
    
    //caso tenha erro na janela GLFW
    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //caso tenha erro no GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar o glad" << std::endl;
        return -1;
    }

    //notificação do framebuffer de redimensionamento de janela
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //coordenadas do vertice do triangulo 1
    float vertices1[] = {
        -1.0f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    //coordenadas do vertice do triangulo 2
    float vertices2[] = {
         0.0f, -0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         0.5f, 0.5f, 0.0f
    };

    //Shaders
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
        "}\0";

    //declara variaveis e uma matriz
    unsigned int vertexShader, fragmentShader, shaderProgram;
    int success;
    char infoLog[512];

    //cria e compila o programa de shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    //Checa se tiveram erros na compilação
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Vertex Shader:\n" << infoLog << std::endl;
    }

    //cria e compila o fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    //checa se houve erro de compilação
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Fragment Shader:\n" << infoLog << std::endl;
    }

    //lika os shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    //checa os erros do link dos shaders
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Erro ao vincular o programa de shader:\n" << infoLog << std::endl;
    }

    //deleta os shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Identificador do VAO1 e VBO1
    unsigned int VAO1, VBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    //vincula o VAO e o VBO
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    //envia os dados do array para o buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    //atributo do layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //Configurações do VAO2 e VBO2
    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //loop principal
    while (!glfwWindowShouldClose(window)) 
    {
        //checa os inputs
        glfwPollEvents();

        //limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo da janela (preto)
        glClear(GL_COLOR_BUFFER_BIT);

        //seta o shader 
        glUseProgram(shaderProgram);

        //vincula o buffer de geometria
        glBindVertexArray(VAO1);
        //chamada do poligono preenchido (TRIANGLES)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //troca os buffers
        glfwSwapBuffers(window);
    }

    //deleta os buffers
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgram);

    //finaliza a GLFW
    glfwTerminate();

    return 0;
}