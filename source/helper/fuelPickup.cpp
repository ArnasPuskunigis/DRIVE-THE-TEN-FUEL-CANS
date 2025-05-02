#include "fuelPickup.h"
#include <glm/glm.hpp>
#include <iostream>

//Constructors
fuelPickup::fuelPickup()
    : position(0.0f), radius(2.0f), collected(false) {}

fuelPickup::fuelPickup(glm::vec3& pos, float radius)
    : position(pos), radius(radius), collected(false) {}



void fuelPickup::updatePosition(glm::vec3 pos) {
    position = pos;
}

bool fuelPickup::checkCollision(const glm::vec3& carPos, float carRadius) {
    glm::vec3 diff = carPos - position;
    float distSq = glm::dot(diff, diff);
    float combinedRadius = carRadius + radius;

    if (distSq < combinedRadius * combinedRadius) {
        collected = true;
        return true;
    }

    return false;
}

bool fuelPickup::isCollected() const {
    return collected;
}

