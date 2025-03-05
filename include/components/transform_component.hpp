#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "components/component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class TransformComponent : public Component {
private:
    glm::mat4 transform_;

public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    TransformComponent()
        : position(0.0f), rotation(glm::quat(glm::vec3(0.0f))), scale(1.0f) {}

    glm::vec3 get_front() const {
        return glm::mat3_cast(rotation) * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 get_up() const {
        return glm::mat3_cast(rotation) * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 get_right() const {
        return glm::mat3_cast(rotation) * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::mat4 get_transform() const {
        return transform_; 
    }
    
    void start(GameObject &game_object) override {}

    void update(GameObject &game_object) override {
        // Update the transform matrix of the game object
        transform_ = glm::translate(glm::mat4(1.0f), position) *
                    glm::mat4_cast(rotation) *
                    glm::scale(glm::mat4(1.0f), scale);
    }
};

#endif // TRANSFORM_COMPONENT_HPP