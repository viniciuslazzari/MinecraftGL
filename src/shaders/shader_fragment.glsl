#version 330 core

in vec4 position_world;
in vec4 normal;
in vec2 texture_coords;
in vec3 gouraud;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D sampler;

uniform int object_id;
#define COW 4


out vec4 color;

void main()
{
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

    Kd = vec3(0.5,0.4,0.08);
    Ks = vec3(0.0,0.0,0.0);
    Ka = vec3(0.4,0.2,0.04);
    q = 2.0;

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);

    // Espectro da luz ambiente
    vec3 ia = vec3(0.5,0.5,0.5);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term = Ks * I * pow(max(0, dot(n, halfway)), q);

    vec3 texture_color = texture(sampler, texture_coords).xyz;

    // Se objeto for a vaca, utilizar Gouraud
    if(object_id == COW){
        color.rgb = gouraud * texture_color;
    }
    else{
        color.rgb = (ambient_term + lambert_diffuse_term) * texture_color + phong_specular_term;
    }

    color.a = 1.0;
} 

