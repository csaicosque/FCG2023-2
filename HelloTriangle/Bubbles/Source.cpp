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

//tamanho player
const float playerSize = 0.05;

// Velocidade do movimento vertical (queda) do quadrado
float fallSpeed = 0.001f;

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
    GLFWwindow* window = glfwCreateWindow(width, height, "Bubbles", nullptr, nullptr);

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

    // Coordenadas de vertice do quadrado e do player

    vector<float> squares = {
        -squareSize, -squareSize, 0.0f,
         squareSize, -squareSize, 0.0f,
         squareSize,  squareSize, 0.0f,
        -squareSize,  squareSize, 0.0f,
    };

    vector<float> player = {
        -playerSize, -playerSize, 0.0f,
         playerSize, -playerSize, 0.0f,
         playerSize,  playerSize, 0.0f,
        -playerSize,  playerSize, 0.0f,
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

    //// Identificador do VAO e VBO
    //unsigned int VAO, VBO;
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);
    //// Vincula o VAO e o VBO
    //glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    ////glBufferData(GL_ARRAY_BUFFER, squareVertices.size() * sizeof(float), squareVertices.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);

    //// Atributo do layout
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    //glEnableVertexAttribArray(0);

        //Identificador do VAO1 e VBO1
    unsigned int VAO1, VBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    //vincula o VAO e o VBO
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    //envia os dados do array para o buffer
    glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);    //atributo do layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //Configurações do VAO2 e VBO2
    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, player.size() * sizeof(float), player.data(), GL_STATIC_DRAW);    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    //---------------------------------------------------

    // Inicializa a posição do quadrado
    float squareX = 0.0f;
    float squareY = 1.0f;

    //Inicia posição do jogador
    float playerX = 0.0f;
    float playerY = -1.0f;

    // Inicializa a semente de números aleatórios
    srand(time(nullptr));

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Checa os inputs
        glfwPollEvents();

        // Atualiza a posição vertical do quadrado (queda)
        squareY -= fallSpeed;



        // Checa se o quadrado atingiu a parte inferior da tela
        if (squareY - squareSize < -1.0f) {
            // Gera uma nova posição X aleatória, seja à esquerda ou à direita
            squareY = 1.0f; // Volta para o topo da tela
            squareX = -1.0f + (rand() / float(RAND_MAX)) * 2.0f;
            
            //TODO: Colocar um maximo de velocidade
            fallSpeed += 0.0001f;
        }

        //PLAYER



        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo da janela (preto)
        glClear(GL_COLOR_BUFFER_BIT);

        // Seta o shader
        glUseProgram(shaderProgram);

        // Atualiza a posição do quadrado1 no VBO
        squares[1] = squareY - squareSize;
        squares[4] = squareY - squareSize;
        squares[7] = squareY + squareSize;
        squares[10] = squareY + squareSize;

        // Atualiza as coordenadas X dos vértices do quadrado1
        squares[0] = squareX - squareSize;
        squares[3] = squareX + squareSize;
        squares[6] = squareX + squareSize;
        squares[9] = squareX - squareSize;

        //glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        //glBindBuffer(GL_ARRAY_BUFFER, VBO2);

        //// Envia os dados do array para o buffer
        //glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, player.size() * sizeof(float), player.data(), GL_STATIC_DRAW);


        //// Vincula o buffer de geometria
        //glBindVertexArray(VAO1);
        //glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);

        glBindVertexArray(VAO1);
        // Chamada para renderizar o quadrado
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, player.size() * sizeof(float), player.data(), GL_STATIC_DRAW);

        glBindVertexArray(VAO2);
        // Chamada para renderizar o quadrado
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Troca os buffers
        glfwSwapBuffers(window);
    }

    // Deleta os buffers
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgram);

    // Finaliza a GLFW
    glfwTerminate();

    return 0;
}
