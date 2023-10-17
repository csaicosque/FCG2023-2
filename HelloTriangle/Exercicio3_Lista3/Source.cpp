#include <iostream>
#include <vector>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Variáveis globais para armazenar a posição do quadrado
float squareX = 0.0f;
float squareY = 0.0f;
float squareSize = 0.1f;

// Função para compilar shaders
unsigned int compileShader(const char* source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Erro ao compilar o shader:\n" << infoLog << std::endl;
    }

    return shader;
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
        std::cerr << "Erro ao criar o programa de shader:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Função para mover o quadrado
void moveSquare(int direction) {
    float step = 0.01f;

    if (direction == GLFW_KEY_W) {  // Tecla W (cima)
        squareY += step;
    }
    else if (direction == GLFW_KEY_S) {  // Tecla S (baixo)
        squareY -= step;
    }
    else if (direction == GLFW_KEY_A) {  // Tecla A (esquerda)
        squareX -= step;
    }
    else if (direction == GLFW_KEY_D) {  // Tecla D (direita)
        squareX += step;
    }
}

// Função de callback para lidar com eventos de teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        moveSquare(key);
    }
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Mover Quadrado com Teclado", NULL, NULL);
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

    // Definição das coordenadas dos vértices e das cores do quadrado
    std::vector<float> vertices = {
        squareX, squareY,
        squareX + squareSize, squareY,
        squareX, squareY - squareSize,
        squareX + squareSize, squareY,
        squareX, squareY - squareSize,
        squareX + squareSize, squareY - squareSize
    };

    std::vector<float> colors = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    // Criação e configuração dos buffers e VAO (Vertex Array Object)...
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

    // Configura a função de callback de teclado
    glfwSetKeyCallback(window, keyCallback);

    // Carrega os shaders (como mostrado no exemplo anterior)...

    // Loop principal (como mostrado no exemplo anterior)...
    while (!glfwWindowShouldClose(window)) {
        // Processa eventos e entrada do GLFW (como mostrado no exemplo anterior)...

        // Renderização
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo preto
        glClear(GL_COLOR_BUFFER_BIT);

        // Atualiza as coordenadas dos vértices do quadrado com base na posição atual
        vertices = {
            squareX, squareY,
            squareX + squareSize, squareY,
            squareX, squareY - squareSize,
            squareX + squareSize, squareY,
            squareX, squareY - squareSize,
            squareX + squareSize, squareY - squareSize
        };

        // Atualiza os dados do buffer de vértices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Desenhe o quadrado
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2);

        // Troca os buffers (como mostrado no exemplo anterior)...
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Encerramento do GLFW (como mostrado no exemplo anterior)...

    return 0;
}
