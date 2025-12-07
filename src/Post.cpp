#define _CRT_SECURE_NO_WARNINGS 
#include "Post.h"
#include <iostream>
#include <cstring>


// Helper Functions
static char* copyString(const char* str) {
    if (!str) return nullptr;
    char* newStr = new char[strlen(str) + 1];
    strcpy(newStr, str);
    return newStr;
}

static char* getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char* date = new char[11];
    sprintf(date, "%02d/%02d/%04d", ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
    return date;
}

Post::Post(const char* id, const char* text, Activity* activity) :
    Entity(id), text(copyString(text)), likes(nullptr), likesCount(0),
    comments(nullptr), commentsCount(0), sharedDate(getCurrentDate()),
    activity(activity), isMemory(false), originalPost(nullptr) {
}

Post::Post(const char* id, const char* text, const Post* originalPost) :
    Entity(id), text(copyString(text)), likes(nullptr), likesCount(0),
    comments(nullptr), commentsCount(0), sharedDate(getCurrentDate()),
    activity(nullptr), isMemory(true), originalPost(originalPost) {
}

Post::~Post() {
    delete[] text;
    delete[] sharedDate;
    delete activity;
    delete[] likes;

    for (int i = 0; i < commentsCount; i++) delete comments[i];
    delete[] comments;
}

void Post::addLike(const Entity* entity) {
    if (likesCount >= 10) return;

    for (int i = 0; i < likesCount; i++) {
        if (strcmp(likes[i]->getId(), entity->getId()) == 0) return;
    }

    const Entity** newLikes = new const Entity * [likesCount + 1];
    for (int i = 0; i < likesCount; i++) newLikes[i] = likes[i];
    newLikes[likesCount] = entity;

    delete[] likes;
    likes = newLikes;
    likesCount++;
}

void Post::addComment(Comment* comment) {
    if (commentsCount >= 10) return;

    Comment** newComments = new Comment * [commentsCount + 1];
    for (int i = 0; i < commentsCount; i++) newComments[i] = comments[i];
    newComments[commentsCount] = comment;

    delete[] comments;
    comments = newComments;
    commentsCount++;
}

void Post::display() const {
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << "Post ID: " << id << std::endl;
    std::cout << "Author: " << getName() << std::endl;

    if (activity) {
        std::cout << " is " << activity->getActivityText();
    }
    std::cout << std::endl;

    std::cout << "\"" << text << "\"" << std::endl;
    std::cout << "Posted on: " << sharedDate << std::endl;

    std::cout << "\nLikes (" << likesCount << "): ";
    if (likesCount > 0) {
        for (int i = 0; i < likesCount; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << likes[i]->getName();
        }
    }
    else {
        std::cout << "No likes yet";
    }
    std::cout << std::endl;

    std::cout << "\nComments (" << commentsCount << "):" << std::endl;
    if (commentsCount > 0) {
        for (int i = 0; i < commentsCount; i++) {
            comments[i]->display();
        }
    }
    else {
        std::cout << "No comments yet" << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;
}

const char* Post::getName() const { return "Post"; }
const char* Post::getText() const { return text; }
const Activity* Post::getActivity() const { return activity; }
int Post::getCommentsCount() const { return commentsCount; }
Comment* const* Post::getComments() const { return comments; }
const char* Post::getSharedDate() const { return sharedDate; }
void Post::setSharedDate(const char* date) {
    delete[] sharedDate;
    sharedDate = copyString(date);
}
int Post::getLikesCount() const { return likesCount; }
const Entity* const* Post::getLikes() const { return likes; }
bool Post::getIsMemory() const { return isMemory; }
const Post* Post::getOriginalPost() const { return originalPost; }
