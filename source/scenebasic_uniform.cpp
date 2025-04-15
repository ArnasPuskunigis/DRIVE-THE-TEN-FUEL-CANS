#include "scenebasic_uniform.h"
#include <cstdio>
#include <cstdlib>
#include "helper/texture.h"
#include <string>
using std::string;

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
using namespace glm;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() :
    //sky(100.0f),
    plane(100.0f, 100.0f, 1, 1), 
    tPrev(0),
    tPrevPbr(0.0f),
    lightPos(vec4(5.0f, 5.0f, 5.0f, 1.0f))

    {
    mesh = ObjMesh::load("media/f1.obj", true);
    texTiles = Texture::loadTexture("media/texture/tiles_d.png");
    texRust = Texture::loadTexture("media/texture/rust.png");
    texCar = Texture::loadTexture("media/texture/f1d.png");
    }

void SceneBasic_Uniform::initScene()
{
    compile();

    ////Skybox
    //skyboxProg.use();

    //skyboxProg.setUniform("Fog.MaxDist", 75.0f);
    //skyboxProg.setUniform("Fog.MinDist", 1.0f);
    //skyboxProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/place/place");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);

    //Camera + projection
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    angle = 0.0f;
    spin = true;
    camDistance = 10.0f;

    ////Plane
    //planeProg.use();
    //planeProg.setUniform("Spot.L", vec3(0.5f));
    //planeProg.setUniform("Spot.La", vec3(0.5f));
    //planeProg.setUniform("Spot.Exponent", 0.5f);
    //planeProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

    //planeProg.setUniform("Fog.MaxDist", 30.0f);
    //planeProg.setUniform("Fog.MinDist", 1.0f);
    //planeProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    //pbr
    prog.use();
    view = lookAt(
        vec3(0.0f, 4.0f, 7.0f),
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
    );


    projection = perspective(radians(50.0f), (float)width / height, 0.5f, 100.0f);
    lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;
    
    prog.use();
    prog.setUniform("Light[0].L", vec3(45.0f));
    prog.setUniform("Light[0].Position", view * lightPos);
    prog.setUniform("Light[1].L", vec3(0.3f));
    prog.setUniform("Light[1].Position", vec4(0.0f, 0.15f, -1.0f, 0.0f));
    prog.setUniform("Light[2].L", vec3(45.0f));
    prog.setUniform("Light[2].Position", vec4(-7.0f, 3.0f, 7.0f, 1.0f));

    //Car
   /* carProg.use();
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    carProg.setUniform("Spot.L", vec3(0.5f));
    carProg.setUniform("Spot.La", vec3(0.5f));
    carProg.setUniform("Spot.Exponent", 0.5f);
    carProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

    carProg.setUniform("Fog.MaxDist", 30.0f);
    carProg.setUniform("Fog.MinDist", 1.0f);
    carProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));*/


}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/pbr.vert");
        prog.compileShader("shader/pbr.frag");
        prog.link();

        //planeProg.compileShader("shader/multiTextureSpotToon.vert");
        //planeProg.compileShader("shader/multiTextureSpotToon.frag");
        //planeProg.link();

        /*carProg.compileShader("shader/toonSpotSingleColor.vert");
        carProg.compileShader("shader/toonSpotSingleColor.frag");
        carProg.link();*/

        //skyboxProg.compileShader("shader/skyBox.vert");
        //skyboxProg.compileShader("shader/skyBox.frag");
        //skyboxProg.link();
    }
    catch (GLSLProgramException& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
    }
}

void SceneBasic_Uniform::update(float t)
{
    //if (spin) {
    //    float deltaT = t - tPrev;
    //    if (tPrev == 0.0f) deltaT = 0.0f;
    //    tPrev = t;
    //    angle += 0.2f * deltaT;
    //    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    //}

    float deltaT = t - tPrevPbr;
    if (tPrevPbr == 0.0f) deltaT = 0.0f;
    tPrevPbr = t;

    lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, two_pi<float>());
    lightPos.x = cos(lightAngle) * 7.0f;
    lightPos.y = 3.0f;
    lightPos.z = sin(lightAngle) * 7.0f;

}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ////Skybox
    //skyboxProg.use();
    //model = mat4(1.0f);
    //model = glm::translate(model, vec3(0.0f, 10.0f, 0.0f));
    //setMatricesSkybox();
    //sky.render();

    //Camera
    view = glm::lookAt(vec3(0.0f, 4.0f, camDistance), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec4 lightPos = vec4(0.0f, 20.0f, 5.0f, 1.0f);
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));

    ////Plane
    //planeProg.use();
    //planeProg.setUniform("Spot.Position", vec3(view * lightPos));
    //planeProg.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));
    //planeProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 0.0f));
    //planeProg.setUniform("Material.Ks", vec3(0.1f, 0.1f, 0.1f));
    //planeProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    //planeProg.setUniform("Material.Shininess", 100.0f);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, texTiles);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, texRust);
    //model = mat4(1.0f);
    //setMatricesPlane();
    //plane.render();

    prog.use();
    prog.setUniform("Light[0].Position", view * lightPos);
    drawScene();

    //Car
    /*carProg.use();
    carProg.setUniform("Spot.Position", vec3(view * lightPos));
    carProg.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    carProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 1.0f));
    carProg.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    carProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    carProg.setUniform("Material.Shininess", 100.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texCar);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle * 100.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
    model = glm::scale(model, vec3(0.02f));
    setMatricesCar();
    mesh->render();*/
    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::drawScene()
{
    drawFloor();

    int numCars = 9;
    vec3 baseColor(0.1f, 0.33f, 0.97f);

    for (int i = 0; i < numCars; i++) {
        float carX = i * (20.0f / (numCars - 1)) - 5.0f;
        float rough = (i + 1) * (1.0f / numCars);
        drawSpot(vec3(carX, 0.0f, 0.0f), rough, 0, baseColor);
    }

    float metalRough = 0.43f;
    drawSpot(vec3(-10.0f, 0.0f, 3.0f), metalRough, 1, vec3(1.0f, 0.71f, 0.29f));
    drawSpot(vec3(-5.0f, 0.0f, 3.0f), metalRough, 1, vec3(0.95f, 0.71f, 0.29f));
    drawSpot(vec3(-0.0f, 0.0f, 3.0f), metalRough, 1, vec3(0.91f, 0.71f, 0.29f));
    drawSpot(vec3(5.0f, 0.0f, 3.0f), metalRough, 1, vec3(0.542f, 0.71f, 0.29f));
    drawSpot(vec3(10.0f, 0.0f, 3.0f), metalRough, 1, vec3(0.95f, 0.71f, 0.29f));

}

void SceneBasic_Uniform::drawFloor()
{
    prog.use();
    prog.setUniform("Material.Rough", 0.9f);
    prog.setUniform("Material.Metal", 0.0f);
    prog.setUniform("Material.Color", vec3(0.2f));

    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, -0.75f, 0.0f));
    setMatricesPlane();
    plane.render();
}

void SceneBasic_Uniform::drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& color)
{
    prog.use();
    prog.setUniform("Material.Rough", rough);
    prog.setUniform("Material.Metal", metal);
    prog.setUniform("Material.Color", color);

    model = mat4(1.0f);
    model = translate(model, pos);
    model = rotate(model, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.02f));
    setMatricesPbr();
    mesh->render();
}

void SceneBasic_Uniform::setMatricesPlane() {
    prog.use();
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesPbr() {
    prog.use();
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

//void SceneBasic_Uniform::setMatricesCar() {
//    carProg.use();
//    mat4 mv = view * model;
//    carProg.setUniform("ModelViewMatrix", mv);
//    carProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
//    carProg.setUniform("MVP", projection * mv);
//}

void SceneBasic_Uniform::setMatricesSkybox() {
    skyboxProg.use();

    skyboxProg.setUniform("ModelMatrix", model);
    skyboxProg.setUniform("ViewMatrix", view);
    skyboxProg.setUniform("ProjectionMatrix", projection);
    skyboxProg.setUniform("CameraPos", glm::vec3(glm::inverse(view)[3]));

}

void SceneBasic_Uniform::upPressed() {
    camDistance -= 0.1f;
}

void SceneBasic_Uniform::spinToggle() {
    if (spin) {
        spin = false;
    }
    else {
        spin = true;
    }
}

void SceneBasic_Uniform::downPressed() {
    camDistance += 0.1f;
}