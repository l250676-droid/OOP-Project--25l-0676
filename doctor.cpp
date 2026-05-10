#include "Doctor.h"
#include <iomanip>
#include <cstring>

Doctor::Doctor() : Person(), fee(0) {
    specialization[0] = '\0';
    contact[0]        = '\0';
}

Doctor::Doctor(int id, const char* n, const char* spec,
               const char* con, const char* pass, double f)
    : Person(id, n, pass), fee(f) {
    strncpy(specialization, spec, 49); 
    specialization[49] = '\0';
    strncpy(contact,        con,  11); 
    contact[11]  = '\0';
}

const char* Doctor::getSpecialization() const { return specialization; }
const char* Doctor::getContact()        const { return contact; }
double      Doctor::getFee()            const { return fee; }

bool Doctor::operator==(const Doctor& other) const {
    return id == other.id;
}

ostream& operator<<(ostream& os, const Doctor& d) {
    os << "ID: "   << d.id
       << " | Name: "   << d.name
       << " | Spec: "   << d.specialization
       << " | Contact: "<< d.contact
       << " | Fee: PKR "
       << fixed << setprecision(2) << d.fee;
    return os;
}

void Doctor::displayMenu() {
    cout << "\nWelcome, Dr. " << name
         << " | Specialization: " << specialization << "\n";
    cout << "===============================================\n";
    cout << "1. View Today's Appointments\n";
    cout << "2. Mark Appointment Complete\n";
    cout << "3. Mark Appointment No-Show\n";
    cout << "4. Write Prescription\n";
    cout << "5. View Patient Medical History\n";
    cout << "6. Logout\n";
    cout << "Choice: ";
}

void Doctor::display() const {
    cout << *this << "\n";
}