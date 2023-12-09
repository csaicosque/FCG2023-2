#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

const int screenWidth = 800;
const int screenHeight = 600;

GLFWwindow* window;

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0); // Cor laranja
    }
)";

void initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(screenWidth, screenHeight, "Triângulo OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

int main() {
    initGLFW();

    // Coordenadas dos vértices do triângulo
    float vertices[] = {
        -0.5f, -0.6f, 0.0f, // Vértice P2
         0.6f, -0.3f, 0.0f, // Vértice P3
         0.0f,  0.6f, 0.0f  // Vértice P1
    };

    GLuint VAO, VBO;

    // Criação e configuração do Vertex Array Object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Criação e configuração do Vertex Buffer Object (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configuração do layout dos atributos dos vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Compilar o Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compilar o Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Vincular o programa do shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhar o triângulo
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpeza dos recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}