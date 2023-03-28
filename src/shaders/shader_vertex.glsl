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

out vec2 texture_coords;
out vec4 position_world;
out vec4 normal;
out vec3 gouraud;

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
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;

        vec4 v = normalize(camera_position - position_world); // Sentido da camera
        vec4 l = normalize(vec4(1.0,1.0,0.5,0.0)); // Sentido da luz
        vec4 n = normalize(normal); // Normal do fragmento
        vec3 I = vec3(1.0,1.0,1.0); // Fonte de luiz
        vec4 h = normalize(v+l); // half vector

        vec3 Iamb = vec3(0.1,0.1,0.1); // Luz ambiente    
        vec3 Idiff = I*max(0,dot(n,l)); // Termo difuso
        vec3 Ispec = I*pow(max(0,dot(n,h)),2.2); // Termo specular

        gouraud = Iamb + Idiff + Ispec;
        gouraud = pow(gouraud, vec3(1.0,1.0,1.0)/2.2);
    }
}
