#ifndef ENTITY_H
#define ENTITY_H

class Entity {
protected:
    char* id;
public:
    Entity(const char* id);
    virtual ~Entity();
    const char* getId() const;
    virtual const char* getName() const = 0;
};

#endif