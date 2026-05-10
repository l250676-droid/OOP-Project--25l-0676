#ifndef PERSON_H
#define PERSON_H

#include <cstring>

class Person {
protected:
    int id;
    char name[50];
    char password[50];
public:
    Person() : id(0) {
        name[0] = '\0';
        password[0] = '\0';
    }
    Person(int id, const char* n, const char* p) : id(id) {
        strncpy(name,     n, 49); 
        name[49]     = '\0';
        strncpy(password, p, 49); 
        password[49] = '\0';
    }
    int  getID()  const {
         return id; 
    }
    const char* getName() const {
         return name;
    }
    const char* getPassword() const { 
        return password; 
    }

    virtual void displayMenu() = 0;
    virtual void display()     const = 0;
    virtual ~Person() {}
};

#endif