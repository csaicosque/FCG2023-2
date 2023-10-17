#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Dimensões da janela
const GLuint width = 800, height = 600;

// Cria a janela GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Função para compilar shaders
unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do shader:\n" << infoLog << std::endl;
    }

    return shaderID;
}

// Função para linkar o programa do shader
unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Erro ao vincular o programa de shader:\n" << infoLog << std::endl;
    }

    return shaderProgram;
}

int main() {
    // Inicialização do GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        return -1;
    }

    // Cria a janela
    GLFWwindow* window = glfwCreateWindow(width, height, "Exercicio 5 - C", nullptr, nullptr);

    // Caso haja erro na janela GLFW
    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializa o GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar o glad" << std::endl;
        return -1;
    }

    // Notificação do framebuffer de redimensionamento de janela
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Coordenadas do vértice do triângulo 1
    float vertices1[] = {
        -1.0f, -0.5f, 0.0f,
         0.0f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    // Coordenadas do vértice do triângulo 2
    float vertices2[] = {
         0.0f, -0.5f, 0.0f,
         1.0f, -0.5f, 0.0f,
         0.5f, 0.5f, 0.0f
    };

    // Fragment shader para pontos (GL_POINTS)
    const char* pointFragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // Vermelho para GL_POINTS\n"
        "}\0";

    // Fragment shader para triângulos (GL_TRIANGLES)
    const char* triangleFragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); // Verde para GL_TRIANGLES\n"
        "}\0";

    // Fragment shader para linha de loop (GL_LINE_LOOP)
    const char* lineLoopFragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // Azul para GL_LINE_LOOP\n"
        "}\0";

    // Compilação dos shaders
    unsigned int pointVertexShader = compileShader("#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0", GL_VERTEX_SHADER);

    unsigned int pointFragmentShader = compileShader(pointFragmentShaderSource, GL_FRAGMENT_SHADER);
    unsigned int pointShaderProgram = linkShaders(pointVertexShader, pointFragmentShader);

    unsigned int triangleVertexShader = compileShader("#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0", GL_VERTEX_SHADER);

    unsigned int triangleFragmentShader = compileShader(triangleFragmentShaderSource, GL_FRAGMENT_SHADER);
    unsigned int triangleShaderProgram = linkShaders(triangleVertexShader, triangleFragmentShader);

    unsigned int lineLoopVertexShader = compileShader("#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0", GL_VERTEX_SHADER);

    unsigned int lineLoopFragmentShader = compileShader(lineLoopFragmentShaderSource, GL_FRAGMENT_SHADER);
    unsigned int lineLoopShaderProgram = linkShaders(lineLoopVertexShader, lineLoopFragmentShader);

    // Declara variáveis e uma matriz
    int success;
    char infoLog[512];

    // Cria e compila o programa de shader padrão
    unsigned int vertexShader = compileShader("#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0", GL_VERTEX_SHADER);

    unsigned int fragmentShader = compileShader("#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); // Branco para o shader padrão\n"
        "}\0", GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = linkShaders(vertexShader, fragmentShader);

    // Identificadores do VAO1 e VBO1
    unsigned int VAO1, VBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    // Vincula o VAO e o VBO
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // Envia os dados do array para o buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    // Atributo do layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Configurações do VAO2 e VBO2
    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Define o tamanho do ponto
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(30);

    // Define a grossura da linha
    glLineWidth(10);

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        // Checa os inputs
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo da janela (preto)
        glClear(GL_COLOR_BUFFER_BIT);

        // Seta o shader para pontos
        glUseProgram(pointShaderProgram);

        // Vincula o buffer de geometria
        glBindVertexArray(VAO1);
        // Chamada do polígono
        glDrawArrays(GL_POINTS, 0, 3);

        // Seta o shader para triângulos
        glUseProgram(triangleShaderProgram);

        // Vincula o buffer de geometria
        glBindVertexArray(VAO1);
        // Chamada do polígono
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Seta o shader para linha de loop
        glUseProgram(lineLoopShaderProgram);

        // Vincula o buffer de geometria
        glBindVertexArray(VAO1);
        // Chamada do polígono
        glDrawArrays(GL_LINE_LOOP, 0, 3);

        // Vincula o buffer de geometria
        glBindVertexArray(VAO2);
        // Chamada do polígono
        glDrawArrays(GL_POINTS, 0, 3);

        // Seta o shader para triângulos
        glUseProgram(triangleShaderProgram);

        // Vincula o buffer de geometria
        glBindVertexArray(VAO2);
        // Chamada do polígono
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Seta o shader para linha de loop
        glUseProgram(lineLoopShaderProgram);

        // Vincula o buffer de geometria
        glBindVertexArray(VAO2);
        // Chamada do polígono
        glDrawArrays(GL_LINE_LOOP, 0, 3);

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