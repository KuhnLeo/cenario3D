#version 330 core

in vec2 TexCoord; // Coordenada UV recebida do Vertex Shader interpolada
in vec3 Normal;
in vec3 FragPos;

// O "sampler2D" é um ponteiro escuro do OpenGL para a Textura na VRAM
uniform sampler2D texture1; 

uniform vec3 direcaoLuz;
uniform vec3 corLuz;
uniform float intensidadeAmbiente;
uniform bool desenhaSol;
uniform bool ehNoite;
const int MAX_LUZES = 64;
uniform int quantidadeLuzes;
uniform vec3 posicoesLuzes[MAX_LUZES];
uniform bool luzesPostesAtivas;

out vec4 frag_colour;

void main () {
    if (desenhaSol) {
        frag_colour = ehNoite
            ? vec4(0.772, 0.772, 0.772, 1.0)
            : vec4(1.0, 0.85, 0.2, 1.0);
        return;
    }

    vec4 textura = texture(texture1, TexCoord);

    vec3 normal = normalize(Normal);

    // direcaoLuz representa o sentido em que os raios viajam.
    // Para iluminar, precisamos do vetor apontando para a luz.
    vec3 paraLuz = normalize(-direcaoLuz);

    float difusa = max(dot(normal, paraLuz), 0.0);
    vec3 iluminacao = (intensidadeAmbiente + difusa) * corLuz;

    if (luzesPostesAtivas) {
        for (int i = 0; i < quantidadeLuzes; i++) {
            vec3 deslocamento = posicoesLuzes[i] - FragPos;
            float distancia = length(deslocamento);
            vec3 direcao = deslocamento / max(distancia, 0.001);
            float difusaPoste = max(dot(normal, direcao), 0.0);
            float atenuacao = 1.0 / (1.0 + 0.09 * distancia
                + 0.032 * distancia * distancia);
            iluminacao += vec3(1.0, 0.85, 0.2) * difusaPoste * atenuacao * 0.2;
        }
    }

    frag_colour = vec4(textura.rgb * iluminacao, textura.a);
}
