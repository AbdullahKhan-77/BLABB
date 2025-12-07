#ifndef POST_H
#define POST_H

#include "Entity.h"
#include "Activity.h"
#include "Comment.h"

class Post : public Entity {
    char* text;
    const Entity** likes;
    int likesCount;
    Comment** comments;
    int commentsCount;
    char* sharedDate;
    Activity* activity;
    bool isMemory;
    const Post* originalPost;
public:
    Post(const char* id, const char* text, Activity* activity = nullptr);
    Post(const char* id, const char* text, const Post* originalPost);
    ~Post();

    void addLike(const Entity* entity);
    void addComment(Comment* comment);
    void display() const;

    const char* getName() const override;
    const char* getText() const;
    const Activity* getActivity() const;
    int getCommentsCount() const;
    Comment* const* getComments() const;
    const char* getSharedDate() const;
    void setSharedDate(const char* date);
    int getLikesCount() const;
    const Entity* const* getLikes() const;
    bool getIsMemory() const;
    const Post* getOriginalPost() const;
};

#endif