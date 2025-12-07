#ifndef ACTIVITY_H
#define ACTIVITY_H

class Activity {
    int type;
    char* value;
public:
    Activity(int type, const char* value);
    ~Activity();
    const char* getActivityText() const;
};

#endif