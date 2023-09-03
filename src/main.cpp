//     Universidade Federal do Rio Grande do Sul
//             Instituto de Inform�tica
//       Departamento de Inform�tica Aplicada
//
//    INF01047 Fundamentos de Computa��o Gr�fica
//               Prof. Eduardo Gastal
//
//                   LABORAT�RIO 5
//

// Arquivos "headers" padr�es de C podem ser inclu�dos em um
// programa C++, sendo necess�rio somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo s�o espec�ficos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Cria��o de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Cria��o de janelas do sistema operacional

// Headers da biblioteca GLM: cria��o de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"

#include "moves.h"

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

// Estrutura que representa um modelo geom�trico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor l� o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, ent�o setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diret�rio dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                    filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};


// Declara��o de fun��es utilizadas para pilha de matrizes de modelagem.
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4& M);

// Declara��o de v�rias fun��es utilizadas em main().  Essas est�o definidas
// logo ap�s a defini��o de main() neste arquivo.
/**Boneco add**/
void DrawCube(GLint render_as_black_uniform); // Desenha um cubo
GLuint BuildTriangles(); // Constr�i tri�ngulos para renderiza��o
GLuint BuildTrianglesNeg(); // Constr�i tri�ngulos para renderiza��o
/**Boneco add**/

void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constr�i representa��o de um ObjModel como malha de tri�ngulos para renderiza��o
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso n�o existam.
void LoadShadersFromFiles(); // Carrega os shaders de v�rtice e fragmento, criando um programa de GPU
void LoadTextureImage(const char* filename); // Fun��o que carrega imagens de textura
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Fun��o utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Fun��o para debugging

// Declara��o de fun��es auxiliares para renderizar texto dentro da janela
// OpenGL. Estas fun��es est�o definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Fun��es abaixo renderizam como texto na janela OpenGL algumas matrizes e
// outras informa��es do programa. Definidas ap�s main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);
void TextRendering_ShowBodyAngles(GLFWwindow* window,Teta angulo);

// Fun��es callback para comunica��o com o sistema operacional e intera��o do
// usu�rio. Veja mais coment�rios nas defini��es das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// Definimos uma estrutura que armazenar� dados necess�rios para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    std::string  name;        // Nome do objeto
    size_t       first_index; // �ndice do primeiro v�rtice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // N�mero de �ndices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasteriza��o (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde est�o armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;
};

// Abaixo definimos vari�veis globais utilizadas em v�rias fun��es do c�digo.

// A cena virtual � uma lista de objetos nomeados, guardados em um dicion�rio
// (map).  Veja dentro da fun��o BuildTrianglesAndAddToVirtualScene() como que s�o inclu�dos
// objetos dentro da vari�vel g_VirtualScene, e veja na fun��o main() como
// estes s�o acessados.
std::map<std::string, SceneObject> g_VirtualScene;

/**Boneco add**/
//std::map<const char*, SceneBoneco> g_VirtualBoneco;
/**Boneco add**/

// Pilha que guardar� as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Raz�o de propor��o da janela (largura/altura). Veja fun��o FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// �ngulos de Euler que controlam a rota��o de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usu�rio est� com o bot�o esquerdo do mouse
// pressionado no momento atual. Veja fun��o MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // An�logo para bot�o direito do mouse
bool g_MiddleMouseButtonPressed = false; // An�logo para bot�o do meio do mouse

// Vari�veis que definem a c�mera em coordenadas esf�ricas, controladas pelo
// usu�rio atrav�s do mouse (veja fun��o CursorPosCallback()). A posi��o
// efetiva da c�mera � calculada dentro da fun��o main(), dentro do loop de
// renderiza��o.
float g_CameraTheta = 0.0f; // �ngulo no plano ZX em rela��o ao eixo Z
float g_CameraPhi = 0.0f;   // �ngulo em rela��o ao eixo Y
float g_CameraDistance = 3.5f; // Dist�ncia da c�mera para a origem

/**free camera add**/
float g_Theta = -3.0 * 3.141592f /4 ;
float g_Phi = 3.141592f /8 ;
/**free camera add**/

// Vari�veis que controlam rota��o do antebra�o
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;

// Vari�veis que controlam transla��o do torso
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;

// Vari�vel que controla o tipo de proje��o utilizada: perspectiva ou ortogr�fica.
bool g_UsePerspectiveProjection = true;

// Vari�vel que controla se o texto informativo ser� mostrado na tela.
bool g_ShowInfoText = true;

// Vari�veis que definem um programa de GPU (shaders). Veja fun��o LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;

// N�mero de texturas carregadas pela fun��o LoadTextureImage()
GLuint g_NumLoadedTextures = 0;
/**camera add**/
bool tecla_W_pressionada = false;
bool tecla_A_pressionada = false;
bool tecla_S_pressionada = false;
bool tecla_D_pressionada = false;
//float delta_t;


        glm::vec4 camera_position_c  = glm::vec4(2.5f,2.5f,2.5f,1.0f); // Ponto "c", centro da c�mera
        glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a c�mera (look-at) estar� sempre olhando
        glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a c�mera est� virada
/**camera add**/

/**Anima��o add**/
// Anima��o baseada no tempo
float dt = 0;
float dt1 = 0;
float dt0 = 0;
float timer = 0;
bool startmove = false;
bool reset = false;
float tetay =0;
glm::vec3 bezier_cintura;
/**Anima��o add**/
bool camera_type = false;

int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impress�o de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL vers�o 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto �, utilizaremos somente as
    // fun��es modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com t�tulo "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "Trab Final - Kelvin Brose / Gustavo Picoli", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a fun��o de callback que ser� chamada sempre que o usu�rio
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os bot�es do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);


        // Definimos a fun��o de callback que ser� chamada sempre que a janela for
    // redimensionada, por consequ�ncia alterando o tamanho do "framebuffer"
    // (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 600); // For�amos a chamada do callback acima, para definir g_ScreenRatio.

    // Indicamos que as chamadas OpenGL dever�o renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas fun��es definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a fun��o de callback que ser� chamada sempre que a janela for
    // redimensionada, por consequ�ncia alterando o tamanho do "framebuffer"
    // (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // For�amos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informa��es sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de v�rtices e de fragmentos que ser�o utilizados
    // para renderiza��o. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

/**Boneco add**/
    // Constru�mos a representa��o de um tri�ngulo
    GLuint vertex_array_object_id = BuildTriangles();
    // Constru�mos a representa��o de um tri�ngulo
    GLuint vertex_array_object_id_neg = BuildTrianglesNeg();

    Teta angulo;
    inicializa_angulos(&angulo, &bezier_cintura);
/**Boneco add**/

    // Carregamos duas imagens para serem utilizadas como textura
    LoadTextureImage("../../data/wood_floor.jpg");      // TextureImage0
    LoadTextureImage("../../data/wood_floor_specular.jpg");// TextureImage1
    LoadTextureImage("../../data/wood_floor_ambient.jpg"); // TextureImage2
    LoadTextureImage("../../data/wood_floor_normal.jpg"); // TextureImage3
    LoadTextureImage("../../data/wood-wall-texture-3.jpg"); // TextureImage4

    // Constru�mos a representa��o de objetos geom�tricos atrav�s de malhas de tri�ngulos
/*    ObjModel spheremodel("../../data/sphere.obj");
    ComputeNormals(&spheremodel);
    BuildTrianglesAndAddToVirtualScene(&spheremodel);

    ObjModel bunnymodel("../../data/bunny.obj");
    ComputeNormals(&bunnymodel);
    BuildTrianglesAndAddToVirtualScene(&bunnymodel);
*/
    ObjModel planemodel("../../data/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o c�digo para renderiza��o de texto.
    TextRendering_Init();

/**Boneco add**/
    // Buscamos o endere�o das vari�veis definidas dentro do Vertex Shader.
    // Utilizaremos estas vari�veis para enviar dados para a placa de v�deo
    // (GPU)! Veja arquivo "shader_vertex.glsl".
    GLint model_uniform           = glGetUniformLocation(g_GpuProgramID, "model"); // Vari�vel da matriz "model"
    GLint view_uniform            = glGetUniformLocation(g_GpuProgramID, "view"); // Vari�vel da matriz "view" em shader_vertex.glsl
    GLint projection_uniform      = glGetUniformLocation(g_GpuProgramID, "projection"); // Vari�vel da matriz "projection" em shader_vertex.glsl
    GLint render_as_black_uniform = glGetUniformLocation(g_GpuProgramID, "render_as_black"); // Vari�vel booleana em shader_vertex.glsl
/**Boneco add**/

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

/**free camera add**/
    // Vari�veis auxiliares utilizadas para chamada � fun��o
    // TextRendering_ShowModelViewProjection(), armazenando matrizes 4x4.
    glm::mat4 the_projection;
    glm::mat4 the_model;
    glm::mat4 the_view;

    float speed = 1.5f; // Velocidade da c�mera
    float prev_time = (float)glfwGetTime();
    float delta_t;
/**free camera add**/

    // Ficamos em um loop infinito, renderizando, at� que o usu�rio feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as opera��es de renderiza��o
        glm::mat4 view;
        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor �
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto �:
        // Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e tamb�m resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de v�rtice e fragmentos).
        glUseProgram(g_GpuProgramID);
/**Anima��o add**/
        // Anima��o baseada no tempo
        dt1 = glfwGetTime();
        dt = dt1 - dt0;
        if(reset)
        {
            inicializa_angulos(&angulo, &bezier_cintura);
            reset=false;
        }
        if(startmove)
        move_one(&angulo, dt, &timer, &bezier_cintura);
/**Anima��o add**/
/**Boneco add**/
        // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
        // v�rtices apontados pelo VAO criado pela fun��o BuildTriangles(). Veja
        // coment�rios detalhados dentro da defini��o de BuildTriangles().
        glBindVertexArray(vertex_array_object_id);
/**Boneco add**/

        // Computamos a posi��o da c�mera utilizando coordenadas esf�ricas.  As
        // vari�veis g_CameraDistance, g_CameraPhi, e g_CameraTheta s�o
        // controladas pelo mouse do usu�rio. Veja as fun��es CursorPosCallback()
        // e ScrollCallback().
        //if(camera_type){
        //float r = g_CameraDistance;
        //float y = r*sin(g_CameraPhi);
        //float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        //float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);
        //}
//**free camera add**/
        //float r = g_CameraDistance;
        if(!camera_type)
        {
        float y = sin(g_Phi);
        float z = cos(g_Phi)*cos(g_Theta);
        float x = cos(g_Phi)*sin(g_Theta);
        camera_view_vector  = glm::vec4(x,-y,z,0.0f);
        }

/**free camera add**/
        if(camera_type)
        {
        float r = g_CameraDistance;
        float y = r*sin(g_Phi);
        float z = r*cos(g_Phi)*cos(g_Theta);
        float x = r*cos(g_Phi)*sin(g_Theta);
        // Abaixo definimos as var�veis que efetivamente definem a c�mera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::vec4 camera_position_c  = glm::vec4(-x,y,-z,1.0f); // Ponto "c", centro da c�mera
        glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a c�mera (look-at) estar� sempre olhando
        glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a c�mera est� virada
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "c�u" (eito Y global)

        view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        }
/**free camera add**/
        // Abaixo definimos as var�veis que efetivamente definem a c�mera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        //glm::vec4 camera_position_c  = glm::vec4(1.0f,1.0f,1.0f,1.0f); // Ponto "c", centro da c�mera
        //glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a c�mera (look-at) estar� sempre olhando
       // glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a c�mera est� virada
        glm::vec4 w;
        glm::vec4 u;
        if(!camera_type)
        {
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "c�u" (eito Y global)
         w = -camera_view_vector;
         u = crossproduct(camera_up_vector,-camera_view_vector);

         view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        }
/**free camera add**/

        // Computamos a matriz "View" utilizando os par�metros da c�mera para
        // definir o sistema de coordenadas da c�mera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
//        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        // Agora computamos a matriz de Proje��o.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da c�mera, os planos near e far
        // est�o no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posi��o do "near plane"
        float farplane  = -100.0f; // Posi��o do "far plane"

        if (g_UsePerspectiveProjection)
        {
            // Proje��o Perspectiva.
            // Para defini��o do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
            float field_of_view = 3.141592 / 3.0f;
            projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        }
        else
        {
            // Proje��o Ortogr�fica.
            // Para defini��o dos valores l, r, b, t ("left", "right", "bottom", "top"),
            // PARA PROJE��O ORTOGR�FICA veja slides 219-224 do documento Aula_09_Projecoes.pdf.
            // Para simular um "zoom" ortogr�fico, computamos o valor de "t"
            // utilizando a vari�vel g_CameraDistance.
            float t = 1.5f*g_CameraDistance/2.5f;
            float b = -t;
            float r = t*g_ScreenRatio;
            float l = -r;
            projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
        }

        //glm::mat4 model = Matrix_Identity(); // Transforma��o identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de v�deo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas s�o
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

/**Boneco add**/
        #define PLANE  0
        #define WALL  1
        #define BONECO 2
        // ##### TAREFAS DO LABORAT�RIO 3

        // Cada c�pia do cubo possui uma matriz de modelagem independente,
        // j� que cada c�pia estar� em uma posi��o (rota��o, escala, ...)
        // diferente em rela��o ao espa�o global (World Coordinates). Veja
        // slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        //
        // Entretanto, neste laborat�rio as matrizes de modelagem dos cubos
        // ser�o constru�das de maneira hier�rquica, tal que opera��es em
        // alguns objetos influenciem outros objetos. Por exemplo: ao
        // transladar o torso, a cabe�a deve se movimentar junto.
        // Veja slides 243-273 do documento Aula_08_Sistemas_de_Coordenadas.pdf
        //
        glm::mat4 model = Matrix_Identity(); // Transforma��o inicial = identidade.

        // Transla��o inicial do torso
        model = model* Matrix_Scale(0.5f, 0.5f, 0.5f) * Matrix_Translate(g_TorsoPositionX - 0.0f, g_TorsoPositionY + 1.5f,g_TorsoPositionX- 2.0f);
        // Guardamos matriz model atual na pilha
/*        PushMatrix(model);
            // Atualizamos a matriz model (multiplica��o � direita) para fazer um escalamento do torso
            model = model * Matrix_Scale(0.8f, 1.0f, 0.2f);
            // Enviamos a matriz "model" para a placa de v�deo (GPU). Veja o
            // arquivo "shader_vertex.glsl", onde esta � efetivamente
            // aplicada em todos os pontos.
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            // Desenhamos um cubo. Esta renderiza��o ir� executar o Vertex
            // Shader definido no arquivo "shader_vertex.glsl", e o mesmo ir�
            // utilizar as matrizes "model", "view" e "projection" definidas
            // acima e j� enviadas para a placa de v�deo (GPU).
            DrawCube(render_as_black_uniform); // #### TORSO
        PopMatrix(model);
 */     glBindVertexArray(vertex_array_object_id_neg);//muda ponto de referencia de constru��o de cubos
            PushMatrix(model);                                  // pilha = I
                model = model * Matrix_Translate(bezier_cintura.x, bezier_cintura.y, bezier_cintura.z); // Posi��o do tor�o (pulos)
                PushMatrix(model);                              // pilha = I * Tcentro
                    model = model // rota��o do tor�o para guia ficar na cintura (pocisionamento)
                        * Matrix_Rotate_Z(0)  // TERCEIRO rota��o Z de Euler
                        * Matrix_Rotate_Y(0)  // SEGUNDO rota��o Y de Euler
                        * Matrix_Rotate_X(0); // PRIMEIRO rota��o X de Euler
                    PushMatrix(model);                          // pilha = I * Tc * Rposicao
                        model = model // rota��o do tor�o para guia ficar na cintura (cumprimento)
                            * Matrix_Rotate_Z(0)  // TERCEIRO rota��o Z de Euler
                            * Matrix_Rotate_Y((angulo.centro.y)*M_PI/180)  // (+)ombro esquerdo para tras
                            * Matrix_Rotate_X((angulo.centro.x)*M_PI/180); // rota��o x (frontal) (+)inclina para frentre (-)inclina para traz
                        PushMatrix(model);                      // pilha = I * Tc * Rp * Rinclinacao
                            model = model * Matrix_Scale(0.8f, 1.0f, 0.2f); //escalamento do tronco
                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                            glUniform1i(g_object_id_uniform, BONECO);
                            DrawCube(render_as_black_uniform); // #### TRONCO // Desenhamos o tronco
                        PopMatrix(model);                       // pilha = I * Tc * Rp * Rinclina��o
                //PopMatrix(model);
            //PopMatrix(model);
        // Tiramos da pilha a matriz model guardada anteriormente
        //PopMatrix(model);
                /*cabe�a*/PushMatrix(model);                    // pilha = I * Tc * Rp * Rinclina��o
                            model = model * Matrix_Translate(0.0f, 1.05f, 0.0f); // transla��o da cabe�a para topo do tronco
                            PushMatrix(model);                  // pilha = I * Tc * Rp * Ri *Tcabeca
                                model = model // Rota��o da cabe�a (posicionamento)
                                      * Matrix_Rotate_Z(0*M_PI)  // 0 para rota��o apartir do pesco�o
                                      * Matrix_Rotate_Y(0)  // rota��o olhar para horizontal (+)direita (-)esquerda
                                      * Matrix_Rotate_X(0); // rota��o olhar para vertrical (+)cima (-)baixo
                                PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Tc * Rpescoco
                                    model = model //  rota��o da cabe�a (anima��o)
                                        * Matrix_Rotate_Z(0*M_PI/180)  // TERCEIRO rota��o Z de Euler
                                        * Matrix_Rotate_Y(0)  // SEGUNDO rota��o Y de Euler
                                        * Matrix_Rotate_X(0*g_AngleX); // PRIMEIRO rota��o X de Euler
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tc * Rp * Rcanimacao
                                        model = model * Matrix_Scale(0.4f, 0.4f, 0.2f); // Atualizamos matriz model (multiplica��o � direita) com um escalamento da cabe�a
                                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                        DrawCube(render_as_black_uniform); // #### CABE�A // Desenhamos CABE�A
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tc * Rp * Rca
                                PopMatrix(model);               // pilha = I * Tc * Rp * Ri * Tc * Rp
                            PopMatrix(model);                   // pilha = I * Tc * Rp * Ri * Tc
                        // Tiramos da pilha a matriz model guardada anteriormente
                        PopMatrix(model);                       // pilha = I * Tc * Rp * Ri
                        glBindVertexArray(vertex_array_object_id); //muda ponto de referencia de constru��o de cubos
                /*R_arm*/PushMatrix(model);                     // pilha = I * Tc * Rp * Ri
                            model = model * Matrix_Translate(-0.55f, 1.0f, 0.0f); // transla��o do bra�o direito
                            PushMatrix(model);                  // pilha = I * Tc * Rp * Ri * Trightarm
                                model = model // rota��o bra�o (posicionamento)
                                      * Matrix_Rotate_Z(0);  // gira 90 graus baixos na linha do corpo
                                      //* Matrix_Rotate_Y(g_AngleY)  // SEGUNDO rota��o Y de Euler
                                      //* Matrix_Rotate_X(g_AngleX); // PRIMEIRO rota��o X de Euler
                                PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Tra * Rrightarmposi��o
                    /*ombro*/   model = model // rota��o do bra�o direito (anima��o)
                                      * Matrix_Rotate_Y((angulo.ombrod.y)*M_PI/180)  // (-)supina�ao(palma cima) (+)prona��o (palma chao)
                                      * Matrix_Rotate_Z((-angulo.ombrod.z)*M_PI/180)  // (-)afasta cotovelo do corpo

                                      * Matrix_Rotate_X((-angulo.ombrod.x)*M_PI/180); // (+) cotovelo para traz (-) cotovelo para frente
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rrightarmanimacao
                                        model = model * Matrix_Scale(0.2f, 0.6f, 0.2f); // escalamento do bra�o direito
                                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                        DrawCube(render_as_black_uniform); // #### BRA�O DIREITO // Desenhamos o bra�o direito
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa
                                        model = model * Matrix_Translate(0.0f, -0.65f, 0.0f); // transla��o do antebra�o direito
                    /*rot cotovelo*/    model = model // rota��o do ante bra�o direito (anima��o)
                                            //* Matrix_Rotate_Y((-angulo.cotovd.y)*M_PI/180)  // SEGUNDO rota��o Z de Euler
                                            * Matrix_Rotate_Z((angulo.cotovd.z)*M_PI/180)
                                            * Matrix_Rotate_X((angulo.cotovd.x)*M_PI/180)* Matrix_Rotate_Y((-angulo.cotovd.y)*M_PI/180); // (-) fexao cotovelo at� ( 0 ) esticado
                                        PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa * (Tabd)Rantebracodiranimacao
                                            model = model * Matrix_Scale(0.17f, 0.55f, 0.2f); // escalamento do antebra�o direito
                                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                            DrawCube(render_as_black_uniform); // #### ANTEBRA�O DIREITO // Desenhamos o antebra�o direito
                                        PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa * (Tabd)Rabda
                                /*M�o*/ PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa * (Tabd)Rabda
                                            model = model * Matrix_Translate(0.0f, -0.6f, 0.0f); // transla��o da mao direita
                                            //model = model // Atualizamos matriz model (multiplica��o � direita) com a rota��o da mao direita
                                                // * Matrix_Rotate_Z(g_ForearmAngleZ)  // SEGUNDO rota��o Z de Euler
                                                // * Matrix_Rotate_X(g_ForearmAngleX); // PRIMEIRO rota��o X de Euler
                                            PushMatrix(model);  // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa * (Tabd)Rabda * Tmaodireita
                                                model = model * Matrix_Scale(0.12f, 0.2f, 0.2f); // Atualizamos matriz model (multiplica��o � direita) com um escalamento da mao direita
                                                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                                DrawCube(render_as_black_uniform); // #### MAO DIREITA // Desenhamos o mao direita
                                            PopMatrix(model);   // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa * (Tabd)Rabda * Tmd
                                        PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa * (Tabd)Rabda
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tra * Rrap * Rraa
                                PopMatrix(model);               // pilha = I * Tc * Rp * Ri * Tra * Rrap
                            PopMatrix(model);                   // pilha = I * Tc * Rp * Ri * Tra
                        PopMatrix(model);                       // pilha = I * Tc * Rp * Ri

                        // Neste ponto a matriz model recuperada � a matriz inicial (transla��o do torso)
                /*L_arm*/
                        PushMatrix(model);                      // pilha = I * Tc * Rp * Ri
                            model = model * Matrix_Translate(0.55f, 1.0f, 0.0f); // transla��o para o bra�o esuerdo
                            PushMatrix(model);                  // pilha = I * Tc * Rp * Ri * Tleftarm
                                model = model // rota��o bra�o (posicionamento)
                                * Matrix_Rotate_Z(0);  // gira 180 graus baixos na linha do corpo
                                //* Matrix_Rotate_Y(g_AngleY)  // SEGUNDO rota��o Y de Euler
                                //* Matrix_Rotate_X(g_AngleX); // PRIMEIRO rota��o X de Euler
                                PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Tla * Rleftarmposi��o
                    /*ombro*/   model = model // rota��o do bra�o esquedro (anima��o)
                                      * Matrix_Rotate_Y((-angulo.ombroe.y)*M_PI/180)  // (+)supina�ao (-)prona��o
                                      * Matrix_Rotate_Z((angulo.ombroe.z)*M_PI/180) // (+)afasta cotovelo do corpo

                                      * Matrix_Rotate_X((-angulo.ombroe.x)*M_PI/180); // (+) cotovelo para traz (-) cotovelo para frente
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rleftarmanimacao
                                        model = model * Matrix_Scale(0.2f, 0.6f, 0.2f); // escalamento do bra�o esquerdo
                                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                        DrawCube(render_as_black_uniform); // #### BRA�O ESQUERDO // Desenhamos o bra�o esquerdo
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa
                                        model = model * Matrix_Translate(0.0f, -0.65f, 0.0f); // transla��o do antebra�o esquerdo
                    /*rot cotovelo*/    model = model // rota��o do antebra�o esquerdo (anima��o)
                                            //* Matrix_Rotate_Y((angulo.cotove.y)*M_PI/180)  // SEGUNDO rota��o Z de Euler
                                            * Matrix_Rotate_Z((angulo.cotove.z)*M_PI/180)
                                            * Matrix_Rotate_X((angulo.cotove.x)*M_PI/180)* Matrix_Rotate_Y((angulo.cotove.y)*M_PI/180); // (-) fexao cotovelo at� ( 0 ) esticado
                                        PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa * (Tabe)Rantebracoesqanimacao
                                            model = model * Matrix_Scale(0.17f, 0.55f, 0.2f); // escalamento do antebra�o esquerdo
                                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                            DrawCube(render_as_black_uniform); // #### ANTEBRA�O ESQUERDO // Desenhamos o antebra�o esquerdo
                                        PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa * (Tabe)Rabea
                                /*M�o*/ PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa * (Tabe)Rabea
                                            model = model * Matrix_Translate(0.0f, -0.6f, 0.0f); // transla��o da mao esquerda
                                            //model = model // Atualizamos matriz model (multiplica��o � direita) com a rota��o da mao esquerda
                                            // * Matrix_Rotate_Z(g_ForearmAngleZ)  // SEGUNDO rota��o Z de Euler
                                            // * Matrix_Rotate_X(g_ForearmAngleX); // PRIMEIRO rota��o X de Euler
                                            PushMatrix(model);  // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa * (Tabe)Rabea * Tmaoesquerda
                                                model = model * Matrix_Scale(0.12f, 0.2f, 0.2f); // escalamento da mao esquerda
                                                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                                DrawCube(render_as_black_uniform); // #### MAO ESQUERDA // Desenhamos a mao esquerda
                                            PopMatrix(model);   // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa * (Tabe)Rabea * Tme
                                        PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa * (Tabe)Rabea
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tla * Rlap * Rlaa
                                PopMatrix(model);               // pilha = I * Tc * Rp * Ri * Tla * Rlap
                            PopMatrix(model);                   // pilha = I * Tc * Rp * Ri * Tla
                        PopMatrix(model);                       // pilha = I * Tc * Rp * Ri

                        // Neste ponto a matriz model recuperada � a matriz inicial (transla��o do torso)
                /*Perna direita*/
                        PushMatrix(model);                      // pilha = I * Tc * Rp * Ri
                            model = model * Matrix_Translate(-0.2f, -0.05f, 0.0f); // transla��o para a perna direita
                            PushMatrix(model);                  // pilha = I * Tc * Rp * Ri * Trightleg
                                model = model // rota��o da perna direita
                                * Matrix_Rotate_Z(0);  // gira 180 graus
                                //* Matrix_Rotate_Y(g_AngleY)  // SEGUNDO rota��o Y de Euler
                                //* Matrix_Rotate_X(g_AngleX); // PRIMEIRO rota��o X de Euler
                                PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Trl * Rrightlegposi��o
                                    model = model // rota��o da perna direita
                                        * Matrix_Rotate_Z((-angulo.quadrild.z)*M_PI/180)  // (-)afasta joelho do centro
                                        * Matrix_Rotate_Y((-angulo.quadrild.y)*M_PI/180)  // (-)rot externa (+)rot interna
                                        * Matrix_Rotate_X((-angulo.quadrild.x)*M_PI/180); // (+) joelho para traz (-) joelho para frente
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrightleganimacao
                                        model = model * Matrix_Scale(0.3f, 0.6f, 0.2f); // escalamento da perna direita
                                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                        DrawCube(render_as_black_uniform); // #### PERNA DIREITA // Desenhamos a perna direita
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla
                                    PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla
                                        model = model * Matrix_Translate(0.0f, -0.65f, 0.0f); //transla��o para a perna direita
                    /*rot joelho*/      model = model // rota��o do perna direita (anima��o)
                                            * Matrix_Rotate_Z(0)  // SEGUNDO rota��o Z de Euler
                                            * Matrix_Rotate_X((-angulo.joelhod.x)*M_PI/180); // (+) fexao joelho at� ( 0 ) esticado
                                        PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla * Trl2
                                            model = model * Matrix_Scale(0.25f, 0.6f, 0.2f); // escalamento da perna direita
                                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                            DrawCube(render_as_black_uniform); // #### PERNA DIREITA // Desenhamos a perna direita
                                        PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla * Trl2
                                /*p�*/  PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla * Trl2
                                            model = model * Matrix_Translate(0.0f, -0.65f, 0.125f); // transla��o do pe direito
                                            model = model // rota��o do pe direito
                                                    * Matrix_Rotate_Y((-angulo.ped.y)*M_PI/180)  // (-)rot externa (+)rot interna
                                                    * Matrix_Rotate_Z((angulo.ped.z)*M_PI/180)  // (-)evers�o do p� (+) invers�o do p�(Yoko)

                                                    * Matrix_Rotate_X((angulo.ped.x)*M_PI/180); // (+) flexao planar(peito do p�) (-) dorsoflex�o (0)= pe 90 graus
                                            PushMatrix(model);  // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla * Trl2 * (Tpedireito)Rpedireitoanima��o
                                                model = model * Matrix_Scale(0.2f, 0.1f, 0.5f); // escalamento do pe direito
                                                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                                DrawCube(render_as_black_uniform); // #### PE DIREITO // Desenhamos o pe direito
                                            PopMatrix(model);   // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla * Trl2 * (Tpd)Rpda
                                        PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla * Trl2
                                    PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Trl * Rrlp * Rrla
                                PopMatrix(model);               // pilha = I * Tc * Rp * Ri * Trl * Rrlp
                            PopMatrix(model);                   // pilha = I * Tc * Rp * Ri * Trl
                        PopMatrix(model);                       // pilha = I * Tc * Rp * Ri

                        // Neste ponto a matriz model recuperada � a matriz inicial (transla��o do torso)
                /*Perna esquerda*/
                        PushMatrix(model);                      // pilha = I * Tc * Rp * Ri
                            model = model * Matrix_Translate(0.2f, -0.05f, 0.0f); // transla��o para a perna esquerda
                            PushMatrix(model);                  // pilha = I * Tc * Rp * Ri * Tleftleg
                                model = model // rota��o da perna esquerda
                                    * Matrix_Rotate_Z(0);  // gira 180 graus
                                    //* Matrix_Rotate_Y(3.1415)  // SEGUNDO rota��o Y de Euler
                                    //* Matrix_Rotate_X(3.1415); // PRIMEIRO rota��o X de Euler
                                PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Tll * Rleftlegposi��o
                                    model = model  // rota��o da perna esquerda
                                        * Matrix_Rotate_Z((angulo.quadrile.z)*M_PI/180)  // (+)afasta joelho do centro
                                        * Matrix_Rotate_Y((angulo.quadrile.y)*M_PI/180)  // (-)rot interna (+)rot externa
                                        * Matrix_Rotate_X((-angulo.quadrile.x)*M_PI/180); // (+) joelho para traz (-) joelho para frente
                                    PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rleftleganimacao
                                        model = model * Matrix_Scale(0.3f, 0.6f, 0.2f); // escalamento da perna esquerda
                                        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                        DrawCube(render_as_black_uniform); // #### PERNA ESQUERDA // Desenhamos a perna esquerda
                                    PopMatrix(model);               // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla
                                    PushMatrix(model);              // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla
                                        model = model * Matrix_Translate(0.0f, -0.65f, 0.0f); //transla��o para a perna esquerda
                    /*rot joelho*/      model = model // rota��o do perna esquerda (anima��o)
                                            * Matrix_Rotate_Z(0)  // SEGUNDO rota��o Z de Euler
                                            * Matrix_Rotate_X((-angulo.joelhoe.x)*M_PI/180); // (+) fexao joelho at� ( 0 ) esticado
                                        PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla * Tll2
                                            model = model * Matrix_Scale(0.25f, 0.6f, 0.2f); // escalamento da perna esqurda
                                            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                            DrawCube(render_as_black_uniform); // #### PERNA ESQUERDA // Desenhamos a perna esquerda
                                        PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla * Tll2
                                /*p�*/  PushMatrix(model);          // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla * Tll2
                                            model = model * Matrix_Translate(0.0f, -0.65f, 0.125f); // transla��o do pe esquerdo
                                            model = model // rota��o do pe esquerdo
                                                    * Matrix_Rotate_Y((angulo.pee.y)*M_PI/180)  // (-)rot interna (+)rot externa
                                                    * Matrix_Rotate_Z((-angulo.pee.z)*M_PI/180)  // (-)invers�o do p�(Yoko) (+) evers�o do p�

                                                    * Matrix_Rotate_X((angulo.pee.x)*M_PI/180); // (+) flexao planar(peito do p�) (-) dorsoflex�o (0)= pe 90 graus
                                            PushMatrix(model);      // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla * Tll2 * (Tpeesquerdo)Rpeesquerdoanima��o
                                                model = model * Matrix_Scale(0.2f, 0.1f, 0.5f); // escalamento do pe esquerdo
                                                glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
                                                DrawCube(render_as_black_uniform); // #### PE ESQUERDO // Desenhamos o pe esquerdo
                                            PopMatrix(model);       // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla * Tll2 * (Tpe)Rpea
                                        PopMatrix(model);           // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla * Tll2
                                    PopMatrix(model);               // pilha = I * Tc * Rp * Ri * Tll * Rllp * Rlla
                                PopMatrix(model);                   // pilha = I * Tc * Rp * Ri * Tll * Rllp
                            PopMatrix(model);                       // pilha = I * Tc * Rp * Ri * Tll
                        PopMatrix(model);                           // pilha = I * Tc * Rp * Ri
                    PopMatrix(model);                           // pilha = I * Tc * Rp
                PopMatrix(model);                           // pilha = I * Tc
            PopMatrix(model);                           // pilha = I
        // Neste ponto a matriz model recuperada � a matriz inicial (entre os p�s)
        PopMatrix(model); //origem
        /*//desenha cubo de teste
        model = model * Matrix_Translate(0.0f, 0.0f, 0.0f); // Posi��o
        model = model // rota��o
            * Matrix_Rotate_Z(0)  // TERCEIRO rota��o Z de Euler
            * Matrix_Rotate_Y(0)  // SEGUNDO rota��o Y de Euler
            * Matrix_Rotate_X(0); // PRIMEIRO rota��o X de Eule
        model = model * Matrix_Scale(1.0f, 1.0f, 1.0f); //escalamento
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model)); // Enviamos matriz model atual para a GPU
        DrawCube(render_as_black_uniform); // #### CUBO // Desenhamos o cubo
        */


        // Agora queremos desenhar os eixos XYZ de coordenadas GLOBAIS.
        // Para tanto, colocamos a matriz de modelagem igual � identidade.
        // Veja slides 2-14 e 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        model = Matrix_Identity();

        // Enviamos a nova matriz "model" para a placa de v�deo (GPU). Veja o
        // arquivo "shader_vertex.glsl".
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        // Pedimos para OpenGL desenhar linhas com largura de 10 pixels.
        glLineWidth(10.0f);

        // Informamos para a placa de v�deo (GPU) que a vari�vel booleana
        // "render_as_black" deve ser colocada como "false". Veja o arquivo
        // "shader_vertex.glsl".
        glUniform1i(render_as_black_uniform, false);

        // Pedimos para a GPU rasterizar os v�rtices dos eixos XYZ
        // apontados pelo VAO como linhas. Veja a defini��o de
        // g_VirtualScene["axes"] dentro da fun��o BuildTriangles(), e veja
        // a documenta��o da fun��o glDrawElements() em
        // http://docs.gl/gl3/glDrawElements.
        glDrawElements(
            g_VirtualScene["axes"].rendering_mode,
            g_VirtualScene["axes"].num_indices,
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["axes"].first_index
        );

        // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindVertexArray(0);


/**Boneco add**/


//        #define SPHERE 0
//        #define BUNNY  1
        #define PLANE  0
        #define WALL  1
        #define BONECO 2
/*
        // Desenhamos o modelo da esfera
        model = Matrix_Translate(-1.0f,0.0f,0.0f)
              * Matrix_Rotate_Z(0.6f)
              * Matrix_Rotate_X(0.2f)
              * Matrix_Rotate_Y(g_AngleY + (float)glfwGetTime() * 0.1f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, SPHERE);
        DrawVirtualObject("the_sphere");

        // Desenhamos o modelo do coelho
        model = Matrix_Translate(1.0f,0.0f,0.0f)
              * Matrix_Rotate_X(g_AngleX + (float)glfwGetTime() * 0.1f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, BUNNY);
        DrawVirtualObject("the_bunny");
*/
        // Desenhamos o plano do ch�o
        model = Matrix_Translate(0.0f, 0.0f, 0.0f)
                * Matrix_Scale(5.0f,1.0f,5.0f);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("the_plane");

/** Paredes e Teto Add **/
        // Desenhamos o plano da parede de fundo
        model = Matrix_Translate(0.0f, 2.0f, -5.0f)
                * Matrix_Scale(5.0f, 2.0f, 1.0f)
                //* Matrix_Rotate_X(M_PI_2)
                * Matrix_Rotate_X(M_PI_2);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        // Desenhamos o plano da parede da frente
        model = Matrix_Translate(0.0f, 2.0f, 5.0f)
                * Matrix_Scale(5.0f, 2.0f, 1.0f)
                * Matrix_Rotate_X(M_PI_2)
                //* Matrix_Rotate_Y(M_PI_2/2)
                * Matrix_Rotate_Z(M_PI);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        // Desenhamos o plano da parede da direita
        model = Matrix_Translate(5.0f, 2.0f, 0.0f)
                * Matrix_Scale(1.0f, 2.0f, 5.0f)
                * Matrix_Rotate_X(M_PI_2)
                * Matrix_Rotate_Z(M_PI_2);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");


        // Desenhamos o plano da parede da esquerda
        model = Matrix_Translate(-5.0f, 2.0f, 0.0f)
                * Matrix_Scale(1.0f, 2.0f, 5.0f)
                * Matrix_Rotate_X(M_PI_2)
                * Matrix_Rotate_Z(3 * M_PI_2);
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, WALL);
        DrawVirtualObject("the_plane");

        // Desenhamos o plano do teto
        model = Matrix_Translate(0.0f, 4.0f, 0.0f)
                * Matrix_Scale(5.0f,1.0f,5.0f)
                * Matrix_Rotate_X(M_PI);
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, PLANE);
        DrawVirtualObject("the_plane");


/** Paredes e Teto Add **/


        TextRendering_ShowBodyAngles(window,angulo);
        // Imprimimos na tela os �ngulos de Euler que controlam a rota��o do
        // terceiro cubo.
        TextRendering_ShowEulerAngles(window);

        // Imprimimos na informa��o sobre a matriz de proje��o sendo utilizada.
        TextRendering_ShowProjection(window);

        // Imprimimos na tela informa��o sobre o n�mero de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

/**free camera add**/
        // Atualiza delta de tempo
        float current_time = (float)glfwGetTime();
        delta_t = current_time - prev_time;
        prev_time = current_time;
 // Realiza movimenta��o de objetos
        if (tecla_D_pressionada)
            // Movimenta c�mera para direita
            camera_position_c += u * speed * delta_t;
        if (tecla_A_pressionada)
            // Movimenta c�mera para direita
            camera_position_c += -u * speed * delta_t;
        if (tecla_W_pressionada)
            // Movimenta c�mera para direita
            camera_position_c += -w * speed * delta_t;
        if (tecla_S_pressionada)
            // Movimenta c�mera para direita
            camera_position_c += w * speed * delta_t;
/**free camera add**/

        // O framebuffer onde OpenGL executa as opera��es de renderiza��o n�o
        // � o mesmo que est� sendo mostrado para o usu�rio, caso contr�rio
        // seria poss�vel ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usu�rio
        // tudo que foi renderizado pelas fun��es acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma intera��o do
        // usu�rio (teclado, mouse, ...). Caso positivo, as fun��es de callback
        // definidas anteriormente usando glfwSet*Callback() ser�o chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
/**anima��o add**/
        // Atualiza o dt0
        dt0 = dt1;
/**anima��o add**/
    } //fim while

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Fun��o que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Par�metros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// Fun��o que desenha um objeto armazenado em g_VirtualScene. Veja defini��o
// dos objetos na fun��o BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // v�rtices apontados pelo VAO criado pela fun��o BuildTrianglesAndAddToVirtualScene(). Veja
    // coment�rios detalhados dentro da defini��o de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    // Setamos as vari�veis "bbox_min" e "bbox_max" do fragment shader
    // com os par�metros da axis-aligned bounding box (AABB) do modelo.
    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Pedimos para a GPU rasterizar os v�rtices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a defini��o de
    // g_VirtualScene[""] dentro da fun��o BuildTrianglesAndAddToVirtualScene(), e veja
    // a documenta��o da fun��o glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Fun��o que carrega os shaders de v�rtices e de fragmentos que ser�o
// utilizados para renderiza��o. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" est�o fixados, sendo que assumimos a exist�ncia
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endere�o das vari�veis definidas dentro do Vertex Shader.
    // Utilizaremos estas vari�veis para enviar dados para a placa de v�deo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Vari�vel da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Vari�vel da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Vari�vel da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Vari�vel "object_id" em shader_fragment.glsl
    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");

    // Vari�veis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage3"), 3);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage4"), 4);



    glUseProgram(0);
}

// Fun��o que pega a matriz M e guarda a mesma no topo da pilha
void PushMatrix(glm::mat4 M)
{
    g_MatrixStack.push(M);
}

// Fun��o que remove a matriz atualmente no topo da pilha e armazena a mesma na vari�vel M
void PopMatrix(glm::mat4& M)
{
    if ( g_MatrixStack.empty() )
    {
        M = Matrix_Identity();
    }
    else
    {
        M = g_MatrixStack.top();
        g_MatrixStack.pop();
    }
}

/**Boneco add**/
// Fun��o que desenha um cubo com arestas em preto, definido dentro da fun��o BuildTriangles().
void DrawCube(GLint render_as_black_uniform)
{
    // Informamos para a placa de v�deo (GPU) que a vari�vel booleana
    // "render_as_black" deve ser colocada como "false". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(render_as_black_uniform, false);

    // Pedimos para a GPU rasterizar os v�rtices do cubo apontados pelo
    // VAO como tri�ngulos, formando as faces do cubo. Esta
    // renderiza��o ir� executar o Vertex Shader definido no arquivo
    // "shader_vertex.glsl", e o mesmo ir� utilizar as matrizes
    // "model", "view" e "projection" definidas acima e j� enviadas
    // para a placa de v�deo (GPU).
    //
    // Veja a defini��o de g_VirtualScene["cube_faces"] dentro da
    // fun��o BuildTriangles(), e veja a documenta��o da fun��o
    // glDrawElements() em http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["cube_faces"].rendering_mode, // Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf
        g_VirtualScene["cube_faces"].num_indices,    //
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["cube_faces"].first_index
    );

    // Pedimos para OpenGL desenhar linhas com largura de 4 pixels.
    glLineWidth(4.0f);

    // Pedimos para a GPU rasterizar os v�rtices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a defini��o de
    // g_VirtualScene["axes"] dentro da fun��o BuildTriangles(), e veja
    // a documenta��o da fun��o glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    //
    // Importante: estes eixos ser�o desenhamos com a matriz "model"
    // definida acima, e portanto sofrer�o as mesmas transforma��es
    // geom�tricas que o cubo. Isto �, estes eixos estar�o
    // representando o sistema de coordenadas do modelo (e n�o o global)!
    glDrawElements(
        g_VirtualScene["axes"].rendering_mode,
        g_VirtualScene["axes"].num_indices,
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["axes"].first_index
    );

    // Informamos para a placa de v�deo (GPU) que a vari�vel booleana
    // "render_as_black" deve ser colocada como "true". Veja o arquivo
    // "shader_vertex.glsl".
    glUniform1i(render_as_black_uniform, true);

    // Pedimos para a GPU rasterizar os v�rtices do cubo apontados pelo
    // VAO como linhas, formando as arestas pretas do cubo. Veja a
    // defini��o de g_VirtualScene["cube_edges"] dentro da fun��o
    // BuildTriangles(), e veja a documenta��o da fun��o
    // glDrawElements() em http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene["cube_edges"].rendering_mode,
        g_VirtualScene["cube_edges"].num_indices,
        GL_UNSIGNED_INT,
        (void*)g_VirtualScene["cube_edges"].first_index
    );
}

// Constr�i tri�ngulos para futura renderiza��o
GLuint BuildTriangles()
{
    // Primeiro, definimos os atributos de cada v�rtice.

    // A posi��o de cada v�rtice � definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geom�trico. Note o uso de coordenadas
    // homog�neas.  Veja as seguintes refer�ncias:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat model_coefficients[] = {
    // V�rtices de um cubo
    //    X      Y     Z     W
        -0.5f,  0.0f,  0.5f, 1.0f, // posi��o do v�rtice 0
        -0.5f, -1.0f,  0.5f, 1.0f, // posi��o do v�rtice 1
         0.5f, -1.0f,  0.5f, 1.0f, // posi��o do v�rtice 2
         0.5f,  0.0f,  0.5f, 1.0f, // posi��o do v�rtice 3
        -0.5f,  0.0f, -0.5f, 1.0f, // posi��o do v�rtice 4
        -0.5f, -1.0f, -0.5f, 1.0f, // posi��o do v�rtice 5
         0.5f, -1.0f, -0.5f, 1.0f, // posi��o do v�rtice 6
         0.5f,  0.0f, -0.5f, 1.0f, // posi��o do v�rtice 7
    // V�rtices para desenhar o eixo X
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 8
         1.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 9
    // V�rtices para desenhar o eixo Y
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 10
         0.0f,  1.0f,  0.0f, 1.0f, // posi��o do v�rtice 11
    // V�rtices para desenhar o eixo Z
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 12
         0.0f,  0.0f,  1.0f, 1.0f, // posi��o do v�rtice 13
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO �
    // um buffer de mem�ria que ir� conter os valores de um certo atributo de
    // um conjunto de v�rtices; por exemplo: posi��o, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos v�rios VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posi��o.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // cont�m a defini��o de v�rios atributos de um certo conjunto de v�rtices;
    // isto �, um VAO ir� conter ponteiros para v�rios VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // est� contido na vari�vel "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID est� contido na
    // vari�vel VBO_model_coefficients_id ser� modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer � de fato um VBO, e
    // ir� conter atributos de v�rtices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos mem�ria para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um n�mero de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" d� uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que n�o pretendemos alterar tais dados (s�o est�ticos: "STATIC"), e
    // tamb�m dizemos que tais dados ser�o utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos ent�o informar um �ndice de "local" ("location"), o qual ser�
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Tamb�m, informamos a dimens�o (n�mero de
    // coeficientes) destes atributos. Como em nosso caso s�o pontos em coordenadas
    // homog�neas, temos quatro coeficientes por v�rtice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados est�o em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta fun��o tamb�m informa que o VBO "ligado" acima em glBindBuffer()
    // est� dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com �ndice de local
    // definido acima, na vari�vel "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada v�rtice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor � definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto �: Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
    GLfloat color_coefficients[] = {
    // Cores dos v�rtices do cubo
    //  R     G     B     A
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 0
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 1
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 2
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 3
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 4
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 5
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 6
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 7
    // Cores para desenhar o eixo X
        1.0f, 0.0f, 0.0f, 1.0f, // cor do v�rtice 8
        1.0f, 0.0f, 0.0f, 1.0f, // cor do v�rtice 9
    // Cores para desenhar o eixo Y
        0.0f, 1.0f, 0.0f, 1.0f, // cor do v�rtice 10
        0.0f, 1.0f, 0.0f, 1.0f, // cor do v�rtice 11
    // Cores para desenhar o eixo Z
        0.0f, 0.0f, 1.0f, 1.0f, // cor do v�rtice 12
        0.0f, 0.0f, 1.0f, 1.0f, // cor do v�rtice 13
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 3; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos ent�o definir pol�gonos utilizando os v�rtices do array
    // model_coefficients.
    //
    // Para refer�ncia sobre os modos de renderiza��o, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLuint indices[] = {
    // Definimos os �ndices dos v�rtices que definem as FACES de um cubo
    // atrav�s de 12 tri�ngulos que ser�o desenhados com o modo de renderiza��o
    // GL_TRIANGLES.
        0, 1, 2, // tri�ngulo 1
        7, 6, 5, // tri�ngulo 2
        3, 2, 6, // tri�ngulo 3
        4, 0, 3, // tri�ngulo 4
        4, 5, 1, // tri�ngulo 5
        1, 5, 6, // tri�ngulo 6
        0, 2, 3, // tri�ngulo 7
        7, 5, 4, // tri�ngulo 8
        3, 6, 7, // tri�ngulo 9
        4, 3, 7, // tri�ngulo 10
        4, 1, 0, // tri�ngulo 11
        1, 6, 2, // tri�ngulo 12
    // Definimos os �ndices dos v�rtices que definem as ARESTAS de um cubo
    // atrav�s de 12 linhas que ser�o desenhadas com o modo de renderiza��o
    // GL_LINES.
        0, 1, // linha 1
        1, 2, // linha 2
        2, 3, // linha 3
        3, 0, // linha 4
        0, 4, // linha 5
        4, 7, // linha 6
        7, 6, // linha 7
        6, 2, // linha 8
        6, 5, // linha 9
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
    // Definimos os �ndices dos v�rtices que definem as linhas dos eixos X, Y,
    // Z, que ser�o desenhados com o modo GL_LINES.
        8 , 9 , // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere �s faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = 0; // Primeiro �ndice est� em indices[0]
    cube_faces.num_indices    = 36;       // �ltimo �ndice est� em indices[35]; total de 36 �ndices.
    cube_faces.rendering_mode = GL_TRIANGLES; // �ndices correspondem ao tipo de rasteriza��o GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_faces"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere �s arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (36*sizeof(GLuint)); // Primeiro �ndice est� em indices[36]
    cube_edges.num_indices    = 24; // �ltimo �ndice est� em indices[59]; total de 24 �ndices.
    cube_edges.rendering_mode = GL_LINES; // �ndices correspondem ao tipo de rasteriza��o GL_LINES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (60*sizeof(GLuint)); // Primeiro �ndice est� em indices[60]
    axes.num_indices    = 6; // �ltimo �ndice est� em indices[65]; total de 6 �ndices.
    axes.rendering_mode = GL_LINES; // �ndices correspondem ao tipo de rasteriza��o GL_LINES.
    g_VirtualScene["axes"] = axes;

    // Criamos um buffer OpenGL para armazenar os �ndices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora � GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos mem�ria para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // N�O fa�a a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de �ndices (GL_ELEMENT_ARRAY_BUFFER) n�o pode ser "desligado",
    // caso contr�rio o VAO ir� perder a informa��o sobre os �ndices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso � tudo que ser� necess�rio para renderizar
    // os tri�ngulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}

// Constr�i tri�ngulos para futura renderiza��o
GLuint BuildTrianglesNeg()
{
    // Primeiro, definimos os atributos de cada v�rtice.

    // A posi��o de cada v�rtice � definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geom�trico. Note o uso de coordenadas
    // homog�neas.  Veja as seguintes refer�ncias:
    //
    //  - slides 35-48 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //  - slides 184-190 do documento Aula_08_Sistemas_de_Coordenadas.pdf;
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLfloat model_coefficients[] = {
    // V�rtices de um cubo
    //    X      Y     Z     W
        -0.5f,  0.0f,  0.5f, 1.0f, // posi��o do v�rtice 0
        -0.5f, 1.0f,  0.5f, 1.0f, // posi��o do v�rtice 1
         0.5f, 1.0f,  0.5f, 1.0f, // posi��o do v�rtice 2
         0.5f,  0.0f,  0.5f, 1.0f, // posi��o do v�rtice 3
        -0.5f,  0.0f, -0.5f, 1.0f, // posi��o do v�rtice 4
        -0.5f, 1.0f, -0.5f, 1.0f, // posi��o do v�rtice 5
         0.5f, 1.0f, -0.5f, 1.0f, // posi��o do v�rtice 6
         0.5f,  0.0f, -0.5f, 1.0f, // posi��o do v�rtice 7
    // V�rtices para desenhar o eixo X
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 8
         1.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 9
    // V�rtices para desenhar o eixo Y
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 10
         0.0f,  1.0f,  0.0f, 1.0f, // posi��o do v�rtice 11
    // V�rtices para desenhar o eixo Z
    //    X      Y     Z     W
         0.0f,  0.0f,  0.0f, 1.0f, // posi��o do v�rtice 12
         0.0f,  0.0f,  1.0f, 1.0f, // posi��o do v�rtice 13
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO �
    // um buffer de mem�ria que ir� conter os valores de um certo atributo de
    // um conjunto de v�rtices; por exemplo: posi��o, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos v�rios VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posi��o.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // cont�m a defini��o de v�rios atributos de um certo conjunto de v�rtices;
    // isto �, um VAO ir� conter ponteiros para v�rios VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // est� contido na vari�vel "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID est� contido na
    // vari�vel VBO_model_coefficients_id ser� modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer � de fato um VBO, e
    // ir� conter atributos de v�rtices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos mem�ria para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um n�mero de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" d� uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que n�o pretendemos alterar tais dados (s�o est�ticos: "STATIC"), e
    // tamb�m dizemos que tais dados ser�o utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos ent�o informar um �ndice de "local" ("location"), o qual ser�
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Tamb�m, informamos a dimens�o (n�mero de
    // coeficientes) destes atributos. Como em nosso caso s�o pontos em coordenadas
    // homog�neas, temos quatro coeficientes por v�rtice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados est�o em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta fun��o tamb�m informa que o VBO "ligado" acima em glBindBuffer()
    // est� dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com �ndice de local
    // definido acima, na vari�vel "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada v�rtice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    // Tal cor � definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto �: Vermelho, Verde, Azul, Alpha (valor de transpar�ncia).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Ilumina��o.
    GLfloat color_coefficients[] = {
    // Cores dos v�rtices do cubo
    //  R     G     B     A
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 0
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 1
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 2
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 3
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 4
        1.0f, 0.5f, 0.0f, 1.0f, // cor do v�rtice 5
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 6
        0.0f, 0.5f, 1.0f, 1.0f, // cor do v�rtice 7
    // Cores para desenhar o eixo X
        1.0f, 0.0f, 0.0f, 1.0f, // cor do v�rtice 8
        1.0f, 0.0f, 0.0f, 1.0f, // cor do v�rtice 9
    // Cores para desenhar o eixo Y
        0.0f, 1.0f, 0.0f, 1.0f, // cor do v�rtice 10
        0.0f, 1.0f, 0.0f, 1.0f, // cor do v�rtice 11
    // Cores para desenhar o eixo Z
        0.0f, 0.0f, 1.0f, 1.0f, // cor do v�rtice 12
        0.0f, 0.0f, 1.0f, 1.0f, // cor do v�rtice 13
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 3; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos ent�o definir pol�gonos utilizando os v�rtices do array
    // model_coefficients.
    //
    // Para refer�ncia sobre os modos de renderiza��o, veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
    //
    GLuint indices[] = {
    // Definimos os �ndices dos v�rtices que definem as FACES de um cubo
    // atrav�s de 12 tri�ngulos que ser�o desenhados com o modo de renderiza��o
    // GL_TRIANGLES.
        0, 2, 1, // tri�ngulo 1
        7, 5, 6,// tri�ngulo 2
        3, 6, 2,// tri�ngulo 3
        4, 3, 0,// tri�ngulo 4
        4, 1, 5,// tri�ngulo 5
        1, 6, 5,// tri�ngulo 6
        0, 3, 2,// tri�ngulo 7
        7, 4, 5,// tri�ngulo 8
        3, 7, 6,// tri�ngulo 9
        4, 7, 3,// tri�ngulo 10
        4, 0, 1,// tri�ngulo 11
        1, 2, 6,// tri�ngulo 12
    // Definimos os �ndices dos v�rtices que definem as ARESTAS de um cubo
    // atrav�s de 12 linhas que ser�o desenhadas com o modo de renderiza��o
    // GL_LINES.
        0, 1, // linha 1
        1, 2, // linha 2
        2, 3, // linha 3
        3, 0, // linha 4
        0, 4, // linha 5
        4, 7, // linha 6
        7, 6, // linha 7
        6, 2, // linha 8
        6, 5, // linha 9
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
    // Definimos os �ndices dos v�rtices que definem as linhas dos eixos X, Y,
    // Z, que ser�o desenhados com o modo GL_LINES.
        8 , 9 , // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere �s faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = 0; // Primeiro �ndice est� em indices[0]
    cube_faces.num_indices    = 36;       // �ltimo �ndice est� em indices[35]; total de 36 �ndices.
    cube_faces.rendering_mode = GL_TRIANGLES; // �ndices correspondem ao tipo de rasteriza��o GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_faces"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere �s arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (36*sizeof(GLuint)); // Primeiro �ndice est� em indices[36]
    cube_edges.num_indices    = 24; // �ltimo �ndice est� em indices[59]; total de 24 �ndices.
    cube_edges.rendering_mode = GL_LINES; // �ndices correspondem ao tipo de rasteriza��o GL_LINES.


    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (60*sizeof(GLuint)); // Primeiro �ndice est� em indices[60]
    axes.num_indices    = 6; // �ltimo �ndice est� em indices[65]; total de 6 �ndices.
    axes.rendering_mode = GL_LINES; // �ndices correspondem ao tipo de rasteriza��o GL_LINES.
    g_VirtualScene["axes"] = axes;


    // Criamos um buffer OpenGL para armazenar os �ndices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora � GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos mem�ria para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // N�O fa�a a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de �ndices (GL_ELEMENT_ARRAY_BUFFER) n�o pode ser "desligado",
    // caso contr�rio o VAO ir� perder a informa��o sobre os �ndices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso � tudo que ser� necess�rio para renderizar
    // os tri�ngulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}
/**Boneco add**/


// Fun��o que computa as normais de um ObjModel, caso elas n�o tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRI�NGULOS.
    // Segundo, computamos as normais dos V�RTICES atrav�s do m�todo proposto
    // por Gouraud, onde a normal de cada v�rtice vai ser a m�dia das normais de
    // todas as faces que compartilham este v�rtice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            const glm::vec4  n = crossproduct(b-a,c-a);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}

// Constr�i tri�ngulos para futura renderiza��o a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o c�digo da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel �
                // comparando se o �ndice retornado � -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name           = model->shapes[shape].name;
        theobject.first_index    = first_index; // Primeiro �ndice
        theobject.num_indices    = last_index - first_index + 1; // N�mero de indices
        theobject.rendering_mode = GL_TRIANGLES;       // �ndices correspondem ao tipo de rasteriza��o GL_TRIANGLES.
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora � GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que opera��es posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja defini��o de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // ser� aplicado nos v�rtices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja defini��o de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // ser� aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Fun��o auxilar, utilizada pelas duas fun��es acima. Carrega c�digo de GPU de
// um arquivo GLSL e faz sua compila��o.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela vari�vel "filename"
    // e colocamos seu conte�do em mem�ria, apontado pela vari�vel
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o c�digo do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o c�digo do shader GLSL (em tempo de execu��o)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compila��o
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos mem�ria para guardar o log de compila��o.
    // A chamada "new" em C++ � equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compila��o
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ � equivalente ao "free()" do C
    delete [] log;
}


// Esta fun��o cria um programa de GPU, o qual cont�m obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Defini��o dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos mem�ria para guardar o log de compila��o.
        // A chamada "new" em C++ � equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ � equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para dele��o ap�s serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Defini��o da fun��o que ser� chamada sempre que a janela do sistema
// operacional for redimensionada, por consequ�ncia alterando o tamanho do
// "framebuffer" (regi�o de mem�ria onde s�o armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda regi�o do framebuffer. A
    // fun��o "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa � a opera��o de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos tamb�m a raz�o que define a propor��o da janela (largura /
    // altura), a qual ser� utilizada na defini��o das matrizes de proje��o,
    // tal que n�o ocorra distor��es durante o processo de "Screen Mapping"
    // acima, quando NDC � mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float � necess�rio pois n�meros inteiros s�o arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Vari�veis globais que armazenam a �ltima posi��o do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Fun��o callback chamada sempre que o usu�rio aperta algum dos bot�es do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, guardamos a
        // posi��o atual do cursor nas vari�veis g_LastCursorPosX e
        // g_LastCursorPosY.  Tamb�m, setamos a vari�vel
        // g_LeftMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, guardamos a
        // posi��o atual do cursor nas vari�veis g_LastCursorPosX e
        // g_LastCursorPosY.  Tamb�m, setamos a vari�vel
        // g_RightMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usu�rio pressionou o bot�o esquerdo do mouse, guardamos a
        // posi��o atual do cursor nas vari�veis g_LastCursorPosX e
        // g_LastCursorPosY.  Tamb�m, setamos a vari�vel
        // g_MiddleMouseButtonPressed como true, para saber que o usu�rio est�
        // com o bot�o esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usu�rio soltar o bot�o esquerdo do mouse, atualizamos a
        // vari�vel abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Fun��o callback chamada sempre que o usu�rio movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o bot�o esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o �ltimo
    // instante de tempo, e usamos esta movimenta��o para atualizar os
    // par�metros que definem a posi��o da c�mera dentro da cena virtual.
    // Assim, temos que o usu�rio consegue controlar a c�mera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

/**free camera add**/
    // Atualizamos par�metros da c�mera com os deslocamentos
  //  g_CameraTheta -= 0.01f*dx;
//    g_CameraPhi   += 0.01f*dy;

    g_Theta -= 0.003f*dx;
    g_Phi   += 0.003f*dy;
/**free camera add**/

        // Em coordenadas esf�ricas, o �ngulo phi deve ficar entre -pi/2 e +pi/2.
        float phimax = 3.141592f/2;
        float phimin = -phimax;

        if (g_CameraPhi > phimax)
            g_CameraPhi = phimax;

        if (g_CameraPhi < phimin)
            g_CameraPhi = phimin;

        // Atualizamos as vari�veis globais para armazenar a posi��o atual do
        // cursor como sendo a �ltima posi��o conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_RightMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;

        // Atualizamos par�metros da antebra�o com os deslocamentos
        g_ForearmAngleZ -= 0.01f*dx;
        g_ForearmAngleX += 0.01f*dy;

        // Atualizamos as vari�veis globais para armazenar a posi��o atual do
        // cursor como sendo a �ltima posi��o conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }

    if (g_MiddleMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        //float dx = xpos - g_LastCursorPosX;
        //float dy = ypos - g_LastCursorPosY;

        // Atualizamos par�metros da antebra�o com os deslocamentos
        //g_TorsoPositionX += 0.01f*dx;
        //g_TorsoPositionY -= 0.01f*dy;

        // Atualizamos as vari�veis globais para armazenar a posi��o atual do
        // cursor como sendo a �ltima posi��o conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Fun��o callback chamada sempre que o usu�rio movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a dist�ncia da c�mera para a origem utilizando a
    // movimenta��o da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    // Uma c�mera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela est� olhando, pois isto gera problemas de divis�o por zero na
    // defini��o do sistema de coordenadas da c�mera. Isto �, a vari�vel abaixo
    // nunca pode ser zero. Vers�es anteriores deste c�digo possu�am este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Defini��o da fun��o que ser� chamada sempre que o usu�rio pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // ==================
    // N�o modifique este loop! Ele � utilizando para corre��o automatizada dos
    // laborat�rios. Deve ser sempre o primeiro comando desta fun��o KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ==================

    // Se o usu�rio pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // O c�digo abaixo implementa a seguinte l�gica:
    //   Se apertar tecla X       ent�o g_AngleX += delta;
    //   Se apertar tecla shift+X ent�o g_AngleX -= delta;
    //   Se apertar tecla Y       ent�o g_AngleY += delta;
    //   Se apertar tecla shift+Y ent�o g_AngleY -= delta;
    //   Se apertar tecla Z       ent�o g_AngleZ += delta;
    //   Se apertar tecla shift+Z ent�o g_AngleZ -= delta;

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        //timer = 0;
        startmove = !startmove;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        timer += 3;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        timer -= 3;
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        camera_type= !camera_type;
    }
    // Se o usu�rio apertar a tecla espa�o, resetamos os �ngulos de Euler para zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
        g_ForearmAngleX = 0.0f;
        g_ForearmAngleZ = 0.0f;
        g_TorsoPositionX = 0.0f;
        g_TorsoPositionY = 0.0f;
        timer = 0;
        reset = true;
    }

    // Se o usu�rio apertar a tecla P, utilizamos proje��o perspectiva.
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = true;
    }

    // Se o usu�rio apertar a tecla O, utilizamos proje��o ortogr�fica.
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        g_UsePerspectiveProjection = false;
    }

    // Se o usu�rio apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        g_ShowInfoText = !g_ShowInfoText;
    }

    // Se o usu�rio apertar a tecla R, recarregamos os shaders dos arquivos "shader_fragment.glsl" e "shader_vertex.glsl".
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }
/**free camera add**/
       // Veja https://www.glfw.org/docs/3.3/input_guide.html#input_key
    if (key == GLFW_KEY_D)
    {
        if (action == GLFW_PRESS)
            // Usu�rio apertou a tecla D, ent�o atualizamos o estado para pressionada
            tecla_D_pressionada = true;

        else if (action == GLFW_RELEASE)
            // Usu�rio largou a tecla D, ent�o atualizamos o estado para N�O pressionada
            tecla_D_pressionada = false;

        else if (action == GLFW_REPEAT)
            // Usu�rio est� segurando a tecla D e o sistema operacional est�
            // disparando eventos de repeti��o. Neste caso, n�o precisamos
            // atualizar o estado da tecla, pois antes de um evento REPEAT
            // necessariamente deve ter ocorrido um evento PRESS.
            ;
    }
    if (key == GLFW_KEY_A)
    {
        if (action == GLFW_PRESS)
            // Usu�rio apertou a tecla A, ent�o atualizamos o estado para pressionada
            tecla_A_pressionada = true;

        else if (action == GLFW_RELEASE)
            // Usu�rio largou a tecla A, ent�o atualizamos o estado para N�O pressionada
            tecla_A_pressionada = false;

        else if (action == GLFW_REPEAT)
            // Usu�rio est� segurando a tecla A e o sistema operacional est�
            // disparando eventos de repeti��o. Neste caso, n�o precisamos
            // atualizar o estado da tecla, pois antes de um evento REPEAT
            // necessariamente deve ter ocorrido um evento PRESS.
            ;
    }
    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS)
            // Usu�rio apertou a tecla W, ent�o atualizamos o estado para pressionada
            tecla_W_pressionada = true;

        else if (action == GLFW_RELEASE)
            // Usu�rio largou a tecla W, ent�o atualizamos o estado para N�O pressionada
            tecla_W_pressionada = false;

        else if (action == GLFW_REPEAT)
            // Usu�rio est� segurando a tecla W e o sistema operacional est�
            // disparando eventos de repeti��o. Neste caso, n�o precisamos
            // atualizar o estado da tecla, pois antes de um evento REPEAT
            // necessariamente deve ter ocorrido um evento PRESS.
            ;
    }
    if (key == GLFW_KEY_S)
    {
        if (action == GLFW_PRESS)
            // Usu�rio apertou a tecla S, ent�o atualizamos o estado para pressionada
            tecla_S_pressionada = true;

        else if (action == GLFW_RELEASE)
            // Usu�rio largou a tecla S, ent�o atualizamos o estado para N�O pressionada
            tecla_S_pressionada = false;

        else if (action == GLFW_REPEAT)
            // Usu�rio est� segurando a tecla S e o sistema operacional est�
            // disparando eventos de repeti��o. Neste caso, n�o precisamos
            // atualizar o estado da tecla, pois antes de um evento REPEAT
            // necessariamente deve ter ocorrido um evento PRESS.
            ;
    }
/**free camera add**/
}

// Definimos o callback para impress�o de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}


void TextRendering_ShowBodyAngles(
    GLFWwindow* window,
    Teta angulo
)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);
//Acentrox eixo central, Acentroy (comprmento)

//abre/fecha,Rot In/Ex, Frente/tras
//Aombrozd , Aombroyd, Aombroxd
//Aombroze , Aombroye, Aombroxe

//supi/pron, ext/flex
//Acotovyd ,Acotovxe
//Acotovyd ,Acotovxe

//abre/fecha, rot ex/int, frent/tras
//Aquadrildz, Aquadrildy, Aquadrildx
//Aquadrilez, Aquadriley, Aquadrilex

//estica/flex
//Ajoelhox

//yoko/inverso, rot ext/in,estica/flex,
//Apedz, Apedy, Apedx
    //TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);

    char buffert[80];
    snprintf(buffert, 80, "Timer     %.2f \n", timer);
    TextRendering_PrintString(window, buffert, -1.0f+pad/10, 1.0f-1*pad, 1.0f);
    char buffero[80];
    snprintf(buffero, 80, "Ombro     %.2f x,%2.2f y,%2.2f z     %.2f x,%2.2f y,%2.2f z \n", angulo.ombroe.x, angulo.ombroe.y, angulo.ombroe.z, angulo.ombrod.x, angulo.ombrod.y, angulo.ombrod.z);
    TextRendering_PrintString(window, buffero, -1.0f+pad/10, 1.0f-2*pad, 1.0f);
    char buffercv[80];
    snprintf(buffercv, 80,"Cotovelo  %.2f x,%2.2f y,%2.2f z     %.2f x,%2.2f y,%2.2f z \n", angulo.cotove.x, angulo.cotove.y, angulo.cotove.z, angulo.cotovd.x, angulo.cotovd.y, angulo.cotovd.z);
    TextRendering_PrintString(window, buffercv, -1.0f+pad/10, 1.0f-3*pad, 1.0f);
    char bufferc[80];
    snprintf(bufferc, 80, "Central             %.2f x,%2.2f y,%2.2f z \n", angulo.centro.x, angulo.centro.y, angulo.centro.z);
    TextRendering_PrintString(window, bufferc, -1.0f+pad/10, 1.0f-4*pad, 1.0f);
    char bufferq[80];
    snprintf(bufferq, 80, "Quadril   %.2f x,%2.2f y,%2.2f z     %.2f x,%2.2f y,%2.2f z \n", angulo.quadrile.x, angulo.quadrile.y, angulo.quadrile.z, angulo.quadrild.x, angulo.quadrild.y, angulo.quadrild.z);
    TextRendering_PrintString(window, bufferq, -1.0f+pad/10, 1.0f-5*pad, 1.0f);
    char bufferj[80];
    snprintf(bufferj, 80, "Joelho    %.2f x,%2.2f y,%2.2f z     %.2f x,%2.2f y,%2.2f z \n", angulo.joelhoe.x, angulo.joelhoe.y, angulo.joelhoe.z, angulo.joelhod.x, angulo.joelhod.y, angulo.joelhod.z);
    TextRendering_PrintString(window, bufferj, -1.0f+pad/10, 1.0f-6*pad, 1.0f);
    char bufferp[80];
    snprintf(bufferp, 80, "Pe        %.2f x,%2.2f y,%2.2f z     %.2f x,%2.2f y,%2.2f z \n", angulo.pee.x, angulo.pee.y, angulo.pee.z, angulo.ped.x, angulo.ped.y, angulo.ped.z);
    TextRendering_PrintString(window, bufferp, -1.0f+pad/10, 1.0f-7*pad, 1.0f);

}


// Esta fun��o recebe um v�rtice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transforma��es.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;
    glm::vec4 p_clip = projection*p_camera;
    glm::vec4 p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-6*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f-9*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-10*pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f-14*pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f-15*pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f-16*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f-17*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-18*pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2( 0,  0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x)/(b.x-a.x), 0.0f, 0.0f, (b.x*p.x - a.x*q.x)/(b.x-a.x),
        0.0f, (q.y - p.y)/(b.y-a.y), 0.0f, (b.y*p.y - a.y*q.y)/(b.y-a.y),
        0.0f , 0.0f , 1.0f , 0.0f ,
        0.0f , 0.0f , 0.0f , 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f-22*pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f-23*pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f-24*pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f-25*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f-26*pad, 1.0f);
}

// Escrevemos na tela os �ngulos de Euler definidos nas vari�veis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    if(!camera_type)
    snprintf(buffer, 80, "Camera Livre");
    if(camera_type)
    snprintf(buffer, 80, "Camera look-at center");

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de proje��o est� sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Perspective", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o n�mero de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Vari�veis est�ticas (static) mant�m seus valores entre chamadas
    // subsequentes da fun��o!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o n�mero de segundos que passou desde a execu��o do programa
    float seconds = (float)glfwGetTime();

    // N�mero de segundos desde o �ltimo c�lculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

// Fun��o para debugging: imprime no terminal todas informa��es de um modelo
// geom�trico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
void PrintObjModelInfo(ObjModel* model)
{
  const tinyobj::attrib_t                & attrib    = model->attrib;
  const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
  const std::vector<tinyobj::material_t> & materials = model->materials;

  printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
  printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
  printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
  printf("# of shapes    : %d\n", (int)shapes.size());
  printf("# of materials : %d\n", (int)materials.size());

  for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  // For each shape
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));

    size_t index_offset = 0;

    assert(shapes[i].mesh.num_face_vertices.size() ==
           shapes[i].mesh.material_ids.size());

    printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

    // For each face
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      size_t fnum = shapes[i].mesh.num_face_vertices[f];

      printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
             static_cast<unsigned long>(fnum));

      // For each vertex in the face
      for (size_t v = 0; v < fnum; v++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
        printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
               static_cast<long>(v), idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
             shapes[i].mesh.material_ids[f]);

      index_offset += fnum;
    }

    printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.tags.size()));
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", static_cast<long>(t),
             shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
                         shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }
  }

  for (size_t i = 0; i < materials.size(); i++) {
    printf("material[%ld].name = %s\n", static_cast<long>(i),
           materials[i].name.c_str());
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname.c_str());
    printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
    printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
    printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
    printf("  <<PBR>>\n");
    printf("  material.Pr     = %f\n", materials[i].roughness);
    printf("  material.Pm     = %f\n", materials[i].metallic);
    printf("  material.Ps     = %f\n", materials[i].sheen);
    printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
    printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
    printf("  material.aniso  = %f\n", materials[i].anisotropy);
    printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
    printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
    printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
    printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
    printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
    printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
    std::map<std::string, std::string>::const_iterator it(
        materials[i].unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n");
  }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

