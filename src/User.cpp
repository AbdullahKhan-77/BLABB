#define _CRT_SECURE_NO_WARNINGS
#include "User.h"
#include "Page.h"
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


User::User(const char* id, const char* name, const char* password) :
    Entity(id), name(copyString(name)), password(copyString(password)),
    friends(nullptr), friendsCount(0), likedPages(nullptr), likedPagesCount(0),
    posts(nullptr), postsCount(0), ownedPages(nullptr), ownedPagesCount(0) {
}

User::~User() {
    if(name)
    delete[] name;
    if(password)
    delete[] password;
	if (friends)
    delete[] friends;
	if (likedPages)
    delete[] likedPages;
	if(posts)
    delete[] posts;
	if (ownedPages)
    delete[] ownedPages;
}

const char* User::getName() const { return name; }

void User::addFriend(User* user) {
    if (user == this) return;

    // Check if already friends
    for (int i = 0; i < friendsCount; i++) {
        if (strcmp(friends[i]->getId(), user->getId()) == 0) return;
    }

    // Add to this user's friend list
    User** newFriends = new User * [friendsCount + 1];
    for (int i = 0; i < friendsCount; i++) newFriends[i] = friends[i];
    newFriends[friendsCount] = user;
    delete[] friends;
    friends = newFriends;
    friendsCount++;

    // Add to the other user's friend list (reciprocal)
    User** userNewFriends = new User * [user->friendsCount + 1];
    for (int i = 0; i < user->friendsCount; i++) userNewFriends[i] = user->friends[i];
    userNewFriends[user->friendsCount] = this;
    delete[] user->friends;
    user->friends = userNewFriends;
    user->friendsCount++;
}


void User::likePage(Page* page) {
    if (!page) return;

    for (int i = 0; i < likedPagesCount; i++) {
        if (strcmp(likedPages[i]->getId(), page->getId()) == 0) {
            return;
        }
    }

    Page** newLikedPages = new Page * [likedPagesCount + 1];
    for (int i = 0; i < likedPagesCount; i++) newLikedPages[i] = likedPages[i];
    newLikedPages[likedPagesCount] = page;

    delete[] likedPages;
    likedPages = newLikedPages;
    likedPagesCount++;

    page->incrementLikes();
}

void User::addOwnedPage(Page* page) {
    Page** newOwnedPages = new Page * [ownedPagesCount + 1];
    for (int i = 0; i < ownedPagesCount; i++) newOwnedPages[i] = ownedPages[i];
    newOwnedPages[ownedPagesCount] = page;
    delete[] ownedPages;
    ownedPages = newOwnedPages;
    ownedPagesCount++;
}

void User::addPost(Post* post) {
    Post** newPosts = new Post * [postsCount + 1];
    for (int i = 0; i < postsCount; i++) newPosts[i] = posts[i];
    newPosts[postsCount] = post;
    delete[] posts;
    posts = newPosts;
    postsCount++;
}

bool User::checkPassword(const char* pwd) const {
    return strcmp(password, pwd) == 0;
}

void User::displayFriendList() const {
    std::cout << name << " - Friend List" << std::endl;
    if (friendsCount == 0) {
        std::cout << "No friends to display" << std::endl;
        return;
    }
    for (int i = 0; i < friendsCount; i++) {
        std::cout << friends[i]->getId() << " - " << friends[i]->getName() << std::endl;
    }
}

void User::displayLikedPages() const {
    std::cout << name << " - Liked Pages" << std::endl;
    if (likedPagesCount == 0) {
        std::cout << "No liked pages to display" << std::endl;
        return;
    }
    for (int i = 0; i < likedPagesCount; i++) {
        std::cout << likedPages[i]->getId() << " - " << likedPages[i]->getTitle() << std::endl;
    }
}

void User::displayTimeline() const {
    std::cout << "\n=== " << name << "'s Timeline ===" << std::endl;
    if (postsCount == 0) {
        std::cout << "No posts to display" << std::endl;
        return;
    }

    for (int i = 0; i < postsCount; i++) {
        posts[i]->display();
        if (posts[i]->getIsMemory()) {
            std::cout << "\n[Memory of]:";
            posts[i]->getOriginalPost()->display();
        }
    }
}

Post* const* User::getPosts() const { return posts; }
int User::getPostsCount() const { return postsCount; }
User* const* User::getFriends() const { return friends; }
int User::getFriendsCount() const { return friendsCount; }
Page* const* User::getLikedPages() const { return likedPages; }
int User::getLikedPagesCount() const { return likedPagesCount; }
Page* const* User::getOwnedPages() const { return ownedPages; }
int User::getOwnedPagesCount() const { return ownedPagesCount; }
