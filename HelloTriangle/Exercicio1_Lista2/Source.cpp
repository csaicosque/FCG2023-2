#include <iostream>
#include <string>
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
    layout (location = 0) in vec2 aPos;
    void main() {
        // Ajuste das coordenadas dos vértices para os novos limites e centralização
        vec2 adjustedPos = vec2(aPos.x / 800.0 * 2.0 - 1.0, 1.0 - aPos.y / 600.0 * 2.0);
        gl_Position = vec4(adjustedPos, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main() {
        FragColor = vec4(color, 1.0);
    }
)";

void initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(screenWidth, screenHeight, "Coração OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

void drawHeart() {
    std::vector<float> heartVertices;

    for (float t = 0.0f; t <= 2.0f * M_PI; t += 0.01f) {
        float x = 16.0f * std::pow(std::sin(t), 3.0f);
        float y = 13.0f * std::cos(t) - 5.0f * std::cos(2.0f * t) - 2.0f * std::cos(3.0f * t) - std::cos(4.0f * t);
        x /= 100.0f;
        y /= 100.0f;
        // Ajuste das coordenadas dos vértices para os novos limites e centralização
        x = (x + 10.0f) * 40.0f;
        y = (y + 10.0f) * 30.0f;
        heartVertices.push_back(x);
        heartVertices.push_back(y);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, heartVertices.size() * sizeof(float), heartVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    GLuint vertexShader, fragmentShader;

    // Compilar o Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compilar o Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Vincular o programa do shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.0f, 0.0f); // Vermelho

    glDrawArrays(GL_LINE_LOOP, 0, heartVertices.size() / 2);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main() {
    initGLFW();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhar o coração
        drawHeart();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
