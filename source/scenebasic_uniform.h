#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"

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
    GLSLProgram planeProg;
    GLSLProgram carProg;
    GLSLProgram skyboxProg;
    void setMatricesPlane();
    void setMatricesCar();
    void setMatricesSkybox();
    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
    void upPressed();
    void downPressed();
    void spinToggle();
};

#endif // SCENEBASIC_UNIFORM_H
