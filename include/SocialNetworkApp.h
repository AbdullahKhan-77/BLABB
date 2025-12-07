#define _CRT_SECURE_NO_WARNINGS
#ifndef SOCIALNETWORKAPP_H
#define SOCIALNETWORKAPP_H
#include "User.h"
#include "Page.h"
#include "Post.h"
#include<ctime>
#include<cstdio>
#include<cstring>

class SocialNetworkApp {
    User** users;
    int usersCount;
    Page** pages;
    int pagesCount;
    Post** posts;
    int postsCount;
    User* currentUser;
    Page* currentPage;
    char* systemDate;

    struct AuthData {
        char* userId;
        char* username;
        char* password;
    };
    AuthData* authData;
    int authDataCount;
    enum AppState { LOGIN, SIGNUP, MAIN_MENU };
    AppState currentState;
    char usernameInput[256] = { 0 };
    char passwordInput[256] = { 0 };
    bool showError;
    char errorMessage[256] = { 0 };

    // Add these in the private section
    enum InputField { USERNAME_FIELD, PASSWORD_FIELD };
    InputField focusedField = USERNAME_FIELD;

public:
    SocialNetworkApp();
    ~SocialNetworkApp();

    // Various method declarations...
    void run();

private:
    // Helper methods
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

    // File I/O methods
    void saveAuthData() const;
    void loadAuthData();
    void saveUsers() const;
    void loadUsers();
    void savePages() const;
    void loadPages();
    void savePosts() const;
    void loadPosts();
    void saveFriendships() const;
    void loadFriendships();
    void saveLikedPages() const;
    void loadLikedPages();
    void savePostLikes() const;
    void loadPostLikes();
    void savePostComments() const;
    void loadPostComments();
    void loginAsPage();
    void switchBackToUser();
    void createPost();
    User* addUser(const char* id, const char* name, const char* password);
    Page* addPage(const char* id, const char* title, User* owner);
    Post* addPost(const char* id, const char* text, Entity* author, Activity* activity);
    Post* addMemoryPost(const char* id, const char* text, const Post* originalPost, Entity* author);
    User* findUser(const char* id) const;
    Page* findPage(const char* id) const;
    Post* findPost(const char* id) const;
    void listAllPosts() const;
    void listAllPages() const;
    void likePage();
    void logout();
    bool login(const char* username, const char* password);
    bool signUp(const char* username, const char* password);
    void viewTimeline()const;
    void setCurrentUser(const char* userId);
    void setSystemDate(const char* date);
    void viewFriendList(const char* userID)const;
    void viewLikedPages(const char* userID)const;
    void viewPage(const char* pageID) const;
    void likePost(const char* postID);
    void viewPostLikes(const char* postID) const;
    void commentOnPost(const char* postID, const char* commentText);
    void viewPost(const char* postID) const;
    void shareMemory(const char* postId, const char* memoryText);
    void createPage();
    void viewHome() const;
    void addFriend();
    void loadAllData();
    void saveAllData() const;
    void showPageSelection();
    void updateLoginScreen();
    void drawLoginScreen();
    void drawMainMenu();
    void drawSignupScreen();
    void updateMainMenu();
    void updateSignupScreen();
    void showPageLikeSelection();
    void showFriendSelection();
    void showPostCreation();
    void  showDateInput();
    void showMemoryCreation();
    void showPostSelection(const char* action);
};
#endif