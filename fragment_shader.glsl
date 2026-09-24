#version 400

in vec2 TexCoord; // Coordenada UV recebida do Vertex Shader interpolada

// O "sampler2D" é um ponteiro escuro do OpenGL para a Textura na VRAM
uniform sampler2D texture1; 

out vec4 frag_colour;

void main () {
    // A função nativa texture() realiza a amostragem de cor (Filtering)
    // Ex: Se UV = (0.5, 0.5), pega o pixel exato no meio da imagem
    frag_colour = texture(texture1, TexCoord);
}
