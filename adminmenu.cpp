#include "hospitalsystem.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
using namespace std;

// ============================================================
// ADMIN MENU
// loops until admin chooses logout
// ============================================================
void HospitalSystem::adminMenu() {
    while (true) {
        admin.displayMenu();
        int choice;
        cin >> choice;

        if      (choice == 1)  addDoctor();
        else if (choice == 2)  removeDoctor();
        else if (choice == 3)  viewAllPatients();
        else if (choice == 4)  viewAllDoctors();
        else if (choice == 5)  viewAllAppointments();
        else if (choice == 6)  viewUnpaidBills();
        else if (choice == 7)  dischargePatient();
        else if (choice == 8)  viewSecurityLog();
        else if (choice == 9)  generateDailyReport();
        else if (choice == 10) break;
        else cout << "Invalid choice.\n";
    }
}

// ============================================================
// 1. ADD DOCTOR
// validates all fields then writes to doctor.txt
// ============================================================
void HospitalSystem::addDoctor() {
    char name[50], spec[50], contact[12], pass[50];
    char feeStr[20];

    // --- name ---
    cout << "Enter doctor name (max 50 chars): ";
    cin.ignore();
    cin.getline(name, 49);

    // --- specialization ---
    cout << "Enter specialization (max 50 chars): ";
    cin.getline(spec, 49);

    // --- contact: must be exactly 11 digits ---
    int attempts = 0;
    bool valid = false;
    while (attempts < 3) {
        cout << "Enter contact (11 digits): ";
        cin >> contact;
        if (Validator::isValidContact(contact)) {
            valid = true;
            break;
        }
        cout << "Invalid contact. Must be exactly 11 digits.\n";
        attempts++;
    }
    if (!valid) return;

    // --- password: minimum 6 characters ---
    attempts = 0;
    valid = false;
    while (attempts < 3) {
        cout << "Enter password (min 6 chars): ";
        cin >> pass;
        if (Validator::isValidPassword(pass)) {
            valid = true;
            break;
        }
        cout << "Invalid password. Must be at least 6 characters.\n";
        attempts++;
    }
    if (!valid) return;

    // --- fee: must be a positive number ---
    attempts = 0;
    valid = false;
    while (attempts < 3) {
        cout << "Enter consultation fee (PKR): ";
        cin >> feeStr;
        if (Validator::isPositiveFloat(feeStr)) {
            valid = true;
            break;
        }
        cout << "Invalid fee. Must be a positive number.\n";
        attempts++;
    }
    if (!valid) return;

    // convert fee manually
    double fee = 0;
    int i = 0;
    while (feeStr[i] != '\0' && feeStr[i] != '.') {
        fee = fee * 10 + (feeStr[i] - '0');
        i++;
    }
    if (feeStr[i] == '.') {
        i++;
        double place = 0.1;
        while (feeStr[i] != '\0') {
            fee += (feeStr[i] - '0') * place;
            place /= 10;
            i++;
        }
    }

    // generate new doctor ID
    int newID = FileHandler::getMaxID("doctor.txt", 0) + 1;

    Doctor newDoc(newID, name, spec, contact, pass, fee);
    doctors.add(newDoc);
    FileHandler::appendDoctor(newDoc);

    cout << "Doctor added successfully. ID: " << newID << "\n";
}

// ============================================================
// 2. REMOVE DOCTOR
// cannot remove if they have pending appointments
// ============================================================
void HospitalSystem::removeDoctor() {
    // display all doctors
    cout << "ID | Name | Specialization | Fee\n";
    cout << "------------------------------------\n";
    for (int i = 0; i < doctors.size(); i++) {
        Doctor& d = doctors.getAll()[i];
        cout << d.getID()             << " | "
             << d.getName()           << " | "
             << d.getSpecialization() << " | "
             << "PKR " << fixed << setprecision(2) << d.getFee() << "\n";
    }

    cout << "Enter Doctor ID to remove: ";
    int did; cin >> did;

    Doctor* doc = doctors.findByID(did);
    if (doc == nullptr) {
        cout << "Doctor not found.\n";
        return;
    }

    // check for pending appointments
    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getDoctorID() == did &&
            strcmp(a.getStatus(), "pending") == 0) {
            cout << "Cannot remove doctor with pending appointments. "
                    "Cancel or reassign them first.\n";
            return;
        }
    }

    doctors.removeByID(did);
    FileHandler::saveAllDoctors(doctors);
    cout << "Doctor removed.\n";
}

// ============================================================
// 3. VIEW ALL PATIENTS
// shows ID, name, age, gender, contact, balance, unpaid bills
// ============================================================
void HospitalSystem::viewAllPatients() {
    if (patients.size() == 0) {
        cout << "No patients registered.\n";
        return;
    }

    cout << "ID | Name | Age | Gender | Contact | Balance | Unpaid Bills\n";
    cout << "--------------------------------------------------------------\n";

    for (int i = 0; i < patients.size(); i++) {
        Patient& p = patients.getAll()[i];
        cout << p.getID()      << " | "
             << p.getName()    << " | "
             << p.getAge()     << " | "
             << p.getGender()  << " | "
             << p.getContact() << " | "
             << "PKR " << fixed << setprecision(2) << p.getBalance() << " | "
             << countUnpaidBills(p.getID()) << "\n";
    }
}

// ============================================================
// 4. VIEW ALL DOCTORS
// ============================================================
void HospitalSystem::viewAllDoctors() {
    if (doctors.size() == 0) {
        cout << "No doctors registered.\n";
        return;
    }

    cout << "ID | Name | Specialization | Contact | Fee\n";
    cout << "---------------------------------------------\n";

    for (int i = 0; i < doctors.size(); i++) {
        Doctor& d = doctors.getAll()[i];
        cout << d.getID()             << " | "
             << d.getName()           << " | "
             << d.getSpecialization() << " | "
             << d.getContact()        << " | "
             << "PKR " << fixed << setprecision(2) << d.getFee() << "\n";
    }
}

// ============================================================
// 5. VIEW ALL APPOINTMENTS
// sorted by date descending
// ============================================================
void HospitalSystem::viewAllAppointments() {
    int n = appointments.size();
    if (n == 0) {
        cout << "No appointments found.\n";
        return;
    }

    // collect all indices
    int idx[100];
    for (int i = 0; i < n; i++) idx[i] = i;

    // bubble sort by date descending
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            const char* dateA = appointments.getAll()[idx[j]].getDate();
            const char* dateB = appointments.getAll()[idx[j+1]].getDate();
            if (Validator::compareDates(dateA, dateB) < 0) {
                int temp  = idx[j];
                idx[j]    = idx[j+1];
                idx[j+1]  = temp;
            }
        }
    }

    cout << "ID | Patient Name | Doctor Name | Date | Time Slot | Status\n";
    cout << "--------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        Appointment& a = appointments.getAll()[idx[i]];
        cout << a.getID()                        << " | "
             << getPatientName(a.getPatientID()) << " | "
             << getDoctorName(a.getDoctorID())   << " | "
             << a.getDate()                      << " | "
             << a.getTimeSlot()                  << " | "
             << a.getStatus()                    << "\n";
    }
}

// ============================================================
// 6. VIEW UNPAID BILLS
// bills older than 7 days are marked [OVERDUE]
// ============================================================
void HospitalSystem::viewUnpaidBills() {
    bool any = false;

    cout << "Bill ID | Patient Name | Amount (PKR) | Date\n";
    cout << "-----------------------------------------------\n";

    char today[12];
    Validator::getCurrentDate(today);
    double todayDays = Validator::dateToSeconds(today);

    for (int i = 0; i < bills.size(); i++) {
        Bill& b = bills.getAll()[i];
        if (strcmp(b.getStatus(), "unpaid") != 0) continue;

        // check if overdue: more than 7 days before today
        double billDays = Validator::dateToSeconds(b.getDate());
        double diff     = todayDays - billDays;
        bool overdue    = diff > 7;

        cout << b.getID()                        << " | "
             << getPatientName(b.getPatientID()) << " | "
             << "PKR " << fixed << setprecision(2) << b.getAmount() << " | "
             << b.getDate();

        if (overdue) cout << " [OVERDUE]";
        cout << "\n";

        any = true;
    }

    if (!any) cout << "No unpaid bills.\n";
}

// ============================================================
// 7. DISCHARGE PATIENT
// cannot discharge if unpaid bills or pending appointments
// archives all records then deletes them
// ============================================================
void HospitalSystem::dischargePatient() {
    cout << "Enter Patient ID: ";
    int pid; cin >> pid;

    Patient* p = patients.findByID(pid);
    if (p == nullptr) {
        cout << "Patient not found.\n";
        return;
    }

    // check for unpaid bills
    if (countUnpaidBills(pid) > 0) {
        cout << "Cannot discharge patient with unpaid bills.\n";
        return;
    }

    // check for pending appointments
    if (countPendingAppointments(pid) > 0) {
        cout << "Cannot discharge patient with pending appointments.\n";
        return;
    }

    // archive first then delete
    FileHandler::archivePatient(*p, appointments, bills, prescriptions);
    FileHandler::deletePatientRecords(pid, patients, appointments,
                                      bills, prescriptions);

    cout << "Patient discharged and archived successfully.\n";
}

// ============================================================
// 8. VIEW SECURITY LOG
// reads security_log.txt and prints line by line
// ============================================================
void HospitalSystem::viewSecurityLog() {
    ifstream fin("security_log.txt");
    if (!fin.is_open()) {
        cout << "No security events logged.\n";
        return;
    }

    char line[256];
    bool any = false;

    while (fin.getline(line, 255)) {
        if (line[0] == '\0') continue;
        cout << line << "\n";
        any = true;
    }
    fin.close();

    if (!any) cout << "No security events logged.\n";
}

// ============================================================
// 9. GENERATE DAILY REPORT
// reads existing data — no separate report file created
// ============================================================
void HospitalSystem::generateDailyReport() {
    char today[12];
    Validator::getCurrentDate(today);

    cout << "\n===== Daily Report: " << today << " =====\n\n";

    // --- appointment counts for today ---
    int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;

    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (strcmp(a.getDate(), today) != 0) continue;

        total++;
        if      (strcmp(a.getStatus(), "pending")   == 0) pending++;
        else if (strcmp(a.getStatus(), "completed")  == 0) completed++;
        else if (strcmp(a.getStatus(), "no-show")    == 0) noshow++;
        else if (strcmp(a.getStatus(), "cancelled")  == 0) cancelled++;
    }

    cout << "Total appointments today: " << total
         << " (Pending: "   << pending
         << " Completed: "  << completed
         << " No-show: "    << noshow
         << " Cancelled: "  << cancelled << ")\n\n";

    // --- revenue from paid bills today ---
    double revenue = 0;
    for (int i = 0; i < bills.size(); i++) {
        Bill& b = bills.getAll()[i];
        if (strcmp(b.getDate(),   today)  == 0 &&
            strcmp(b.getStatus(), "paid") == 0)
            revenue += b.getAmount();
    }
    cout << "Revenue collected today (paid bills): PKR "
         << fixed << setprecision(2) << revenue << "\n\n";

    // --- patients with outstanding unpaid bills ---
    cout << "Patients with outstanding unpaid bills:\n";
    cout << "Patient Name | Total Owed\n";
    cout << "---------------------------\n";

    bool anyUnpaid = false;
    for (int i = 0; i < patients.size(); i++) {
        Patient& p  = patients.getAll()[i];
        double owed = 0;

        for (int j = 0; j < bills.size(); j++) {
            Bill& b = bills.getAll()[j];
            if (b.getPatientID() == p.getID() &&
                strcmp(b.getStatus(), "unpaid") == 0)
                owed += b.getAmount();
        }

        if (owed > 0) {
            cout << p.getName() << " | PKR "
                 << fixed << setprecision(2) << owed << "\n";
            anyUnpaid = true;
        }
    }
    if (!anyUnpaid) cout << "None.\n";

    // --- doctor wise summary for today ---
    cout << "\nDoctor-wise summary for today:\n";
    cout << "Doctor Name | Completed | Pending | No-show\n";
    cout << "----------------------------------------------\n";

    for (int i = 0; i < doctors.size(); i++) {
        Doctor& d = doctors.getAll()[i];
        int dc = 0, dp = 0, dn = 0;

        for (int j = 0; j < appointments.size(); j++) {
            Appointment& a = appointments.getAll()[j];
            if (a.getDoctorID() != d.getID())         continue;
            if (strcmp(a.getDate(), today) != 0)       continue;

            if      (strcmp(a.getStatus(), "completed") == 0) dc++;
            else if (strcmp(a.getStatus(), "pending")   == 0) dp++;
            else if (strcmp(a.getStatus(), "no-show")   == 0) dn++;
        }

        // only show doctors who had appointments today
        if (dc + dp + dn > 0) {
            cout << d.getName() << " | "
                 << dc << " | "
                 << dp << " | "
                 << dn << "\n";
        }
    }

    cout << "\n=========================================\n";
}