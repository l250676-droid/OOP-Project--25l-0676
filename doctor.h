#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"
#include <iostream>
using namespace std;

class Doctor : public Person {
    char   specialization[50];
    char   contact[12];
    double fee;
public:
    Doctor();
    Doctor(int id, const char* n, const char* spec,
           const char* con, const char* pass, double f);

    const char* getSpecialization() const;
    const char* getContact()        const;
    double      getFee()            const;

    bool operator==(const Doctor& other) const;

    friend ostream& operator<<(ostream& os, const Doctor& d);
    void displayMenu() override;
    void display()     const override;
};

#endif