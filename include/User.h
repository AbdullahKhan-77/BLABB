#ifndef USER_H
#define USER_H

#include "Entity.h"
#include "Page.h"
#include "Post.h"

class User : public Entity {
    char* name;
    char* password;
    User** friends;
    int friendsCount;
    Page** likedPages;
    int likedPagesCount;
    Post** posts;
    int postsCount;
    Page** ownedPages;
    int ownedPagesCount;
public:
    User(const char* id, const char* name, const char* password = "");
    ~User();

    const char* getName() const override;

    void addFriend(User* user);
    void likePage(Page* page);
    void addOwnedPage(Page* page);
    void addPost(Post* post);

    bool checkPassword(const char* pwd) const;

    void displayFriendList() const;
    void displayLikedPages() const;
    void displayTimeline() const;

    Post* const* getPosts() const;
    int getPostsCount() const;
    User* const* getFriends() const;
    int getFriendsCount() const;
    Page* const* getLikedPages() const;
    int getLikedPagesCount() const;
    Page* const* getOwnedPages() const;
    int getOwnedPagesCount() const;
};

#endif
