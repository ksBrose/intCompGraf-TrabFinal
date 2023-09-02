#include <string>
#include <glad/glad.h>
#define M_PI 3.1415

typedef struct
{
    float x;
    float y;
    float z;
}xyz;


typedef struct
{
    glm::vec3 centro;
    glm::vec3 ombrod;
    glm::vec3 ombroe;
    glm::vec3 cotovd;
    glm::vec3 cotove;
    glm::vec3 quadrild;
    glm::vec3 quadrile;
    glm::vec3 joelhod;
    glm::vec3 joelhoe;
    glm::vec3 ped;
    glm::vec3 pee;
}Teta;

void move_one(Teta *angulo, float dt, float *timer, glm::vec3 *bezier_cintura);

void inicializa_angulos(Teta *angulo, glm::vec3 *bezier_cintura);

glm::vec3 get3DBezierCurveGrau2(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t);

glm::vec3 get3DBezierCurveGrau1(glm::vec3 p0, glm::vec3 p1, float t);
