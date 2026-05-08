#include "Admin.h"

Admin::Admin() : Person() {}

Admin::Admin(int id, const char* n, const char* pass)
    : Person(id, n, pass) {}

void Admin::displayMenu() {
    cout << "\nAdmin Panel - MediCore\n";
    cout << "======================\n";
    cout << "1.  Add Doctor\n";
    cout << "2.  Remove Doctor\n";
    cout << "3.  View All Patients\n";
    cout << "4.  View All Doctors\n";
    cout << "5.  View All Appointments\n";
    cout << "6.  View Unpaid Bills\n";
    cout << "7.  Discharge Patient\n";
    cout << "8.  View Security Log\n";
    cout << "9.  Generate Daily Report\n";
    cout << "10. Logout\n";
    cout << "Choice: ";
}

void Admin::display() const {
    cout << "Admin: " << name << "\n";
}