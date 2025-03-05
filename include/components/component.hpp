#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class GameObject; // Forward declaration

class Component {
public:
    virtual ~Component() = default;
    virtual void start(GameObject &game_object) {};
    virtual void update(GameObject &gameObject) {};
};

#endif // COMPONENT_HPP