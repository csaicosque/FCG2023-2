#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <ctime>
#include <windows.h>
#include <math.h>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GL/gl.h>
#include <GL/glu.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define STB_IMAGE_IMPLEMENTATION

//STB_IMAGE
#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H  

// Tipos
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

//----------------------------Declarando-variáveis-------------------------------------------

const GLuint width = 1080, height = 1920;
const float windowWidth = static_cast<float>(width) / static_cast<float>(height) * 2.0f;
const float squareSize = 0.02f;
const float playerSize = 0.05f;
const float maxFallSpeed = 0.05f;
float fallSpeed = 0.001f;
float playerX = 0.0f;
float playerY = -0.85f; 
float playerSpeed = 0.002f;
float squareX = 0.0f;
float squareY = 1.0f;
int success;

int playerPoints = 0;   // Contador de pontos para colisões com o jogador
int bottomPoints = 0;   // Contador de pontos para colisões com a borda inferior

bool CheckCollision(float playerX, float playerY, float  squareX, float squareY, float playerSize, float squareSize) {
    return(playerX + playerSize >= squareX - squareSize && playerX - playerSize <= squareX + squareSize) &&
        (playerY + playerSize >= squareY - squareSize && playerY - playerSize <= squareY + squareSize);
}

//---------------------------------------Chamada-função-callback-window-----------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

//---------------------------------------Função-para-contador-de-pontos------------------------------
//void RenderScore(GLFWwindow* window) {
//    // Define a posição do contador na tela
//    float x = -0.9f; // Coordenada x da tela
//    float y = 0.9f;  // Coordenada y da tela
//
//    // Configura a matriz de projeção ortográfica para coordenadas de tela
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//    glOrtho(-1, 1, -1, 1, -1, 1);
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();
//
//    // Define a cor do texto (por exemplo, branco)
//    glColor3f(1.0f, 1.0f, 1.0f);
//
//    // Posicione o cursor na tela
//    glRasterPos2f(x, y);
//
//    // Renderize a pontuação na tela usando a função `printf`
//    char scoreText[50];
//    snprintf(scoreText, sizeof(scoreText), "Pontuação: %d", playerPoints);
//    for (int i = 0; scoreText[i] != '\0'; i++) {
//        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreText[i]);
//    }
//
//    // Restaure a matriz de projeção e modelo
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//    glMatrixMode(GL_MODELVIEW);
//    glPopMatrix();
//}

void RenderText(
    GLuint shaderID,
    std::string text,
    float x, float y, float scale, glm::vec3 color,
    std::map<char, Character> Characters,
    unsigned int VAO, unsigned int VBO,
    glm::mat4 projection
    ) {
    // activate corresponding render state	
    glUseProgram(shaderID);
    glUniform3f(glGetUniformLocation(shaderID, "textColor"), color.x, color.y, color.z);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


int setupTexture(const char* filePath)
{
    GLuint textureID;
    //gera ID textura
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Definições de como será o wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Leitura do arquivo de imagem
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    if (data) {
        if (nrChannels == 3) //jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else //png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Falha ao carregar a textura: " << filePath << std::endl;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

GLuint shaderProgramBackground;


//---------------------------------------Função-principal--------------------------------------------------------------

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar o GLFW" << std::endl;
        return -1;
    }

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 42);

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

    // Carregar a textura do fundo
    GLuint texID = setupTexture("textures/forest.jpg");

    // Load character textures (font)
    std::map<char, Character> Characters;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Shader fonte

    const char* vShaderFontSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
        out vec2 TexCoords;

        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }  
    )";

    const char* fShaderFontSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;

        uniform sampler2D text;
        uniform vec3 textColor;

        void main()
        {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        }  
    )";

    //-------------------------Shader-fundo--------------------------------------------------

    const char* vertexShaderBackgroundSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        out vec2 TexCoords;
        
        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoords = (aPos.xy + 1.0) / 2.0;
        }
    )";

    const char* fragmentShaderBackgroundSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 FragColor;
        
        uniform sampler2D backgroundTexture;
        
        void main() {
            FragColor = texture(backgroundTexture, TexCoords);
        }
    )";

    unsigned int vertexShaderBackground, fragmentShaderBackground;
    vertexShaderBackground = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderBackground, 1, &vertexShaderBackgroundSource, nullptr);
    glCompileShader(vertexShaderBackground);

    fragmentShaderBackground = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderBackground, 1, &fragmentShaderBackgroundSource, nullptr);
    glCompileShader(fragmentShaderBackground);

    shaderProgramBackground = glCreateProgram();
    glAttachShader(shaderProgramBackground, vertexShaderBackground);
    glAttachShader(shaderProgramBackground, fragmentShaderBackground);
    glLinkProgram(shaderProgramBackground);

    glDeleteShader(vertexShaderBackground);
    glDeleteShader(fragmentShaderBackground);

    // Desenhar o fundo
    glUseProgram(shaderProgramBackground);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    char infoLog[512];

    if (texID == 0) {
        std::cerr << "Erro ao criar a textura." << std::endl;
        return -1;
    }

    GLint success;
    glGetShaderiv(vertexShaderBackground, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShaderBackground, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Vertex Shader Background:\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShaderBackground, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderBackground, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Fragment Shader Background:\n" << infoLog << std::endl;
    }

    glGetProgramiv(shaderProgramBackground, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramBackground, 512, nullptr, infoLog);
        std::cerr << "Erro na linkagem do Shader Program Background:\n" << infoLog << std::endl;
    }


    //---------------------------Shader-objetos-----------------------------------------------------

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

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderRed, 1, &fragmentShaderRedSource, nullptr);
    glCompileShader(fragmentShaderRed);

    fragmentShaderBlue = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderBlue, 1, &fragmentShaderBlueSource, nullptr);
    glCompileShader(fragmentShaderBlue);

    GLuint vShaderFont = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShaderFont, 1, &vShaderFontSource, 0);
    glCompileShader(vShaderFont);

    GLuint fShaderFont = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShaderFont, 1, &fShaderFontSource, 0);
    glCompileShader(fShaderFont);

    glGetShaderiv(fragmentShaderRed, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderRed, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Fragment Shader Red:\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShaderBlue, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderBlue, 512, nullptr, infoLog);
        std::cerr << "Erro na compilação do Fragment Shader Blue:\n" << infoLog << std::endl;
    }

    glGetShaderiv(fShaderFont, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fShaderFont, 512, 0, infoLog);
        std::cerr << "Erro shader fonte" << std::endl;
    }

    shaderProgramRed = glCreateProgram();
    glAttachShader(shaderProgramRed, vertexShader);
    glAttachShader(shaderProgramRed, fragmentShaderRed);
    glLinkProgram(shaderProgramRed);

    shaderProgramBlue = glCreateProgram();
    glAttachShader(shaderProgramBlue, vertexShader);
    glAttachShader(shaderProgramBlue, fragmentShaderBlue);
    glLinkProgram(shaderProgramBlue);

    GLuint shaderProgramFont = glCreateProgram();
    glAttachShader(shaderProgramFont, vShaderFont);
    glAttachShader(shaderProgramFont, fShaderFont);
    glLinkProgram(shaderProgramFont);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderRed);
    glDeleteShader(fragmentShaderBlue);
    glDeleteShader(vShaderFont);
    glDeleteShader(fShaderFont);

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

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

    unsigned int VAOFont, VBOFont;
    glGenVertexArrays(1, &VAOFont);
    glGenBuffers(1, &VBOFont);
    glBindVertexArray(VAOFont);
    glBindBuffer(GL_ARRAY_BUFFER, VBOFont);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::vector<std::pair<float, float>> squaresData; // Armazena a posição e a velocidade de cada quadrado

    srand(time(nullptr));

    //---------------------------------------Loop-Principal------------------------------------------------------------

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (bottomPoints >= 3) {
            goto gameover;
        }

        squareY -= fallSpeed;

        //------------------------Loop-colisões-----------------------------
        // Calcula os limites do square
        float squareLeft = squareSize;
        float squareRight = squareSize;
        float squareTop = squareSize + 1.0f;
        float squareBottom = squareSize + 1.0f;

        // Calcula o limite superior do player
        float playerLeft = playerSize;
        float playerRight = playerSize;
        float playerTop = playerSize + 1.0f;
        float playerBottom = playerSize + 1.0f;;

        // Verifica se houve colisão
        if (CheckCollision(playerX,playerY, squareX, squareY, playerSize, squareSize)) {
            // Colisão detectada, você pode adicionar sua lógica de resposta aqui
            playerPoints += 1;
            //std::cout << "Colisão com quadrado detectada! Pontos do jogador: " << playerPoints << std::endl;

            squareY = 1.0f;
            squareX = -1.0f + (rand() / float(RAND_MAX)) * 2.0f;
            squareY += 0.0001f;
        } else if (squareY - squareSize < -1.0f) { // Verifica se houve colisão com a borda inferior
            // Colisão com a borda inferior detectada
            bottomPoints += 1;
            //std::cout << "Colisão com a borda inferior! Pontos da borda inferior: " << bottomPoints << std::endl;

            squareY = 1.0f;
            squareX = -1.0f + (rand() / float(RAND_MAX)) * 2.0f;
            squareY += 0.0001f;
        }

        //------------------------Colissões-com-a-janela-----------------------------------------

        // Verifique a colisão com as bordas da tela para o jogador
        if (playerX - playerSize < -1.0f) {
            // Colisão com a borda esquerda da tela
            playerX = -1.0f + playerSize; // Reverta a posição
        }

        if (playerX + playerSize > 1.0f) {
            // Colisão com a borda direita da tela
            playerX = 1.0f - playerSize; // Reverta a posição
        }

        // Verifique a colisão com as bordas da tela para o "square"
        if (squareX - squareSize < -1.0f) {
            // Colisão com a borda esquerda da tela
            squareX = -1.0f + squareSize; // Reverta a posição
        }

        if (squareX + squareSize > 1.0f) {
            // Colisão com a borda direita da tela
            squareX = 1.0f - squareSize; // Reverta a posição
        }

        //--------------------------Movimentação-player---------------------------------

        if ((glfwGetKey(window, GLFW_KEY_D)     == GLFW_PRESS) || 
            (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
            playerX += playerSpeed; // Move o jogador para a direita
        }

        if ((glfwGetKey(window, GLFW_KEY_A)    == GLFW_PRESS) ||
            (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
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

        //-----------------------------Config-fundo-de-tela----------------------------------

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramBackground);
        glBindTexture(GL_TEXTURE_2D, texID);
        glActiveTexture(GL_TEXTURE);

        //-----------------------------Config-objetos------------------------------------------

        glUseProgram(shaderProgramRed);

        squares[1] = squareY - squareSize;
        squares[4] = squareY - squareSize;
        squares[7] = squareY + squareSize;
        squares[10] = squareY + squareSize;

        squares[0] = squareX - squareSize;
        squares[3] = squareX + squareSize;
        squares[6] = squareX + squareSize;
        squares[9] = squareX - squareSize;

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, squares.size() * sizeof(float), squares.data(), GL_STATIC_DRAW);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUseProgram(shaderProgramBlue);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, player.size() * sizeof(float), player.data(), GL_STATIC_DRAW);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Renderize o contador de pontos
        //RenderScore(window);

        RenderText(shaderProgramFont, "Bubbles", 0.5f, 0.5f, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
            Characters, VAOFont, VBOFont, projection);

        RenderText(shaderProgramFont, to_string(playerPoints), 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f),
            Characters, VAOFont, VBOFont, projection);

    gameover:
        if (bottomPoints >= 3) {
            RenderText(shaderProgramFont, "Game Over", 400.0f, 300.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f),
                Characters, VAOFont, VBOFont, projection);
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);

    glDeleteVertexArrays(1, &VAOFont);
    glDeleteBuffers(1, &VBOFont);

    glDeleteProgram(shaderProgramRed);
    glDeleteProgram(shaderProgramBlue);
    glDeleteProgram(shaderProgramFont);

    glfwTerminate();

    return 0;
}