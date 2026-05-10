#include "Patient.h"
#include <iomanip>
#include <cstring>

Patient::Patient() : Person(), age(0), balance(0) {
    gender[0]  = '\0';
    contact[0] = '\0';
}

Patient::Patient(int id, const char* n, int a, const char* g, const char* con, const char* pass, double bal)
    : Person(id, n, pass), age(a), balance(bal) {
    strncpy(gender,  g,   1); 
     gender[1]   = '\0';
    strncpy(contact, con, 11); 
    contact[11] = '\0';
}

int         Patient::getAge()     const { return age; }
const char* Patient::getGender()  const { return gender; }
const char* Patient::getContact() const { return contact; }
double      Patient::getBalance() const { return balance; }
void        Patient::setBalance(double b) { balance = b; }

Patient& Patient::operator+=(double amount) {
    balance += amount;
    return *this;
}

Patient& Patient::operator-=(double amount) {
    balance -= amount;
    return *this;
}

bool Patient::operator==(const Patient& other) const {
    return id == other.id;
}

ostream& operator<<(ostream& os, const Patient& p) {
    os << "ID: "      << p.id
       << " | Name: " << p.name
       << " | Age: "  << p.age
       << " | Gender: "  << p.gender
       << " | Contact: " << p.contact
       << " | Balance: PKR "
       << fixed << setprecision(2) << p.balance;
    return os;
}

void Patient::displayMenu() {
    cout << "\nWelcome, " << name << "\n";
    cout << "Balance: PKR "
         << fixed << setprecision(2) << balance << "\n";
    cout << "========================\n";
    cout << "1. Book Appointment\n";
    cout << "2. Cancel Appointment\n";
    cout << "3. View My Appointments\n";
    cout << "4. View My Medical Records\n";
    cout << "5. View My Bills\n";
    cout << "6. Pay Bill\n";
    cout << "7. Top Up Balance\n";
    cout << "8. Logout\n";
    cout << "Choice: ";
}

void Patient::display() const {
    cout << *this << "\n";
}