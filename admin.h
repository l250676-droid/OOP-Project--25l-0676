#ifndef ADMIN_H
#define ADMIN_H

#include "Person.h"
#include <iostream>
using namespace std;

class Admin : public Person {
public:
    Admin();
    Admin(int id, const char* n, const char* pass);
    void displayMenu() override;
    void display()     const override;
};

#endif