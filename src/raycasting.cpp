// #include "raycasting.hpp"  
// #include <limits>  

// Ray screen_to_world_ray(int mouse_x, int mouse_y, int screen_width, int screen_height,  
//                         const glm::mat4& view, const glm::mat4& projection) {  
//     // Convert screen coordinates to NDC  
//     float x = (2.0f * mouse_x) / screen_width - 1.0f;  
//     float y = 1.0f - (2.0f * mouse_y) / screen_height;  // FIXED Y-AXIS  
//     float z = 1.0f;  

//     glm::vec4 ray_clip(x, y, z, 1.0f);  
//     glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;  
//     ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);  // Direction vector  

//     glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));  
//     glm::vec3 ray_origin = glm::vec3(glm::inverse(view)[3]);  

//     return { ray_origin, ray_wor };  
// }  

// std::shared_ptr<GameObject> raycast(const Ray& ray,  
//                                    const std::vector<std::shared_ptr<GameObject>>& game_objects) {  
//     float closest_t = std::numeric_limits<float>::max();  
//     std::shared_ptr<GameObject> closest_obj = nullptr;  

//     for (const auto& obj : game_objects) {  
//         auto transform = obj->get_component<TransformComponent>();  
//         auto render_mesh = obj->get_component<RenderMeshComponent>();  
//         if (!transform || !render_mesh) continue;  

//         // Use scaled mesh bounds  
//         glm::vec3 scaled_min = transform->position + render_mesh->aabb_min * transform->scale;  
//         glm::vec3 scaled_max = transform->position + render_mesh->aabb_max * transform->scale;  

//         // Branchless slab method  
//         float tmin = -INFINITY, tmax = INFINITY;  

//         for (int axis = 0; axis < 3; axis++) {  
//             float inv_dir = 1.0f / (ray.direction[axis] + 1e-6f);  // Avoid division by zero  
//             float t0 = (scaled_min[axis] - ray.origin[axis]) * inv_dir;  
//             float t1 = (scaled_max[axis] - ray.origin[axis]) * inv_dir;  

//             if (inv_dir < 0.0f) std::swap(t0, t1);  
//             tmin = std::max(tmin, t0);  
//             tmax = std::min(tmax, t1);  

//             if (tmax <= tmin) break;  
//         }  

//         if (tmax > tmin && tmin < closest_t && tmin > 0.0f) {  
//             closest_t = tmin;  
//             closest_obj = obj;  
//         }  
//     }  

//     return closest_obj;  
// }  