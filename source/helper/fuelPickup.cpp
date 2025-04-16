#include "fuelPickup.h"
#include <glm/glm.hpp>
#include <iostream>

fuelPickup::fuelPickup(const glm::vec3& pos, float radius)
    : position(pos), radius(radius), collected(false) {}

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

