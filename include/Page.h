#ifndef PAGE_H
#define PAGE_H

#include "Entity.h"
#include "Post.h"

class User;

class Page : public Entity {
    char* title;
    User* owner;
    Post** posts;
    int postsCount;
    int likesCount;
public:
    Page(const char* id, const char* title, User* owner);
    ~Page();

    const char* getName() const override;
    const char* getTitle() const;
    User* getOwner() const;

    void addPost(Post* post);
    void incrementLikes();
    void decrementLikes();
    void display() const;

    Post* const* getPosts() const;
    int getPostsCount() const;
    int getLikesCount() const;
};

#endif