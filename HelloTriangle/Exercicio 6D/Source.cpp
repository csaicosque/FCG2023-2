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

    window = glfwCreateWindow(screenWidth, screenHeight, "Pizza OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

void drawPacman(float centerX, float centerY, float radius, float mouthAngle) {
    // ... (Código do Pac-Man permanece o mesmo)
}

void drawPizzaSlice(float centerX, float centerY, float radius, float startAngle, float endAngle) {
    std::vector<float> pizzaVertices;

    // Coordenadas do centro da fatia de pizza
    pizzaVertices.push_back(centerX);
    pizzaVertices.push_back(centerY);

    // Coordenadas do início da fatia
    pizzaVertices.push_back(centerX + radius * std::cos(startAngle));
    pizzaVertices.push_back(centerY + radius * std::sin(startAngle));

    // Coordenadas do final da fatia
    pizzaVertices.push_back(centerX + radius * std::cos(endAngle));
    pizzaVertices.push_back(centerY + radius * std::sin(endAngle));

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, pizzaVertices.size() * sizeof(float), pizzaVertices.data(), GL_STATIC_DRAW);

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
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.5f, 0.0f); // Laranja

    glDrawArrays(GL_TRIANGLES, 0, pizzaVertices.size() / 2);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main() {
    initGLFW();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhe o Pac-Man
        float pacmanCenterX = -0.4f;
        float pacmanCenterY = 0.0f;
        float pacmanRadius = 0.2f;
        float mouthAngle = 60.0f; // Ângulo de abertura da boca do Pac-Man (em graus)
        drawPacman(pacmanCenterX, pacmanCenterY, pacmanRadius, mouthAngle);

        // Desenhe a fatia de pizza
        float pizzaCenterX = 0.4f;
        float pizzaCenterY = 0.0f;
        float pizzaRadius = 0.3f;
        float startAngle = 0.5f; // Ângulo de início da fatia (em radianos)
        float endAngle = 2.0f;   // Ângulo de término da fatia (em radianos)
        drawPizzaSlice(pizzaCenterX, pizzaCenterY, pizzaRadius, startAngle, endAngle);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}