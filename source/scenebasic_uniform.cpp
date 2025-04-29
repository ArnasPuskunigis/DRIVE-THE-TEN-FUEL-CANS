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
    sky(500.0f),
    plane(1000.0f, 1000.0f, 1, 1),
    fuelCan(vec3(10.0f, 1.0f, 10.0f), 2.0f),
    lightPos(vec4(10.0f, 5.0f, 10.0f, 1.0f)),
    particleLifeTime(300.0f),
    nParticles(800),
    emitterPos(250, 50, 0),
    emitterDir(90, 1, -120),
    carPos(0.0f, 1.0f, 0.0f),
    carForward(0.0f, 1.0f, 0.0f),
    carFuelCount(100.0f),
    fuelLossRate(1.0f)
    {
    mesh = ObjMesh::load("media/f1.obj", true);
    fuelMesh = ObjMesh::load("media/fuel_can.obj", true);
    texCar = Texture::loadTexture("media/texture/f1d.png");
    fuelTex = Texture::loadTexture("media/texture/fuel_can_Albedo.png");
    particleTex = Texture::loadTexture("media/texture/fuel_can_Albedo.png");
    }

void SceneBasic_Uniform::initScene()
{
    compile();

    //Skybox
    skyboxProg.use();

    skyboxProg.setUniform("Fog.MaxDist", 500.0f);
    skyboxProg.setUniform("Fog.MinDist", 1.0f);
    skyboxProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    

    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/place/place");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);


    //Particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);

    initBuffers();
    glActiveTexture(GL_TEXTURE0);
    particleTex = Texture::loadTexture("media/texture/smoke.png");

    particleProg.use();
    particleProg.setUniform("ParticleTex", 0);
    particleProg.setUniform("ParticleLifeTime", particleLifeTime);
    particleProg.setUniform("ParticleSize", 2.0f);
    particleProg.setUniform("Gravity", vec3(0.0f, 2.0f, 0.0f));
    particleProg.setUniform("EmitterPos", emitterPos);

    //Can
    fuelCanProg.use();
    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 10.0f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    fuelCanProg.setUniform("Spot.L", vec3(0.5f));
    fuelCanProg.setUniform("Spot.La", vec3(0.5f));
    fuelCanProg.setUniform("Spot.Exponent", 0.5f);
    fuelCanProg.setUniform("Spot.Cutoff", glm::radians(30.0f));

    fuelCanProg.setUniform("Fog.MaxDist", 50.0f);
    fuelCanProg.setUniform("Fog.MinDist", 1.0f);
    fuelCanProg.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    std::cout << "Car fuel count:" << carFuelCount << std::endl;

    //PBR Car + Plane
    pbrProg.use();
    view = lookAt(
        vec3(0.0f, 4.0f, 7.0f),
        vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
    );

    //Lighting for PBR
    pbrProg.use();
    pbrProg.setUniform("Light[0].L", vec3(45.0f));
    pbrProg.setUniform("Light[0].Position", view * lightPos);
    pbrProg.setUniform("Light[1].L", vec3(0.3f));
    pbrProg.setUniform("Light[1].Position", vec4(0.0f, 0.15f, -1.0f, 0.0f));
    pbrProg.setUniform("Light[2].L", vec3(45.0f));
    pbrProg.setUniform("Light[2].Position", vec4(-7.0f, 3.0f, 7.0f, 1.0f));

}

void SceneBasic_Uniform::compile()
{
    try {
        
        try {
            pbrProg.compileShader("shader/pbr.vert");
            pbrProg.compileShader("shader/pbr.frag");
            pbrProg.link();
            std::cout << "pbr shader fine!" << std::endl;
        }
        catch (GLSLProgramException& e) {
            std::cerr << "Error compiling: " << e.what() << std::endl;
        }

        try {
            particleProg.compileShader("shader/particle.vert");
            particleProg.compileShader("shader/particle.frag");
            particleProg.link();
            std::cout << "particle shader fine!" << std::endl;
        }
        catch (GLSLProgramException& e) {
            std::cerr << "Error compiling: " << e.what() << std::endl;
        }

        //planeProg.compileShader("shader/multiTextureSpotToon.vert");
        //planeProg.compileShader("shader/multiTextureSpotToon.frag");
        //planeProg.link();

        fuelCanProg.compileShader("shader/toonSpotSingleColor.vert");
        fuelCanProg.compileShader("shader/toonSpotSingleColor.frag");
        fuelCanProg.link();

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
    time = t;

    float delta = t - previousTime;
    timeElapsed += delta;

    if (timeElapsed >= fuelLossRate) {
        timeElapsed = 0.0f;
        carFuelCount -= 5.0f;
        std::cout << carFuelCount << std::endl;
    }

    previousTime = t;
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
    float camDistance = 15.0f;
    float camHeight = 5.0f;
    vec3 camPos = carPos - carForward * camDistance + glm::vec3(0, camHeight, 0);
    view = glm::lookAt(camPos, carPos + vec3(0, 1.0f, 0), vec3(0, 1, 0));

    
    
    //Fuel cans

    if (!fuelCan.isCollected()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fuelTex);
        model = mat4(1.0f);
        //model = glm::rotate(model, glm::radians(angle * 100.0f), vec3(0.0f, 1.0f, 0.0f));
        //model = glm::translate(model, vec3(0.0f, 2.0f, 0.0f));
        model = translate(model, vec3(10.0f, 1.0f, 10.0f));
        model = glm::scale(model, vec3(0.1f));
        setMatricesCar();
        drawFuelCan();
        fuelMesh->render();
        if (fuelCan.checkCollision(carPos, 2.0f)) {
            std::cout << "Fuel collected!" << std::endl;
            carFuelCount += 40.0f;
            std::cout << "Car fuel at: " << carFuelCount << std::endl;
        }
    }

    //Plane
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, particleTex);
    model = mat4(1.0f);
    drawFloor();

    //Scene (pbr car + floor + light)
    drawScene();
    pbrProg.setUniform("Light[0].Position", view * lightPos);


    //Particles
    glBindTexture(GL_TEXTURE_2D, particleTex);
    glDepthMask(GL_FALSE);
    particleProg.use();
    particleProg.setUniform("Time", time);
    setMatrices(particleProg);

    glBindVertexArray(particles);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

   

    
    
}

bool SceneBasic_Uniform::checkCollision(glm::vec3 posA, float radiusA, glm::vec3 posB, float radiusB) {
    float distance = glm::length(posA - posB);
    return distance < (radiusA + radiusB);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 1000.0f);
}

void SceneBasic_Uniform::drawScene()
{
    drawCar(vec3(carPos), 0.43f, 1, vec3(1.0f, 0.71f, 0.29f));
    //drawFuelCan(vec3(10.0f, 1.0f, 10.0f), 0.43f, 1, vec3(1.0f, 0.71f, 0.29f));
}

void SceneBasic_Uniform::drawFuelCan() 
{
    fuelCanProg.use();

    //mat4 mv = view * model;
    fuelCanProg.setUniform("Spot.Position", vec3(view * lightPos));
    //fuelCanProg.setUniform("Spot.Direction", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) * vec3(-lightPos)));

    fuelCanProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 1.0f));
    fuelCanProg.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    fuelCanProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    fuelCanProg.setUniform("Material.Shininess", 100.0f);

}

void SceneBasic_Uniform::drawFloor()
{
    fuelCanProg.use();

    //mat4 mv = view * model;
    fuelCanProg.setUniform("Spot.Position", vec3(view * lightPos));
    //fuelCanProg.setUniform("Spot.Direction", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) * vec3(-lightPos)));

    fuelCanProg.setUniform("Material.Kd", vec3(0.0f, 0.0f, 1.0f));
    fuelCanProg.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    fuelCanProg.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.2f));
    fuelCanProg.setUniform("Material.Shininess", 100.0f);
    
    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, -0.75f, 0.0f));
    setMatricesPlane();
    plane.render();
}
//
//void SceneBasic_Uniform::drawFloor()
//{
//    pbrProg.use();
//    pbrProg.setUniform("Material.Rough", 0.9f);
//    pbrProg.setUniform("Material.Metal", 0.0f);
//    pbrProg.setUniform("Material.Color", vec3(0.2f));
//
//    model = mat4(1.0f);
//    model = translate(model, vec3(0.0f, -0.75f, 0.0f));
//    setMatricesPlane();
//    plane.render();
//}

void SceneBasic_Uniform::drawCar(const glm::vec3& pos, float rough, int metal, const glm::vec3& color)
{
    pbrProg.use();
    pbrProg.setUniform("Material.Rough", rough);
    pbrProg.setUniform("Material.Metal", metal);
    pbrProg.setUniform("Material.Color", color);

    model = mat4(1.0f);
    model = translate(model, vec3(pos));
    model = rotate(model, radians(carAngle), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.02f));
    setMatricesPbr();
    mesh->render();
    carForward = -glm::normalize(glm::vec3(model[0]));
}

//void SceneBasic_Uniform::drawFuelCan(const glm::vec3& pos, float rough, int metal, const glm::vec3& color)
//{
//    pbrProg.use();
//    pbrProg.setUniform("Material.Rough", rough);
//    pbrProg.setUniform("Material.Metal", metal);
//    pbrProg.setUniform("Material.Color", color);
//
//    model = mat4(1.0f);
//    model = translate(model, vec3(pos));
//    //model = rotate(model, radians(carAngle), vec3(0.0f, 1.0f, 0.0f));
//    model = glm::scale(model, vec3(0.02f));
//    setMatricesPbr();
//    fuelMesh->render();
//    //carForward = -glm::normalize(glm::vec3(model[0]));
//}

void SceneBasic_Uniform::setMatricesPlane() {
    fuelCanProg.use();
    mat4 mv = view * model;
    fuelCanProg.setUniform("ModelViewMatrix", mv);
    fuelCanProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    fuelCanProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesPbr() {
    pbrProg.use();
    
    mat4 mv = view * model;
    pbrProg.setUniform("ModelViewMatrix", mv);
    pbrProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    pbrProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesCar() {
    fuelCanProg.use();
    mat4 mv = view * model;
    fuelCanProg.setUniform("ModelViewMatrix", mv);
    fuelCanProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    fuelCanProg.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setMatricesSkybox() {
    skyboxProg.use();

    skyboxProg.setUniform("ModelMatrix", model);
    skyboxProg.setUniform("ViewMatrix", view);
    skyboxProg.setUniform("ProjectionMatrix", projection);
    skyboxProg.setUniform("CameraPos", glm::vec3(glm::inverse(view)[3]));

}

void SceneBasic_Uniform::setMatrices(GLSLProgram &program) {
    mat4 mv = view * model;
    program.setUniform("ModelViewMatrix", mv);
    //program.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    program.setUniform("MVP", projection * mv);
    program.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::initBuffers() {

    glGenBuffers(1, &initVel);
    glGenBuffers(1, &startTime);
    int size = nParticles * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);

    for (uint32_t i = 0; i < nParticles; i++) {
        theta = glm::mix(0.0f, glm::pi<float>() / 4.0f, randFloat()); 
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        velocity = glm::mix(10.0f, 25.0f, randFloat());
        v = glm::normalize(emitterBasis * v) * velocity;

        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());
    float rate = particleLifeTime / nParticles;
    for (int i = 0; i < nParticles; i++) {
        data[i] = rate * i;
    }
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());
    //glBindBuffer(GL_ARRAY_BUFFER, 1);
    glGenVertexArrays(1, &particles);
    glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glBindVertexArray(0);
}

float SceneBasic_Uniform::randFloat() {
    return rand.nextFloat();
}

void SceneBasic_Uniform::upPressed() {
    carPos += carForward * 0.1f;
}

void SceneBasic_Uniform::downPressed() {
    carPos -= carForward * 0.1f;
}

void SceneBasic_Uniform::leftPressed() {
    carAngle += 1.0f;

}

void SceneBasic_Uniform::rightPressed() {
    carAngle -= 1.0f;

}
