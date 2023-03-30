#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTriangle() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int object_id;
uniform sampler2D sampler;

out vec2 texture_coords;
out vec4 position_world;
out vec4 normal;
out vec4 gouraud;

#define COW 4

void main()
{
    gl_Position = projection * view * model * model_coefficients;

    position_world = model * model_coefficients;

    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    texture_coords = texture_coefficients;
    // Utilizar gouraud se o objeto for a vaca
    if(object_id == COW){
        vec4 origin = vec4(0.0, 2.0, 1.0, 1.0);

        vec4 camera_position = inverse(view) * origin;

        vec4 p = position_world;
        vec4 n = normalize(normal);

        // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
        vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));

        // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - p);

        vec4 halfway = normalize(l + v);

        vec3 Kd; // Refletância difusa
        vec3 Ks; // Refletância especular
        vec3 Ka; // Refletância ambiente
        float q; // Expoente especular para o modelo de iluminação de Phong

        Kd = vec3(1.0,1.0,1.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.4,0.2,0.04);
        q = 1.0;

        // Espectro da fonte de iluminação
        vec3 I = vec3(1.0,1.0,1.0);

        // Espectro da luz ambiente
        vec3 Ia = vec3(1.0,1.0,1.0);

        // Termo difuso utilizando a lei dos cossenos de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        // Termo ambiente
        vec3 ambient_term = Ka * Ia;

        // Termo especular utilizando o modelo de iluminação de Phong
        vec3 phong_specular_term = Ks * I * pow(max(0, dot(n, halfway)), q);

        vec3 texture_color = texture(sampler, texture_coefficients).xyz;

        gouraud.rgb = (ambient_term + lambert_diffuse_term) * texture_color + phong_specular_term;
        gouraud.a = 1;
    }
}
