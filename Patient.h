#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
#include <iostream>
using namespace std;

class Patient : public Person {
    int  age;
    char gender[2];
    char contact[12];
    double balance;
public:
    Patient();
    Patient(int id, const char* n, int a, const char* g,
            const char* con, const char* pass, double bal);

    int getAge()     const;
    const char* getGender()  const;
    const char* getContact() const;
    double getBalance() const;
    void setBalance(double b);

    Patient& operator+=(double amount);
    Patient& operator-=(double amount);
    bool operator==(const Patient& other) const;

    friend ostream& operator<<(ostream& os, const Patient& p);
    void displayMenu() override;
    void display() const override;
};

#endif