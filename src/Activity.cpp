#define _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include "Activity.h"
#include <cstring>
static char* copyString(const char* str) {
    if (!str) return nullptr;
    char* newStr = new char[strlen(str) + 1];
    strcpy(newStr, str);
    return newStr;
}
Activity::Activity(int type, const char* value) : type(type), value(copyString(value)) {}

Activity::~Activity() { delete[] value; }

const char* Activity::getActivityText() const {
    static const char* typeText[] = { "feeling", "thinking about", "making", "celebrating" };
    static char buffer[100];
    sprintf(buffer, "%s %s", typeText[type - 1], value);
    return buffer;
}