#version 330 core
layout(location = 0) in vec3 vertex_posicao;
layout(location = 1) in vec2 vertex_textura;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 matriz, view, proj;

out vec2 TexCoord;
out vec3 Normal;

void main () {
    TexCoord = vertex_textura;

    mat3 matrizNormal = transpose(inverse(mat3(matriz)));
    Normal = matrizNormal * vertex_normal;

    gl_Position = proj * view * matriz * vec4(vertex_posicao, 1.0);
}
