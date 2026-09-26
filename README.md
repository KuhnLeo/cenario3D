# Vila Virtual em OpenGL

Projeto desenvolvido para a disciplina de Processamento Gráfico na Unisinos, com o objetivo de construir uma cena 3D interativa utilizando OpenGL moderno. A cena consiste em uma vila virtual composta por múltiplos objetos instanciados a partir de primitivas básicas, com câmera navegável, animações contínuas, texturas e minimapa.

> 📄 Este projeto segue a mesma estrutura e configuração do repositório base da disciplina. Para configurar o ambiente do zero (instalação de dependências, CMake, VS Code, compilador), consulte o guia oficial:
> **[ConfiguracaoOpenGL.md](https://github.com/guilhermechagaskurtz/OpenGLProject/blob/main/ConfiguracaoOpenGL.md)**

---

## ⚠️ Pré-requisito: GLAD

É necessário baixar a GLAD manualmente antes de compilar.

1. Acesse [https://glad.dav1d.de/](https://glad.dav1d.de/) e gere com as configurações:
   - **API:** OpenGL
   - **Version:** 3.3+
   - **Profile:** Core
   - **Language:** C/C++

2. Extraia e copie os arquivos para os diretórios corretos:

```
glad.h        →  include/glad/
khrplatform.h →  include/glad/KHR/
glad.c        →  common/
```

🚨 Sem esses arquivos a compilação falhará.

---

## 📂 Estrutura do projeto

```
📂 cenario3D/
├── 📂 include/         # Cabeçalhos de terceiros (GLAD, stb_image)
├── 📂 common/          # glad.c
├── 📂 src/             # Código-fonte principal
├── 📂 assets/          # Modelos 3D, texturas e shaders
├── 📂 build/           # Gerado pelo CMake (não versionado)
├── 📄 CMakeLists.txt
└── 📄 README.md
```

---

## Compilando e executando

```bash
# Na raiz do projeto
mkdir build && cd build
cmake ..
make
./main
```

Se precisar limpar o cache do CMake:

```bash
cd build
rm -rf CMakeCache.txt CMakeFiles/
cmake ..
make
```

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