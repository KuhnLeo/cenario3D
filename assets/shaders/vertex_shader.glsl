#version 400
layout(location = 0) in vec3 vertex_posicao;
layout(location = 1) in vec2 vertex_textura;

uniform mat4 matriz, view, proj;
out vec2 TexCoord;

void main () {
    TexCoord = vertex_textura;
    gl_Position = proj * view * matriz * vec4(vertex_posicao, 1.0);
    //gl_Position = vec4(vertex_posicao, 1.0);
}
