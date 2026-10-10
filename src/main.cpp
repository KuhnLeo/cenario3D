#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
using namespace std;

GLFWwindow *Window = nullptr;
GLuint Shader_programm = 0;

//SOL
GLuint vaoSol = 0;
GLuint vboSol = 0;
int nVerticesSol = 0;

// Aponta para cima e à frente da câmera inicial.
const glm::vec3 DIRECAO_PARA_SOL =
    glm::normalize(glm::vec3(0.5f, 0.8f, -1.0f));

//CHÃO
GLuint Vao = 0;
int NVertices;
unsigned int texture1;

//ESTRADA
GLuint VaoEstrada = 0;
int NVerticesEstrada;
GLuint texturaEstrada;

//PINHEIRO
GLuint VaoPinheiro = 0;
int NVerticesPinheiro;
GLuint texturaPinheiro;

//TERRENO
const float PASSO_TERRENO = 0.5f;
const int TERRENO_X_MIN = -25, TERRENO_X_MAX = 25;
const int Z_FIM = -59;
struct Colina {float x, z, raio, altura;};
const Colina COLINAS[] = {
    {-34.0f, -26.0f, 16.0f, 3.5f},
    { 32.0f, -52.0f, 18.0f, 4.0f},
    {-30.0f, -82.0f, 20.0f, 5.0f},
    { 28.0f, -98.0f, 14.0f, 3.0f},
    {-42.0f, -52.0f, 10.0f, 2.0f},
    { 20.0f, -14.0f,  8.0f, 1.5f},
    {-18.0f, -62.0f,  7.0f, 1.5f},
    { 40.0f, -22.0f,  9.0f, 2.0f},
};
struct ZonaPlana { float x, z, raio; };
const ZonaPlana ZONAS_PLANAS[] = {
    {-8.0f, -20.0f, 9.0f},
    { 8.0f, -34.0f, 9.0f},
};
std::vector<int> niveisTerreno;
float alturaTerreno(float x, float z) {
    if (std::fabs(x) < 4.0f) return 0.0f;
    for (const ZonaPlana &zp : ZONAS_PLANAS)
        if (std::hypot(x - zp.x, z - zp.z) < zp.raio) return 0.0f;

    float h = 0.0f;
    for (const Colina &c : COLINAS) {
        float d = std::hypot(x - c.x, z - c.z);
        if (d < c.raio)
            h = std::max(h, c.altura * 0.5f * (1.0f + std::cos(3.14159265f * d / c.raio)));
    }
    return h;
}
int indiceTerreno(int bx, int bz) {
    return (bz - Z_FIM) * (TERRENO_X_MAX - TERRENO_X_MIN + 1) + (bx - TERRENO_X_MIN);
}
void inicializaTerreno() {
    int largura = TERRENO_X_MAX - TERRENO_X_MIN + 1;
    int profund = -Z_FIM + 1;
    niveisTerreno.assign(largura * profund, 0);
    for (int bz = Z_FIM; bz <= 0; bz++)
        for (int bx = TERRENO_X_MIN; bx <= TERRENO_X_MAX; bx++)
            niveisTerreno[indiceTerreno(bx, bz)] =
                (int)std::lround(alturaTerreno(bx * 2.0f, bz * 2.0f) / PASSO_TERRENO);
}
int nivelTerreno(int bx, int bz, int padrao) {
    if (bx < TERRENO_X_MIN || bx > TERRENO_X_MAX || bz < Z_FIM || bz > 0) return padrao;
    return niveisTerreno[indiceTerreno(bx, bz)];
}

//MODELOS
struct Modelo {
    GLuint vao = 0;
    int nVertices = 0;
    GLuint textura = 0;
};

Modelo parede, paredePorta, paredeJanela, paredeCanto, telhado, telhadoCanto;

int WIDTH = 1000;
int HEIGHT = 800; 
int FB_WIDTH = 0;
int FB_HEIGHT = 0;
float ESCALA_RETINA = 1.0f;

float Tempo_entre_frames = 0.0f; // variavel utilizada para movimentar a camera

// Variáveis referentes a câmera virtual e sua projeção
float Cam_speed = 10.0f;                             // velocidade da camera aumentada um pouco para navegação livre
glm::vec3 Cam_pos = glm::vec3(0.0f, 3.0f, 9.0f);    // posicao inicial da câmera
glm::vec3 Cam_front = glm::vec3(0.0f, 0.0f, -1.0f); // vetor para onde a câmera está olhando
glm::vec3 Cam_up = glm::vec3(0.0f, 1.0f, 0.0f);     // vetor "para cima" global

float Cam_yaw = 0.0f;   // ângulo de rotação da câmera (esquerda/direita)
float Cam_pitch = 0.0f; // ângulo de inclinação da câmera (cima/baixo)

// Variáveis de controle do mouse
double lastX = WIDTH / 2.0;
double lastY = HEIGHT / 2.0;
bool primeiro_mouse = true;

int loadSimpleOBJ(string filePATH, int &nVertices)
{
    // Vetores temporários para armazenar os dados lidos do arquivo
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    // Buffer final que será enviado para a placa de vídeo (intercalado)
    std::vector<GLfloat> vBuffer;

    std::ifstream arqEntrada(filePATH.c_str());
    if (!arqEntrada.is_open())
    {
        std::cerr << "Erro ao tentar ler o arquivo " << filePATH << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(arqEntrada, line))
    {
        std::istringstream ssline(line);
        std::string word;
        ssline >> word;

        // 1. Lê as Posições (v)
        if (word == "v")
        {
            glm::vec3 vertice;
            ssline >> vertice.x >> vertice.y >> vertice.z;
            vertices.push_back(vertice);
        }
        // 2. Lê as Coordenadas de Textura (vt)
        else if (word == "vt")
        {
            glm::vec2 vt;
            ssline >> vt.s >> vt.t;
            texCoords.push_back(vt);
        }
        // 3. Lê os Vetores Normais (vn)
        else if (word == "vn")
        {
            glm::vec3 normal;
            ssline >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        // 4. Lê as Faces (f) e monta o buffer intercalado
        else if (word == "f")
        {
            while (ssline >> word)
            {
                int vi = 0, ti = 0, ni = 0;
                std::istringstream ss(word);
                std::string index;

                // Separa os índices pelo delimitador '/' (formato v/vt/vn)
                if (std::getline(ss, index, '/'))
                    vi = !index.empty() ? std::stoi(index) - 1 : 0;
                if (std::getline(ss, index, '/'))
                    ti = !index.empty() ? std::stoi(index) - 1 : 0;
                if (std::getline(ss, index))
                    ni = !index.empty() ? std::stoi(index) - 1 : 0;

                // Empacota a Posição (3 floats)
                vBuffer.push_back(vertices[vi].x);
                vBuffer.push_back(vertices[vi].y);
                vBuffer.push_back(vertices[vi].z);

                // Empacota a Textura (2 floats)
                vBuffer.push_back(texCoords[ti].s);
                vBuffer.push_back(texCoords[ti].t);

                // Empacota a Normal (3 floats)
                vBuffer.push_back(normals[ni].x);
                vBuffer.push_back(normals[ni].y);
                vBuffer.push_back(normals[ni].z);
            }
        }
    }

    arqEntrada.close();
    
    std::cout << "Gerando o buffer de geometria..." << std::endl;
    GLuint VBO, VAO;

    // Geração e Bind do VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);

    // Geração e Bind do VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Definição do Stride (tamanho do pacote de 1 vértice = 8 floats)
    int stride = 8 * sizeof(GLfloat);

    // Atributo 0: Posição (X, Y, Z)
    // Tamanho: 3. Offset: 0 (começa no início do pacote)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: Coordenada de Textura (S, T)
    // Tamanho: 2. Offset: Pula 3 floats (as posições X, Y, Z)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Atributo 2: Vetor Normal (Nx, Ny, Nz)
    // Tamanho: 3. Offset: Pula 5 floats (3 da posição + 2 da textura)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Desvincula os buffers por segurança
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Agora o vértice total possui 8 componentes (X, Y, Z, S, T, Nx, Ny, Nz)
    nVertices = vBuffer.size() / 8;

    std::cout << "Buffer de geometria gerado com " << nVertices << " vertices" << std::endl;

    return VAO;
}

Modelo carregaModelo(const string &obj, GLuint textura) {
    Modelo m;
    m.vao = loadSimpleOBJ(obj, m.nVertices);
    m.textura = textura;
    return m;
}

void desenhaModelo(const Modelo &m, const glm::mat4 &matriz) {
    GLint loc = glGetUniformLocation(Shader_programm, "matriz");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matriz));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.textura);
    glBindVertexArray(m.vao);
    glDrawArrays(GL_TRIANGLES, 0, m.nVertices);
}

std::string leShaderDoArquivo(const char *caminhoArquivo)
{
    std::ifstream arquivoShader(caminhoArquivo);

    // Verifica se conseguiu abrir o arquivo
    if (!arquivoShader.is_open())
    {
        std::cerr << "ERRO: Nao foi possivel abrir o arquivo do shader: " << caminhoArquivo << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    // Lê o buffer do arquivo e joga no stream
    shaderStream << arquivoShader.rdbuf();
    arquivoShader.close();

    // Retorna o stream convertido para string
    return shaderStream.str();
}

GLuint carregaTextura(string filePATH)
{
    std::cout << "1 - Entrando na funcao" << std::endl;
    stbi_set_flip_vertically_on_load(true);
    std::cout << "2 - flip OK" << std::endl;

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePATH.c_str(), &width, &height, &nrChannels, 0);
    std::cout << "3 - stbi_load OK, data = " << (void*)data << std::endl;

    if (!data) {
        std::cerr << "Falha: " << stbi_failure_reason() << std::endl;
        return 0;
    }
    std::cout << "4 - " << width << "x" << height << " canais=" << nrChannels << std::endl;

    GLenum format = GL_RGB;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;
    std::cout << "5 - format OK" << std::endl;
    
    GLuint textura;
    glGenTextures(1, &textura);
    std::cout << "6 - glGenTextures OK" << std::endl;

    glBindTexture(GL_TEXTURE_2D, textura);
    std::cout << "7 - glBindTexture OK" << std::endl;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::cout << "8 - TexParameteri OK" << std::endl;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    std::cout << "9 - glTexImage2D OK" << std::endl;

    glGenerateMipmap(GL_TEXTURE_2D);
    std::cout << "10 - Textura completa!" << std::endl;

    stbi_image_free(data);
    return textura;
}

void atualizaTamanhoFramebuffer()
{
    glfwGetFramebufferSize(Window, &FB_WIDTH, &FB_HEIGHT);
    ESCALA_RETINA = (float)FB_WIDTH / (float)WIDTH;
}

void redimensionaCallback(GLFWwindow *window, int w, int h)
{
    WIDTH = w;
    HEIGHT = h;
    atualizaTamanhoFramebuffer();
    glViewport(0, 0, FB_WIDTH, FB_HEIGHT);
}

// Callback responsável por ler a posição do mouse e girar a câmera
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (primeiro_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        primeiro_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido, pois as coordenadas Y vão de cima para baixo

    lastX = xpos;
    lastY = ypos;

    float sensibilidade = 0.1f;
    xoffset *= sensibilidade;
    yoffset *= sensibilidade;

    Cam_yaw -= xoffset;
    Cam_pitch += yoffset;

    // Trava do pitch para evitar que a câmera dê uma cambalhota
    if (Cam_pitch > 89.0f)
        Cam_pitch = 89.0f;
    if (Cam_pitch < -89.0f)
        Cam_pitch = -89.0f;
}

void inicializaOpenGL()
{
    if (!glfwInit())
    {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // obrigatório no macOS

    Window = glfwCreateWindow(WIDTH, HEIGHT, "Exemplo - Camera Livre com Mouse", NULL, NULL);
    if (!Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(Window, redimensionaCallback);

    // Registra a função do mouse e oculta o cursor na tela
    glfwSetCursorPosCallback(Window, mouse_callback);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Falha ao inicializar o GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "GLAD OK" << std::endl;
    std::cout << "Versao OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
}

void inicializaSol()
{
    const int faixas = 12;
    const int setores = 24;
    const float PI = 3.14159265f;

    vector<float> vertices;

    auto ponto = [&](int faixa, int setor) {
        float latitude = -PI / 2.0f + PI * faixa / faixas;
        float longitude = 2.0f * PI * setor / setores;

        return glm::vec3(
            cos(latitude) * cos(longitude),
            sin(latitude),
            cos(latitude) * sin(longitude)
        );
    };

    auto adiciona = [&](glm::vec3 p) {
        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(p.z);
    };

    for (int i = 0; i < faixas; i++) {
        for (int j = 0; j < setores; j++) {
            glm::vec3 a = ponto(i, j);
            glm::vec3 b = ponto(i, j + 1);
            glm::vec3 c = ponto(i + 1, j);
            glm::vec3 d = ponto(i + 1, j + 1);

            // Dois triângulos por região da esfera.
            adiciona(a);
            adiciona(b);
            adiciona(c);

            adiciona(b);
            adiciona(d);
            adiciona(c);
        }
    }

    nVerticesSol = static_cast<int>(vertices.size() / 3);

    glGenVertexArrays(1, &vaoSol);
    glGenBuffers(1, &vboSol);

    glBindVertexArray(vaoSol);
    glBindBuffer(GL_ARRAY_BUFFER, vboSol);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), nullptr
    );
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void inicializaObjetos()
{
    texture1 = carregaTextura("../assets/Modelos3D/colormap.png");
    Vao = loadSimpleOBJ("../assets/Modelos3D/block-grass-low-large.obj", NVertices);

    texturaEstrada = carregaTextura("../assets/Modelos3D/colormap-fantasy.png");
    VaoEstrada = loadSimpleOBJ("../assets/Modelos3D/road.obj", NVerticesEstrada);   

    texturaPinheiro = texture1; // mesma paleta do chão (Platformer Kit), não precisa carregar de novo
    VaoPinheiro = loadSimpleOBJ("../assets/Modelos3D/tree-pine.obj", NVerticesPinheiro);

    const string pasta = "../assets/Modelos3D/";
    parede       = carregaModelo(pasta + "wall.obj",              texturaEstrada);
    paredePorta  = carregaModelo(pasta + "wall-door.obj",         texturaEstrada);
    paredeJanela = carregaModelo(pasta + "wall-window-small.obj", texturaEstrada);
    paredeCanto  = carregaModelo(pasta + "wall-corner.obj",       texturaEstrada);
    telhado      = carregaModelo(pasta + "roof.obj",              texturaEstrada);
    telhadoCanto = carregaModelo(pasta + "roof-corner.obj",       texturaEstrada);

    inicializaSol();
    inicializaTerreno();
}

void inicializaShaders()
{
    // 1. Lê o código dos arquivos externos
    //std::string vertexCode = leShaderDoArquivo("../assets/shaders/vertex_shader.glsl");
    //std::string fragmentCode = leShaderDoArquivo("../assets/shaders/fragment_shader.glsl");

    std::string vertexCode = leShaderDoArquivo("../assets/shaders/vertex_shader.glsl");
    std::string fragmentCode = leShaderDoArquivo("../assets/shaders/fragment_shader.glsl");

    // 2. Converte de std::string para const char* para o OpenGL ler
    const char *vertex_shader = vertexCode.c_str();
    const char *fragment_shader = fragmentCode.c_str();

    // 3. Compila o vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    GLint success;
    char infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        std::cerr << "Erro no vertex shader:\n"
                  << infoLog << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 3. Compila o fragment shader shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fs, 512, NULL, infoLog);
        std::cerr << "Erro no fragment shader:\n"
                  << infoLog << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // 5. Especificação do Shader Programm
    Shader_programm = glCreateProgram();
    glAttachShader(Shader_programm, vs);
    glAttachShader(Shader_programm, fs);
    glLinkProgram(Shader_programm);

    glGetProgramiv(Shader_programm, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(Shader_programm, 512, NULL, infoLog);
        std::cerr << "Erro na linkagem do shader:\n"
                  << infoLog << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Define os parâmetros da iluminação.
    glUseProgram(Shader_programm);
    glm::vec3 direcaoRaios = -DIRECAO_PARA_SOL;

    glUniform3f(
        glGetUniformLocation(Shader_programm, "direcaoLuz"),
        direcaoRaios.x,
        direcaoRaios.y,
        direcaoRaios.z
    );

    glUniform1i(
        glGetUniformLocation(Shader_programm, "desenhaSol"),
        GL_FALSE
    );

    glUniform1i(
        glGetUniformLocation(Shader_programm, "texture1"), 0
    );

    glUniform3f(
        glGetUniformLocation(Shader_programm, "corLuz"),
        1.0f, 0.95f, 0.85f
    );

    glUniform1f(
        glGetUniformLocation(Shader_programm, "intensidadeAmbiente"),
        0.25f
    );
}

void atualizaDirecaoCamera()
{
    // Recalcula o vetor de direção considerando tanto o Yaw quanto o Pitch
    glm::vec3 front;
    front.x = sin(glm::radians(-Cam_yaw)) * cos(glm::radians(Cam_pitch));
    front.y = sin(glm::radians(Cam_pitch));
    front.z = -cos(glm::radians(-Cam_yaw)) * cos(glm::radians(Cam_pitch));
    Cam_front = glm::normalize(front);
}

void especificaMatrizVisualizacao()
{
    glm::mat4 visualizacao = glm::lookAt(Cam_pos, Cam_pos + Cam_front, Cam_up);

    GLint transformLoc = glGetUniformLocation(Shader_programm, "view");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(visualizacao));
}

void especificaMatrizProjecao()
{
    float znear = 0.1f;
    float zfar = 200.0f;
    float fov = glm::radians(67.0f);
    float aspecto = (float)WIDTH / (float)HEIGHT;

    glm::mat4 projecao = glm::perspective(fov, aspecto, znear, zfar);

    GLint transformLoc = glGetUniformLocation(Shader_programm, "proj");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projecao));
}

void especificaMatrizVisualizacaoMinimapa() {
    glm::vec3 posicaoTopo = glm::vec3(Cam_pos.x, 25.0f, Cam_pos.z);
    glm::vec3 alvo = glm::vec3(Cam_pos.x, 0.0f, Cam_pos.z);
    glm::vec3 upMinimapa = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::mat4 visualizacao = glm::lookAt(posicaoTopo, alvo, upMinimapa);

    GLint transformLoc = glGetUniformLocation(Shader_programm, "view");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(visualizacao));
}

void especificaMatrizProjecaoMinimapa() {
    // Projeção ortográfica: o minimapa mostra um quadrado de 2 * tamanho unidades de lado
    float tamanho = 20.0f;

    glm::mat4 projecao = glm::ortho(-tamanho, tamanho, -tamanho, tamanho, 0.1f, 100.0f);

    GLint transformLoc = glGetUniformLocation(Shader_programm, "proj");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projecao));
}

void inicializaCamera()
{
    atualizaDirecaoCamera();
    especificaMatrizVisualizacao();
    especificaMatrizProjecao();
}

void inicializaCameraMinimapa() {
    atualizaDirecaoCamera();
    especificaMatrizVisualizacaoMinimapa();
    especificaMatrizProjecaoMinimapa();
}

void trataTeclado()
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }

    // Calcula o vetor "Direita" da câmera usando o Cross product
    glm::vec3 Cam_right = glm::normalize(glm::cross(Cam_front, Cam_up));

    // A/D - Movimento lateral (Strafe)
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Cam_pos -= Cam_right * Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Cam_pos += Cam_right * Cam_speed * Tempo_entre_frames;
    }

    // W/S - Movimento para frente e para trás
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Cam_pos += Cam_front * Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Cam_pos -= Cam_front * Cam_speed * Tempo_entre_frames;
    }

    // Subir / Descer estritamente no eixo Y global (Q e E agora fazem isso, já que o mouse cuida da rotação)
    if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
    {
        Cam_pos.y += Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        Cam_pos.y -= Cam_speed * Tempo_entre_frames;
    }
}

void desenhaSol()
{
    glm::vec3 posicao = Cam_pos + DIRECAO_PARA_SOL * 120.0f;

    glm::mat4 modelo = glm::translate(glm::mat4(1.0f), posicao);
    modelo = glm::scale(modelo, glm::vec3(6.0f));

    glUniformMatrix4fv(
        glGetUniformLocation(Shader_programm, "matriz"),
        1, GL_FALSE, glm::value_ptr(modelo)
    );

    GLint modoSol = glGetUniformLocation(Shader_programm, "desenhaSol");
    glUniform1i(modoSol, GL_TRUE);

    glBindVertexArray(vaoSol);
    glDrawArrays(GL_TRIANGLES, 0, nVerticesSol);
    glBindVertexArray(0);

    glUniform1i(modoSol, GL_FALSE);
}

void desenhaChao() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBindVertexArray(Vao);

    auto bloco = [&](int x, int z, float y) {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x * 2.0f, y, z * 2.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(t));
        glDrawArrays(GL_TRIANGLES, 0, NVertices);
    };

    const float Y_BASE = -0.5f;   // a mesma altura do chão plano de antes

    for (int x = TERRENO_X_MIN; x <= TERRENO_X_MAX; x++) {
        for (int z = Z_FIM; z <= 0; z++) {
            int n = nivelTerreno(x, z, 0);
            int viz = std::min(std::min(nivelTerreno(x + 1, z, n), nivelTerreno(x - 1, z, n)),
                               std::min(nivelTerreno(x, z + 1, n), nivelTerreno(x, z - 1, n)));

            bloco(x, z, Y_BASE + n * PASSO_TERRENO);                 // bloco do topo
            for (int k = n - 1; k > viz; k--)                        // preenche vãos nas encostas
                bloco(x, z, Y_BASE + k * PASSO_TERRENO);
        }
    }
}  

void desenhaEstrada() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaEstrada);
    glBindVertexArray(VaoEstrada);

    for (int z = -59; z <= 0; z++) {
        glm::mat4 transformacao = glm::mat4(1.0f);
        transformacao = glm::translate(transformacao, glm::vec3(0 * 2.0f, 0.0f, z * 2.0f));
        transformacao = glm::scale(transformacao, glm::vec3(2.0f, 1.0f, 2.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

        glDrawArrays(GL_TRIANGLES, 0, NVerticesEstrada);
    }
}

void desenhaPinheiro() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaPinheiro);
    glBindVertexArray(VaoPinheiro);

    for (int z = -59; z <= 0; z += 10) {

        for (int lado = -1; lado <= 1; lado += 2) {
            glm::mat4 transformacao = glm::mat4(1.0f);
            transformacao = glm::translate(transformacao, glm::vec3(lado * 2.0f, 0.0f, z * 2.0f));
            transformacao = glm::scale(transformacao, glm::vec3(2.0f)); // dobra o tamanho do pinheiro
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

            glDrawArrays(GL_TRIANGLES, 0, NVerticesPinheiro);
    }
}
    }

void desenhaCasa(glm::vec3 pos, float rotY = 0.0f, float escala = 2.0f)
{
    const int LARGURA = 3;
    const int PROFUNDIDADE = 2;
    const float ALTURA_PAREDE = 1.0f; 

    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, pos);
    base = glm::rotate(base, glm::radians(rotY), glm::vec3(0, 1, 0));
    base = glm::scale(base, glm::vec3(escala));

    auto coloca = [&](const Modelo &m, glm::vec3 local, float rot) {
        glm::mat4 t = glm::translate(base, local);
        t = glm::rotate(t, glm::radians(rot), glm::vec3(0, 1, 0));
        desenhaModelo(m, t);
    };

    for (int i = 0; i < LARGURA; i++) {
        for (int j = 0; j < PROFUNDIDADE; j++) {
            float x = i - (LARGURA - 1) / 2.0f;
            float z = j - (PROFUNDIDADE - 1) / 2.0f;

            // Frente (+z): porta no meio, janelas ao lado
            if (j == PROFUNDIDADE - 1)
                coloca(i == LARGURA / 2 ? paredePorta : paredeJanela, glm::vec3(x, 0, z), -90.0f);
            // Fundo (-z)
            if (j == 0)
                coloca(parede, glm::vec3(x, 0, z), 90.0f);
            // Lado esquerdo (-x)
            if (i == 0)
                coloca(paredeJanela, glm::vec3(x, 0, z), 180.0f);
            // Lado direito (+x)
            if (i == LARGURA - 1)
                coloca(paredeJanela, glm::vec3(x, 0, z), 0.0f);

            // Telhado: beiral para fora, ponto alto no meio da casa
            if (j >= PROFUNDIDADE / 2)
                coloca(telhado, glm::vec3(x, ALTURA_PAREDE, z), 90.0f);   // metade da frente
            else
                coloca(telhado, glm::vec3(x, ALTURA_PAREDE, z), -90.0f);  // metade de trás
        }
    }
}

void inicializaRenderizacao()
{
    double tempo_anterior = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(Window))
    {
        atualizaTamanhoFramebuffer();
        double tempo_frame_atual = glfwGetTime();
        Tempo_entre_frames = (float)(tempo_frame_atual - tempo_anterior);
        tempo_anterior = tempo_frame_atual;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(Shader_programm);

        trataTeclado();

        glActiveTexture(GL_TEXTURE0);
        
        // Ativa Gaveta 0
        // glBindTexture(GL_TEXTURE_2D, texture1); // Pluga a Textura 1 nela
        // glBindVertexArray(Vao);

        glm::mat4 transformacao = glm::mat4(1.0f);
        transformacao = glm::translate(transformacao, glm::vec3(0.0f, -2.0f, 0.0f));
        // transformacao = glm::rotate(transformacao, (float)glfwGetTime() * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f));

        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

        glViewport(0, 0, FB_WIDTH, FB_HEIGHT);
        inicializaCamera();

        desenhaSol();
        desenhaChao();
        desenhaEstrada();
        desenhaPinheiro();
        desenhaCasa(glm::vec3(-8.0f, 0.0f, -20.0f),  90.0f);
        desenhaCasa(glm::vec3( 8.0f, 0.0f, -34.0f), -90.0f);

        int tamanhoMinimapa = 180;
        int margem = 5;
        glViewport(FB_WIDTH - tamanhoMinimapa - margem, FB_HEIGHT - tamanhoMinimapa - margem, tamanhoMinimapa, tamanhoMinimapa);

        // Limpa cor e profundidade só no retângulo do minimapa, para a cena principal não interferir nele
        glEnable(GL_SCISSOR_TEST);
        glScissor(FB_WIDTH - tamanhoMinimapa - margem, FB_HEIGHT - tamanhoMinimapa - margem, tamanhoMinimapa, tamanhoMinimapa);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        inicializaCameraMinimapa();
        desenhaChao();
        desenhaEstrada();
        desenhaPinheiro();
        desenhaCasa(glm::vec3(-8.0f, 0.0f, -20.0f),  90.0f);
        desenhaCasa(glm::vec3( 8.0f, 0.0f, -34.0f), -90.0f);

        glfwPollEvents();
        glfwSwapBuffers(Window);
    }

    glDeleteBuffers(1, &vboSol);
    glDeleteVertexArrays(1, &vaoSol);
    glfwTerminate();
}

int main() {
    inicializaOpenGL();
    std::cout << "OpenGL OK" << std::endl;

    inicializaObjetos();
    std::cout << "Objetos OK" << std::endl;

    inicializaShaders();
    std::cout << "Shaders OK" << std::endl;

    inicializaRenderizacao();
    return 0;
}
