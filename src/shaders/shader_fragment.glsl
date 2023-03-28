#version 330 core

in vec4 position_world;
in vec4 normal;
in vec2 texture_coords;
in vec4 gouraud_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D sampler;
uniform int gouraud;

out vec4 color;

vec4 origin = vec4(0.0, 2.0, 1.0, 1.0);

// Vetor que define o sentido da fonte de luz em relação ao ponto atual.
vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));

// Espectro da fonte de iluminação
vec3 I = vec3(1.0,1.0,1.0);

// Espectro da luz ambiente
vec3 Ia = vec3(0.5,0.5,0.5);

vec3 Kd = vec3(0.5,0.4,0.08);; // Refletância difusa
vec3 Ks = vec3(0.0,0.0,0.0);; // Refletância especular
vec3 Ka = vec3(0.4,0.2,0.04);; // Refletância ambiente
float q = 2.0;; // Expoente especular para o modelo de iluminação de Phong

// Termo ambiente
vec3 ambient_term = Ka * Ia;

void main(){
    if (gouraud == 1){
        color = gouraud_color;
    } else {
        vec4 camera_position = inverse(view) * origin;

        vec4 n = normalize(normal);

        // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - position_world);

        vec4 halfway = normalize(l + v);

        // Termo difuso utilizando a lei dos cossenos de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        // Termo especular utilizando o modelo de iluminação de Phong
        vec3 phong_specular_term = Ks * I * pow(max(0, dot(n, halfway)), q);

        vec3 texture_color = texture(sampler, texture_coords).xyz;

        color.rgb = (ambient_term + lambert_diffuse_term) * texture_color + phong_specular_term;

        color.a = 1.0;
    }
} 