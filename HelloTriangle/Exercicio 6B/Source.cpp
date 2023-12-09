#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <cmath>

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
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
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

    window = glfwCreateWindow(screenWidth, screenHeight, "Pentágono OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

void drawPentagon(float centerX, float centerY, float radius, float rotation, float r, float g, float b) {
    std::vector<float> vertices;

    for (int i = 0; i < 5; i++) { // Cinco vértices para um pentágono
        float angle = rotation + (static_cast<float>(i) * 2.0f * 3.14159265359f / 5.0f); // 2.0f * M_PI / 5.0f para um pentágono
        float x = radius * std::cos(angle) + centerX;
        float y = radius * std::sin(angle) + centerY;
        vertices.push_back(x);
        vertices.push_back(y);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

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
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), r, g, b);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 2);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main() {
    initGLFW();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        float centerX = 0.0f;
        float centerY = 0.0f;
        float radius = 0.5f;
        float rotation = 0.0f; // Sem rotação para o pentágono

        // Defina a cor do pentágono (azul)
        float r = 0.0f;
        float g = 0.0f;
        float b = 1.0f;

        drawPentagon(centerX, centerY, radius, rotation, r, g, b);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}