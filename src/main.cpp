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
using namespace std;

GLFWwindow *Window = nullptr;
GLuint Shader_programm = 0;

//SOL
GLuint vaoSol = 0;
GLuint vboSol = 0;
int nVerticesSol = 0;

//CHÃO
GLuint Vao = 0;
GLuint vboChao = 0;
int NVertices;
unsigned int texture1;

//ESTRADA
GLuint VaoEstrada = 0;
GLuint vboEstrada = 0;
int NVerticesEstrada;
GLuint texturaEstrada;

//PINHEIRO
GLuint VaoPinheiro = 0;
GLuint vboPinheiro = 0;
int NVerticesPinheiro;
GLuint texturaPinheiro;

// CHARACTER
GLuint texturaCharacter;

//MODELOS
struct Modelo {
    GLuint vao = 0;
    GLuint vbo = 0;
    int nVertices = 0;
    GLuint textura = 0;
};

Modelo parede, paredePorta, paredeJanela, telhado, barracaVerde, barracaVermelha;
Modelo moinhoBase, moinhoMadeira, moinhoTelhado, moinhoPas, poste;
Modelo cerca, portao, carrinho, planta, flores, barril, caixa, pedras, placa, cogumelos,
       mesa, banco, deck;
Modelo femaleB, femaleC, femaleF, maleA, maleB, maleF;

// para poder controlar posição e rotação dos objetos
struct Transformacao {
    glm::vec3 posicao;
    float rotacaoY;
    float escala;
};

struct InstanciaModelo {
    const Modelo *modelo;
    Transformacao transformacao;
};

// POSIÇÔES
const Transformacao CASAS[] = {
    {glm::vec3(-8.0f, 0.0f, -20.0f),  90.0f, 2.0f},
    {glm::vec3( 8.0f, 0.0f, -34.0f), -90.0f, 2.0f},
    {glm::vec3(-8.0f, 0.0f, -4.0f),  90.0f, 2.0f},
    {glm::vec3( 8.0f, 0.0f, -64.0f), -90.0f, 2.0f},
    {glm::vec3(-6.0f, 0.0f, -52.0f),  90.0f, 2.0f}
};

const InstanciaModelo BARRACAS[] = {
    {&barracaVerde,     {glm::vec3(10.0f, 0.0f, -52.0f), 90.0f, 2.0f}},
    {&barracaVermelha,  {glm::vec3(8.0f, 0.0f, -44.0f), 90.0f, 2.0f}}
};

const Transformacao MOINHO = {
    glm::vec3(8.0f, 0.0f, -8.0f), 180.0f, 2.0f
};

const InstanciaModelo DECORACOES[] = {
    // Estoque atrás do comerciante da barraca verde
    {&caixa,  {glm::vec3(9.2f, 0.0f, -55.0f), 15.0f, 2.0f}},
    {&barril, {glm::vec3(11.2f, 0.0f, -55.0f), 0.0f, 2.0f}},
    {&barril, {glm::vec3(11.2f, 0.0f, -56.0f), 0.0f, 2.0f}},
    // Estoque atrás do comerciante da barraca vermelha
    {&caixa,  {glm::vec3(7.2f, 0.0f, -41.0f), -15.0f, 2.0f}},
    {&barril, {glm::vec3(9.2f, 0.0f, -41.0f), 0.0f, 2.0f}},
    {&barril, {glm::vec3(9.2f, 0.0f, -40.0f), 0.0f, 2.0f}},
   
    {&placa,  {glm::vec3(2.0f, 0.0f, -42.8f), -90.0f, 2.0f}},

    {&pedras, {glm::vec3(-14.5f, 0.0f, -28.0f), 30.0f, 2.0f}},
    {&pedras, {glm::vec3(-12.5f, 0.0f, -14.0f), 30.0f, 2.0f}},
    {&pedras, {glm::vec3(10.5f, 0.0f, -6.0f), 30.0f, 2.0f}},
    {&pedras, {glm::vec3(4.5f, 0.0f, -10.0f), 30.0f, 2.0f}},
    {&placa,  {glm::vec3(2.3f, 0.0f, 0.0f), -90.0f, 2.0f}},
    {&cogumelos, {glm::vec3(-4.5f, 0.0f, -37.0f), 0.0f, 2.0f}},
    {&cogumelos, {glm::vec3(-10.0f, 0.0f, -23.5f), 0.0f, 2.0f}},
    {&carrinho, {glm::vec3(-7.0f, 0.0f, -32.2f), 90.0f, 2.0f}},
};

const Transformacao FAZENDA = {
    glm::vec3(-10.0f, 0.0f, -40.0f), 90.0f, 2.0f
};

const InstanciaModelo CHARACTERS[] = {
    // Comerciante da barraca
    {&maleA, {glm::vec3(10.0f, 0.0f, -53.2f), 0.0f, 2.0f}},
    {&maleF, {glm::vec3(8.0f, 0.0f, -42.8f), 180.0f, 2.0f}},
    // Senhor entrando na casa
    {&femaleF, {glm::vec3(-5.0f, 0.0f, -20.0f), 90.0f, 2.0f}},
    // Senhor dentro da fazenda
    {&maleB, {glm::vec3(-10.0f, 0.0f, -40.0f), 90.0f, 2.0f}},
    {&femaleC, {glm::vec3(-8.5f, 0.0f, -36.0f), 270.0f, 2.0f}},
    // Senhor andando pela rua
    {&femaleB, {glm::vec3(0.0f, 0.0f, -30.0f), 180.0f, 2.0f}}
};

// VARIÁVEIS PARA CONTROLE DE FUNÇÔES
bool moinhoLigado = true;
const float VELOCIDADE_PAS = 1.0f; // radianos por segundo
float anguloPas = 0.0f;
bool teclaMAnterior = false;
bool ehNoite = false;          
bool teclaNAnterior = false;
bool desenhandoMinimapa = false; // verdadeiro enquanto a cena é desenhada no minimapa

// Postes: intercalados com os pinheiros, mais perto da estrada.
const int POSTES_Z_INICIO = -54;
const int POSTES_Z_FIM = 0;
const int PASSO_POSTES = 4;
const float AFASTAMENTO_POSTES_X = 1.3f;
const float ESCALA_POSTES = 2.0f;
const float ALTURA_LAMPADA = 1.40f * ESCALA_POSTES;
const float RAIO_LAMPADA = 0.2f;
const float RAIO_MARCADOR_MINIMAPA = 0.6f; // raio do ponto que representa cada poste no minimapa

// Sol: direção a partir da câmera, distância e raio.
// Aponta para cima e à frente da câmera inicial.
const glm::vec3 DIRECAO_PARA_SOL =
    glm::normalize(glm::vec3(0.5f, 0.8f, -1.0f));

const float DISTANCIA_SOL = 120.0f;
const float RAIO_SOL = 6.0f;

// Distribuição dos trechos da estrada e das fileiras de pinheiros.
const int ESTRADA_Z_INICIO = -59;
const int ESTRADA_Z_FIM = 0;

// Estradas do comércio: paralelas em direção a +X.
const int QUANTIDADE_ESTRADAS_COMERCIO = 3;
const float ESTRADA_COMERCIO_Z = -46.0f;
const float DISTANCIA_ENTRE_ESTRADAS_COMERCIO = 2.0f;
const int ESTRADA_COMERCIO_X_FIM = 15;
const float ESTRADA_COMERCIO_X_INICIO = 2.0f; // primeiro trecho após a estrada principal
const glm::vec3 ESCALA_ESTRADA(2.0f, 1.0f, 2.0f);
const float ESPACAMENTO_ESTRADA = 2.0f;

// Fim efetivo da rua, considerando o espaçamento das peças.
const float ULTIMO_TRECHO_COMERCIO_X = ESTRADA_COMERCIO_X_INICIO
    + std::floor((ESTRADA_COMERCIO_X_FIM - ESTRADA_COMERCIO_X_INICIO)
                 / ESPACAMENTO_ESTRADA) * ESPACAMENTO_ESTRADA;
const float FIM_RUA_COMERCIO_X = ULTIMO_TRECHO_COMERCIO_X + ESPACAMENTO_ESTRADA / 2.0f;
// Deck encostado no fim da rua; poste atrás dele.
const float DECK_COMERCIO_X = FIM_RUA_COMERCIO_X;
const float POSTE_COMERCIO_X = ESTRADA_COMERCIO_X_FIM + 1.5f;
const float DECK_COMERCIO_Y = 0.0f;
const float ESCALA_DECK_COMERCIO = 2.0f;
const float ALTURA_DECK_COMERCIO = 0.06f * ESCALA_DECK_COMERCIO;
const float LAZER_COMERCIO_X = FIM_RUA_COMERCIO_X - 2.0f;
const float AFASTAMENTO_LAZER_COMERCIO = 2.0f;
const float AFASTAMENTO_BANCOS_MESA = 1.2f;
const float ESCALA_MOVEIS_COMERCIO = 2.0f;

const int PINHEIROS_Z_INICIO = -59;
const int PINHEIROS_Z_FIM = 0;
const int PASSO_PINHEIROS = 10;
const float ESPACAMENTO_PINHEIROS_Z = 2.0f;
const float AFASTAMENTO_PINHEIROS_X = 2.0f;
const float ALTURA_PINHEIROS = 0.0f;
const float ESCALA_PINHEIROS = 2.0f;

// Grade do terreno: espaçamento horizontal e altura da base.
const float ESPACAMENTO_TERRENO = 2.0f;
const float ALTURA_BASE_TERRENO = -0.5f;

//TERRENO: limites, colinas e áreas planas da vila.
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
    {-8.0f, -48.0f, 9.0f},
    { 8.0f, -64.0f, 9.0f},
    {-14.0f, -6.0f, 10.0f},
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
                (int)std::lround(alturaTerreno(bx * ESPACAMENTO_TERRENO, bz * ESPACAMENTO_TERRENO) / PASSO_TERRENO);
}
int nivelTerreno(int bx, int bz, int padrao) {
    if (bx < TERRENO_X_MIN || bx > TERRENO_X_MAX || bz < Z_FIM || bz > 0) return padrao;
    return niveisTerreno[indiceTerreno(bx, bz)];
}


// VARIÁVEIS PADRÃO DOS PROGRAMAS
int WIDTH = 1000;
int HEIGHT = 800; 
int FB_WIDTH = 0;
int FB_HEIGHT = 0;

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

int loadSimpleOBJ(string filePATH, int &nVertices, GLuint &VBO)
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
    GLuint VAO;

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


// preenche a struct Modelo com os dados corretos
Modelo carregaModelo(const string &obj, GLuint textura) {
    Modelo m;
    m.vao = loadSimpleOBJ(obj, m.nVertices, m.vbo);
    m.textura = textura;
    return m;
}

// aplica as transformações e desenha o modelo
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
    const string pasta = "../assets/Modelos3D/";

    texture1 = carregaTextura(pasta + "colormap.png");
    Vao = loadSimpleOBJ(pasta + "block-grass-low-large.obj", NVertices, vboChao);

    texturaEstrada = carregaTextura(pasta + "colormap-fantasy.png");
    texturaCharacter = carregaTextura(pasta + "colormap-mini-characters.png");
    VaoEstrada = loadSimpleOBJ(pasta + "road.obj", NVerticesEstrada, vboEstrada);   

    texturaPinheiro = texture1;
    VaoPinheiro = loadSimpleOBJ(pasta + "tree-pine.obj", NVerticesPinheiro, vboPinheiro);

    parede = carregaModelo(pasta + "wall.obj", texturaEstrada);
    paredePorta = carregaModelo(pasta + "wall-door.obj", texturaEstrada);
    paredeJanela = carregaModelo(pasta + "wall-window-small.obj", texturaEstrada);
    telhado = carregaModelo(pasta + "roof.obj", texturaEstrada);
    
    barracaVerde = carregaModelo(pasta + "stall-green.obj", texturaEstrada);
    barracaVermelha = carregaModelo(pasta + "stall-red.obj", texturaEstrada);

    maleA = carregaModelo(pasta + "character-male-a.obj", texturaCharacter);
    maleB = carregaModelo(pasta + "character-male-b.obj", texturaCharacter);
    maleF = carregaModelo(pasta + "character-male-f.obj", texturaCharacter);
    femaleB = carregaModelo(pasta + "character-female-b.obj", texturaCharacter);
    femaleC = carregaModelo(pasta + "character-female-c.obj", texturaCharacter);
    femaleF = carregaModelo(pasta + "character-female-f.obj", texturaCharacter);

    moinhoBase = carregaModelo(pasta + "wall-block.obj", texturaEstrada);
    moinhoMadeira = carregaModelo(pasta + "wall-wood-block.obj", texturaEstrada);
    moinhoTelhado = carregaModelo(pasta + "roof-high-point.obj", texturaEstrada);
    moinhoPas = carregaModelo(pasta + "windmill.obj", texturaEstrada);
    
    poste = carregaModelo(pasta + "lantern.obj", texturaEstrada);
    
    cerca = carregaModelo(pasta + "fence.obj", texturaEstrada);
    portao = carregaModelo(pasta + "fence-gate.obj", texturaEstrada);
    placa = carregaModelo(pasta + "sign.obj", texture1);

    carrinho = carregaModelo(pasta + "cart.obj", texturaEstrada);
    planta = carregaModelo(pasta + "plant.obj", texture1);
    flores = carregaModelo(pasta + "flowers.obj", texture1);
    barril = carregaModelo(pasta + "barrel.obj", texture1);
    caixa = carregaModelo(pasta + "crate.obj", texture1);
    pedras = carregaModelo(pasta + "rocks.obj", texture1);
    cogumelos = carregaModelo(pasta + "mushrooms.obj", texture1);
    mesa = carregaModelo(pasta + "stall.obj", texturaEstrada);
    banco = carregaModelo(pasta + "stall-bench.obj", texturaEstrada);
    deck = carregaModelo(pasta + "planks-half.obj", texture1);

    inicializaSol();
    inicializaTerreno();
}

void inicializaShaders()
{
    std::string vertexCode = leShaderDoArquivo("../assets/shaders/vertex_shader.glsl");
    std::string fragmentCode = leShaderDoArquivo("../assets/shaders/fragment_shader.glsl");

    const char *vertex_shader = vertexCode.c_str();
    const char *fragment_shader = fragmentCode.c_str();

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
    bool teclaM = glfwGetKey(Window, GLFW_KEY_M) == GLFW_PRESS;
    if (teclaM && !teclaMAnterior) {
        moinhoLigado = !moinhoLigado;
    }
    teclaMAnterior = teclaM;
    bool teclaN = glfwGetKey(Window, GLFW_KEY_N) == GLFW_PRESS;
    if (teclaN && !teclaNAnterior) {
        ehNoite = !ehNoite;
    }
    teclaNAnterior = teclaN;
}

void aplicaIluminacao() {
    GLint locCor = glGetUniformLocation(Shader_programm, "corLuz");
    GLint locAmbiente = glGetUniformLocation(Shader_programm, "intensidadeAmbiente");

    if (ehNoite){
        glUniform3f(locCor, 0.25f, 0.30f, 0.45f);
        glUniform1f(locAmbiente, 0.15f);
    } else {
        glUniform3f(locCor, 1.0f, 0.95f, 0.85f);
        glUniform1f(locAmbiente, 0.25f);
    }
}

void desenhaCorpoSeleste()
{
    glm::vec3 posicao = Cam_pos + DIRECAO_PARA_SOL * DISTANCIA_SOL;

    glm::mat4 modelo = glm::translate(glm::mat4(1.0f), posicao);
    modelo = glm::scale(modelo, glm::vec3(RAIO_SOL));

    glUniformMatrix4fv(
        glGetUniformLocation(Shader_programm, "matriz"),
        1, GL_FALSE, glm::value_ptr(modelo)
    );

    GLint modoSol = glGetUniformLocation(Shader_programm, "desenhaSol");
    glUniform1i(modoSol, GL_TRUE);
    glUniform1i(glGetUniformLocation(Shader_programm, "ehNoite"), ehNoite);

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
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x * ESPACAMENTO_TERRENO, y, z * ESPACAMENTO_TERRENO));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(t));
        glDrawArrays(GL_TRIANGLES, 0, NVertices);
    };

    for (int x = TERRENO_X_MIN; x <= TERRENO_X_MAX; x++) {
        for (int z = Z_FIM; z <= 0; z++) {
            int n = nivelTerreno(x, z, 0);
            int viz = std::min(std::min(nivelTerreno(x + 1, z, n), nivelTerreno(x - 1, z, n)),
                               std::min(nivelTerreno(x, z + 1, n), nivelTerreno(x, z - 1, n)));

            bloco(x, z, ALTURA_BASE_TERRENO + n * PASSO_TERRENO);                 // bloco do topo
            for (int k = n - 1; k > viz; k--)                        // preenche vãos nas encostas
                bloco(x, z, ALTURA_BASE_TERRENO + k * PASSO_TERRENO);
        }
    }
}  

void desenhaEstrada() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaEstrada);
    glBindVertexArray(VaoEstrada);

    for (int z = ESTRADA_Z_INICIO; z <= ESTRADA_Z_FIM; z++) {
        glm::mat4 transformacao = glm::mat4(1.0f);
        transformacao = glm::translate(transformacao, glm::vec3(0.0f, 0.0f, z * ESPACAMENTO_ESTRADA));
        transformacao = glm::scale(transformacao, ESCALA_ESTRADA);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

        glDrawArrays(GL_TRIANGLES, 0, NVerticesEstrada);
    }

    for (int i = 0; i < QUANTIDADE_ESTRADAS_COMERCIO; i++) {
        const float z = ESTRADA_COMERCIO_Z - i * DISTANCIA_ENTRE_ESTRADAS_COMERCIO;
        for (float x = ESTRADA_COMERCIO_X_INICIO; x <= ESTRADA_COMERCIO_X_FIM; x += ESPACAMENTO_ESTRADA) {
            glm::mat4 transformacao = glm::mat4(1.0f);
            transformacao = glm::translate(transformacao, glm::vec3(x, 0.0f, z));
            transformacao = glm::rotate(transformacao, glm::radians(90.0f), glm::vec3(0, 1, 0));
            transformacao = glm::scale(transformacao, ESCALA_ESTRADA);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));
    
            glDrawArrays(GL_TRIANGLES, 0, NVerticesEstrada);
        }

    }
}

void desenhaPinheiro() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaPinheiro);
    glBindVertexArray(VaoPinheiro);

    for (int z = PINHEIROS_Z_INICIO; z <= PINHEIROS_Z_FIM; z += PASSO_PINHEIROS) {

        for (int lado = -1; lado <= 1; lado += 2) {
            glm::mat4 transformacao = glm::mat4(1.0f);
            transformacao = glm::translate(transformacao, glm::vec3(lado * AFASTAMENTO_PINHEIROS_X, ALTURA_PINHEIROS, z * ESPACAMENTO_PINHEIROS_Z));
            transformacao = glm::scale(transformacao, glm::vec3(ESCALA_PINHEIROS));
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

            glDrawArrays(GL_TRIANGLES, 0, NVerticesPinheiro);
        }
    }
}

void desenhaBarraca() {
    for (const InstanciaModelo &barraca : BARRACAS) {
        const Transformacao &t = barraca.transformacao;

        glm::mat4 matriz = glm::translate(glm::mat4(1.0f), t.posicao);
        matriz = glm::rotate(
            matriz, glm::radians(t.rotacaoY), glm::vec3(0, 1, 0)
        );
        matriz = glm::scale(matriz, glm::vec3(t.escala));

        desenhaModelo(*barraca.modelo, matriz);
    }
}

void desenhaCharacter()
{
    for (const InstanciaModelo &personagem : CHARACTERS) {
        const Transformacao &t = personagem.transformacao;
        glm::mat4 matriz = glm::mat4(1.0f);

        matriz = glm::translate(matriz, t.posicao);

        matriz = glm::rotate(
            matriz,
            glm::radians(t.rotacaoY),
            glm::vec3(0, 1, 0)
        );

        matriz = glm::scale(
            matriz,
            glm::vec3(t.escala)
        );

        desenhaModelo(*personagem.modelo, matriz);
    }
}

void desenhaCasa(glm::vec3 pos, float rotY = 0.0f, float escala = 2.0f)
{
    const int LARGURA = 3;
    const int PROFUNDIDADE = 2;
    const float ALTURA_PAREDE = 1.0f; 

    // aplica as transformações da casa de acordo com o que for passado
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, pos);
    base = glm::rotate(base, glm::radians(rotY), glm::vec3(0, 1, 0));
    base = glm::scale(base, glm::vec3(escala));

    // função pra posicionar cada parte da casa
    auto coloca = [&](const Modelo &m, glm::vec3 local, float rot) {
        glm::mat4 t = glm::translate(base, local);
        t = glm::rotate(t, glm::radians(rot), glm::vec3(0, 1, 0));
        desenhaModelo(m, t);
    };

    // posiciona cada parte da casa
    for (int i = 0; i < LARGURA; i++) {
        for (int j = 0; j < PROFUNDIDADE; j++) {
            float x = i - (LARGURA - 1) / 2.0f;
            float z = j - (PROFUNDIDADE - 1) / 2.0f;

            // parte da frente
            if (j == PROFUNDIDADE - 1)
                //porta no meio e janelas no lado
                coloca(i == LARGURA / 2 ? paredePorta : paredeJanela, glm::vec3(x, 0, z), -90.0f);
            // parte de trás
            if (j == 0)
                coloca(parede, glm::vec3(x, 0, z), 90.0f);
            // esquerda
            if (i == 0)
                coloca(paredeJanela, glm::vec3(x, 0, z), 180.0f);
            // direita
            if (i == LARGURA - 1)
                coloca(paredeJanela, glm::vec3(x, 0, z), 0.0f);

            // telhado
            if (j >= PROFUNDIDADE / 2)
                coloca(telhado, glm::vec3(x, ALTURA_PAREDE, z), 90.0f);
            else
                coloca(telhado, glm::vec3(x, ALTURA_PAREDE, z), -90.0f);
        }
    }
}

// para cada casa desenha a casa
void desenhaCasas()
{
    for (const Transformacao &casa : CASAS) {
        desenhaCasa(casa.posicao, casa.rotacaoY, casa.escala);
    }
}

void desenhaMoinho(){
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, MOINHO.posicao);
    base = glm::rotate(base, glm::radians(MOINHO.rotacaoY), glm::vec3(0,1,0));
    base = glm::scale(base, glm::vec3(MOINHO.escala));

    desenhaModelo(moinhoBase, base);
    desenhaModelo(moinhoMadeira, glm::translate(base, glm::vec3(0.0f, 1.0f, 0.0f)));
    desenhaModelo(moinhoMadeira, glm::translate(base, glm::vec3(0.0f, 2.0f, 0.0f)));
    desenhaModelo(moinhoTelhado, glm::translate(base, glm::vec3(0.0f, 3.0f, 0.0f)));

    glm::mat4 pas = glm::translate(base, glm::vec3(0.75f, 2.5f, 0.0f));
    pas = glm::rotate(pas, anguloPas, glm::vec3(1, 0, 0));
    desenhaModelo(moinhoPas, pas);
}

void desenhaFazenda()
{
    const int W = 5; // width, largura
    const int D = 4; // depth, profundidade
    const float ROT_CERCA_X = 90.0f;
    const float ROT_CERCA_Z = 0.0f;
    const float ESCALA_PLANTAS = 1.0f;
    const float CERCA_DESLOC = 0.45f;

    //faz a transformação base da fazenda
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, FAZENDA.posicao);
    base = glm::rotate(base, glm::radians(FAZENDA.rotacaoY), glm::vec3(0, 1, 0));
    base = glm::scale(base, glm::vec3(FAZENDA.escala));

    // função auxiliar pra posicionar os objetos da fazenda
    auto coloca = [&](const Modelo &m, glm::vec3 local, float rot, float esc = 1.0f) {
        glm::mat4 t = glm::translate(base, local);
        t = glm::rotate(t, glm::radians(rot), glm::vec3(0, 1, 0));
        t = glm::scale(t, glm::vec3(esc));
        desenhaModelo(m, t);
    };

    // desenha a linha da cerca
    auto colocaCerca = [&](const Modelo &m, glm::vec3 linha, float rot) {
        glm::mat4 giro = glm::rotate(glm::mat4(1.0f), glm::radians(rot), glm::vec3(0, 1, 0));
        glm::vec3 desloc = glm::vec3(giro * glm::vec4(CERCA_DESLOC, 0.0f, 0.0f, 1.0f));
        glm::mat4 t = glm::translate(base, linha - desloc) * giro;
        desenhaModelo(m, t);
    };

    float meiaL = W / 2.0f;
    float meiaP = D / 2.0f;

    // posiciona ascercas
    for (int i = 0; i < W; i++) {
        float x = i - (W - 1) / 2.0f;
        // frente
        colocaCerca(i == W / 2 ? portao : cerca, glm::vec3(x, 0, meiaP), ROT_CERCA_X);
        // trás
        colocaCerca(cerca, glm::vec3(x, 0, -meiaP), ROT_CERCA_X);
    }
    for (int j = 0; j < D; j++) {
        float z = j - (D - 1) / 2.0f;
        // esquerda
        colocaCerca(cerca, glm::vec3(-meiaL, 0, z), ROT_CERCA_Z);
        // direita
        colocaCerca(cerca, glm::vec3( meiaL, 0, z), ROT_CERCA_Z);
    }

    // posiciona as plantas
    float larguraUtil = W - 1.0f;
    for (int r = 0; r < D - 1; r++) {
        float z = r - (D - 2) / 2.0f;
        const Modelo &tipo = (r % 2 == 0) ? planta : flores;
        for (int c = 0; c < 6; c++) {
            float x = -larguraUtil / 2.0f + c * larguraUtil / 5.0f;
            coloca(tipo, glm::vec3(x, 0, z), 0.0f, ESCALA_PLANTAS);
        }
    }
}

void desenhaLampada(glm::vec3 posicao, float raio) {
    glm::mat4 modelo = glm::translate(glm::mat4(1.0f), posicao);
    modelo = glm::scale(modelo, glm::vec3(raio));

    glUniformMatrix4fv(glGetUniformLocation(Shader_programm, "matriz"), 1, GL_FALSE, glm::value_ptr(modelo));
   
    GLint modoSol = glGetUniformLocation(Shader_programm, "desenhaSol");
    GLint modoNoite = glGetUniformLocation(Shader_programm, "ehNoite");

    glUniform1i(modoSol, GL_TRUE);
    glUniform1i(modoNoite, GL_FALSE);

    glBindVertexArray(vaoSol);
    glDrawArrays(GL_TRIANGLES, 0, nVerticesSol);
    
    glUniform1i(modoSol, GL_FALSE);
    glUniform1i(modoNoite, ehNoite); 
}

// para cada decoração desenha a decoração na posição e rotação especificadas
void desenhaDecoracoes()
{
    for (const InstanciaModelo &d : DECORACOES) {
        const Transformacao &t = d.transformacao;
        glm::mat4 matriz = glm::translate(glm::mat4(1.0f), t.posicao);
        matriz = glm::rotate(matriz, glm::radians(t.rotacaoY), glm::vec3(0, 1, 0));
        matriz = glm::scale(matriz, glm::vec3(t.escala));
        desenhaModelo(*d.modelo, matriz);
    }
}

void desenhaPostes(){
    auto colocaPoste = [&](const glm::vec3 &posicao) {
        glm::mat4 matriz = glm::translate(glm::mat4(1.0f), posicao);
        matriz = glm::scale(matriz, glm::vec3(ESCALA_POSTES));
        desenhaModelo(poste, matriz);

        glm::vec3 posLampada = posicao + glm::vec3(0.0f, ALTURA_LAMPADA, 0.0f);
        if (desenhandoMinimapa) {
            desenhaLampada(posLampada, RAIO_MARCADOR_MINIMAPA);
        } else if (ehNoite) {
            desenhaLampada(posLampada, RAIO_LAMPADA);
        }
    };

    for (int z = POSTES_Z_INICIO; z <= POSTES_Z_FIM; z += PASSO_POSTES) {
        for (int lado = -1; lado <= 1; lado += 2) {
            colocaPoste(glm::vec3(lado * AFASTAMENTO_POSTES_X, 0.0f, z * 2.0f));
        }
    }

    // Poste atrás do deck, centralizado entre as faixas do comércio.
    if (QUANTIDADE_ESTRADAS_COMERCIO > 0) {
        const float centroZ = ESTRADA_COMERCIO_Z
            - (QUANTIDADE_ESTRADAS_COMERCIO - 1) * DISTANCIA_ENTRE_ESTRADAS_COMERCIO / 2.0f;
        colocaPoste(glm::vec3(POSTE_COMERCIO_X, 0.0f, centroZ));
    }
}

void desenhaDecksComercio() {
    if (QUANTIDADE_ESTRADAS_COMERCIO <= 0) return;

    const float ultimoZ = ESTRADA_COMERCIO_Z
        - (QUANTIDADE_ESTRADAS_COMERCIO - 1) * DISTANCIA_ENTRE_ESTRADAS_COMERCIO;
    // Cada peça mede uma unidade em Z antes da escala.
    const int quantidadeDecks = static_cast<int>(std::ceil(
        (ESTRADA_COMERCIO_Z - ultimoZ + ESPACAMENTO_ESTRADA) / ESCALA_DECK_COMERCIO));
    const float inicioZ = ESTRADA_COMERCIO_Z + ESPACAMENTO_ESTRADA / 2.0f
        - ESCALA_DECK_COMERCIO / 2.0f;
    for (int i = 0; i < quantidadeDecks; i++) {
        const float z = inicioZ - i * ESCALA_DECK_COMERCIO;
        glm::mat4 matriz = glm::translate(glm::mat4(1.0f),
            glm::vec3(DECK_COMERCIO_X, DECK_COMERCIO_Y, z));
        matriz = glm::scale(matriz, glm::vec3(ESCALA_DECK_COMERCIO));
        desenhaModelo(deck, matriz);
    }

    const float bordaNorte = ESTRADA_COMERCIO_Z + ESPACAMENTO_ESTRADA / 2.0f;
    const float bordaSul = inicioZ - (quantidadeDecks - 1) * ESCALA_DECK_COMERCIO
        - ESCALA_DECK_COMERCIO / 2.0f;
    auto coloca = [&](const Modelo &m, glm::vec3 posicao, float rotacao, float escala) {
        glm::mat4 matriz = glm::translate(glm::mat4(1.0f), posicao);
        matriz = glm::rotate(matriz, glm::radians(rotacao), glm::vec3(0, 1, 0));
        matriz = glm::scale(matriz, glm::vec3(escala));
        desenhaModelo(m, matriz);
    };

    // Banco central apoiado no deck, com o comprimento acompanhando Z.
    coloca(banco, glm::vec3(
        DECK_COMERCIO_X + 0.25f * ESCALA_DECK_COMERCIO,
        DECK_COMERCIO_Y + ALTURA_DECK_COMERCIO,
        (ESTRADA_COMERCIO_Z + ultimoZ) / 2.0f),
        0.0f, ESCALA_MOVEIS_COMERCIO);

    // Uma mesa em cada lateral da rua, com um banco de cada lado.
    for (int lado = -1; lado <= 1; lado += 2) {
        const float mesaZ = lado > 0
            ? bordaNorte + AFASTAMENTO_LAZER_COMERCIO
            : bordaSul - AFASTAMENTO_LAZER_COMERCIO;
        coloca(mesa, glm::vec3(LAZER_COMERCIO_X, 0.0f, mesaZ),
            90.0f, ESCALA_MOVEIS_COMERCIO);
        for (int bancoLado = -1; bancoLado <= 1; bancoLado += 2) {
            coloca(banco, glm::vec3(LAZER_COMERCIO_X, 0.0f,
                mesaZ + bancoLado * AFASTAMENTO_BANCOS_MESA),
                bancoLado > 0 ? 90.0f : -90.0f, ESCALA_MOVEIS_COMERCIO);
        }
    }
}

// A mesma composição é usada na câmera principal e no minimapa.
void desenhaVila()
{
    desenhaChao();
    desenhaEstrada();
    desenhaPinheiro();
    desenhaBarraca();
    desenhaCharacter();
    desenhaCasas();
    desenhaMoinho();
    desenhaFazenda();
    desenhaPostes();
    desenhaDecoracoes();
    desenhaDecksComercio();
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

        if (ehNoite) {
            glClearColor(0.02f, 0.03f, 0.08f, 1.0f);
        } else {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(Shader_programm);
        aplicaIluminacao();

        trataTeclado();

        glActiveTexture(GL_TEXTURE0);

        if (moinhoLigado){
            anguloPas += VELOCIDADE_PAS * Tempo_entre_frames;
        }
        

        glm::mat4 transformacao = glm::mat4(1.0f);
        transformacao = glm::translate(transformacao, glm::vec3(0.0f, -2.0f, 0.0f));
        // transformacao = glm::rotate(transformacao, (float)glfwGetTime() * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f));

        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

        glViewport(0, 0, FB_WIDTH, FB_HEIGHT);
        inicializaCamera();
        desenhaCorpoSeleste();
        desenhaVila();

        int tamanhoMinimapa = 180;
        int margem = 5;
        glViewport(FB_WIDTH - tamanhoMinimapa - margem, FB_HEIGHT - tamanhoMinimapa - margem, tamanhoMinimapa, tamanhoMinimapa);

        // Limpa cor e profundidade só no retângulo do minimapa, para a cena principal não interferir nele
        glEnable(GL_SCISSOR_TEST);
        glScissor(FB_WIDTH - tamanhoMinimapa - margem, FB_HEIGHT - tamanhoMinimapa - margem, tamanhoMinimapa, tamanhoMinimapa);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        inicializaCameraMinimapa();
        desenhandoMinimapa = true;
        desenhaVila();
        desenhandoMinimapa = false;

        glfwPollEvents();
        glfwSwapBuffers(Window);
    }

    const Modelo *modelos[] = {
        &parede, &paredePorta, &paredeJanela, &telhado, 
        &barracaVerde, &barracaVermelha,
        &maleA, &maleB, &maleF, &femaleB, &femaleC, &femaleF, 
        &moinhoBase, &moinhoMadeira, &moinhoTelhado, &moinhoPas,
        &poste, &cerca, &portao, &carrinho, &planta, &flores, &barril,
        &caixa, &pedras, &placa, &cogumelos, &mesa, &banco, &deck
    };
    for (const Modelo *modelo : modelos) {
        glDeleteBuffers(1, &modelo->vbo);
        glDeleteVertexArrays(1, &modelo->vao);
    }

    const GLuint buffers[] = {vboSol, vboChao, vboEstrada, vboPinheiro};
    const GLuint vaos[] = {vaoSol, Vao, VaoEstrada, VaoPinheiro};
    // Os modelos e os pinheiros compartilham estas texturas: apagar cada uma uma vez.
    const GLuint texturas[] = {texture1, texturaEstrada, texturaCharacter};
    glDeleteBuffers(4, buffers);
    glDeleteVertexArrays(4, vaos);
    glDeleteTextures(3, texturas);
    glDeleteProgram(Shader_programm);

    glfwDestroyWindow(Window);
    Window = nullptr;
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
