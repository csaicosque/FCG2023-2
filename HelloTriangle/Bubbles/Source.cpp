#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Dimensões da janela
const GLuint width = 1080, height = 1920;

// Tamanho do quadrado
const float squareSize = 0.02f;

// Velocidade do movimento vertical (queda) do quadrado
const float fallSpeed = 0.02f;

float moveSpeed = 0.01f;  // Ajuste a velocidade conforme necessário

int maxAttempts = 100;

bool positionFound = false;

//-------------------------------------------------------------------

// Cria a janela GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

//-------------------------------------------------------------------

//função principal
int main() {
    // Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        return -1;
    }

    // Cria a janela
    GLFWwindow* window = glfwCreateWindow(width, height, "Quadrado em Movimento", nullptr, nullptr);

    // Caso haja erro na janela GLFW
    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Caso haja erro no GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar o glad" << std::endl;
        return -1;
    }

    // Notificação do framebuffer de redimensionamento de janela
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Coordenadas do quadrado
    vector<float> squareVertices = {
        -squareSize, -squareSize, 0.0f,
        squareSize, -squareSize, 0.0f,
        squareSize, squareSize, 0.0f,
        -squareSize, squareSize, 0.0f
    };

    // Shaders
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

    // Declara variáveis e uma matriz
    unsigned int vertexShader, fragmentShader, shaderProgram;
    int success;
    char infoLog[512];

    // Cria e compila o programa de shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    // Checa se houve erro na compilação
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Vertex Shader:\n" << infoLog << std::endl;
    }

    // Cria e compila o fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    // Checa se houve erro de compilação
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Fragment Shader:\n" << infoLog << std::endl;
    }

    // Linka os shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Checa os erros do link dos shaders
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Erro ao vincular o programa de shader:\n" << infoLog << std::endl;
    }

    // Deleta os shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Identificador do VAO e VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Vincula o VAO e o VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Envia os dados do array para o buffer
    glBufferData(GL_ARRAY_BUFFER, squareVertices.size() * sizeof(float), squareVertices.data(), GL_STATIC_DRAW);
    // Atributo do layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //---------------------------------------------------

    // Inicializa a posição do quadrado
    float squareX = 0.0f;
    float squareY = 1.0f;

    //variavel com dimensoes da tela
    const float leftBound = -1.0f;
    const float rightBound = 1.0f;
    const float topBound = 1.0f;
    const float bottomBound = -1.0f;

    // Inicializa a semente de números aleatórios
    srand(time(nullptr));

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Checa os inputs
        glfwPollEvents();

        // Atualize a posição horizontal do quadrado
        squareX += moveSpeed;

        // Verifique se o quadrado atingiu a borda direita da tela
        if (squareX + squareSize > rightBound) {
            // Restrinja o quadrado à borda direita da tela e inverta a direção
            squareX = rightBound - squareSize;
            moveSpeed = -moveSpeed; // Inverta a direção
        }

        // Verifique se o quadrado atingiu a borda esquerda da tela
        if (squareX - squareSize < leftBound) {
            // Restrinja o quadrado à borda esquerda da tela e inverta a direção
            squareX = leftBound + squareSize;
            moveSpeed = -moveSpeed; // Inverta a direção
        }

        // Atualize a posição vertical do quadrado (queda)
        squareY -= fallSpeed;

        // Verifique se o quadrado atingiu a parte inferior da tela
        if (squareY - squareSize < bottomBound) {
            // Restrinja o quadrado à parte inferior da tela e faça ele pular
            squareY = bottomBound + squareSize;
        }


        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo da janela (preto)
        glClear(GL_COLOR_BUFFER_BIT);

        // Seta o shader
        glUseProgram(shaderProgram);

        // Atualiza a posição do quadrado no VBO
        squareVertices[1] = squareY - squareSize;
        squareVertices[4] = squareY - squareSize;
        squareVertices[7] = squareY + squareSize;
        squareVertices[10] = squareY + squareSize;

        // Atualiza as coordenadas X dos vértices do quadrado
        squareVertices[0] = squareX - squareSize;
        squareVertices[3] = squareX + squareSize;
        squareVertices[6] = squareX + squareSize;
        squareVertices[9] = squareX - squareSize;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, squareVertices.size() * sizeof(float), squareVertices.data());

        // Vincula o buffer de geometria
        glBindVertexArray(VAO);
        // Chamada para renderizar o quadrado
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Troca os buffers
        glfwSwapBuffers(window);
    }

    // Deleta os buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Finaliza a GLFW
    glfwTerminate();

    return 0;
}
