#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/teapot.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/random.h"
#include "helper/grid.h"
#include "helper/particleutils.h"

class SceneBasic_Uniform : public Scene
{
private:
    SkyBox sky;
    Plane plane;
    float tPrev;
    GLuint texTiles;
    GLuint texRust;
    GLuint texCar;
    std::unique_ptr<ObjMesh> mesh;
    float angle;
    bool spin;
    float camDistance;
    float time;
    GLSLProgram planeProg;
    GLSLProgram carProg;
    GLSLProgram skyboxProg;
    GLSLProgram prog;

    float tPrevPbr, lightAngle, lightRotationSpeed;
    glm::vec4 lightPos2;

    void setMatricesTeapot();
    void drawScene();
    void drawFloor();
    void drawCar(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);

    GLSLProgram particleProg;
    float particleLifeTime;
    GLSLProgram flatProg;
    Random rand;
    GLuint initVel, startTime, particles, nParticles;
    Grid grid;
    glm::vec3 emitterPos, emitterDir;

    glm::vec3 carPos;
    float carAngle;
    glm::vec3 carForward;

    void initBuffers();
    float randFloat();

    void setMatrices(GLSLProgram&);

    void setMatricesPlane();
    //void setMatricesCar();
    void setMatricesSkybox();
    void setMatricesPbr();
    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
    void upPressed();
    void downPressed();
    void leftPressed();
    void rightPressed();
    void spinToggle();
};

#endif // SCENEBASIC_UNIFORM_H
