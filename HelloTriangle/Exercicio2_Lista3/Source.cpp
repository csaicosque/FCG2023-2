#include <iostream>
#include <vector>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Função para compilar shaders
unsigned int compileShader(const char* source, unsigned int type) {
    unsigned int shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Erro ao compilar shader:\n" << infoLog << std::endl;
        return 0;
    }

    return shaderID;
}

// Função para criar um programa de shader
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Erro ao vincular programa de shader:\n" << infoLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main() {
    // Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        return -1;
    }

    // Configuração do GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Criação de uma janela GLFW
    GLFWwindow* window = glfwCreateWindow(800, 600, "Quadrados Coloridos em Grid", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Configuração do OpenGL e do GLAD
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar o GLAD" << std::endl;
        return -1;
    }

    // Definição das coordenadas dos vértices e das cores dos quadrados em um grid
    const int numRows = 5;
    const int numCols = 5;
    const float squareSize = 0.2f;

    std::vector<float> vertices;
    std::vector<float> colors;

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            float x = col * squareSize;
            float y = row * squareSize;

            // Cores aleatórias para cada quadrado
            float r = static_cast<float>(rand()) / RAND_MAX;
            float g = static_cast<float>(rand()) / RAND_MAX;
            float b = static_cast<float>(rand()) / RAND_MAX;

            // Definição dos vértices e cores dos quadrados
            // Triângulo 1
            vertices.push_back(x);       vertices.push_back(y);
            vertices.push_back(x + squareSize); vertices.push_back(y);
            vertices.push_back(x);       vertices.push_back(y + squareSize);

            colors.push_back(r); colors.push_back(g); colors.push_back(b);
            colors.push_back(r); colors.push_back(g); colors.push_back(b);
            colors.push_back(r); colors.push_back(g); colors.push_back(b);

            // Triângulo 2
            vertices.push_back(x + squareSize); vertices.push_back(y);
            vertices.push_back(x);       vertices.push_back(y + squareSize);
            vertices.push_back(x + squareSize); vertices.push_back(y + squareSize);

            colors.push_back(r); colors.push_back(g); colors.push_back(b);
            colors.push_back(r); colors.push_back(g); colors.push_back(b);
            colors.push_back(r); colors.push_back(g); colors.push_back(b);
        }
    }

    // Criação e configuração dos buffers e VAO (Vertex Array Object)
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Atributo de posição
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Criação e configuração do buffer de cores
    unsigned int colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

    // Atributo de cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Carrega os shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec3 aColor;
        out vec3 ourColor;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            ourColor = aColor;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 ourColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(ourColor, 1.0);
        }
    )";

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Processa eventos e entrada do GLFW
        glfwPollEvents();

        // Configura a cor de fundo para preto
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Desenhe os quadrados
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2);

        // Troca os buffers
        glfwSwapBuffers(window);
    }

    // Encerramento do GLFW
    glfwTerminate();

    return 0;
}
