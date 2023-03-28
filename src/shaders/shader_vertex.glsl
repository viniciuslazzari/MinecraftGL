#version 330 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D sampler;
uniform int gouraud;

out vec2 texture_coords;
out vec4 position_world;
out vec4 normal;
out vec4 gouraud_color;

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
    position_world = model * model_coefficients;
    texture_coords = texture_coefficients;

    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    gl_Position = projection * view * position_world;
    gouraud_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    if (gouraud == 1){
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

        gouraud_color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

        gouraud_color.a = 1.0;
    }
}