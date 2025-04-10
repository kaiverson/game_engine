#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <glm/glm.hpp>

class BoundingBox {
private:
    glm::vec3 min_ = glm::vec3(FLT_MAX);
    glm::vec3 max_ = glm::vec3(FLT_MIN);

public:
    void grow_to_include(glm::vec3 vertex) {
        min_.x = vertex.x < min_.x ? vertex.x : min_.x;
        min_.y = vertex.y < min_.y ? vertex.y : min_.y;
        min_.z = vertex.z < min_.z ? vertex.z : min_.z;

        max_.x = vertex.x < max_.x ? vertex.x : max_.x;
        max_.y = vertex.y < max_.y ? vertex.y : max_.y;
        max_.z = vertex.z < max_.z ? vertex.z : max_.z;
        
    }

    glm::vec3 get_center() {
        return (min_ + max_) * 0.5f;
    }

};

#endif /* BOUNDING_BOX_HPP */
