#include <glm/glm.hpp>

class fuelPickup {
public:
    fuelPickup();
    fuelPickup(glm::vec3& pos, float radius);

    bool checkCollision(const glm::vec3& carPos, float carRadius);
    bool isCollected() const;
    void updatePosition(glm::vec3 pos);

    glm::vec3 position;

private:
    float radius;
    bool collected;
};
