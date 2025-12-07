#define _CRT_SECURE_NO_WARNINGS
#include "Entity.h"
#include <cstring>

static char* copyString(const char* str) {
    if (!str) return nullptr;
    char* newStr = new char[strlen(str) + 1];
    strcpy(newStr, str);
    return newStr;
}

Entity::Entity(const char* id) : id(copyString(id)) {}

Entity::~Entity() { delete[] id; }

const char* Entity::getId() const { return id; }
