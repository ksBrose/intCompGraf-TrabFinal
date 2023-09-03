#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;
/**/
in vec4 cor_interpolada_pelo_rasterizador;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
//#define SPHERE 0
//#define BUNNY  1
#define PLANE  0
#define WALL 1
#define BONECO 2
#define DOOR 3
#define CHEST  4
#define CUBE 5
#define HEAD 6
#define MAOPE 7
#define TABLE 8
#define BONSAI1 9
#define BONSAI2 10
#define BONSAI3 11
#define BONSAI4 12

uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;
uniform sampler2D TextureImage5;
uniform sampler2D TextureImage6;
uniform sampler2D TextureImage7;
uniform sampler2D TextureImage8;
uniform sampler2D TextureImage9;
uniform sampler2D TextureImage10;
uniform sampler2D TextureImage11;
uniform sampler2D TextureImage12;
uniform sampler2D TextureImage13;
uniform sampler2D TextureImage14;
uniform sampler2D TextureImage15;
uniform sampler2D TextureImage16;


// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
uniform bool render_as_black;
void main()
{
/**/// Definimos a cor final de cada fragmento utilizando a cor interpolada
    // pelo rasterizador.
    color = cor_interpolada_pelo_rasterizador;


    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0.0,1.0,0.0,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;


    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2.0 * n * dot(n,l); // PREENCHA AQUI o vetor de reflexão especular ideal

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

        if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;
        Kd = texture(TextureImage0, vec2(U,V)).rgb;
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
     else   if ( object_id == WALL )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;
        Kd = texture(TextureImage1, vec2(U,V)).rgb;
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
     else if ( object_id == DOOR )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;
        Kd = texture(TextureImage2, vec2(U,V)).rgb;
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
    else if ( object_id == CHEST )
    {

        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        if( position_model.z == 0.5 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage3, vec2(U,V)).rgb;
        }
        if( position_model.z == -0.5 )
        {
            U = (position_model.x-maxx)/(minx-maxx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage4, vec2(U,V)).rgb;
        }
        if( position_model.x == -0.5 )
        {
            U = (position_model.z-maxz)/(minz-maxz);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage5, vec2(U,V)).rgb;
        }
        if( position_model.x == 0.5 )
        {
            U = (position_model.z-maxz)/(minz-maxz);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage5, vec2(U,V)).rgb;
        }
        if( position_model.y == 0 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage3, vec2(U,V)).rgb;
        }
        if( position_model.y == 1 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage3, vec2(U,V)).rgb;
        }
        //U = (position_model.z-minx)/(maxx-minx);
        //V = (position_model.y-miny)/(maxy-miny);
        //Kd = texture(TextureImage6, vec2(U,V)).rgb;
        Ks = vec3(0.1,0.1,0.1);     //Refletância especular no modelo RGB = (0.8, 0.8, 0.8)
        Ka = Kd/2;    //Refletância ambiente no modelo RGB = metade da refletância difusa
        q = 32.0;                   //Expoente especular de Phong = 32.0

    }
        else if ( object_id == CUBE )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        if( position_model.z == 0.5 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
        }
        if( position_model.z == -0.5 )
        {
            U = (position_model.x-maxx)/(minx-maxx);
            V = (position_model.y-miny)/(maxy-miny);
        }
        if( position_model.x == -0.5 )
        {
            U = (position_model.z-maxz)/(minz-maxz);
            V = (position_model.y-miny)/(maxy-miny);
        }
        if( position_model.x == 0.5 )
        {
            U = (position_model.z-maxz)/(minz-maxz);
            V = (position_model.y-miny)/(maxy-miny);
        }
        if( position_model.y == 0 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
        }
        if( position_model.y == 1 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
        }
        //U = (position_model.z-minx)/(maxx-minx);
        //V = (position_model.y-miny)/(maxy-miny);
        Kd = texture(TextureImage6, vec2(U,V)).rgb;
        Ks = vec3(0.1,0.1,0.1);     //Refletância especular no modelo RGB = (0.8, 0.8, 0.8)
        Ka = Kd/2;    //Refletância ambiente no modelo RGB = metade da refletância difusa
        q = 32.0;                   //Expoente especular de Phong = 32.0

    }
    else if ( object_id == HEAD )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        if( position_model.z == 0.5 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage7, vec2(U,V)).rgb;
        }
        if( position_model.z == -0.5 )
        {
            U = (position_model.x-maxx)/(minx-maxx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage9, vec2(U,V)).rgb;
        }
        if( position_model.x == -0.5 )
        {
            U = (position_model.z-maxz)/(minz-maxz);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage8, vec2(U,V)).rgb;
        }
        if( position_model.x == 0.5 )
        {
            U = (position_model.z-maxz)/(minz-maxz);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage8, vec2(U,V)).rgb;
        }
        if( position_model.y == 0 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.y-miny)/(maxy-miny);
            Kd = texture(TextureImage7, vec2(U,V)).rgb;
        }
        if( position_model.y == 1 )
        {
            U = (position_model.x-minx)/(maxx-minx);
            V = (position_model.z-minz)/(maxz-minz);
            Kd = texture(TextureImage10, vec2(U,V)).rgb;
        }
        //U = (position_model.z-minx)/(maxx-minx);
        //V = (position_model.y-miny)/(maxy-miny);
        //Kd = texture(TextureImage6, vec2(U,V)).rgb;
        Ks = vec3(0.1,0.1,0.1);     //Refletância especular no modelo RGB = (0.8, 0.8, 0.8)
        Ka = Kd/2;    //Refletância ambiente no modelo RGB = metade da refletância difusa
        q = 32.0;                   //Expoente especular de Phong = 32.0

    }
    else   if ( object_id == MAOPE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;
        Kd = texture(TextureImage11, vec2(U,V)).rgb;
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
    else   if ( object_id == TABLE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;
        Kd = texture(TextureImage12, vec2(U,V)).rgb;
        // PREENCHA AQUI
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
    else   if ( object_id == BONSAI1 )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage13, vec2(U,V)).rgb;
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
    else   if ( object_id == BONSAI2 )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage14, vec2(U,V)).rgb;
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
    else   if ( object_id == BONSAI3 )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage15, vec2(U,V)).rgb;
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
        else   if ( object_id == BONSAI4 )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage16, vec2(U,V)).rgb;
        // Propriedades espectrais do plano
        //Kd = vec3(0.2,0.2,0.2);     //Refletância difusa no modelo RGB = (0.2, 0.2, 0.2)
        Ks = vec3(0.3,0.3,0.3);     //Refletância especular no modelo RGB = (0.3, 0.3, 0.3)
        Ka = Kd/2;     //Refletância ambiente no modelo RGB = zero.
        q = 20.0;                   //Expoente especular de Phong = 20.0

    }
    else // Objeto desconhecido = preto
    {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }
    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0); // PREENCH AQUI o espectro da fonte de luz

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2); // PREENCHA AQUI o espectro da luz ambiente

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0, dot(n,l)); // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
    vec3 ambient_term = Ka * Ia; // PREENCHA AQUI o termo ambiente

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term  = Ks * I * pow(max(0,dot(r,v)),q); // PREENCH AQUI o termo especular de Phong

    // Meio caminho v e l
    vec4 h = normalize(l+v);

    // Termo especular Blinn-Phong
    vec3 blinn_phong_specular_term = Ks * I * pow(max(0.0,dot(n,h)), q);

    color.a = 1;

    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

    color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);

    if ( object_id == BONECO )
    {
     color = cor_interpolada_pelo_rasterizador;
    }
}

