#define _CRT_SECURE_NO_WARNINGS
#include "Comment.h"
#include <ctime>
#include <iostream>
#include<cstdio>

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

Comment::Comment(const char* text, const Entity* author) :
    text(copyString(text)), author(author), timestamp(getCurrentDate()) {
}

Comment::~Comment() {
    delete[] text;
    delete[] timestamp;
}

const char* Comment::getText() const { return text; }
const char* Comment::getTimestamp() const { return timestamp; }
const Entity* Comment::getAuthor() const { return author; }

void Comment::display() const {
    std::cout << author->getName() << ": " << text << " (" << timestamp << ")" << std::endl;
}
