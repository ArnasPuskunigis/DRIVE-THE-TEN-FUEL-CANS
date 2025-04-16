#include <glm/glm.hpp>

class fuelPickup {
public:
    fuelPickup(const glm::vec3& pos, float radius);

    bool checkCollision(const glm::vec3& carPos, float carRadius);
    bool isCollected() const;

private:
    glm::vec3 position;
    float radius;
    bool collected;
};
