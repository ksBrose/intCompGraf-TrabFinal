#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "moves.h"

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
glm::vec3 p0, p1, p2, p3;

void inicializa_angulos(Teta *angulo, glm::vec3 *bezier_cintura)
{
    angulo->centro.x = 0;
    angulo->centro.y = 0;
    angulo->centro.z = 0;
    angulo->ombrod.x = 0;
    angulo->ombrod.y = 0;
    angulo->ombrod.z = 0;
    angulo->ombroe.x = 0;
    angulo->ombroe.y = 0;
    angulo->ombroe.z = 0;
    angulo->cotovd.x = 0;
    angulo->cotovd.y = 0;
    angulo->cotovd.z = 0;
    angulo->cotove.x = 0;
    angulo->cotove.y = 0;
    angulo->cotove.z = 0;
    angulo->quadrild.x = 0;
    angulo->quadrild.y = 0;
    angulo->quadrild.z = 0;
    angulo->quadrile.x = 0;
    angulo->quadrile.y = 0;
    angulo->quadrile.z = 0;
    angulo->joelhod.x = 0;
    angulo->joelhod.y = 0;
    angulo->joelhod.z = 0;
    angulo->joelhoe.x = 0;
    angulo->joelhoe.y = 0;
    angulo->joelhoe.z = 0;
    angulo->ped.x = 0;
    angulo->ped.y = 0;
    angulo->ped.z = 0;
    angulo->pee.x = 0;
    angulo->pee.y = 0;
    angulo->pee.z = 0;
    bezier_cintura->x = 0;
    bezier_cintura->y = 0;
    bezier_cintura->z = 0;

}

// Cálculo da Curva de Bezier 3D de 1o grau
glm::vec3 get3DBezierCurveGrau1(glm::vec3 p0, glm::vec3 p1, float t){

    float x = (1-t)*p0.x + t*p1.x;
    float y = (1-t)*p0.y + t*p1.y ;
    float z = (1-t)*p0.z + t*p1.z;

    return glm::vec3(x, y, z);
}
// Cálculo da Curva de Bezier 3D de 2o grau
glm::vec3 get3DBezierCurveGrau2(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float t){

    float x = pow(1-t,2)*p0.x + 2*t*(1-t)*p1.x + pow(t,2)*p2.x;
    float y = pow(1-t,2)*p0.y + 2*t*(1-t)*p1.y + pow(t,2)*p2.y;
    float z = pow(1-t,2)*p0.z + 2*t*(1-t)*p1.z + pow(t,2)*p2.z;

    return glm::vec3(x, y, z);
}
// Cálculo da Curva de Bezier 3D de 3o grau
glm::vec3 get3DBezierCurveGrau3(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t){

    float x = pow(1-t,3)*p0.x + 3*pow(t,2)*(1-t)*p1.x + 3*t*pow(1-t,2)*p2.x + pow(t,3)*p3.x;
    float y = pow(1-t,3)*p0.y + 3*pow(t,2)*(1-t)*p1.y + 3*t*pow(1-t,2)*p2.y + pow(t,3)*p3.y;
    float z = pow(1-t,3)*p0.z + 3*pow(t,2)*(1-t)*p1.z + 3*t*pow(1-t,2)*p2.z + pow(t,3)*p3.z;

    return glm::vec3(x, y, z);
}

void move_one(Teta *angulo, float dt, float *timer, glm::vec3 *bezier_cintura)
{
    float tt = 0;
    *timer += dt;
    if (*timer >= 0 && *timer <= 3)
    {
        tt = (*timer )/3;
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 30.0f, 0.0f), tt);
        //angulo->ped.y += dt * 10;
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 30.0f, 0.0f), tt);
        //angulo->pee.y += dt * 10;
    }
    if (*timer > 3 && *timer <= 9)//comprimento
    {
        angulo->centro.x = 30* sin((*timer-3) * 30 * M_PI/180);
        angulo->quadrild.x = 30* sin((*timer-3) * 30 * M_PI/180);
        angulo->quadrile.x = 30* sin((*timer-3) * 30 * M_PI/180);
    }
    if (*timer > 9 && *timer <= 12)//posição de preparação pt1
    {
        tt = (*timer - 9)/3;
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 10.0f), tt);//abre uma perna
                                                            //fecha um pouco os braços-v
        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(15.0f, 87.0f, 36.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(15.0f, 87.0f, 24.0f), tt);
                                //braço a 30 graus com diferença para nao ficarem no mesmo espaço---^
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
    }
    if (*timer > 12 && *timer <= 15)//posição de preparação pt2
    {
        tt = (*timer - 12)/3;
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 10.0f), tt);//abre a outra perna
        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(15.0f, 87.0f, 36.0f),glm::vec3(0.0f, 87.0f, 30.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(15.0f, 87.0f, 24.0f),glm::vec3(0.0f, 87.0f, 30.0f), tt);
                                                                            //realinha os braços------^
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(-15.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(-15.0f, 0.0f, 0.0f), tt);
    }
    if (*timer > 15 && *timer <= 16.5)//descida e virada para esquerda pt1
    {
        tt = (*timer - 15)/1.5;
        p0 = glm::vec3(0.0f, 0.0f, 0.0f);
        p1 = glm::vec3(0.0f, -0.2f, 0.0f);
        p2 = glm::vec3(0.65f, -0.25f, 0.0f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt/2);  //desce para frente
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 00.0f, 0.0f),glm::vec3(0.0f, 90.0f, 1.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt/2);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt/2);

        angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt/2);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(0.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt/2);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(0.0f, 30.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt/2);

        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f, 30.0f),glm::vec3(24.0f, 87.0f, 54.0f), tt); //prepara defesa
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f, 30.0f),glm::vec3(66.0f, 63.0f, 60.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-15.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-15.0f, 0.0f, 0.0f),glm::vec3(-90.0f, 90.0f, 0.0f), tt);
}
    if (*timer > 16.5 && *timer <= 18)//descida e virada para esquerda pt2
    {
        tt = (*timer - 16.5)/1.5;
        p0 = glm::vec3(0.0f, 0.0f, 0.0f);
        p1 = glm::vec3(0.0f, -0.2f, 0.0f);
        p2 = glm::vec3(0.65f, -0.25f, 0.f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, 0.5+tt/2);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 90.0f, 0.0f),glm::vec3(0.0f, 90.0f-45.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), 0.5+tt/2);
        angulo->quadrile.y = (1-tt)*0.0f + tt*(45.0f);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), 0.5+tt/2);
        angulo->quadrild.y = (1-tt)*0.0f + tt*(-45.0f);

        angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), 0.5+tt/2);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(0.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), 0.5+tt/2);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(0.0f, 30.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), 0.5+tt/2);

        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(24.0f, 87.0f, 54.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);   //defende embaixo
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(66.0f, 63.0f, 60.0f),glm::vec3(0.0f, 87.0f - 45.0f, 45.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-90.0f, 90.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
    }
    if (*timer > 18 && *timer <= 21)//caminhada direita para frente e soco
    {
        tt = (*timer - 18)/3;
        p0 = glm::vec3(0.65f, -0.25f, 0.0f);
        p1 = glm::vec3(1.f*1.5, -0.25f, 0.0f);
        p2 = glm::vec3(1.f*2, -0.25f, 0.f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 90.0f-45.0f, 0.0f),glm::vec3(0.0f, 90.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(-37.0f, -45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f - 45.0f, 45.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 21 && *timer <= 24)//virada 180 pela direita / defendendo
    {
        tt = (*timer - 21)/3;
        p0 = glm::vec3(2.0f, -0.25f, 0.f);
        p1 = glm::vec3(0.0f, -0.25f, 0.0f);
        p2 = glm::vec3(0.4f, -0.25f, -0.40f);
        p3 = glm::vec3(0.65f, -0.25f, -0.80f);
        *bezier_cintura = get3DBezierCurveGrau3(p0, p1, p2, p3, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f+45.0f, 0.0f), tt);
                                                            //quadril da frente a 45º---v
        angulo->quadrile = get3DBezierCurveGrau2(glm::vec3(-37.0f, 0.0f, 10.0f), glm::vec3(140.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau2(glm::vec3(45.0f, 0.0f, 10.0f), glm::vec3(70.0f, 45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*0.0f + tt*(-45.0f);
        angulo->quadrild.y = (1-tt)*0.0f + tt*(45.0f);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-190.0f) + pow(tt,2)*0.0f;
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-110.0f) + pow(tt,2)*(-45.0f-21.0f);

        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(0.0f, 87.0f -45.0f, 45.0f), tt); //prepara defesa
        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(24.0f*6, 87.0f*2.5, 54.0f*2.5),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    //
    if (*timer > 24 && *timer <= 27)//caminhada esquerda para frente / soco
    {
        tt = (*timer - 24)/3;
        p0 = glm::vec3(0.65f, -0.25f, -0.80f);
        p1 = glm::vec3(-0.15f, -0.25f, -0.80f);
        p2 = glm::vec3(-0.80f, -0.25f, -0.80f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, -90.0f+45.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);

        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(45.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(-37.0f, -45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f - 45.0f, 45.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 27 && *timer <= 30) //virada para esquerda pela frente /defesa
    {
        tt = (*timer - 27)/3;
        p0 = glm::vec3(-0.80f, -0.25f, -0.80f);
        p1 = glm::vec3(0.2f, -0.25f, -0.80f);
        p2 = glm::vec3(0.2f, -0.25f, -0.3f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(0.0f, -45.0f, 0.0f), tt);
                                                            //quadril da frente a 45º---v
        angulo->quadrild = get3DBezierCurveGrau2(glm::vec3(-37.0f, 0.0f, 10.0f), glm::vec3(140.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau2(glm::vec3(45.0f, 0.0f, 10.0f), glm::vec3(70.0f, 45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*0.0f + tt*(45.0f);
        angulo->quadrild.y = (1-tt)*0.0f + tt*(-45.0f);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-190.0f) + pow(tt,2)*0.0f;
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-110.0f) + pow(tt,2)*(-45.0f-21.0f);

        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(0.0f, 87.0f-45.0f , 45.0f), tt); //prepara defesa
        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(24.0f*6, 87.0f*2.5, 54.0f*2.5),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);


    }
    if (*timer > 30 && *timer <= 33)//passada direita para frente /defende em cima
    {
        tt = (*timer - 30)/3;
        p0 = glm::vec3(0.2f, -0.25f, -0.3f);
        p1 = glm::vec3(0.2f, -0.25f, 0.5f);
        p2 = glm::vec3(0.2f, -0.25f, 1.3f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, -45.0f, 0.0f),glm::vec3(0.0f, 45.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(-37.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*45.0f + tt*(-45.0f);
        angulo->quadrild.y = (1-tt)*-45.0f + tt*(45.0f);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);k
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau3(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(50.0f, 120.0f, 0.0f),glm::vec3(150.0f, 60.0f, 40.0f),glm::vec3(200.0f, -90.0f-45.0f , 20.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f -45.0f, 45.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(-45.0f, 00.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);


    }
    if (*timer > 33 && *timer <= 36)//passada esquerda para frente
    {
        tt = (*timer - 33)/3;
        p0 = glm::vec3(0.2f, -0.25f, 1.3f);
        p1 = glm::vec3(0.2f, -0.25f, 2.1f);
        p2 = glm::vec3(0.2f, -0.25f, 2.9f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 45.0f, 0.0f),glm::vec3(0.0f, -45.0f, 0.0f), tt);

        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(-37.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*-45.0f + tt*(45.0f);
        angulo->quadrild.y = (1-tt)*45.0f + tt*(-45.0f);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau3(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(50.0f, 120.0f, 0.0f),glm::vec3(150.0f, 60.0f, 40.0f),glm::vec3(200.0f, -90.0f-45.0f , 20.0f), tt);
        angulo->ombrod = get3DBezierCurveGrau3(glm::vec3(200.0f, -90.0f-45.0f , 20.0f),glm::vec3(150.0f, 80.0f, 40.0f),glm::vec3(50.0f, 140.0f, 0.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(-45.0f, 00.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-45.0f, 00.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);
    }
    if (*timer > 36 && *timer <= 39)//passada direita para frente
    {
        tt = (*timer - 36)/3;
        p0 = glm::vec3(0.2f, -0.25f, 2.9f);
        p1 = glm::vec3(0.2f, -0.25f, 3.7f);
        p2 = glm::vec3(0.2f, -0.25f, 4.5f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, -45.0f, 0.0f),glm::vec3(0.0f, 45.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(-37.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*45.0f + tt*(-45.0f);
        angulo->quadrild.y = (1-tt)*-45.0f + tt*(45.0f);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau3(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(50.0f, 120.0f, 0.0f),glm::vec3(150.0f, 60.0f, 40.0f),glm::vec3(200.0f, -90.0f-45.0f , 20.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau3(glm::vec3(200.0f, -90.0f-45.0f , 20.0f),glm::vec3(150.0f, 80.0f, 40.0f),glm::vec3(50.0f, 140.0f, 0.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(-45.0f, 00.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-45.0f, 00.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 39 && *timer <= 42) //virada pelas costas para direita /defesa
    {
        tt = (*timer - 39)/3;
        p0 = glm::vec3(0.2f, -0.25f, 4.5f);
        p1 = glm::vec3(0.2f, -0.25f, 5.0f);
        p2 = glm::vec3(-0.8f, -0.25f, 5.8f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 45.0f, 0.0f),glm::vec3(0.0f, 270.0f -45.0f, 0.0f), tt);
                                                                        //quadril da frente a 45º---v
        angulo->quadrile = get3DBezierCurveGrau2(glm::vec3(-37.0f, -45.0f, 10.0f), glm::vec3(140.0f, 45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau2(glm::vec3(45.0f, 45.0f, 10.0f), glm::vec3(70.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*-45.0f + tt*(45.0f);
        angulo->quadrild.y = (1-tt)*45.0f + tt*(-45.0f);

        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-190.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-110.0f) + pow(tt,2)*0.0f;

        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(0.0f, 87.0f -45.0f, 45.0f), tt); //prepara defesa
        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(200.0f, -90.0f-45.0f , 20.0f),glm::vec3(24.0f*4, 87.0f*2.0, 54.0f*1.0),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(-45.0f, 00.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);


    }
    if (*timer > 42 && *timer <= 45)//passada direita para frente /soco
    {
        tt = (*timer - 42)/3;
        p0 = glm::vec3(-0.8f, -0.25f, 5.8f);
        p1 = glm::vec3(-1.6f, -0.25f, 5.8f);
        p2 = glm::vec3(-2.4f, -0.25f, 5.8f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 270.0f-45.0f, 0.0f),glm::vec3(0.0f, 270.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(-37.0f, -45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f -45.0f , 45.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 45 && *timer <= 48)//virada 180 pela direita / defesa
    {
        tt = (*timer - 45)/3;
        p0 = glm::vec3(-2.4f, -0.25f, 5.8f);
        p1 = glm::vec3(-0.4f, -0.25f, 5.8f);
        p2 = glm::vec3(-0.6f, -0.25f, 6.2f);
        p3 = glm::vec3(-0.8f, -0.25f, 6.4f);

        *bezier_cintura = get3DBezierCurveGrau3(p0, p1, p2, p3, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 270.0f, 0.0f),glm::vec3(0.0f, 90.0f+45.0f, 0.0f), tt);
                                                                       //quadril da frente a 45º---v
        angulo->quadrile = get3DBezierCurveGrau2(glm::vec3(-37.0f, 0.0f, 10.0f), glm::vec3(140.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau2(glm::vec3(45.0f, 0.0f, 10.0f), glm::vec3(70.0f, 45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*0.0f + tt*(-45.0f);
        angulo->quadrild.y = (1-tt)*0.0f + tt*(45.0f);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-190.0f) + pow(tt,2)*0.0f;
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-110.0f) + pow(tt,2)*(-45.0f-21.0f);

        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(0.0f, 87.0f -45.0f, 45.0f), tt); //prepara defesa
        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(24.0f*6, 87.0f*2.5, 54.0f*2.5),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 48 && *timer <= 51)//passada esquerda para frente / soco
    {
        tt = (*timer - 48)/3;
        p0 = glm::vec3(-0.8f, -0.25f, 6.4f);
        p1 = glm::vec3(0.0f, -0.25f, 6.4f);
        p2 = glm::vec3(0.8f, -0.25f, 6.4f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 90.0f+45.0f, 0.0f),glm::vec3(0.0f, 90.0f, 0.0f), tt);

        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(45.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(-37.0f, -45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f - 45.0f, 45.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 51 && *timer <= 54) //virada para esquerda pela frente /defesa
    {
        tt = (*timer - 51)/3;
        p0 = glm::vec3(0.8f, -0.25f, 6.4f);
        p1 = glm::vec3(-0.2f, -0.25f, 6.4f);
        p2 = glm::vec3(-0.2f, -0.25f, 5.9f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 90.0f, 0.0f),glm::vec3(0.0f, 180.0f-45.0f, 0.0f), tt);
                                                                         //quadril da frente a 45º---v
        angulo->quadrild = get3DBezierCurveGrau2(glm::vec3(-37.0f, 0.0f, 10.0f), glm::vec3(140.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau2(glm::vec3(45.0f, 0.0f, 10.0f), glm::vec3(70.0f, 45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile.y = (1-tt)*0.0f + tt*(45.0f);
        angulo->quadrild.y = (1-tt)*0.0f + tt*(-45.0f);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-190.0f) + pow(tt,2)*0.0f;
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-110.0f) + pow(tt,2)*(-45.0f-21.0f);

        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(0.0f, 87.0f-45.0f , 45.0f), tt); //prepara defesa
        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(24.0f*6, 87.0f*2.5, 54.0f*2.5),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);



    }
    if (*timer > 54 && *timer <= 57)//passada direita para frente /soco1
    {
        tt = (*timer - 54)/3;
        p0 = glm::vec3(-0.2f, -0.25f, 5.9f);
        p1 = glm::vec3(-0.2f, -0.25f, 5.1f);
        p2 = glm::vec3(-0.2f, -0.25f, 4.3f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 180.0f-45.0f, 0.0f),glm::vec3(0.0f, 180.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 45.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(-37.0f, -45.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(0.0f, 87.0f-45.0f , 45.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer > 57 && *timer <= 60)//passada esquerda para frente /soco2
    {
        tt = (*timer - 57)/3;
        p0 = glm::vec3(-0.2f, -0.25f, 4.3f);
        p1 = glm::vec3(-0.2f, -0.25f, 3.5f);
        p2 = glm::vec3(-0.2f, -0.25f, 2.7f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 180.0f, 0.0f),glm::vec3(0.0f, 180.0f, 0.0f), tt);

        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(-37.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
    if (*timer >60 && *timer <= 63)//passada direita para frente /soco3
    {
        tt = (*timer - 60)/3;
        p0 = glm::vec3(-0.2f, -0.25f, 2.7f);
        p1 = glm::vec3(-0.2f, -0.25f, 1.9f);
        p2 = glm::vec3(-0.2f, -0.25f, 1.1f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 180.0f, 0.0f),glm::vec3(0.0f, 180.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(-37.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(-37.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*0.0f;
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-37.0f, 30.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(-21.0f, 00.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau1(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(75.0f, 25.0f , 0.0f), tt);
        angulo->ombroe = get3DBezierCurveGrau1(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(-60.0f, 0.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau1(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(0.0f, -90.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau1(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(-110.0f, 90.0f, 0.0f), tt);

    }
        if (*timer > 63 && *timer <= 66) //virada pelas costas para direita
    {
        tt = (*timer - 63)/3;
        p0 = glm::vec3(-0.2f, -0.25f, 1.1f);
        p1 = glm::vec3(-0.2f, -0.25f, 0.6f);
        p2 = glm::vec3(0.8f, -0.25f, -0.2f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 180.0f, 0.0f),glm::vec3(0.0f, 360.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau2(glm::vec3(-37.0f, 0.0f, 10.0f), glm::vec3(140.0f, 45.0f, 10.0f),glm::vec3(30.0f, 90.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau2(glm::vec3(45.0f, 0.0f, 10.0f), glm::vec3(70.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(0.0f) + 2*tt*(1-tt)*(-190.0f) + pow(tt,2)*(-10.0f);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-110.0f) + pow(tt,2)*(-45.0f-21.0f);

        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-37.0f, 30.0f, 0.0f),glm::vec3(25.0f, 0.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-21.0f, 00.0f, 0.0f),glm::vec3(-21.0f, 0.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(-60.0f, 0.0f, 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(45.0f, -90.0f , 0.0f), tt); //prepara defesa
        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(75.0f, 25.0f , 0.0f),glm::vec3(24.0f*4, 87.0f*1.5, 54.0f*1.5),glm::vec3(0.0f, 90.0f, 20.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-110.0f, 90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(-90.0f, -90.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(0.0f, -90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-90.0f, 90.0f, 0.0f), tt);

    }
        if (*timer >66 && *timer <= 69)//passada direita para frente
    {
        tt = (*timer - 66)/3;
        p0 = glm::vec3(0.8f, -0.25f, -0.2f);
        p1 = glm::vec3(1.6f, -0.25f, -0.2f);
        p2 = glm::vec3(2.4f, -0.25f, -0.2f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 360.0f, 0.0f),glm::vec3(0.0f, 495.0f, 0.0f), tt);

        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(30.0f, 90.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(30.0f, 90.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-10.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-10.0f);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(25.0f, 0.0f, 0.0f),glm::vec3(-21.0f, 0.0f, 0.0f), tt);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(-21.0f, 0.0f, 0.0f),glm::vec3(25.0f, 0.0f, 0.0f), tt);

        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(0.0f, 90.0f, 20.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(45.0f, -90.0f , 0.0f), tt); //prepara defesa
        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(45.0f, -90.0f , 0.0f),glm::vec3(24.0f*4, 87.0f*1.5, 54.0f*1.5),glm::vec3(0.0f, 90.0f, 20.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(-90.0f, 90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(-90.0f, -90.0f, 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-90.0f, -90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-90.0f, 90.0f, 0.0f), tt);
    }
    if (*timer > 69 && *timer <= 72)//virada 180 pela direita
    {
        tt = (*timer - 69)/3;
        p0 = glm::vec3(2.4f, -0.25f, -0.2f);
        p1 = glm::vec3(1.6f, -0.25f, -0.2f);
        p2 = glm::vec3(0.8f, -0.25f, -0.8f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 495.0f, 0.0f),glm::vec3(0.0f, 360.0f, 0.0f), tt);

        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(30.0f, 90.0f, 10.0f),glm::vec3(30.0f, 90.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-10.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-10.0f);
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(25.0f, 0.0f, 0.0f),glm::vec3(25.0f, 0.0f, 0.0f), tt);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 0.0f, 0.0f),glm::vec3(-21.0f, 0.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(0.0f, 90.0f, 20.0f),glm::vec3(24.0f*4, 87.0f*1.5, 54.0f*1.5),glm::vec3(0.0f, 90.0f, 20.0f), tt); //prepara defesa
        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(45.0f, -90.0f , 0.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(45.0f, -90.0f , 0.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-90.0f, 90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-90.0f, 90.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(-90.0f, -90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(-90.0f, -90.0f, 0.0f), tt);

    }
    if (*timer > 72 && *timer <= 75)//passada esquerda para frente
    {
        tt = (*timer - 72)/3;
        p0 = glm::vec3(0.8f, -0.25f, -0.8f);
        p1 = glm::vec3(0.0f, -0.25f, -0.8f);
        p2 = glm::vec3(-0.8f, -0.25f, -0.8f);
        *bezier_cintura = get3DBezierCurveGrau2(p0, p1, p2, tt);
        angulo->centro = get3DBezierCurveGrau1(glm::vec3(0.0f, 360.0f, 0.0f),glm::vec3(0.0f, 225.0f, 0.0f), tt);

        angulo->quadrild = get3DBezierCurveGrau1(glm::vec3(30.0f, 90.0f, 10.0f),glm::vec3(45.0f, 0.0f, 10.0f), tt);
        angulo->quadrile = get3DBezierCurveGrau1(glm::vec3(45.0f, 0.0f, 10.0f),glm::vec3(30.0f, 90.0f, 10.0f), tt);

        //angulo->joelhoe = get3DBezierCurveGrau1(glm::vec3(-45.0f-21.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 0.0f), tt);
        angulo->joelhod.x = pow(1-tt,2)*(-10.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-45.0f-21.0f);
        //angulo->joelhod = get3DBezierCurveGrau1(glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(-45.0f-21.0f, 0.0f, 0.0f), tt);
        angulo->joelhoe.x = pow(1-tt,2)*(-45.0f-21.0f) + 2*tt*(1-tt)*(-90.0f) + pow(tt,2)*(-10.0f);
        angulo->ped = get3DBezierCurveGrau1(glm::vec3(25.0f, 0.0f, 0.0f),glm::vec3(-21.0f, 0.0f, 0.0f), tt);
        angulo->pee = get3DBezierCurveGrau1(glm::vec3(-21.0f, 0.0f, 0.0f),glm::vec3(25.0f, 0.0f, 0.0f), tt);

        angulo->ombroe = get3DBezierCurveGrau2(glm::vec3(0.0f, 90.0f, 20.0f),glm::vec3(66.0f*1.5, 63.0f*1.6, 60.0f*1.6),glm::vec3(45.0f, -90.0f , 0.0f), tt); //prepara defesa
        angulo->ombrod = get3DBezierCurveGrau2(glm::vec3(45.0f, -90.0f , 0.0f),glm::vec3(24.0f*4, 87.0f*1.5, 54.0f*1.5),glm::vec3(0.0f, 90.0f, 20.0f), tt);
        angulo->cotove = get3DBezierCurveGrau2(glm::vec3(-90.0f, 90.0f, 0.0f),glm::vec3(-90.0f*1.7, 90.0f*1.7, 0.0f),glm::vec3(-90.0f, -90.0f, 0.0f), tt);
        angulo->cotovd = get3DBezierCurveGrau2(glm::vec3(-90.0f, -90.0f, 0.0f),glm::vec3(110.0f, -90.0f, 0.0f),glm::vec3(-90.0f, 90.0f, 0.0f), tt);

    }
    printf("%f\n", *timer);
}

