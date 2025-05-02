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
#include "helper/fuelPickup.h"

class SceneBasic_Uniform : public Scene
{
private:
    SkyBox sky;
    Plane plane;
    fuelPickup fuelCan[10];

    GLuint texCar;
    GLuint fuelTex;
    GLuint particleTex;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> fuelMesh;

    GLSLProgram particleProg;
    GLSLProgram flatProg;
    GLSLProgram planeProg;
    GLSLProgram fuelCanProg;
    GLSLProgram skyboxProg;
    GLSLProgram pbrProg;

    int fuelCanCount;
    int fuelCansRemaining;

    float particleLifeTime;
    Random rand;
    GLuint initVel, startTime, particles, nParticles;
    glm::vec3 emitterPos, emitterDir;

    glm::vec3 carPos;
    float carAngle;
    glm::vec3 carForward;
    float carSpeed;
    float carRotSpeed;

    float timeElapsed;
    float delta;
    float previousTime;
    float time;
    float carFuelCount;
    float fuelLossRate;
    bool hasWon;

    void drawScene();
    void drawFloor();
    void drawCar(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);
    void drawFuelCan();

    void initBuffers();
    float randFloat();

    bool checkCollision(glm::vec3, float, glm::vec3, float);
    
    void setMatrices(GLSLProgram&);
    void setMatricesPlane();
    void setMatricesCar();
    void setMatricesSkybox();
    void setMatricesPbr();
    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
    void upPressed(float t);
    void downPressed(float t);
    void leftPressed();
    void rightPressed();
};

#endif // SCENEBASIC_UNIFORM_H
