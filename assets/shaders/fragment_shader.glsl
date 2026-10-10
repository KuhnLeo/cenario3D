#version 330 core

in vec2 TexCoord; // Coordenada UV recebida do Vertex Shader interpolada
in vec3 Normal;

// O "sampler2D" é um ponteiro escuro do OpenGL para a Textura na VRAM
uniform sampler2D texture1; 

uniform vec3 direcaoLuz;
uniform vec3 corLuz;
uniform float intensidadeAmbiente;
uniform bool desenhaSol;

out vec4 frag_colour;

void main () {
    if (desenhaSol) {
        frag_colour = vec4(1.0, 0.85, 0.2, 1.0);
        return;
    }
    vec4 textura = texture(texture1, TexCoord);

    vec3 normal = normalize(Normal);

    // direcaoLuz representa o sentido em que os raios viajam.
    // Para iluminar, precisamos do vetor apontando para a luz.
    vec3 paraLuz = normalize(-direcaoLuz);

    float difusa = max(dot(normal, paraLuz), 0.0);
    vec3 iluminacao = (intensidadeAmbiente + difusa) * corLuz;

    frag_colour = vec4(textura.rgb * iluminacao, textura.a);
}
