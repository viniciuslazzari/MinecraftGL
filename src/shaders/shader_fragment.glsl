#version 330 core

in vec4 position_world;
in vec4 normal;
in vec2 texture_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D sampler;

out vec4 color;

void main()
{
    vec4 origin = vec4(0.0, 2.0, 1.0, 1.0);

    vec4 camera_position = inverse(view) * origin;

    vec4 p = position_world;
    vec4 n = normalize(normal);
    vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));
    vec4 v = normalize(camera_position - p);
    vec4 r = -l + 2 * n * dot(n, l);

    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    Kd = vec3(0.8,0.4,0.08);
    Ks = vec3(0.0,0.0,0.0);
    Ka = vec3(0.4,0.2,0.04);
    q = 1.0;

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term = Ks * I * pow(max(0, dot(r, v)), q); // PREENCHA AQUI o termo especular de Phong

    vec3 texture_color = texture(sampler, texture_coords).xyz;

    color.rgb = (ambient_term + lambert_diffuse_term) * texture_color + phong_specular_term;

    color.a = 1.0;
} 

