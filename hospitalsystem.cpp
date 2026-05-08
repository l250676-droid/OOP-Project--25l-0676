#include "hospitalsystem.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
using namespace std;

// the 8 fixed daily time slots
const char* HospitalSystem::TIME_SLOTS[8] = {
    "09:00", "10:00", "11:00", "12:00",
    "13:00", "14:00", "15:00", "16:00"
};

// ============================================================
// CONSTRUCTOR
// loads everything from files into memory on startup
// ============================================================
HospitalSystem::HospitalSystem() : currentPatient(nullptr), currentDoctor(nullptr) {
    loadAllData();
}

void HospitalSystem::loadAllData() {
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);

    // if admin.txt doesnt exist create a default admin
    if (!FileHandler::loadAdmin(admin)) {
        admin = Admin(1, "Admin", "admin123");
        ofstream f("admin.txt");
        f << "1,Admin,admin123\n";
    }
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

// returns doctor name given their ID
// returns "Unknown" if ID not found
const char* HospitalSystem::getDoctorName(int doctorID) {
    Doctor* d = doctors.findByID(doctorID);
    return d ? d->getName() : "Unknown";
}

// returns patient name given their ID
const char* HospitalSystem::getPatientName(int patientID) {
    Patient* p = patients.findByID(patientID);
    return p ? p->getName() : "Unknown";
}

// returns doctor fee given their ID
double HospitalSystem::getDoctorFee(int doctorID) {
    Doctor* d = doctors.findByID(doctorID);
    return d ? d->getFee() : 0;
}

// returns doctor specialization given their ID
const char* HospitalSystem::getDoctorSpec(int doctorID) {
    Doctor* d = doctors.findByID(doctorID);
    return d ? d->getSpecialization() : "";
}

// checks if a slot is already booked for a doctor on a date
// a slot is taken if an appointment exists with same doctor,
// same date, same slot AND status is not cancelled
bool HospitalSystem::slotTaken(int doctorID, const char* date, const char* slot) {
    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getDoctorID()  == doctorID        &&
            strcmp(a.getDate(),     date) == 0   &&
            strcmp(a.getTimeSlot(), slot) == 0   &&
            strcmp(a.getStatus(), "cancelled")!= 0) {
            return true;
        }
    }
    return false;
}

// finds the bill ID for a given appointment
// returns -1 if no bill found
int HospitalSystem::getBillForAppointment(int appointmentID) {
    for (int i = 0; i < bills.size(); i++) {
        if (bills.getAll()[i].getAppointmentID() == appointmentID)
            return bills.getAll()[i].getID();
    }
    return -1;
}

// counts how many unpaid bills a patient has
int HospitalSystem::countUnpaidBills(int patientID) {
    int count = 0;
    for (int i = 0; i < bills.size(); i++) {
        Bill& b = bills.getAll()[i];
        if (b.getPatientID() == patientID &&
            strcmp(b.getStatus(), "unpaid") == 0)
            count++;
    }
    return count;
}

// counts how many pending appointments a patient has
int HospitalSystem::countPendingAppointments(int patientID) {
    int count = 0;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getPatientID() == patientID &&
            strcmp(a.getStatus(), "pending") == 0)
            count++;
    }
    return count;
}

// ============================================================
// MAIN RUN LOOP
// shows the startup menu and routes to correct login
// ============================================================
void HospitalSystem::run() {
    while (true) {
        cout << "\nWelcome to MediCore Hospital Management System\n";
        cout << "===============================================\n";
        cout << "Login as:\n";
        cout << "1. Patient\n";
        cout << "2. Doctor\n";
        cout << "3. Admin\n";
        cout << "4. Exit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        if      (choice == 1) { if (loginPatient()) patientMenu(); }
        else if (choice == 2) { if (loginDoctor())  doctorMenu();  }
        else if (choice == 3) { if (loginAdmin())   adminMenu();   }
        else if (choice == 4) { cout << "Goodbye!\n"; break;       }
        else                  { cout << "Invalid choice.\n";       }
    }
}

// ============================================================
// LOGIN FUNCTIONS
// each tries 3 times then locks
// logs every failed attempt to security_log.txt
// ============================================================
bool HospitalSystem::loginPatient() {
    char idStr[20], pass[50];
    int attempts = 0;

    while (attempts < 3) {
        cout << "Enter Patient ID: ";   cin >> idStr;
        cout << "Enter Password: ";     cin >> pass;

        // convert id string to int manually
        int id = 0;
        for (int i = 0; idStr[i] != '\0'; i++)
            id = id * 10 + (idStr[i] - '0');

        Patient* p = patients.findByID(id);

        if (p != nullptr && strcmp(p->getPassword(), pass) == 0) {
            currentPatient = p;
            return true;
        }

        attempts++;
        FileHandler::logSecurity("Patient", idStr, "FAILED");
        cout << "Invalid credentials. "
             << (3 - attempts) << " attempt(s) remaining.\n";
    }

    cout << "Account locked. Contact admin.\n";
    return false;
}

bool HospitalSystem::loginDoctor() {
    char idStr[20], pass[50];
    int attempts = 0;

    while (attempts < 3) {
        cout << "Enter Doctor ID: ";  cin >> idStr;
        cout << "Enter Password: ";   cin >> pass;

        int id = 0;
        for (int i = 0; idStr[i] != '\0'; i++)
            id = id * 10 + (idStr[i] - '0');

        Doctor* d = doctors.findByID(id);

        if (d != nullptr && strcmp(d->getPassword(), pass) == 0) {
            currentDoctor = d;
            return true;
        }

        attempts++;
        FileHandler::logSecurity("Doctor", idStr, "FAILED");
        cout << "Invalid credentials. "
             << (3 - attempts) << " attempt(s) remaining.\n";
    }

    cout << "Account locked. Contact admin.\n";
    return false;
}

bool HospitalSystem::loginAdmin() {
    char idStr[20], pass[50];
    int attempts = 0;

    while (attempts < 3) {
        cout << "Enter Admin ID: ";   cin >> idStr;
        cout << "Enter Password: ";   cin >> pass;

        int id = 0;
        for (int i = 0; idStr[i] != '\0'; i++)
            id = id * 10 + (idStr[i] - '0');

        if (id == admin.getID() &&
            strcmp(admin.getPassword(), pass) == 0)
            return true;

        attempts++;
        FileHandler::logSecurity("Admin", idStr, "FAILED");
        cout << "Invalid credentials. "
             << (3 - attempts) << " attempt(s) remaining.\n";
    }

    cout << "Account locked. Contact admin.\n";
    return false;
}

// ============================================================
// PATIENT MENU
// loops until patient chooses logout
// ============================================================
void HospitalSystem::patientMenu() {
    while (true) {
        currentPatient->displayMenu();
        int choice;
        cin >> choice;

        if      (choice == 1) bookAppointment();
        else if (choice == 2) cancelAppointment();
        else if (choice == 3) viewMyAppointments();
        else if (choice == 4) viewMyMedicalRecords();
        else if (choice == 5) viewMyBills();
        else if (choice == 6) payBill();
        else if (choice == 7) topUpBalance();
        else if (choice == 8) {
            currentPatient = nullptr;
            break;
        }
        else cout << "Invalid choice.\n";
    }
}

// ============================================================
// 1. BOOK APPOINTMENT
// ============================================================
void HospitalSystem::bookAppointment() {
    // --- step 1: search by specialization ---
    char spec[50];
    cout << "Enter specialization to search (e.g. Cardiology): ";
    cin.ignore();
    cin.getline(spec, 49);

    // display matching doctors — case insensitive manual compare
    bool anyDoctors = false;
    for (int i = 0; i < doctors.size(); i++) {
        Doctor& d = doctors.getAll()[i];
        if (Validator::strEqualCI(d.getSpecialization(), spec)) {
            cout << "ID: " << d.getID()
                 << " | Name: " << d.getName()
                 << " | Fee: PKR " << d.getFee() << "\n";
            anyDoctors = true;
        }
    }
    if (!anyDoctors) {
        cout << "No doctors available for that specialization.\n";
        return;
    }

    // --- step 2: pick a doctor ---
    char idStr[20];
    cout << "Enter Doctor ID: ";
    cin >> idStr;

    int did = 0;
    for (int i = 0; idStr[i] != '\0'; i++)
        did = did * 10 + (idStr[i] - '0');

    Doctor* doc = doctors.findByID(did);
    if (doc == nullptr) {
        cout << "Doctor not found.\n";
        return;
    }

    // --- step 3: enter date with up to 3 attempts ---
    char dateStr[12];
    int dateAttempts = 0;
    bool validDate   = false;

    while (dateAttempts < 3) {
        cout << "Enter date (DD-MM-YYYY): ";
        cin >> dateStr;

        if (Validator::isValidDate(dateStr) &&
            Validator::dateNotPast(dateStr)) {
            validDate = true;
            break;
        }
        cout << "Invalid date. Use format DD-MM-YYYY.\n";
        dateAttempts++;
    }
    if (!validDate) return;

    // --- step 4: show available slots ---
    // lambda-style helper using a local function approach
    // we show this twice so we put it in a block we can repeat
    auto showAvailableSlots = [&]() {
        cout << "Available slots on " << dateStr << ":\n";
        bool anySlots = false;
        for (int i = 0; i < 8; i++) {
            if (!slotTaken(did, dateStr, TIME_SLOTS[i])) {
                cout << "  " << TIME_SLOTS[i] << "\n";
                anySlots = true;
            }
        }
        if (!anySlots) cout << "  No slots available.\n";
    };

    showAvailableSlots();

    // --- step 5: pick a slot ---
    char slot[6];
    while (true) {
        cout << "Enter time slot (e.g. 09:00): ";
        cin >> slot;

        if (!Validator::isValidTimeSlot(slot)) {
            cout << "Invalid time slot.\n";
            continue;
        }

        try {
            if (slotTaken(did, dateStr, slot))
                throw SlotUnavailableException(
                    "That slot is already booked. Please choose another.");
            break; // slot is free — exit loop
        }
        catch (SlotUnavailableException& e) {
            cout << e.what() << "\n";
            showAvailableSlots();
        }
    }

    // --- step 6: check balance ---
    try {
        if (currentPatient->getBalance() < doc->getFee())
            throw InsufficientFundsException(
                "Insufficient balance. Please top up your account.");
    }
    catch (InsufficientFundsException& e) {
        cout << e.what() << "\n";
        return;
    }

    // --- step 7: deduct fee using -= operator ---
    *currentPatient -= doc->getFee();

    // --- step 8: generate new IDs ---
    int newAppID  = FileHandler::getMaxID("appointments.txt", 0) + 1;
    int newBillID = FileHandler::getMaxID("bill.txt",         0) + 1;

    char today[12];
    Validator::getCurrentDate(today);

    // --- step 9: create appointment and bill ---
    Appointment newApp(newAppID,
                       currentPatient->getID(),
                       did,
                       dateStr,
                       slot,
                       "pending");

    Bill newBill(newBillID,
                 currentPatient->getID(),
                 newAppID,
                 doc->getFee(),
                 "unpaid",
                 today);

    // add to storage
    appointments.add(newApp);
    bills.add(newBill);

    // write to files
    FileHandler::appendAppointment(newApp);
    FileHandler::appendBill(newBill);
    FileHandler::saveAllPatients(patients); // save updated balance

    cout << "Appointment booked successfully. "
         << "Appointment ID: " << newAppID << "\n";
}

// ============================================================
// 2. CANCEL APPOINTMENT
// ============================================================
void HospitalSystem::cancelAppointment() {
    // show all pending appointments for this patient
    bool any = false;
    cout << "Appointment ID | Doctor Name | Date | Time Slot\n";
    cout << "--------------------------------------------------\n";

    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getPatientID() == currentPatient->getID() &&
            strcmp(a.getStatus(), "pending") == 0) {
            cout << a.getID()              << " | "
                 << getDoctorName(a.getDoctorID()) << " | "
                 << a.getDate()            << " | "
                 << a.getTimeSlot()        << "\n";
            any = true;
        }
    }

    if (!any) {
        cout << "You have no pending appointments.\n";
        return;
    }

    // ask which one to cancel
    cout << "Enter Appointment ID to cancel: ";
    int apID; cin >> apID;

    Appointment* ap = appointments.findByID(apID);

    // validate it belongs to this patient and is pending
    if (ap == nullptr ||
        ap->getPatientID() != currentPatient->getID() ||
        strcmp(ap->getStatus(), "pending") != 0) {
        cout << "Invalid appointment ID.\n";
        return;
    }

    // get the fee to refund
    double fee = getDoctorFee(ap->getDoctorID());

    // update appointment status
    ap->setStatus("cancelled");

    // refund fee using += operator
    *currentPatient += fee;

    // update the corresponding bill to cancelled
    int billID = getBillForAppointment(apID);
    if (billID != -1) {
        Bill* b = bills.findByID(billID);
        if (b != nullptr) b->setStatus("cancelled");
    }

    // save everything to files
    FileHandler::saveAllAppointments(appointments);
    FileHandler::saveAllBills(bills);
    FileHandler::saveAllPatients(patients);

    cout << "Appointment cancelled. PKR "
         << fixed << setprecision(2) << fee
         << " refunded to your balance.\n";
}

// ============================================================
// 3. VIEW MY APPOINTMENTS
// sorted by date ascending — manual bubble sort
// ============================================================
void HospitalSystem::viewMyAppointments() {
    // collect indices of this patient's appointments
    int idx[100];
    int n = 0;

    for (int i = 0; i < appointments.size(); i++) {
        if (appointments.getAll()[i].getPatientID() == currentPatient->getID())
            idx[n++] = i;
    }

    if (n == 0) {
        cout << "No appointments found.\n";
        return;
    }

    // bubble sort by date ascending
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            const char* dateA = appointments.getAll()[idx[j]].getDate();
            const char* dateB = appointments.getAll()[idx[j+1]].getDate();
            if (Validator::compareDates(dateA, dateB) > 0) {
                int temp  = idx[j];
                idx[j]    = idx[j+1];
                idx[j+1]  = temp;
            }
        }
    }

    cout << "ID | Doctor Name | Specialization | Date | Time Slot | Status\n";
    cout << "----------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        Appointment& a = appointments.getAll()[idx[i]];
        cout << a.getID()                          << " | "
             << getDoctorName(a.getDoctorID())     << " | "
             << getDoctorSpec(a.getDoctorID())     << " | "
             << a.getDate()                        << " | "
             << a.getTimeSlot()                    << " | "
             << a.getStatus()                      << "\n";
    }
}

// ============================================================
// 4. VIEW MY MEDICAL RECORDS
// sorted by date descending — manual bubble sort
// ============================================================
void HospitalSystem::viewMyMedicalRecords() {
    int idx[100];
    int n = 0;

    for (int i = 0; i < prescriptions.size(); i++) {
        if (prescriptions.getAll()[i].getPatientID() == currentPatient->getID())
            idx[n++] = i;
    }

    if (n == 0) {
        cout << "No medical records found.\n";
        return;
    }

    // bubble sort by date descending (most recent first)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            const char* dateA = prescriptions.getAll()[idx[j]].getDate();
            const char* dateB = prescriptions.getAll()[idx[j+1]].getDate();
            // descending so swap if A is earlier than B
            if (Validator::compareDates(dateA, dateB) < 0) {
                int temp  = idx[j];
                idx[j]    = idx[j+1];
                idx[j+1]  = temp;
            }
        }
    }

    cout << "Date | Doctor Name | Medicines | Notes\n";
    cout << "----------------------------------------\n";

    for (int i = 0; i < n; i++) {
        Prescription& p = prescriptions.getAll()[idx[i]];
        cout << p.getDate()                    << " | "
             << getDoctorName(p.getDoctorID()) << " | "
             << p.getMedicines()               << " | "
             << p.getNotes()                   << "\n";
    }
}

// ============================================================
// 5. VIEW MY BILLS
// ============================================================
void HospitalSystem::viewMyBills() {
    bool any       = false;
    double unpaid  = 0;

    cout << "Bill ID | Appointment ID | Amount (PKR) | Status | Date\n";
    cout << "-----------------------------------------------------------\n";

    for (int i = 0; i < bills.size(); i++) {
        Bill& b = bills.getAll()[i];
        if (b.getPatientID() == currentPatient->getID()) {
            cout << b.getID()            << " | "
                 << b.getAppointmentID() << " | "
                 << fixed << setprecision(2) << b.getAmount() << " | "
                 << b.getStatus()        << " | "
                 << b.getDate()          << "\n";

            if (strcmp(b.getStatus(), "unpaid") == 0)
                unpaid += b.getAmount();

            any = true;
        }
    }

    if (!any) {
        cout << "No bills found.\n";
        return;
    }

    cout << "Total outstanding unpaid: PKR "
         << fixed << setprecision(2) << unpaid << "\n";
}

// ============================================================
// 6. PAY BILL
// ============================================================
void HospitalSystem::payBill() {
    // show unpaid bills
    bool any = false;
    cout << "Unpaid Bills:\n";
    cout << "Bill ID | Amount | Date\n";
    cout << "------------------------\n";

    for (int i = 0; i < bills.size(); i++) {
        Bill& b = bills.getAll()[i];
        if (b.getPatientID() == currentPatient->getID() &&
            strcmp(b.getStatus(), "unpaid") == 0) {
            cout << b.getID() << " | PKR "
                 << fixed << setprecision(2) << b.getAmount()
                 << " | " << b.getDate() << "\n";
            any = true;
        }
    }

    if (!any) {
        cout << "No unpaid bills.\n";
        return;
    }

    cout << "Enter Bill ID to pay: ";
    int billID; cin >> billID;

    Bill* b = bills.findByID(billID);

    // validate bill belongs to this patient and is unpaid
    if (b == nullptr ||
        b->getPatientID() != currentPatient->getID() ||
        strcmp(b->getStatus(), "unpaid") != 0) {
        cout << "Invalid bill.\n";
        return;
    }

    // check balance
    try {
        if (currentPatient->getBalance() < b->getAmount())
            throw InsufficientFundsException(
                "Insufficient balance to pay this bill. Please top up.");
    }
    catch (InsufficientFundsException& e) {
        cout << e.what() << "\n";
        return;
    }

    // deduct using -= operator
    *currentPatient -= b->getAmount();
    b->setStatus("paid");

    FileHandler::saveAllBills(bills);
    FileHandler::saveAllPatients(patients);

    cout << "Bill paid successfully. Remaining balance: PKR "
         << fixed << setprecision(2) << currentPatient->getBalance() << "\n";
}

// ============================================================
// 7. TOP UP BALANCE
// up to 3 attempts using InvalidInputException
// ============================================================
void HospitalSystem::topUpBalance() {
    int attempts = 0;

    while (attempts < 3) {
        cout << "Enter amount to add (PKR): ";
        char inp[50];
        cin >> inp;

        try {
            if (!Validator::isPositiveFloat(inp))
                throw InvalidInputException(
                    "Invalid amount. Must be a positive number greater than 0.");

            // convert manually
            double amt = 0;
            int i = 0;
            while (inp[i] != '\0' && inp[i] != '.') {
                amt = amt * 10 + (inp[i] - '0');
                i++;
            }
            if (inp[i] == '.') {
                i++;
                double place = 0.1;
                while (inp[i] != '\0') {
                    amt += (inp[i] - '0') * place;
                    place /= 10;
                    i++;
                }
            }

            // add to balance using += operator
            *currentPatient += amt;
            FileHandler::saveAllPatients(patients);

            cout << "Balance updated. New balance: PKR "
                 << fixed << setprecision(2)
                 << currentPatient->getBalance() << "\n";
            return;
        }
        catch (InvalidInputException& e) {
            cout << e.what() << "\n";
            attempts++;
        }
    }
}

// ============================================================
// DOCTOR MENU
// loops until doctor chooses logout
// ============================================================
void HospitalSystem::doctorMenu() {
    while (true) {
        currentDoctor->displayMenu();
        int choice;
        cin >> choice;

        if      (choice == 1) viewTodayAppointments();
        else if (choice == 2) markComplete();
        else if (choice == 3) markNoShow();
        else if (choice == 4) writePrescription();
        else if (choice == 5) viewPatientHistory();
        else if (choice == 6) {
            currentDoctor = nullptr;
            break;
        }
        else cout << "Invalid choice.\n";
    }
}

// ============================================================
// 1. VIEW TODAY'S APPOINTMENTS
// sorted by time slot ascending
// ============================================================
void HospitalSystem::viewTodayAppointments() {
    char today[12];
    Validator::getCurrentDate(today);

    int idx[100];
    int n = 0;

    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getDoctorID() == currentDoctor->getID() &&
            strcmp(a.getDate(), today) == 0)
            idx[n++] = i;
    }

    if (n == 0) {
        cout << "No appointments scheduled for today.\n";
        return;
    }

    // bubble sort by time slot ascending (string compare works
    // here because slots are in HH:MM format with leading zeros)
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            const char* tA = appointments.getAll()[idx[j]].getTimeSlot();
            const char* tB = appointments.getAll()[idx[j+1]].getTimeSlot();
            if (strcmp(tA, tB) > 0) {
                int temp  = idx[j];
                idx[j]    = idx[j+1];
                idx[j+1]  = temp;
            }
        }
    }

    cout << "Appointment ID | Patient Name | Time Slot | Status\n";
    cout << "-----------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        Appointment& a = appointments.getAll()[idx[i]];
        cout << a.getID()                           << " | "
             << getPatientName(a.getPatientID())    << " | "
             << a.getTimeSlot()                     << " | "
             << a.getStatus()                       << "\n";
    }
}

// ============================================================
// 2. MARK APPOINTMENT COMPLETE
// ============================================================
void HospitalSystem::markComplete() {
    char today[12];
    Validator::getCurrentDate(today);

    // show today's pending appointments for this doctor
    bool any = false;
    cout << "Today's pending appointments:\n";

    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getDoctorID() == currentDoctor->getID() &&
            strcmp(a.getDate(),   today)     == 0      &&
            strcmp(a.getStatus(), "pending") == 0) {
            cout << a.getID() << " | "
                 << getPatientName(a.getPatientID()) << " | "
                 << a.getTimeSlot() << "\n";
            any = true;
        }
    }

    if (!any) {
        cout << "No pending appointments today.\n";
        return;
    }

    cout << "Enter Appointment ID: ";
    int apID; cin >> apID;

    Appointment* ap = appointments.findByID(apID);

    if (ap == nullptr                              ||
        ap->getDoctorID() != currentDoctor->getID()||
        strcmp(ap->getStatus(), "pending")  != 0   ||
        strcmp(ap->getDate(),   today)      != 0) {
        cout << "Invalid appointment.\n";
        return;
    }

    ap->setStatus("completed");
    FileHandler::saveAllAppointments(appointments);
    cout << "Appointment marked as completed.\n";
}

// ============================================================
// 3. MARK APPOINTMENT NO-SHOW
// no refund — bill becomes cancelled
// ============================================================
void HospitalSystem::markNoShow() {
    char today[12];
    Validator::getCurrentDate(today);

    bool any = false;
    cout << "Today's pending appointments:\n";

    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getDoctorID() == currentDoctor->getID() &&
            strcmp(a.getDate(),   today)     == 0      &&
            strcmp(a.getStatus(), "pending") == 0) {
            cout << a.getID() << " | "
                 << getPatientName(a.getPatientID()) << " | "
                 << a.getTimeSlot() << "\n";
            any = true;
        }
    }

    if (!any) {
        cout << "No pending appointments today.\n";
        return;
    }

    cout << "Enter Appointment ID: ";
    int apID; cin >> apID;

    Appointment* ap = appointments.findByID(apID);

    if (ap == nullptr                              ||
        ap->getDoctorID() != currentDoctor->getID()||
        strcmp(ap->getStatus(), "pending")  != 0   ||
        strcmp(ap->getDate(),   today)      != 0) {
        cout << "Invalid appointment.\n";
        return;
    }

    ap->setStatus("no-show");

    // mark the bill as cancelled — no refund
    int billID = getBillForAppointment(apID);
    if (billID != -1) {
        Bill* b = bills.findByID(billID);
        if (b != nullptr) b->setStatus("cancelled");
    }

    FileHandler::saveAllAppointments(appointments);
    FileHandler::saveAllBills(bills);
    cout << "Appointment marked as no-show.\n";
}

// ============================================================
// 4. WRITE PRESCRIPTION
// ============================================================
void HospitalSystem::writePrescription() {
    cout << "Enter Appointment ID: ";
    int apID; cin >> apID;

    Appointment* ap = appointments.findByID(apID);

    // must belong to this doctor and be completed
    if (ap == nullptr                               ||
        ap->getDoctorID() != currentDoctor->getID() ||
        strcmp(ap->getStatus(), "completed") != 0) {
        cout << "Invalid appointment. Must be completed "
                "and belong to you.\n";
        return;
    }

    // check if prescription already written for this appointment
    for (int i = 0; i < prescriptions.size(); i++) {
        if (prescriptions.getAll()[i].getAppointmentID() == apID) {
            cout << "Prescription already written "
                    "for this appointment.\n";
            return;
        }
    }

    // read medicines — max 499 chars, truncate silently
    char medicines[500];
    cout << "Enter medicines "
            "(e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
    cin.ignore();
    cin.getline(medicines, 499);

    // read notes — max 299 chars, truncate silently
    char notes[300];
    cout << "Enter notes (max 300 chars): ";
    cin.getline(notes, 299);

    // generate new prescription ID
    int newID = FileHandler::getMaxID("prescription.txt", 0) + 1;

    Prescription pr(newID,
                    apID,
                    ap->getPatientID(),
                    currentDoctor->getID(),
                    ap->getDate(),
                    medicines,
                    notes);

    prescriptions.add(pr);
    FileHandler::appendPrescription(pr);
    cout << "Prescription saved.\n";
}

// ============================================================
// 5. VIEW PATIENT MEDICAL HISTORY
// doctor can only see their own patients
// ============================================================
void HospitalSystem::viewPatientHistory() {
    cout << "Enter Patient ID: ";
    int pid; cin >> pid;

    Patient* pat = patients.findByID(pid);
    if (pat == nullptr) {
        cout << "Access denied. You can only view "
                "records of your own patients.\n";
        return;
    }

    // check this doctor has at least one completed
    // appointment with this patient
    bool hasRecord = false;
    for (int i = 0; i < appointments.size(); i++) {
        Appointment& a = appointments.getAll()[i];
        if (a.getPatientID() == pid                   &&
            a.getDoctorID()  == currentDoctor->getID() &&
            strcmp(a.getStatus(), "completed") == 0) {
            hasRecord = true;
            break;
        }
    }

    if (!hasRecord) {
        cout << "Access denied. You can only view "
                "records of your own patients.\n";
        return;
    }

    // collect prescriptions written by this doctor for this patient
    int idx[100];
    int n = 0;

    for (int i = 0; i < prescriptions.size(); i++) {
        Prescription& p = prescriptions.getAll()[i];
        if (p.getPatientID() == pid &&
            p.getDoctorID()  == currentDoctor->getID())
            idx[n++] = i;
    }

    if (n == 0) {
        cout << "No prescriptions found.\n";
        return;
    }

    // sort by date descending
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            const char* dateA = prescriptions.getAll()[idx[j]].getDate();
            const char* dateB = prescriptions.getAll()[idx[j+1]].getDate();
            if (Validator::compareDates(dateA, dateB) < 0) {
                int temp  = idx[j];
                idx[j]    = idx[j+1];
                idx[j+1]  = temp;
            }
        }
    }

    cout << "Date | Medicines | Notes\n";
    cout << "--------------------------\n";

    for (int i = 0; i < n; i++) {
        Prescription& p = prescriptions.getAll()[idx[i]];
        cout << p.getDate()      << " | "
             << p.getMedicines() << " | "
             << p.getNotes()     << "\n";
    }
}