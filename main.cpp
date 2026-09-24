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
using namespace std;

GLFWwindow *Window = nullptr;
GLuint Shader_programm = 0;
GLuint Vao = 0;
int NVertices;
unsigned int texture1;
int WIDTH = 1000;
int HEIGHT = 800;

float Tempo_entre_frames = 0.0f; // variavel utilizada para movimentar a camera

// Variáveis referentes a câmera virtual e sua projeção
float Cam_speed = 2.5f;                             // velocidade da camera aumentada um pouco para navegação livre
glm::vec3 Cam_pos = glm::vec3(0.0f, 0.0f, 2.0f);    // posicao inicial da câmera
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

void carregaTextura(string filePATH)
{
    // AVISO: Imagens são lidas de Cima para Baixo pelo STB.
    // O OpenGL espera o Y(0) embaixo. É VITAL mandar o STB inverter o eixo Y!
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePATH.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        // Verifica dinamicamente quantos canais a imagem tem para não bugar a memória
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // 1. Gera e faz o bind do ID da textura
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // 2. Configura os parâmetros (Wrapping / Filtering) no objeto ativo
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 3. Envia os bytes da RAM para a VRAM usando a variável 'format' correta
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Falha ao ler textura" << std::endl;
    }
    // 4. Libera a RAM, a GPU já assumiu a carga
    stbi_image_free(data);
}

void redimensionaCallback(GLFWwindow *window, int w, int h)
{
    WIDTH = w;
    HEIGHT = h;
    glViewport(0, 0, WIDTH, HEIGHT);
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
}

void inicializaObjetos()
{
    carregaTextura("../assets/Modelos3D/Suzanne.png");
    Vao = loadSimpleOBJ("../assets/Modelos3D/Suzanne.obj", NVertices);
}

void inicializaShaders()
{
    // 1. Lê o código dos arquivos externos
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
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
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
    float zfar = 100.0f;
    float fov = glm::radians(67.0f);
    float aspecto = (float)WIDTH / (float)HEIGHT;

    glm::mat4 projecao = glm::perspective(fov, aspecto, znear, zfar);

    GLint transformLoc = glGetUniformLocation(Shader_programm, "proj");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projecao));
}

void especificaMatrizVisualizacaoMinimapa() {
    glm::vec3 posicaoTopo = glm::vec3(Cam_pos.x, 10.0f, Cam_pos.z);
    glm::vec3 alvo = glm::vec3(Cam_pos.x, 0.0f, Cam_pos.z);
    glm::vec3 upMinimapa = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::mat4 visualizacao = glm::lookAt(posicaoTopo, alvo, upMinimapa);

    GLint transformLoc = glGetUniformLocation(Shader_programm, "view");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(visualizacao));
}

void especificaMatrizProjecaoMinimapa() {
    float tamanho = 3.0f;

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

void inicializaRenderizacao()
{
    double tempo_anterior = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(Window))
    {
        double tempo_frame_atual = glfwGetTime();
        Tempo_entre_frames = (float)(tempo_frame_atual - tempo_anterior);
        tempo_anterior = tempo_frame_atual;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(Shader_programm);

        trataTeclado();

        glActiveTexture(GL_TEXTURE0);           // Ativa Gaveta 0
        glBindTexture(GL_TEXTURE_2D, texture1); // Pluga a Textura 1 nela
        glBindVertexArray(Vao);

        glm::mat4 transformacao = glm::mat4(1.0f);
        transformacao = glm::rotate(transformacao, (float)glfwGetTime() * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f));

        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformacao));

        glViewport(0, 0, WIDTH, HEIGHT);
        inicializaCamera();
        glDrawArrays(GL_TRIANGLES, 0, NVertices);

        int tamanhoMinimapa = 200;
        int margem = 10;
        glViewport(WIDTH - tamanhoMinimapa - margem, HEIGHT - tamanhoMinimapa - margem, tamanhoMinimapa, tamanhoMinimapa);
        
        inicializaCameraMinimapa();
        glDrawArrays(GL_TRIANGLES, 0, NVertices);


        glfwPollEvents();
        glfwSwapBuffers(Window);
    }

    glfwTerminate();
}

int main()
{
    inicializaOpenGL();

    inicializaObjetos();
    inicializaShaders();
    inicializaRenderizacao();

    return 0;
}
