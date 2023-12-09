#include <iostream>
#include <vector>
#include <cmath>

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
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
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

    window = glfwCreateWindow(screenWidth, screenHeight, "Três Triângulos OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
}

void drawTriangle(float x, float y, float rotation, float scale, const float color[3], const float vertices[]) {
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_STATIC_DRAW);

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

    // Matriz de transformação manual
    float transformMatrix[16] = {
        cos(rotation) * scale, -sin(rotation) * scale, 0.0f, x,
        sin(rotation) * scale, cos(rotation) * scale, 0.0f, y,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, transformMatrix);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, color);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main() {
    initGLFW();

    float vertices1[] = {
        -0.8f, 0.8f,
        -0.6f, 0.8f,
        -0.7f, 0.6f
    };

    float vertices2[] = {
        0.6f, 0.8f,
        0.8f, 0.8f,
        0.7f, 0.6f
    };

    float vertices3[] = {
        -0.8f, -0.8f,
        -0.6f, -0.8f,
        -0.7f, -0.6f
    };

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Triângulo no quadrante superior esquerdo (vermelho)
        drawTriangle(-0.5f, 0.5f, 0.0f, 1.0f, new float[3] {1.0f, 0.0f, 0.0f}, vertices1);

        // Triângulo no quadrante superior direito (verde)
        drawTriangle(0.5f, 0.5f, 0.0f, 1.0f, new float[3] {0.0f, 1.0f, 0.0f}, vertices2);

        // Triângulo no quadrante inferior esquerdo (azul)
        drawTriangle(-0.5f, -0.5f, 0.0f, 1.0f, new float[3] {0.0f, 0.0f, 1.0f}, vertices3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
