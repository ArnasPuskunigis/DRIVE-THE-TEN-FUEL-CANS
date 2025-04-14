#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include "helper/texture.h"
#include <string>
using std::string;

#include <sstream>

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() :
    sky(100.0f),
    plane(100.0f, 100.0f, 1, 1), 
    tPrev(0){
    mesh = ObjMesh::load("media/f1.obj", true);
    texTiles = Texture::loadTexture("media/texture/tiles_d.png");
    texRust = Texture::loadTexture("media/texture/rust.png");
    texCar = Texture::loadTexture("media/texture/f1d.png");
}

void SceneBasic_Uniform::initScene()
{
    compile();

    //Skybox
    skyboxProg.use();

    skyboxProg.setUniform("Fog.MaxDist", 75.0f);
    skyboxProg.setUniform("Fog.MinDist", 1.0f);
    skyboxProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/place/place");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    glEnable(GL_DEPTH_TEST);

    //Camera + projection
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    angle = 0.0f;
    spin = true;
    camDistance = 10.0f;

    //Plane
    planeProg.use();
    planeProg.setUniform("Spot.L", vec3(0.5f));
    planeProg.setUniform("Spot.La", vec3(0.5f));
    planeProg.setUniform("Spot.Exponent", 0.5f);
    planeProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

    planeProg.setUniform("Fog.MaxDist", 30.0f);
    planeProg.setUniform("Fog.MinDist", 1.0f);
    planeProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    //Car
    carProg.use();
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    carProg.setUniform("Spot.L", vec3(0.5f));
    carProg.setUniform("Spot.La", vec3(0.5f));
    carProg.setUniform("Spot.Exponent", 0.5f);
    carProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

    carProg.setUniform("Fog.MaxDist", 30.0f);
    carProg.setUniform("Fog.MinDist", 1.0f);
    carProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));


}

void SceneBasic_Uniform::compile()
{
    try {
        planeProg.compileShader("shader/multiTextureSpotToon.vert");
        planeProg.compileShader("shader/multiTextureSpotToon.frag");
        planeProg.link();

        carProg.compileShader("shader/toonSpotSingleColor.vert");
        carProg.compileShader("shader/toonSpotSingleColor.frag");
        carProg.link();

        skyboxProg.compileShader("shader/skyBox.vert");
        skyboxProg.compileShader("shader/skyBox.frag");
        skyboxProg.link();
    }
    catch (GLSLProgramException& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
    }
}

void SceneBasic_Uniform::update(float t)
{
    if (spin) {
        float deltaT = t - tPrev;
        if (tPrev == 0.0f) deltaT = 0.0f;
        tPrev = t;
        angle += 0.2f * deltaT;
        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Skybox
    skyboxProg.use();
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 10.0f, 0.0f));
    setMatricesSkybox();
    sky.render();

    //Camera
    view = glm::lookAt(vec3(0.0f, 4.0f, camDistance), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec4 lightPos = vec4(0.0f, 20.0f, 5.0f, 1.0f);
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));

    //Plane
    planeProg.use();
    planeProg.setUniform("Spot.Position", vec3(view * lightPos));
    planeProg.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));
    planeProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 0.0f));
    planeProg.setUniform("Material.Ks", vec3(0.1f, 0.1f, 0.1f));
    planeProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    planeProg.setUniform("Material.Shininess", 100.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texTiles);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texRust);
    model = mat4(1.0f);
    setMatricesPlane();
    plane.render();

    //Car
    carProg.use();
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
    mesh->render();
    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatricesPlane() {
    planeProg.use();
    mat4 mv = view * model;
    planeProg.setUniform("ModelViewMatrix", mv);
    planeProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    planeProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesCar() {
    carProg.use();
    mat4 mv = view * model;
    carProg.setUniform("ModelViewMatrix", mv);
    carProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    carProg.setUniform("MVP", projection * mv);
}

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