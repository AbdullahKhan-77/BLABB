#define _CRT_SECURE_NO_WARNINGS
#include "SocialNetworkApp.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include "raylib.h"


SocialNetworkApp::SocialNetworkApp() :
    users(nullptr), usersCount(0),
    pages(nullptr), pagesCount(0),
    posts(nullptr), postsCount(0),
    currentUser(nullptr), currentPage(nullptr),
    systemDate(getCurrentDate()),
    authData(nullptr), authDataCount(0),
    currentState(LOGIN), showError(false) {
}

SocialNetworkApp::~SocialNetworkApp() {
    if (users)
        for (int i = 0; i < usersCount; i++) delete users[i];
    delete[] users;
    if (pages)
        for (int i = 0; i < pagesCount; i++) delete pages[i];
    delete[] pages;
    if (posts)
        for (int i = 0; i < postsCount; i++) delete posts[i];
    delete[] posts;
    if (systemDate)
        delete[] systemDate;
    if (authData)
    {
        for (int i = 0; i < authDataCount; i++) {
            delete[] authData[i].userId;
            delete[] authData[i].username;
            delete[] authData[i].password;
        }

        delete[] authData;
    }
}


// ... [All the existing methods remain the same until the run() method]
void SocialNetworkApp::addFriend() {
    if (!currentUser) {
        std::cout << "No user logged in" << std::endl;
        return;
    }

    char friendId[20];
    std::cout << "Enter friend's user ID: ";
    std::cin.getline(friendId, sizeof(friendId));

    User* friendUser = findUser(friendId);
    if (!friendUser) {
        std::cout << "User not found" << std::endl;
        return;
    }

    currentUser->addFriend(friendUser);
    std::cout << friendUser->getName() << " added as friend" << std::endl;
}

void SocialNetworkApp::loginAsPage() {
    if (!currentUser) {
        std::cout << "No user logged in" << std::endl;
        return;
    }

    if (currentUser->getOwnedPagesCount() == 0) {
        std::cout << "You don't own any pages" << std::endl;
        return;
    }

    std::cout << "Your Pages:" << std::endl;
    for (int i = 0; i < currentUser->getOwnedPagesCount(); i++) {
        std::cout << i + 1 << ". " << currentUser->getOwnedPages()[i]->getTitle() << std::endl;
    }

    int choice;
    std::cout << "Select page to login (1-" << currentUser->getOwnedPagesCount() << "): ";
    std::cin >> choice;
    std::cin.ignore();

    if (choice > 0 && choice <= currentUser->getOwnedPagesCount()) {
        currentPage = currentUser->getOwnedPages()[choice - 1];
        std::cout << "Now posting as " << currentPage->getTitle() << std::endl;
    }
}

void SocialNetworkApp::switchBackToUser() {
    if (!currentPage) {
        std::cout << "Not logged in as a page" << std::endl;
        return;
    }
    currentPage = nullptr;
    std::cout << "Switched back to user account" << std::endl;
}

void SocialNetworkApp::createPost() {
    Entity* author = currentPage ? static_cast<Entity*>(currentPage) : static_cast<Entity*>(currentUser);
    if (!author) {
        std::cout << "Not logged in" << std::endl;
        return;
    }

    char text[256];
    std::cout << "Enter post text: ";
    std::cin.getline(text, sizeof(text));

    int activityChoice;
    std::cout << "Add activity? (1-Yes, 0-No): ";
    std::cin >> activityChoice;
    std::cin.ignore();

    Activity* activity = nullptr;
    if (activityChoice == 1) {
        std::cout << "Select activity type:\n";
        std::cout << "1. Feeling\n";
        std::cout << "2. Thinking about\n";
        std::cout << "3. Making\n";
        std::cout << "4. Celebrating\n";
        std::cout << "Enter choice (1-4): ";
        int type;
        std::cin >> type;
        std::cin.ignore();

        char value[100];
        std::cout << "Enter activity value: ";
        std::cin.getline(value, sizeof(value));

        activity = new Activity(type, value);
    }

    char postId[20];
    sprintf(postId, "post%d", postsCount + 1);

    Post* newPost = addPost(postId, text, author, activity);
    if (newPost) {
        if (currentPage) {
            currentPage->addPost(newPost);
        }
        else {
            currentUser->addPost(newPost);
        }
        std::cout << "Post created successfully! Post ID: " << postId << std::endl;
    }
    else {
        std::cout << "Failed to create post" << std::endl;
        delete activity;
    }
}

User* SocialNetworkApp::addUser(const char* id, const char* name, const char* password = "") {
    if (findUser(id)) return nullptr;

    User** newUsers = new User * [usersCount + 1];
    for (int i = 0; i < usersCount; i++) newUsers[i] = users[i];
    newUsers[usersCount] = new User(id, name, password);

    delete[] users;
    users = newUsers;
    usersCount++;

    return newUsers[usersCount - 1];
}

Page* SocialNetworkApp::addPage(const char* id, const char* title, User* owner) {
    if (findPage(id)) return nullptr;

    Page** newPages = new Page * [pagesCount + 1];
    for (int i = 0; i < pagesCount; i++) newPages[i] = pages[i];
    newPages[pagesCount] = new Page(id, title, owner);

    delete[] pages;
    pages = newPages;
    pagesCount++;

    owner->addOwnedPage(newPages[pagesCount - 1]);
    return newPages[pagesCount - 1];
}

Post* SocialNetworkApp::addPost(const char* id, const char* text, Entity* author, Activity* activity = nullptr) {
    if (findPost(id)) return nullptr;

    Post** newPosts = new Post * [postsCount + 1];
    for (int i = 0; i < postsCount; i++) newPosts[i] = posts[i];
    newPosts[postsCount] = new Post(id, text, activity);

    delete[] posts;
    posts = newPosts;
    postsCount++;

    if (User* user = dynamic_cast<User*>(author)) {
        user->addPost(newPosts[postsCount - 1]);
    }
    else if (Page* page = dynamic_cast<Page*>(author)) {
        page->addPost(newPosts[postsCount - 1]);
    }

    return newPosts[postsCount - 1];
}

Post* SocialNetworkApp::addMemoryPost(const char* id, const char* text, const Post* originalPost, Entity* author) {
    if (findPost(id)) return nullptr;

    Post** newPosts = new Post * [postsCount + 1];
    for (int i = 0; i < postsCount; i++) newPosts[i] = posts[i];
    newPosts[postsCount] = new Post(id, text, originalPost);

    delete[] posts;
    posts = newPosts;
    postsCount++;

    if (User* user = dynamic_cast<User*>(author)) {
        user->addPost(newPosts[postsCount - 1]);
    }
    else if (Page* page = dynamic_cast<Page*>(author)) {
        page->addPost(newPosts[postsCount - 1]);
    }

    return newPosts[postsCount - 1];
}

User* SocialNetworkApp::findUser(const char* id) const {
    for (int i = 0; i < usersCount; i++) {
        if (strcmp(users[i]->getId(), id) == 0) return users[i];
    }
    return nullptr;
}

Page* SocialNetworkApp::findPage(const char* id) const {
    for (int i = 0; i < pagesCount; i++) {
        if (strcmp(pages[i]->getId(), id) == 0) return pages[i];
    }
    return nullptr;
}

Post* SocialNetworkApp::findPost(const char* id) const {
    for (int i = 0; i < postsCount; i++) {
        if (strcmp(posts[i]->getId(), id) == 0) return posts[i];
    }
    return nullptr;
}

bool SocialNetworkApp::signUp(const char* username, const char* password) {
    // Validate input
    if (!username || !password || strlen(username) < 3 || strlen(password) < 4) {
        std::cout << "Username must be at least 3 chars and password at least 4 chars" << std::endl;
        return false;
    }

    loadAuthData();

    // Check if username exists
    for (int i = 0; i < authDataCount; i++) {
        if (strcmp(authData[i].username, username) == 0) {
            std::cout << "Username already exists" << std::endl;
            return false;
        }
    }

    char userId[10];
    sprintf(userId, "u%d", usersCount + 1);

    User* newUser = addUser(userId, username, password);
    if (!newUser) {
        std::cout << "Failed to create user" << std::endl;
        return false;
    }

    // Add to auth data
    AuthData* newAuthData = new AuthData[authDataCount + 1];
    for (int i = 0; i < authDataCount; i++) newAuthData[i] = authData[i];

    newAuthData[authDataCount].userId = copyString(userId);
    newAuthData[authDataCount].username = copyString(username);
    newAuthData[authDataCount].password = copyString(password);

    delete[] authData;
    authData = newAuthData;
    authDataCount++;

    saveAuthData();
    saveUsers();

    // Set the newly created user as current user
    currentUser = newUser;
    std::cout << "Account created successfully! User ID: " << userId << std::endl;

    // Clear password input buffer for security
    memset(passwordInput, 0, sizeof(passwordInput));

    return true;
}
bool SocialNetworkApp::login(const char* username, const char* password) {
    loadAuthData();

    for (int i = 0; i < authDataCount; i++) {
        if (strcmp(authData[i].username, username) == 0 &&
            strcmp(authData[i].password, password) == 0) {

            currentUser = findUser(authData[i].userId);
            if (currentUser) {
                std::cout << "Login successful! Welcome, " << username << std::endl;
                return true;
            }
        }
    }

    std::cout << "Invalid username or password" << std::endl;
    return false;
}

void SocialNetworkApp:: logout() {
    if (currentUser || currentPage) {
        currentUser = nullptr;
        currentPage = nullptr;
        currentState = LOGIN;
        memset(usernameInput, 0, sizeof(usernameInput));
        memset(passwordInput, 0, sizeof(passwordInput));
        showError = false;
    }
}

void SocialNetworkApp::setCurrentUser(const char* userId) {
    User* user = findUser(userId);
    if (user) {
        currentUser = user;
        std::cout << user->getName() << " set as current user" << std::endl;
    }
    else {
        std::cout << "User not found" << std::endl;
    }
}

void SocialNetworkApp::setSystemDate(const char* date) {
    delete[] systemDate;
    systemDate = copyString(date);
    std::cout << "System date set to " << systemDate << std::endl;
}

void SocialNetworkApp::viewHome() const {
    if (!currentUser) {
        std::cout << "No user logged in" << std::endl;
        return;
    }

    std::cout << currentUser->getName() << "'s Home" << std::endl;

    // Friends' posts
    std::cout << "\nFriends' Posts:" << std::endl;
    bool hasPosts = false;
    for (int i = 0; i < currentUser->getFriendsCount(); i++) {
        User* friendUser = currentUser->getFriends()[i];
        for (int j = 0; j < friendUser->getPostsCount(); j++) {
            friendUser->getPosts()[j]->display();
            hasPosts = true;
        }
    }
    if (!hasPosts) std::cout << "No posts from friends" << std::endl;

    // Liked pages' posts
    std::cout << "\nLiked Pages' Posts:" << std::endl;
    hasPosts = false;
    for (int i = 0; i < currentUser->getLikedPagesCount(); i++) {
        Page* page = currentUser->getLikedPages()[i];
        for (int j = 0; j < page->getPostsCount(); j++) {
            page->getPosts()[j]->display();
            hasPosts = true;
        }
    }
    if (!hasPosts) std::cout << "No posts from liked pages" << std::endl;
}

void SocialNetworkApp::viewTimeline() const {
    Entity* entity = currentPage ? static_cast<Entity*>(currentPage) : static_cast<Entity*>(currentUser);
    if (!entity) {
        std::cout << "Not logged in" << std::endl;
        return;
    }

    if (User* user = dynamic_cast<User*>(entity)) {
        std::cout << "\n=== " << user->getName() << "'s Timeline ===" << std::endl;
        if (user->getPostsCount() == 0) {
            std::cout << "No posts to display" << std::endl;
            return;
        }
        for (int i = 0; i < user->getPostsCount(); i++) {
            user->getPosts()[i]->display();
            if (user->getPosts()[i]->getIsMemory()) {
                std::cout << "\n[Memory of]:" << std::endl;
                user->getPosts()[i]->getOriginalPost()->display();
            }
        }
    }
    else if (Page* page = dynamic_cast<Page*>(entity)) {
        std::cout << "\n=== " << page->getTitle() << "'s Timeline ===" << std::endl;
        if (page->getPostsCount() == 0) {
            std::cout << "No posts to display" << std::endl;
            return;
        }
        for (int i = 0; i < page->getPostsCount(); i++) {
            page->getPosts()[i]->display();
        }
    }
}
//void SocialNetworkApp::createPage() {
//    if (!currentUser) {
//        std::cout << "No user logged in" << std::endl;
//        return;
//    }
//
//    char title[100];
//    std::cout << "Enter page title: ";
//    std::cin.getline(title, sizeof(title));
//
//    char pageId[20];
//    sprintf(pageId, "p%d", pagesCount + 1);
//
//    Page* newPage = addPage(pageId, title, currentUser);
//    if (newPage) {
//        std::cout << "Page created successfully! ID: " << pageId << std::endl;
//    }
//    else {
//        std::cout << "Failed to create page" << std::endl;
//    }
//}

//void SocialNetworkApp::loginAsPage() {
//    if (!currentUser) {
//        std::cout << "No user logged in" << std::endl;
//        return;
//    }
//
//    if (currentUser->getOwnedPagesCount() == 0) {
//        std::cout << "You don't own any pages" << std::endl;
//        return;
//    }
//
//    std::cout << "Your Pages:" << std::endl;
//    for (int i = 0; i < currentUser->getOwnedPagesCount(); i++) {
//        std::cout << i + 1 << ". " << currentUser->getOwnedPages()[i]->getTitle() << std::endl;
//    }
//
//    int choice;
//    std::cout << "Select page to login (1-" << currentUser->getOwnedPagesCount() << "): ";
//    std::cin >> choice;
//    std::cin.ignore();
//
//    if (choice > 0 && choice <= currentUser->getOwnedPagesCount()) {
//        currentPage = currentUser->getOwnedPages()[choice - 1];
//        std::cout << "Now posting as " << currentPage->getTitle() << std::endl;
//    }
//}

void SocialNetworkApp::viewFriendList(const char* userId) const {
    User* user = findUser(userId);
    if (user) {
        user->displayFriendList();
    }
    else {
        std::cout << "User not found" << std::endl;
    }
}

void SocialNetworkApp::viewLikedPages(const char* userId) const {
    User* user = findUser(userId);
    if (user) {
        user->displayLikedPages();
    }
    else {
        std::cout << "User not found" << std::endl;
    }
}

void SocialNetworkApp::viewPage(const char* pageId) const {
    Page* page = findPage(pageId);
    if (page) {
        page->display();
    }
    else {
        std::cout << "Page not found" << std::endl;
    }
}

void SocialNetworkApp::likePost(const char* postId) {
    Entity* liker = currentPage ? static_cast<Entity*>(currentPage) : static_cast<Entity*>(currentUser);
    if (!liker) {
        std::cout << "Not logged in" << std::endl;
        return;
    }

    Post* post = findPost(postId);
    if (post) {
        post->addLike(liker);
        std::cout << liker->getName() << " liked post " << postId << std::endl;
    }
    else {
        std::cout << "Post not found" << std::endl;
    }
}

void SocialNetworkApp::viewPostLikes(const char* postId) const {
    Post* post = findPost(postId);
    if (!post) {
        std::cout << "Post not found" << std::endl;
        return;
    }

    std::cout << "Post " << postId << " liked by:" << std::endl;
    if (post->getLikesCount() == 0) {
        std::cout << "No likes yet" << std::endl;
        return;
    }

    for (int i = 0; i < post->getLikesCount(); i++) {
        std::cout << post->getLikes()[i]->getName() << std::endl;
    }
}

void SocialNetworkApp::commentOnPost(const char* postId, const char* commentText) {
    Entity* author = currentPage ? static_cast<Entity*>(currentPage) : static_cast<Entity*>(currentUser);
    if (!author) {
        std::cout << "Not logged in" << std::endl;
        return;
    }

    Post* post = findPost(postId);
    if (post) {
        Comment* comment = new Comment(commentText, author);
        post->addComment(comment);
        std::cout << author->getName() << " commented on post " << postId << std::endl;
    }
    else {
        std::cout << "Post not found" << std::endl;
    }
}

void SocialNetworkApp::viewPost(const char* postId) const {
    Post* post = findPost(postId);
    if (post) {
        post->display();
    }
    else {
        std::cout << "Post not found" << std::endl;
    }
}

void SocialNetworkApp::shareMemory(const char* postId, const char* memoryText) {
    Entity* author = currentPage ? static_cast<Entity*>(currentPage) : static_cast<Entity*>(currentUser);
    if (!author) {
        std::cout << "Not logged in" << std::endl;
        return;
    }

    Post* originalPost = findPost(postId);
    if (!originalPost) {
        std::cout << "Original post not found" << std::endl;
        return;
    }

    char memoryId[20];
    sprintf(memoryId, "memory_%s", postId);

    Post* memoryPost = addMemoryPost(memoryId, memoryText, originalPost, author);
    if (memoryPost) {
        std::cout << author->getName() << " shared a memory of post " << postId << std::endl;
    }
}

void SocialNetworkApp::createPage() {
    if (!currentUser) {
        std::cout << "No user logged in" << std::endl;
        return;
    }

    char title[100];
    std::cout << "Enter page title: ";
    std::cin.getline(title, sizeof(title));

    char pageId[20];
    sprintf(pageId, "p%d", pagesCount + 1);

    Page* newPage = addPage(pageId, title, currentUser);
    if (newPage) {
        std::cout << "Page created successfully! ID: " << pageId << std::endl;
    }
    else {
        std::cout << "Failed to create page" << std::endl;
    }
}

void SocialNetworkApp::listAllPosts() const {
    if (!currentUser) {
        std::cout << "No user logged in" << std::endl;
        return;
    }

    std::cout << "\nAll Posts:" << std::endl;
    for (int i = 0; i < postsCount; i++) {
        std::cout << posts[i]->getId() << " - ";
        const char* text = posts[i]->getText();
        size_t len = strlen(text) > 20 ? 20 : strlen(text);
        for (int j = 0; j < len; j++) std::cout << text[j];
        if (strlen(text) > 20) std::cout << "...";
        std::cout << std::endl;
    }
}

void SocialNetworkApp::listAllPages() const {
    std::cout << "\nAll Pages:" << std::endl;
    for (int i = 0; i < pagesCount; i++) {
        std::cout << pages[i]->getId() << " - " << pages[i]->getTitle()
            << " (Owner: " << pages[i]->getOwner()->getName() << ")" << std::endl;
    }
}

void SocialNetworkApp::likePage() {
    if (!currentUser) {
        std::cout << "No user logged in" << std::endl;
        return;
    }

    listAllPages();

    char pageId[20];
    std::cout << "Enter page ID to like: ";
    std::cin.getline(pageId, sizeof(pageId));

    Page* page = findPage(pageId);
    if (page) {
        currentUser->likePage(page);
        std::cout << "Liked " << page->getTitle() << std::endl;
    }
    else {
        std::cout << "Page not found" << std::endl;
    }
}

// File I/O methods
void SocialNetworkApp::saveAuthData() const {
    std::ofstream file("auth_data.txt");
    if (file.is_open()) {
        for (int i = 0; i < authDataCount; i++) {
            file << authData[i].userId << ","
                << authData[i].username << ","
                << authData[i].password << "\n";
        }
        file.close();
    }
}

void SocialNetworkApp::loadAuthData() {
    std::ifstream file("auth_data.txt");
    if (!file.is_open()) return;

    // Clear existing data
    for (int i = 0; i < authDataCount; i++) {
        delete[] authData[i].userId;
        delete[] authData[i].username;
        delete[] authData[i].password;
    }
    delete[] authData;
    authData = nullptr;
    authDataCount = 0;

    char line[256];
    while (file.getline(line, sizeof(line))) {
        char* userId = strtok(line, ",");
        char* username = strtok(nullptr, ",");
        char* password = strtok(nullptr, ",");

        if (userId && username && password) {
            AuthData* newAuthData = new AuthData[authDataCount + 1];
            for (int i = 0; i < authDataCount; i++) newAuthData[i] = authData[i];

            newAuthData[authDataCount].userId = copyString(userId);
            newAuthData[authDataCount].username = copyString(username);
            newAuthData[authDataCount].password = copyString(password);

            delete[] authData;
            authData = newAuthData;
            authDataCount++;
        }
    }
    file.close();
}
void SocialNetworkApp::saveUsers() const {
    std::ofstream file("users.txt");
    if (file.is_open()) {
        for (int i = 0; i < usersCount; i++) {
            file << users[i]->getId() << "," << users[i]->getName() << "\n";
        }
        file.close();
    }
}

void SocialNetworkApp::loadUsers() {
    std::ifstream file("users.txt");
    if (file.is_open()) {
        char line[256];
        while (file.getline(line, sizeof(line))) {
            char* id = strtok(line, ",");
            char* name = strtok(nullptr, ",");

            if (id && name) {
                addUser(id, name);
            }
        }
        file.close();
    }
}

void SocialNetworkApp::savePages() const {
    std::ofstream file("pages.txt");
    if (file.is_open()) {
        for (int i = 0; i < pagesCount; i++) {
            file << pages[i]->getId() << ","
                << pages[i]->getTitle() << ","
                << pages[i]->getOwner()->getId() << "\n";
        }
        file.close();
    }
}

void SocialNetworkApp::loadPages() {
    std::ifstream file("pages.txt");
    if (file.is_open()) {
        char line[256];
        while (file.getline(line, sizeof(line))) {
            char* id = strtok(line, ",");
            char* title = strtok(nullptr, ",");
            char* ownerId = strtok(nullptr, ",");

            if (id && title && ownerId) {
                User* owner = findUser(ownerId);
                if (owner) {
                    addPage(id, title, owner);
                }
            }
        }
        file.close();
    }
}

void SocialNetworkApp::savePosts() const {
    std::ofstream file("posts.txt");
    if (file.is_open()) {
        for (int i = 0; i < postsCount; i++) {
            Post* post = posts[i];
            file << post->getId() << ","
                << post->getText() << ",";

            if (post->getActivity()) {
                file << "1," << post->getActivity()->getActivityText();
            }
            else {
                file << "0,";
            }

            file << "," << post->getSharedDate() << ","
                << (post->getIsMemory() ? "1" : "0") << ","
                << (post->getIsMemory() ? post->getOriginalPost()->getId() : "") << "\n";
        }
        file.close();
    }
}

void SocialNetworkApp::loadPosts() {
    std::ifstream file("posts.txt");
    if (file.is_open()) {
        char line[512];
        while (file.getline(line, sizeof(line))) {
            char* id = strtok(line, ",");
            char* text = strtok(nullptr, ",");
            char* hasActivity = strtok(nullptr, ",");
            char* activityValue = strtok(nullptr, ",");
            char* sharedDate = strtok(nullptr, ",");
            char* isMemory = strtok(nullptr, ",");
            char* originalPostId = strtok(nullptr, ",");

            if (id && text) {
                Activity* activity = nullptr;
                if (hasActivity && strcmp(hasActivity, "1") == 0 && activityValue) {
                    activity = new Activity(1, activityValue);
                }

                if (isMemory && strcmp(isMemory, "1") == 0 && originalPostId) {
                    Post* originalPost = findPost(originalPostId);
                    if (originalPost) {
                        Post* memoryPost = addMemoryPost(id, text, originalPost, nullptr);
                        if (memoryPost && sharedDate) {
                            memoryPost->setSharedDate(sharedDate);
                        }
                    }
                }
                else {
                    Post* post = addPost(id, text, nullptr, activity);
                    if (post && sharedDate) {
                        post->setSharedDate(sharedDate);
                    }
                }
            }
        }
        file.close();
    }
}

void SocialNetworkApp::saveFriendships() const {
    std::ofstream file("friendships.txt");
    if (file.is_open()) {
        for (int i = 0; i < usersCount; i++) {
            User* user = users[i];
            for (int j = 0; j < user->getFriendsCount(); j++) {
                file << user->getId() << "," << user->getFriends()[j]->getId() << "\n";
            }
        }
        file.close();
    }
}

void SocialNetworkApp::loadFriendships() {
    std::ifstream file("friendships.txt");
    if (file.is_open()) {
        char line[256];
        while (file.getline(line, sizeof(line))) {
            char* userId = strtok(line, ",");
            char* friendId = strtok(nullptr, ",");

            if (userId && friendId) {
                User* user = findUser(userId);
                User* friendUser = findUser(friendId);
                if (user && friendUser) {
                    user->addFriend(friendUser);
                }
            }
        }
        file.close();
    }
}

void SocialNetworkApp::saveLikedPages() const {
    std::ofstream file("likedpages.txt");
    if (file.is_open()) {
        for (int i = 0; i < usersCount; i++) {
            User* user = users[i];
            for (int j = 0; j < user->getLikedPagesCount(); j++) {
                file << user->getId() << "," << user->getLikedPages()[j]->getId() << "\n";
            }
        }
        file.close();
    }
}

void SocialNetworkApp::loadLikedPages() {
    std::ifstream file("likedpages.txt");
    if (file.is_open()) {
        char line[256];
        while (file.getline(line, sizeof(line))) {
            char* userId = strtok(line, ",");
            char* pageId = strtok(nullptr, ",");

            if (userId && pageId) {
                User* user = findUser(userId);
                Page* page = findPage(pageId);
                if (user && page) {
                    user->likePage(page);
                }
            }
        }
        file.close();
    }
}

void SocialNetworkApp::savePostLikes() const {
    std::ofstream file("postlikes.txt");
    if (file.is_open()) {
        for (int i = 0; i < postsCount; i++) {
            Post* post = posts[i];
            for (int j = 0; j < post->getLikesCount(); j++) {
                file << post->getId() << "," << post->getLikes()[j]->getId() << "\n";
            }
        }
        file.close();
    }
}

void SocialNetworkApp::loadPostLikes() {
    std::ifstream file("postlikes.txt");
    if (file.is_open()) {
        char line[256];
        while (file.getline(line, sizeof(line))) {
            char* postId = strtok(line, ",");
            char* likerId = strtok(nullptr, ",");

            if (postId && likerId) {
                Post* post = findPost(postId);
                if (post) {
                    Entity* liker = findUser(likerId);
                    if (!liker) liker = findPage(likerId);
                    if (liker) post->addLike(liker);
                }
            }
        }
        file.close();
    }
}

void SocialNetworkApp::savePostComments() const {
    std::ofstream file("postcomments.txt");
    if (file.is_open()) {
        for (int i = 0; i < postsCount; i++) {
            Post* post = posts[i];
            for (int j = 0; j < post->getCommentsCount(); j++) {
                Comment* comment = post->getComments()[j];
                file << post->getId() << ","
                    << comment->getAuthor()->getId() << ","
                    << comment->getText() << ","
                    << (dynamic_cast<const User*>(comment->getAuthor()) ? "user" : "page") << "\n";
            }
        }
        file.close();
    }
}

void SocialNetworkApp::loadPostComments() {
    std::ifstream file("postcomments.txt");
    if (file.is_open()) {
        char line[512];
        while (file.getline(line, sizeof(line))) {
            char* postId = strtok(line, ",");
            char* authorId = strtok(nullptr, ",");
            char* commentText = strtok(nullptr, ",");
            char* authorType = strtok(nullptr, ",");

            if (postId && authorId && commentText && authorType) {
                Post* post = findPost(postId);
                Entity* author = nullptr;

                if (strcmp(authorType, "user") == 0) {
                    author = findUser(authorId);
                }
                else {
                    author = findPage(authorId);
                }

                if (post && author) {
                    Comment* comment = new Comment(commentText, author);
                    post->addComment(comment);
                }
            }
        }
        file.close();
    }
}

void SocialNetworkApp::loadAllData() {
    loadAuthData();
    loadUsers();
    loadPages();
    loadPosts();
    loadFriendships();
    loadLikedPages();
    loadPostLikes();
    loadPostComments();
}

void SocialNetworkApp::saveAllData() const {
    saveAuthData();
    saveUsers();
    savePages();
    savePosts();
    saveFriendships();
    saveLikedPages();
    savePostLikes();
    savePostComments();
}
void SocialNetworkApp::showPageSelection()
{
    char pageId[100] = { 0 };
    bool exit = false;

    while (!exit && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Enter Page ID:", 100, 100, 20, DARKGRAY);
        Rectangle inputBox = { 100, 130, 300, 40 };
        DrawRectangleRec(inputBox, LIGHTGRAY);
        DrawRectangleLinesEx(inputBox, 2, DARKGRAY);
        DrawText(pageId, inputBox.x + 5, inputBox.y + 10, 20, DARKGRAY);

        Rectangle viewButton = { 100, 180, 140, 40 };
        DrawRectangleRec(viewButton, BLUE);
        DrawText("View Page", viewButton.x + 20, viewButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, 180, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle input
        int key = GetCharPressed();
        while (key > 0 && strlen(pageId) < 99) {
            pageId[strlen(pageId)] = (char)key;
            pageId[strlen(pageId) + 1] = '\0';
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(pageId);
            if (len > 0) pageId[len - 1] = '\0';
        }

        // Handle buttons
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), viewButton)) {
                viewPage(pageId);
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
void SocialNetworkApp::showPostSelection(const char* action) {
    char postId[100] = { 0 };
    char commentText[256] = { 0 };
    bool exit = false;
    bool typingInComment = false; // Track where we're typing

    while (!exit && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("%s a Post", action), 100, 50, 30, DARKGRAY);
        DrawText("Enter Post ID:", 100, 100, 20, DARKGRAY);

        Rectangle postIdBox = { 100, 130, 300, 40 };
        DrawRectangleRec(postIdBox, LIGHTGRAY);
        DrawRectangleLinesEx(postIdBox, 2, typingInComment ? DARKGRAY : BLUE);
        DrawText(postId, postIdBox.x + 5, postIdBox.y + 10, 20, DARKGRAY);

        Rectangle commentBox = { 0 }; // Initialize properly
        if (strcmp(action, "comment") == 0) {
            DrawText("Enter Comment:", 100, 180, 20, DARKGRAY);
            commentBox = Rectangle{ 100.0f, 210.0f, 300.0f, 80.0f };
            DrawRectangleRec(commentBox, LIGHTGRAY);
            DrawRectangleLinesEx(commentBox, 2, typingInComment ? BLUE : DARKGRAY);
            DrawText(commentText, commentBox.x + 5, commentBox.y + 10, 20, DARKGRAY);
        }

        // Handle mouse click to focus
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), postIdBox)) {
                typingInComment = false;
            }
            else if (strcmp(action, "comment") == 0 && CheckCollisionPointRec(GetMousePosition(), commentBox)) {
                typingInComment = true;
            }
        }

        // Handle keyboard input
        int key = GetCharPressed();
        while (key > 0) {
            if (!typingInComment && strlen(postId) < 99) {
                postId[strlen(postId)] = (char)key;
                postId[strlen(postId) + 1] = '\0';
            }
            else if (typingInComment && strlen(commentText) < 255) {
                commentText[strlen(commentText)] = (char)key;
                commentText[strlen(commentText) + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (!typingInComment && strlen(postId) > 0) {
                postId[strlen(postId) - 1] = '\0';
            }
            else if (typingInComment && strlen(commentText) > 0) {
                commentText[strlen(commentText) - 1] = '\0';
            }
        }

        // Action buttons
        Rectangle actionButton = { 100, (strcmp(action, "comment") == 0) ? 300 : 180, 140, 40 };
        DrawRectangleRec(actionButton, BLUE);
        DrawText(action, actionButton.x + 40, actionButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, (strcmp(action, "comment") == 0) ? 300 : 180, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle button clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), actionButton)) {
                if (strcmp(action, "like") == 0) {
                    likePost(postId); // Fixed: No argument needed here
                }
                else {
                    commentOnPost(postId, commentText);
                }
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
//void likePage() {  // Removed parameter since we get it from GUI
//    char pageId[100] = { 0 };
//    bool exit = false;

//    while (!exit && !WindowShouldClose()) {
//        BeginDrawing();
//        ClearBackground(RAYWHITE);

//        DrawText("Like a Page", 100, 50, 30, DARKGRAY);
//        DrawText("Enter Page ID:", 100, 100, 20, DARKGRAY);

//        Rectangle inputBox = { 100, 130, 300, 40 };
//        DrawRectangleRec(inputBox, LIGHTGRAY);
//        DrawRectangleLinesEx(inputBox, 2, DARKGRAY);
//        DrawText(pageId, inputBox.x + 5, inputBox.y + 10, 20, DARKGRAY);

//        // Display available pages
//        DrawText("Available Pages:", 100, 180, 20, DARKGRAY);
//        int yPos = 210;
//        for (int i = 0; i < pagesCount && yPos < GetScreenHeight() - 100; i++) {
//            DrawText(TextFormat("%s - %s", pages[i]->getId(), pages[i]->getTitle()),
//                120, yPos, 18, DARKGRAY);
//            yPos += 25;
//        }

//        Rectangle likeButton = { 100, GetScreenHeight() - 100, 140, 40 };
//        DrawRectangleRec(likeButton, BLUE);
//        DrawText("Like Page", likeButton.x + 20, likeButton.y + 10, 20, WHITE);

//        Rectangle cancelButton = { 260, GetScreenHeight() - 100, 140, 40 };
//        DrawRectangleRec(cancelButton, GRAY);
//        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

//        // Handle input
//        int key = GetCharPressed();
//        while (key > 0 && strlen(pageId) < 99) {
//            pageId[strlen(pageId)] = (char)key;
//            pageId[strlen(pageId) + 1] = '\0';
//            key = GetCharPressed();
//        }

//        if (IsKeyPressed(KEY_BACKSPACE) && strlen(pageId) > 0) {
//            pageId[strlen(pageId) - 1] = '\0';
//        }

//        // Handle buttons
//        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//            if (CheckCollisionPointRec(GetMousePosition(), likeButton)) {
//                // Call the actual likePage function with the collected pageId
//                Page* page = findPage(pageId);
//                if (page) {
//                    currentUser->likePage(page);
//                }
//                exit = true;
//            }
//            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
//                exit = true;
//            }
//        }

//        EndDrawing();
//    }
//}
void SocialNetworkApp::showMemoryCreation() {
    char postId[100] = { 0 };
    char memoryText[256] = { 0 };
    bool exit = false;
    bool typingInMemory = false;

    while (!exit && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Share a Memory", 100, 50, 30, DARKGRAY);
        DrawText("Original Post ID:", 100, 100, 20, DARKGRAY);

        // Post ID input
        Rectangle postIdBox = { 100, 130, 300, 40 };
        DrawRectangleRec(postIdBox, LIGHTGRAY);
        DrawRectangleLinesEx(postIdBox, 2, typingInMemory ? DARKGRAY : BLUE);
        DrawText(postId, postIdBox.x + 5, postIdBox.y + 10, 20, DARKGRAY);

        // Memory text input
        DrawText("Memory Text:", 100, 180, 20, DARKGRAY);
        Rectangle memoryBox = { 100, 210, 300, 80 };
        DrawRectangleRec(memoryBox, LIGHTGRAY);
        DrawRectangleLinesEx(memoryBox, 2, typingInMemory ? BLUE : DARKGRAY);
        DrawText(memoryText, memoryBox.x + 5, memoryBox.y + 10, 20, DARKGRAY);

        // Handle focus
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            typingInMemory = CheckCollisionPointRec(GetMousePosition(), memoryBox);
        }

        // Handle input
        int key = GetCharPressed();
        while (key > 0) {
            if (!typingInMemory && strlen(postId) < 99) {
                postId[strlen(postId)] = (char)key;
                postId[strlen(postId) + 1] = '\0';
            }
            else if (typingInMemory && strlen(memoryText) < 255) {
                memoryText[strlen(memoryText)] = (char)key;
                memoryText[strlen(memoryText) + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (!typingInMemory && strlen(postId) > 0) {
                postId[strlen(postId) - 1] = '\0';
            }
            else if (typingInMemory && strlen(memoryText) > 0) {
                memoryText[strlen(memoryText) - 1] = '\0';
            }
        }

        // Action buttons
        Rectangle shareButton = { 100, 300, 140, 40 };
        DrawRectangleRec(shareButton, BLUE);
        DrawText("Share", shareButton.x + 40, shareButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, 300, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle button clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), shareButton)) {
                shareMemory(postId, memoryText);
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
void SocialNetworkApp::showDateInput() {
    char date[20] = { 0 };
    bool exit = false;

    while (!exit && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Set System Date", 100, 50, 30, DARKGRAY);
        DrawText("Enter date (DD/MM/YYYY):", 100, 100, 20, DARKGRAY);

        Rectangle dateBox = { 100, 130, 300, 40 };
        DrawRectangleRec(dateBox, LIGHTGRAY);
        DrawRectangleLinesEx(dateBox, 2, DARKGRAY);
        DrawText(date, dateBox.x + 5, dateBox.y + 10, 20, DARKGRAY);

        // Format hint
        DrawText("Example: 15/05/2023", 100, 180, 18, GRAY);

        // Action buttons
        Rectangle setButton = { 100, 230, 140, 40 };
        DrawRectangleRec(setButton, BLUE);
        DrawText("Set Date", setButton.x + 30, setButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, 230, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle input
        int key = GetCharPressed();
        while (key > 0 && strlen(date) < 19) {
            if ((key >= '0' && key <= '9') || key == '/') {
                date[strlen(date)] = (char)key;
                date[strlen(date) + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(date) > 0) {
            date[strlen(date) - 1] = '\0';
        }

        // Handle buttons
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), setButton)) {
                setSystemDate(date);
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
void SocialNetworkApp::showFriendSelection() {
    char friendId[100] = { 0 };
    bool exit = false;

    while (!exit && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Add Friend", 100, 50, 30, DARKGRAY);
        DrawText("Enter Friend's User ID:", 100, 100, 20, DARKGRAY);

        Rectangle inputBox = { 100, 130, 300, 40 };
        DrawRectangleRec(inputBox, LIGHTGRAY);
        DrawRectangleLinesEx(inputBox, 2, DARKGRAY);
        DrawText(friendId, inputBox.x + 5, inputBox.y + 10, 20, DARKGRAY);

        // Display available users (excluding current user and existing friends)
        DrawText("Available Users:", 100, 180, 20, DARKGRAY);
        int yPos = 210;
        for (int i = 0; i < usersCount && yPos < GetScreenHeight() - 100; i++) {
            if (strcmp(users[i]->getId(), currentUser->getId()) != 0) {
                bool isAlreadyFriend = false;
                for (int j = 0; j < currentUser->getFriendsCount(); j++) {
                    if (strcmp(currentUser->getFriends()[j]->getId(), users[i]->getId()) == 0) {
                        isAlreadyFriend = true;
                        break;
                    }
                }

                if (!isAlreadyFriend) {
                    DrawText(TextFormat("%s - %s", users[i]->getId(), users[i]->getName()),
                        120, yPos, 18, DARKGRAY);
                    yPos += 25;
                }
            }
        }

        // Action buttons
        Rectangle addButton = { 100, GetScreenHeight() - 100, 140, 40 };
        DrawRectangleRec(addButton, BLUE);
        DrawText("Add Friend", addButton.x + 20, addButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, GetScreenHeight() - 100, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle input
        int key = GetCharPressed();
        while (key > 0 && strlen(friendId) < 99) {
            friendId[strlen(friendId)] = (char)key;
            friendId[strlen(friendId) + 1] = '\0';
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(friendId) > 0) {
            friendId[strlen(friendId) - 1] = '\0';
        }

        // Handle buttons
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), addButton)) {
                addFriend();
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
void SocialNetworkApp::showPostCreation() {
    char postText[256] = { 0 };
    char activityValue[100] = { 0 };
    int activityType = 0;
    bool exit = false;
    bool typingInPost = true;
    bool typingInActivity = false;

    while (!exit && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Create Post", 100, 50, 30, DARKGRAY);
        DrawText("Post Text:", 100, 100, 20, DARKGRAY);

        // Post text input
        Rectangle postBox = { 100, 130, 300, 80 };
        DrawRectangleRec(postBox, LIGHTGRAY);
        DrawRectangleLinesEx(postBox, 2, typingInPost ? BLUE : DARKGRAY);
        DrawText(postText, postBox.x + 5, postBox.y + 10, 20, DARKGRAY);

        // Activity selection
        DrawText("Add Activity (optional):", 100, 220, 20, DARKGRAY);

        const char* activityTypes[] = { "None", "Feeling", "Thinking about", "Making", "Celebrating" };
        Rectangle activityButtons[5];

        for (int i = 0; i < 5; i++) {
            activityButtons[i] = Rectangle{ static_cast<float>(100 + i * 80), 250.0f, 75.0f, 30.0f };
            Color btnColor = (activityType == i) ? BLUE : LIGHTGRAY;
            DrawRectangleRec(activityButtons[i], btnColor);
            DrawRectangleLinesEx(activityButtons[i], 1, DARKGRAY);
            DrawText(activityTypes[i], activityButtons[i].x + 5, activityButtons[i].y + 5, 12,
                (activityType == i) ? WHITE : DARKGRAY);
        }

        // Activity value input (if activity selected)
        if (activityType > 0) {
            DrawText("Activity Value:", 100, 290, 20, DARKGRAY);
            Rectangle activityValueBox = { 100, 320, 300, 40 };
            DrawRectangleRec(activityValueBox, LIGHTGRAY);
            DrawRectangleLinesEx(activityValueBox, 2, typingInActivity ? BLUE : DARKGRAY);
            DrawText(activityValue, activityValueBox.x + 5, activityValueBox.y + 10, 20, DARKGRAY);

            // Handle activity value input focus
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                typingInActivity = CheckCollisionPointRec(GetMousePosition(), activityValueBox);
                typingInPost = !typingInActivity;
            }
        }

        // Handle post text focus
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), postBox)) {
            typingInPost = true;
            typingInActivity = false;
        }

        // Handle keyboard input
        int key = GetCharPressed();
        while (key > 0) {
            if (typingInPost && strlen(postText) < 255) {
                postText[strlen(postText)] = (char)key;
                postText[strlen(postText) + 1] = '\0';
            }
            else if (typingInActivity && strlen(activityValue) < 99) {
                activityValue[strlen(activityValue)] = (char)key;
                activityValue[strlen(activityValue) + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (typingInPost && strlen(postText) > 0) {
                postText[strlen(postText) - 1] = '\0';
            }
            else if (typingInActivity && strlen(activityValue) > 0) {
                activityValue[strlen(activityValue) - 1] = '\0';
            }
        }

        // Handle activity type selection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int i = 0; i < 5; i++) {
                if (CheckCollisionPointRec(GetMousePosition(), activityButtons[i])) {
                    activityType = i;
                    if (i == 0) {
                        typingInActivity = false;
                        typingInPost = true;
                    }
                }
            }
        }

        // Action buttons
        Rectangle postButton = { 100, 380, 140, 40 };
        DrawRectangleRec(postButton, BLUE);
        DrawText("Create Post", postButton.x + 20, postButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, 380, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle button clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), postButton)) {
                // Create the activity if selected
                Activity* activity = nullptr;
                if (activityType > 0 && strlen(activityValue) > 0) {
                    activity = new Activity(activityType, activityValue);
                }

                // Generate post ID
                char postId[20];
                sprintf(postId, "post%d", postsCount + 1);

                // Create and add the post
                addPost(postId, postText, currentUser, activity);
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
void SocialNetworkApp::showPageLikeSelection() {
    char pageId[100] = { 0 };
    bool exit = false;

    while (!exit && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Like a Page", 100, 50, 30, DARKGRAY);
        DrawText("Enter Page ID:", 100, 100, 20, DARKGRAY);

        Rectangle inputBox = { 100, 130, 300, 40 };
        DrawRectangleRec(inputBox, LIGHTGRAY);
        DrawRectangleLinesEx(inputBox, 2, DARKGRAY);
        DrawText(pageId, inputBox.x + 5, inputBox.y + 10, 20, DARKGRAY);

        // Display available pages
        DrawText("Available Pages:", 100, 180, 20, DARKGRAY);
        int yPos = 210;
        for (int i = 0; i < pagesCount && yPos < GetScreenHeight() - 100; i++) {
            DrawText(TextFormat("%s - %s", pages[i]->getId(), pages[i]->getTitle()),
                120, yPos, 18, DARKGRAY);
            yPos += 25;
        }

        Rectangle likeButton = { 100, GetScreenHeight() - 100, 140, 40 };
        DrawRectangleRec(likeButton, BLUE);
        DrawText("Like Page", likeButton.x + 20, likeButton.y + 10, 20, WHITE);

        Rectangle cancelButton = { 260, GetScreenHeight() - 100, 140, 40 };
        DrawRectangleRec(cancelButton, GRAY);
        DrawText("Cancel", cancelButton.x + 40, cancelButton.y + 10, 20, WHITE);

        // Handle input
        int key = GetCharPressed();
        while (key > 0 && strlen(pageId) < 99) {
            pageId[strlen(pageId)] = (char)key;
            pageId[strlen(pageId) + 1] = '\0';
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(pageId) > 0) {
            pageId[strlen(pageId) - 1] = '\0';
        }

        // Handle buttons
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), likeButton)) {
                likePage();
                exit = true;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), cancelButton)) {
                exit = true;
            }
        }

        EndDrawing();
    }
}
void SocialNetworkApp::drawLoginScreen() {
    ClearBackground(RAYWHITE);

    // Title
    DrawText("Social Network App", GetScreenWidth() / 2 - MeasureText("Social Network App", 40) / 2, 50, 40, DARKGRAY);

    // Username field
    DrawText("Username:", 100, 150, 20, DARKGRAY);
    Rectangle usernameBox = { 100, 180, 300, 40 };
    DrawRectangleRec(usernameBox, LIGHTGRAY);
    if (focusedField == USERNAME_FIELD) {
        DrawRectangleLinesEx(usernameBox, 2, BLUE);
    }
    else {
        DrawRectangleLinesEx(usernameBox, 2, DARKGRAY);
    }
    DrawText(usernameInput, usernameBox.x + 5, usernameBox.y + 10, 20, DARKGRAY);

    // Password field
    DrawText("Password:", 100, 250, 20, DARKGRAY);
    Rectangle passwordBox = { 100, 280, 300, 40 };
    DrawRectangleRec(passwordBox, LIGHTGRAY);
    if (focusedField == PASSWORD_FIELD) {
        DrawRectangleLinesEx(passwordBox, 2, BLUE);
    }
    else {
        DrawRectangleLinesEx(passwordBox, 2, DARKGRAY);
    }

    // Show password as asterisks
    char hiddenPassword[256] = { 0 };
    for (int i = 0; i < strlen(passwordInput); i++) {
        hiddenPassword[i] = '*';
    }
    DrawText(hiddenPassword, passwordBox.x + 5, passwordBox.y + 10, 20, DARKGRAY);

    // Login button
    Rectangle loginButton = { 100, 350, 140, 40 };
    bool loginHover = CheckCollisionPointRec(GetMousePosition(), loginButton);
    DrawRectangleRec(loginButton, loginHover ? BLUE : DARKBLUE);
    DrawText("Login", loginButton.x + 40, loginButton.y + 10, 20, RAYWHITE);

    // Signup button
    Rectangle signupButton = { 260, 350, 140, 40 };
    bool signupHover = CheckCollisionPointRec(GetMousePosition(), signupButton);
    DrawRectangleRec(signupButton, signupHover ? BLUE : DARKBLUE);
    DrawText("Sign Up", signupButton.x + 40, signupButton.y + 10, 20, RAYWHITE);

    // Error message
    if (showError) {
        DrawText(errorMessage, 100, 410, 20, RED);
    }

    // Switch to signup text
    DrawText("Don't have an account? Click Sign Up", 100, 450, 20, DARKGRAY);

    // Show which field is focused (optional)
    if (focusedField == USERNAME_FIELD) {
        DrawText("(Username field focused)", 100, 220, 15, BLUE);
    }
    else {
        DrawText("(Password field focused)", 100, 320, 15, BLUE);
    }
}

void SocialNetworkApp::drawSignupScreen() {
    ClearBackground(RAYWHITE);

    // Title
    DrawText("Create Account", GetScreenWidth() / 2 - MeasureText("Create Account", 40) / 2, 50, 40, DARKGRAY);

    // Username field
    DrawText("Choose Username:", 100, 150, 20, DARKGRAY);
    Rectangle usernameBox = { 100, 180, 300, 40 };
    DrawRectangleRec(usernameBox, LIGHTGRAY);
    if (focusedField == USERNAME_FIELD) {
        DrawRectangleLinesEx(usernameBox, 2, BLUE);
    }
    else {
        DrawRectangleLinesEx(usernameBox, 2, DARKGRAY);
    }
    DrawText(usernameInput, usernameBox.x + 5, usernameBox.y + 10, 20, DARKGRAY);

    // Password field
    DrawText("Choose Password:", 100, 250, 20, DARKGRAY);
    Rectangle passwordBox = { 100, 280, 300, 40 };
    DrawRectangleRec(passwordBox, LIGHTGRAY);
    if (focusedField == PASSWORD_FIELD) {
        DrawRectangleLinesEx(passwordBox, 2, BLUE);
    }
    else {
        DrawRectangleLinesEx(passwordBox, 2, DARKGRAY);
    }

    // Show password as asterisks
    char hiddenPassword[256] = { 0 };
    for (int i = 0; i < strlen(passwordInput); i++) {
        hiddenPassword[i] = '*';
    }
    DrawText(hiddenPassword, passwordBox.x + 5, passwordBox.y + 10, 20, DARKGRAY);

    // Create Account button
    Rectangle createButton = { 100, 350, 300, 40 };
    bool createHover = CheckCollisionPointRec(GetMousePosition(), createButton);
    DrawRectangleRec(createButton, createHover ? BLUE : DARKBLUE);
    DrawText("Create Account", createButton.x + 70, createButton.y + 10, 20, RAYWHITE);

    // Back button
    Rectangle backButton = { 100, 410, 300, 40 };
    bool backHover = CheckCollisionPointRec(GetMousePosition(), backButton);
    DrawRectangleRec(backButton, backHover ? LIGHTGRAY : GRAY);
    DrawText("Back to Login", backButton.x + 90, backButton.y + 10, 20, DARKGRAY);

    // Error message
    if (showError) {
        DrawText(errorMessage, 100, 470, 20, RED);
    }

    // Show which field is focused (optional)
    if (focusedField == USERNAME_FIELD) {
        DrawText("(Username field focused)", 100, 220, 15, BLUE);
    }
    else {
        DrawText("(Password field focused)", 100, 320, 15, BLUE);
    }
}

void SocialNetworkApp::drawMainMenu() {
    ClearBackground(RAYWHITE);

    // Title
    const char* title;
    if (currentPage) {
        title = TextFormat("Page: %s", currentPage->getTitle());
    }
    else if (currentUser) {
        title = TextFormat("User: %s", currentUser->getName());
    }
    else {
        title = "Main Menu";
    }

    DrawText(title, GetScreenWidth() / 2 - MeasureText(title, 30) / 2, 20, 30, DARKGRAY);

    // Menu options
    const char* menuItems[] = {
        "View Home",
        "View Timeline",
        "View Friends",
        "View Liked Pages",
        "View a Page",
        "Like a Post",
        "Comment on a Post",
        "Share a Memory",
        currentPage ? "Switch Back to User" : "Login as Page",
        "Create a Post",
        "Create a Page",
        "Add Friend",
        "Like a Page",
        "Set System Date",
        "Logout"
    };

    int itemHeight = 40;
    int startY = 80;
    int spacing = 10;

    for (int i = 0; i < 15; i++) {
        Rectangle itemRect = { 50, startY + i * (itemHeight + spacing), 300, itemHeight };
        bool hover = CheckCollisionPointRec(GetMousePosition(), itemRect);

        DrawRectangleRec(itemRect, hover ? BLUE : DARKBLUE);
        DrawText(menuItems[i], itemRect.x + 10, itemRect.y + 10, 20, RAYWHITE);
    }
}

// Global or static variables
bool isUsernameActive = true;
bool isPasswordActive = false;
void SocialNetworkApp::updateLoginScreen() {
    // Handle field focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle usernameBox = { 100, 180, 300, 40 };
        Rectangle passwordBox = { 100, 280, 300, 40 };

        if (CheckCollisionPointRec(GetMousePosition(), usernameBox)) {
            focusedField = USERNAME_FIELD;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), passwordBox)) {
            focusedField = PASSWORD_FIELD;
        }
    }

    // Handle text input based on focused field
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125) {
            if (focusedField == USERNAME_FIELD) {
                int len = strlen(usernameInput);
                if (len < 255) {
                    usernameInput[len] = (char)key;
                    usernameInput[len + 1] = '\0';
                }
            }
            else {
                int len = strlen(passwordInput);
                if (len < 255) {
                    passwordInput[len] = (char)key;
                    passwordInput[len + 1] = '\0';
                }
            }
        }
        key = GetCharPressed();
    }

    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (focusedField == USERNAME_FIELD) {
            int len = strlen(usernameInput);
            if (len > 0) usernameInput[len - 1] = '\0';
        }
        else {
            int len = strlen(passwordInput);
            if (len > 0) passwordInput[len - 1] = '\0';
        }
    }

    // Handle tab to switch fields
    if (IsKeyPressed(KEY_TAB)) {
        focusedField = (focusedField == USERNAME_FIELD) ? PASSWORD_FIELD : USERNAME_FIELD;
    }

    // Check for button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle loginButton = { 100, 350, 140, 40 };
        Rectangle signupButton = { 260, 350, 140, 40 };

        if (CheckCollisionPointRec(GetMousePosition(), loginButton)) {
            if (login(usernameInput, passwordInput)) {
                currentState = MAIN_MENU;
                // Clear password after login for security
                memset(passwordInput, 0, sizeof(passwordInput));
            }
            else {
                showError = true;
                strcpy(errorMessage, "Invalid username or password");
            }
        }
        else if (CheckCollisionPointRec(GetMousePosition(), signupButton)) {
            currentState = SIGNUP;
            usernameInput[0] = '\0';
            passwordInput[0] = '\0';
            showError = false;
            focusedField = USERNAME_FIELD;
        }
    }
    //// Draw UI
    //DrawText("Username:", 100, 170, 20, DARKGRAY);
    //DrawRectangleLines(100, 200, 300, 40, isUsernameActive ? BLUE : GRAY);
    //DrawText(usernameInput, 110, 210, 20, BLACK);

    //DrawText("Password:", 100, 230, 20, DARKGRAY);
    //DrawRectangleLines(100, 260, 300, 40, isPasswordActive ? BLUE : GRAY);

    //// Draw asterisks for password
    //char hiddenPassword[32];
    //memset(hiddenPassword, '*', strlen(passwordInput));
    //hiddenPassword[strlen(passwordInput)] = '\0';
    //DrawText(hiddenPassword, 110, 270, 20, BLACK);

    //DrawRectangle(100, 350, 140, 40, DARKGREEN);
    //DrawText("Login", 135, 360, 20, WHITE);

    //DrawRectangle(260, 350, 140, 40, DARKBLUE);
    //DrawText("Sign Up", 285, 360, 20, WHITE);

    //if (showError) {
    //    DrawText(errorMessage, 100, 410, 20, RED);
    //}
}
void SocialNetworkApp::updateSignupScreen() {
    // Handle field focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle usernameBox = { 100, 180, 300, 40 };
        Rectangle passwordBox = { 100, 280, 300, 40 };

        if (CheckCollisionPointRec(GetMousePosition(), usernameBox)) {
            focusedField = USERNAME_FIELD;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), passwordBox)) {
            focusedField = PASSWORD_FIELD;
        }
    }

    // Handle text input based on focused field
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125) {
            if (focusedField == USERNAME_FIELD) {
                int len = strlen(usernameInput);
                if (len < 255) {
                    usernameInput[len] = (char)key;
                    usernameInput[len + 1] = '\0';
                }
            }
            else {
                int len = strlen(passwordInput);
                if (len < 255) {
                    passwordInput[len] = (char)key;
                    passwordInput[len + 1] = '\0';
                }
            }
        }
        key = GetCharPressed();
    }

    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (focusedField == USERNAME_FIELD) {
            int len = strlen(usernameInput);
            if (len > 0) usernameInput[len - 1] = '\0';
        }
        else {
            int len = strlen(passwordInput);
            if (len > 0) passwordInput[len - 1] = '\0';
        }
    }

    // Handle tab to switch fields
    if (IsKeyPressed(KEY_TAB)) {
        focusedField = (focusedField == USERNAME_FIELD) ? PASSWORD_FIELD : USERNAME_FIELD;
    }

    // Check for button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle createButton = { 100, 350, 300, 40 };
        Rectangle backButton = { 100, 410, 300, 40 };

        if (CheckCollisionPointRec(GetMousePosition(), createButton)) {
            if (signUp(usernameInput, passwordInput)) {
                currentState = MAIN_MENU;
                // Clear password after signup for security
                memset(passwordInput, 0, sizeof(passwordInput));
            }
            else {
                showError = true;
                strcpy(errorMessage, "Username already exists or invalid input");
            }
        }
        else if (CheckCollisionPointRec(GetMousePosition(), backButton)) {
            currentState = LOGIN;
            usernameInput[0] = '\0';
            passwordInput[0] = '\0';
            showError = false;
            focusedField = USERNAME_FIELD;
        }
    }
}


void SocialNetworkApp::updateMainMenu() {
    if (!currentUser && !currentPage) {
        currentState = LOGIN;
        return;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int itemHeight = 40;
        int startY = 80;
        int spacing = 10;

        for (int i = 0; i < 15; i++) {
            Rectangle itemRect = { 50, startY + i * (itemHeight + spacing), 300, itemHeight };

            if (CheckCollisionPointRec(GetMousePosition(), itemRect)) {
                switch (i) {
                case 0: viewHome(); break;
                case 1: viewTimeline(); break;
                case 2: viewFriendList(currentUser->getId()); break;
                case 3: viewLikedPages(currentUser->getId()); break;
                case 4: showPageSelection(); break;
                case 5: showPostSelection("like"); break;
                case 6: showPostSelection("comment"); break;
                case 7: showMemoryCreation(); break;
                case 8:
                    if (currentPage) switchBackToUser();
                    else loginAsPage();
                    break;
                case 9: showPostCreation(); break;
                case 10: createPage(); break;
                case 11: showFriendSelection(); break;
                case 12: showPageLikeSelection(); break;
                case 13: showDateInput(); break;
                case 14: logout(); break;
                }
            }
        }
    }
}
void SocialNetworkApp::run()
{
    loadAllData();

    InitWindow(1280,900 , "BLABB");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        switch (currentState) {
        case LOGIN:
            updateLoginScreen();
            break;
        case SIGNUP:
            updateSignupScreen();
            break;
        case MAIN_MENU:
            updateMainMenu();
            break;
        }

        // Handle text input for all states
        if (currentState == LOGIN || currentState == SIGNUP) {
            // Simple text input handling (for demo)
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125) {
                    char str[2] = { (char)key, '\0' };
                    strcat(usernameInput, str);
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(usernameInput);
                if (len > 0) usernameInput[len - 1] = '\0';
            }
        }

        // Draw
        BeginDrawing();

        switch (currentState) {
        case LOGIN:
            drawLoginScreen();
            break;
        case SIGNUP:
            drawSignupScreen();
            break;
        case MAIN_MENU:
            drawMainMenu();
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    saveAllData();
};

