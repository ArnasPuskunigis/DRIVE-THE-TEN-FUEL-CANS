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
    fuelPickup fuelCan;


    GLuint texCar;
    GLuint fuelTex;
    GLuint particleTex;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> fuelMesh;
    float time;
    GLSLProgram planeProg;
    GLSLProgram fuelCanProg;
    GLSLProgram skyboxProg;
    GLSLProgram pbrProg;

    glm::vec4 lightPos;

    void drawScene();
    void drawFloor();
    void drawCar(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);
    //void drawFuelCan(const glm::vec3& pos, float rough, int metal, const glm::vec3& color);


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

    float carFuelCount;
    float previousTime;
    float timeElapsed;
    float fuelLossRate;
    
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
    void upPressed();
    void downPressed();
    void leftPressed();
    void rightPressed();
};

#endif // SCENEBASIC_UNIFORM_H
