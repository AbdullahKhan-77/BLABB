#ifndef COMMENT_H
#define COMMENT_H

#include "Entity.h"

class Comment {
    char* text;
    const Entity* author;
    char* timestamp;
public:
    Comment(const char* text, const Entity* author);
    ~Comment();
    const char* getText() const;
    const char* getTimestamp() const;
    const Entity* getAuthor() const;
    void display() const;
};

#endif