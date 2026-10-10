# Vila Virtual em OpenGL

Projeto desenvolvido para a disciplina de Processamento Gráfico na Unisinos, com o objetivo de construir uma cena 3D interativa utilizando OpenGL moderno. A cena consiste em uma vila virtual composta por múltiplos objetos instanciados a partir de primitivas básicas, com câmera navegável, animações contínuas, texturas e minimapa.

> 📄 Este projeto segue a mesma estrutura e configuração do repositório base da disciplina. Para configurar o ambiente do zero (instalação de dependências, CMake, VS Code, compilador), consulte o guia oficial:
> **[ConfiguracaoOpenGL.md](https://github.com/guilhermechagaskurtz/OpenGLProject/blob/main/ConfiguracaoOpenGL.md)**

---

## Requisitos gráficos

O projeto utiliza **OpenGL 3.3 Core** e shaders **GLSL 330 Core**. Os arquivos da GLAD já estão incluídos em `include/glad/` e `Common/glad.c`. Para configurar o ambiente e instalar as dependências, siga o guia do repositório base linkado acima.

---

## 📂 Estrutura do projeto

```
📂 cenario3D/
├── 📂 include/         # Cabeçalhos da GLAD
├── 📂 Common/          # glad.c
├── 📂 src/             # Código-fonte principal
├── 📂 assets/          # Modelos 3D, texturas e shaders
├── 📂 build/           # Gerado pelo CMake (não versionado)
├── 📄 CMakeLists.txt
└── 📄 README.md
```

---

## Compilando e executando

Após configurar o ambiente conforme o guia do repositório base:

```bash
# Na raiz do projeto
cmake -S . -B build
cmake --build build
cd build
./main
```

Execute a partir de `build/`, pois os caminhos dos modelos, texturas e shaders são relativos a esse diretório.

---

## Controles

| Tecla | Ação |
|-------|------|
| `W` / `S` | Mover para frente / trás |
| `A` / `D` | Mover para os lados |
| `E` / `Q` | Subir / descer |
| `Mouse` | Rotacionar câmera |
| `ESC` | Fechar |

---

*Mais detalhes sobre os elementos da cena, texturas e funcionalidades serão adicionados conforme o desenvolvimento avança.*
