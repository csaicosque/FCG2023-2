#include <iostream>
#include <string>
#include <vector>

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

    window = glfwCreateWindow(screenWidth, screenHeight, "Triângulo Equilátero OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

void drawEquilateralTriangle(float x, float y, float size) {
    float halfSize = size / 2.0f;
    std::vector<float> triangleVertices = {
        x, y + size,                     // Topo
        x - halfSize, y - halfSize,     // Canto inferior esquerdo
        x + halfSize, y - halfSize      // Canto inferior direito
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(float), triangleVertices.data(), GL_STATIC_DRAW);

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

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main() {
    initGLFW();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhar quatro triângulos, um em cada quadrante
        drawEquilateralTriangle(0.25f, 0.25f, 0.2f);              // Quadrante superior esquerdo
        drawEquilateralTriangle(-0.25f, 0.25f, 0.2f);            // Quadrante superior direito
        drawEquilateralTriangle(0.25f, -0.25f, 0.2f);            // Quadrante inferior esquerdo
        drawEquilateralTriangle(-0.25f, -0.25f, 0.2f);          // Quadrante inferior direito

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
