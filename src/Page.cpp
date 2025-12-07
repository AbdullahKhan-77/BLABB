#define _CRT_SECURE_NO_WARNINGS
#include "Page.h"
#include "User.h"
#include <iostream>

// Helper Functions
static char* copyString(const char* str) {
    if (!str) return nullptr;
    char* newStr = new char[strlen(str) + 1];
    strcpy(newStr, str);
    return newStr;
}

Page::Page(const char* id, const char* title, User* owner) :
    Entity(id), title(copyString(title)), owner(owner),
    posts(nullptr), postsCount(0), likesCount(0) {
}

Page::~Page() {
    delete[] title;
    for (int i = 0; i < postsCount; i++) delete posts[i];
    delete[] posts;
}

const char* Page::getName() const { return title; }
const char* Page::getTitle() const { return title; }
User* Page::getOwner() const { return owner; }

void Page::addPost(Post* post) {
    Post** newPosts = new Post * [postsCount + 1];
    for (int i = 0; i < postsCount; i++) newPosts[i] = posts[i];
    newPosts[postsCount] = post;
    delete[] posts;
    posts = newPosts;
    postsCount++;
}

void Page::incrementLikes() { likesCount++; }
void Page::decrementLikes() { if (likesCount > 0) likesCount--; }

void Page::display() const {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Page ID: " << getId() << " - " << title << std::endl;
    std::cout << "Likes: " << likesCount << std::endl;

    if (postsCount == 0) {
        std::cout << "No posts to display" << std::endl;
        return;
    }
    for (int i = 0; i < postsCount; i++) {
        posts[i]->display();
    }
}

Post* const* Page::getPosts() const { return posts; }
int Page::getPostsCount() const { return postsCount; }
int Page::getLikesCount() const { return likesCount; }

