#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const GLuint width = 1080, height = 1920;
const float windowWidth = static_cast<float>(width) / static_cast<float>(height) * 2.0f;
const float squareSize = 0.02f;
const float playerSize = 0.05;
const float maxFallSpeed = 0.05f;
float fallSpeed = 0.001f;

int playerPoints = 0;   // Contador de pontos para colis�es com o jogador
int bottomPoints = 0;   // Contador de pontos para colis�es com a borda inferior

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, "Bubbles", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar o glad" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    vector<float> squares = {
        -squareSize, -squareSize, 0.0f,
        squareSize, -squareSize, 0.0f,
        squareSize, squareSize, 0.0f,
        -squareSize, squareSize, 0.0f,
    };

    vector<float> player = {
        -playerSize, -playerSize, 0.0f,
        playerSize, -playerSize, 0.0f,
        playerSize, playerSize, 0.0f,
        -playerSize, playerSize, 0.0f,
    };

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char* fragmentShaderRedSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
        "}\0";

    const char* fragmentShaderBlueSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
        "}\0";

    unsigned int vertexShader, fragmentShaderRed, fragmentShaderBlue, shaderProgramRed, shaderProgramBlue;
    int success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderRed, 1, &fragmentShaderRedSource, nullptr);
    glCompileShader(fragmentShaderRed);

    fragmentShaderBlue = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderBlue, 1, &fragmentShaderBlueSource, nullptr);
    glCompileShader(fragmentShaderBlue);

    glGetShaderiv(fragmentShaderRed, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderRed, 512, nullptr, infoLog);
        std::cerr << "Erro na compila��o do Fragment Shader Red:\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShaderBlue, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderBlue, 512, nullptr, infoLog);
        std::cerr << "Erro na compila��o do Fragment Shader Blue:\n" << infoLog << std::endl;
    }

    shaderProgramRed = glCreateProgram();
    glAttachShader(shaderProgramRed, vertexShader);
    glAttachShader(shaderProgramRed, fragmentShaderRed);
    glLinkProgram(shaderProgramRed);

    shaderProgramBlue = glCreateProgram();
    glAttachShader(shaderProgramBlue, vertexShader);
    glAttachShader(shaderProgramBlue, fragmentShaderBlue);
    glLinkProgram(shaderProgramBlue);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderRed);
    glDeleteShader(fragmentShaderBlue);

    unsigned int VAO1, VBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, player.size() * sizeof(float), player.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    float playerX = 0.0f;
    float playerY = -1.0f; // Atualizado para -1.0f
    float playerSpeed = 0.002f;
    float squareX = 0.0f;
    float squareY = 1.0f;

    bool maxSpeedReached = false; // Vari�vel para rastrear se a velocidade m�xima foi atingida
    std::vector<std::pair<float, float>> squaresData; // Armazena a posi��o e a velocidade de cada quadrado

    srand(time(nullptr));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        squareY -= fallSpeed;

        if (squareY - squareSize < -1.0f) {
            squareY = 1.0f;
            squareX = -1.0f + (rand() / static_cast<float>(RAND_MAX)) * 2.0f;

            squareY += 0.00001f;
            // Verificar colis�es
            for (const auto& squareData : squaresData) {
                const float& squareY = squareData.first;
                const float& squareSpeed = squareData.second;

                // Calcula os limites dos quadrados
                float squareLeft = squareX - squareSize;
                float squareRight = squareX + squareSize;
                float squareTop = squareY + squareSize;
                float squareBottom = squareY - squareSize;

                // Calcula os limites do jogador
                float playerLeft = playerX - playerSize;
                float playerRight = playerX + playerSize;
                float playerTop = playerY + playerSize;
                float playerBottom = playerY - playerSize;

                // Verifica se houve colis�o
                if (playerRight > squareLeft && playerLeft < squareRight &&
                    playerTop > squareBottom && playerBottom < squareTop) {
                    // Colis�o detectada, voc� pode adicionar sua l�gica de resposta aqui
                    playerPoints++;
                    std::cout << "Colis�o com quadrado detectada! Pontos do jogador: " << playerPoints << std::endl;
                }
                // Verifica se houve colis�o com a borda inferior
                if (squareY - squareSize < -1.0f) {
                    // Colis�o com a borda inferior detectada
                    bottomPoints++;
                    std::cout << "Colis�o com a borda inferior! Pontos da borda inferior: " << bottomPoints << std::endl;
                }
            }
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            playerX += playerSpeed; // Move o jogador para a direita
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            playerX -= playerSpeed; // Move o jogador para a esquerda
        }

        player[0] = playerX - playerSize;
        player[3] = playerX + playerSize;
        player[6] = playerX + playerSize;
        player[9] = playerX - playerSize;

        player[1] = playerY - playerSize;
        player[4] = playerY - playerSize;
        player[7] = playerY + playerSize;
        player[10] = playerY + playerSize;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramRed);

        squares[1] = squareY - squareSize;
        squares[4] = squareY - squareSize;
        squares[7] = squareY + squareSize;
        squares[10] = squareY + squareSize;

        squares[0] = squareX - squareSize;
        squares[3] = squareX + squareSize;
        squares[6] = squareX + squareSize;
        squares[9] = squareX - squareSize;

        // Verifique a colis�o com as bordas da tela para o jogador
        if (playerX - playerSize < -1.0f) {
            // Colis�o com a borda esquerda da tela
            playerX = -1.0f + playerSize; // Reverta a posi��o
        }

        if (playerX + playerSize > 1.0f) {
            // Colis�o com a borda direita da tela
            playerX = 1.0f - playerSize; // Reverta a posi��o
        }

        // Verifique a colis�o com as bordas da tela para o "square"
        if (squareX - squareSize < -1.0f) {
            // Colis�o com a borda esquerda da tela
            squareX = -1.0f + squareSize; // Reverta a posi��o
        }

        if (squareX + squareSize > 1.0f) {
            // Colis�o com a borda direita da tela
            squareX = 1.0f - squareSize; // Reverta a posi��o
        }

        

       

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUseProgram(shaderProgramBlue);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, player.size() * sizeof(float), player.data(), GL_STATIC_DRAW);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgramRed);
    glDeleteProgram(shaderProgramBlue);

    glfwTerminate();

    return 0;
}
