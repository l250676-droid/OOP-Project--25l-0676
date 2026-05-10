#include "filehandler.h"
#include <fstream>
#include <cstring>
#include <ctime>
// splitCSV
// takes one line like "1,Ahmed Ali,28,M,03001234567,pass123,5000.00"
// splits it into fields array:
// fields[0]="1"  fields[1]="Ahmed Ali"  fields[2]="28" etc
// count tells you how many fields were found
void FileHandler::splitCSV(const char* line, char fields[][256], int& count) {
    count = 0;
    int fieldIndex = 0;
    int charIndex  = 0;
    fields[0][0]   = '\0';

    for (int i = 0; line[i] != '\0' && line[i] != '\n' && line[i] != '\r'; i++) {
        if (line[i] == ',') {
            // end of this field — null terminate and move to next
            fields[fieldIndex][charIndex] = '\0';
            fieldIndex++;
            charIndex = 0;
            if (fieldIndex < 20) fields[fieldIndex][0] = '\0';
        } else {
            // add character to current field
            if (charIndex < 255) {
                fields[fieldIndex][charIndex] = line[i];
                charIndex++;
            }
        }
    }
    // null terminate the last field
    fields[fieldIndex][charIndex] = '\0';
    count = fieldIndex + 1;
}

// parsePatient
// line format: id,name,age,gender,contact,password,balance
// example:     1,Ahmed Ali,28,M,03001234567,pass123,5000.00
Patient FileHandler::parsePatient(const char* line) {
    char f[10][256];
    int c;
    splitCSV(line, f, c);
    if (c < 7) return Patient();

    // f[0]=id  f[1]=name  f[2]=age  f[3]=gender
    // f[4]=contact  f[5]=password  f[6]=balance
    int id  = 0;
    int age = 0;

    // convert id manually
    for (int i = 0; f[0][i] != '\0'; i++)
        id = id * 10 + (f[0][i] - '0');

    // convert age manually
    for (int i = 0; f[2][i] != '\0'; i++)
        age = age * 10 + (f[2][i] - '0');

    // convert balance manually
    double balance = 0;
    int j = 0;
    while (f[6][j] != '\0' && f[6][j] != '.') {
        balance = balance * 10 + (f[6][j] - '0');
        j++;
    }
    if (f[6][j] == '.') {
        j++;
        double place = 0.1;
        while (f[6][j] != '\0') {
            balance += (f[6][j] - '0') * place;
            place /= 10;
            j++;
        }
    }

    return Patient(id, f[1], age, f[3], f[4], f[5], balance);
}

// parseDoctor
// line format: id,name,specialization,contact,password,fee
// example:     1,Dr. Sara Khan,Cardiology,03111234567,doc456,1500.00
Doctor FileHandler::parseDoctor(const char* line) {
    char f[10][256];
    int c;
    splitCSV(line, f, c);
    if (c < 6) return Doctor();

    int id = 0;
    for (int i = 0; f[0][i] != '\0'; i++)
        id = id * 10 + (f[0][i] - '0');

    double fee = 0;
    int j = 0;
    while (f[5][j] != '\0' && f[5][j] != '.') {
        fee = fee * 10 + (f[5][j] - '0');
        j++;
    }
    if (f[5][j] == '.') {
        j++;
        double place = 0.1;
        while (f[5][j] != '\0') {
            fee += (f[5][j] - '0') * place;
            place /= 10;
            j++;
        }
    }

    return Doctor(id, f[1], f[2], f[3], f[4], fee);
}
// parseAppointment
// line format: id,patientID,doctorID,date,timeSlot,status
// example:     1,1,1,15-04-2025,09:00,completed
Appointment FileHandler::parseAppointment(const char* line) {
    char f[10][256];
    int c;
    splitCSV(line, f, c);
    if (c < 6) return Appointment();

    int id  = 0, pid = 0, did = 0;
    for (int i = 0; f[0][i] != '\0'; i++) id  = id  * 10 + (f[0][i] - '0');
    for (int i = 0; f[1][i] != '\0'; i++) pid = pid * 10 + (f[1][i] - '0');
    for (int i = 0; f[2][i] != '\0'; i++) did = did * 10 + (f[2][i] - '0');

    return Appointment(id, pid, did, f[3], f[4], f[5]);
}

// parseBill
// line format: id,patientID,appointmentID,amount,status,date
// example:     1,1,1,1500.00,paid,15-04-2025
Bill FileHandler::parseBill(const char* line) {
    char f[10][256];
    int c;
    splitCSV(line, f, c);
    if (c < 6) return Bill();

    int id = 0, pid = 0, apid = 0;
    for (int i = 0; f[0][i] != '\0'; i++) id   = id   * 10 + (f[0][i] - '0');
    for (int i = 0; f[1][i] != '\0'; i++) pid  = pid  * 10 + (f[1][i] - '0');
    for (int i = 0; f[2][i] != '\0'; i++) apid = apid * 10 + (f[2][i] - '0');

    double amount = 0;
    int j = 0;
    while (f[3][j] != '\0' && f[3][j] != '.') {
        amount = amount * 10 + (f[3][j] - '0');
        j++;
    }
    if (f[3][j] == '.') {
        j++;
        double place = 0.1;
        while (f[3][j] != '\0') {
            amount += (f[3][j] - '0') * place;
            place /= 10;
            j++;
        }
    }

    return Bill(id, pid, apid, amount, f[4], f[5]);
}
// parsePrescription
// line format: id,appointmentID,patientID,doctorID,date,medicines,notes
// example:     1,1,1,1,15-04-2025,Paracetamol 500mg,Take after meals
Prescription FileHandler::parsePrescription(const char* line) {
    char f[10][256];
    int c;
    splitCSV(line, f, c);
    if (c < 7) return Prescription();

    int id = 0, apid = 0, pid = 0, did = 0;
    for (int i = 0; f[0][i] != '\0'; i++) id   = id   * 10 + (f[0][i] - '0');
    for (int i = 0; f[1][i] != '\0'; i++) apid = apid * 10 + (f[1][i] - '0');
    for (int i = 0; f[2][i] != '\0'; i++) pid  = pid  * 10 + (f[2][i] - '0');
    for (int i = 0; f[3][i] != '\0'; i++) did  = did  * 10 + (f[3][i] - '0');

    return Prescription(id, apid, pid, did, f[4], f[5], f[6]);
}
// LOAD FUNCTIONS
// open the file, read line by line, parse each line into
// an object and add it to the Storage
void FileHandler::loadPatients(Storage<Patient>& s) {
    ifstream fin("patient.txt");
    if (!fin.is_open()) {
        // file doesnt exist yet — create empty file
        ofstream fout("patient.txt");
        return;
    }
    char line[512];
    while (fin.getline(line, 511)) {
        if (line[0] == '\0') continue; // skip empty lines
        Patient p = parsePatient(line);
        if (p.getID() > 0) s.add(p);
    }
    fin.close();
}

void FileHandler::loadDoctors(Storage<Doctor>& s) {
    ifstream fin("doctor.txt");
    if (!fin.is_open()) {
        ofstream fout("doctor.txt");
        return;
    }
    char line[512];
    while (fin.getline(line, 511)) {
        if (line[0] == '\0') continue;
        Doctor d = parseDoctor(line);
        if (d.getID() > 0) s.add(d);
    }
    fin.close();
}

void FileHandler::loadAppointments(Storage<Appointment>& s) {
    ifstream fin("appointments.txt");
    if (!fin.is_open()) {
        ofstream fout("appointments.txt");
        return;
    }
    char line[512];
    while (fin.getline(line, 511)) {
        if (line[0] == '\0') continue;
        Appointment a = parseAppointment(line);
        if (a.getID() > 0) s.add(a);
    }
    fin.close();
}

void FileHandler::loadBills(Storage<Bill>& s) {
    ifstream fin("bill.txt");
    if (!fin.is_open()) {
        ofstream fout("bill.txt");
        return;
    }
    char line[512];
    while (fin.getline(line, 511)) {
        if (line[0] == '\0') continue;
        Bill b = parseBill(line);
        if (b.getID() > 0) s.add(b);
    }
    fin.close();
}

void FileHandler::loadPrescriptions(Storage<Prescription>& s) {
    ifstream fin("prescription.txt");
    if (!fin.is_open()) {
        ofstream fout("prescription.txt");
        return;
    }
    char line[1024];
    while (fin.getline(line, 1023)) {
        if (line[0] == '\0') continue;
        Prescription p = parsePrescription(line);
        if (p.getID() > 0) s.add(p);
    }
    fin.close();
}

bool FileHandler::loadAdmin(Admin& admin) {
    ifstream fin("admin.txt");
    if (!fin.is_open()) return false;
    char line[256];
    if (fin.getline(line, 255)) {
        char f[10][256];
        int c;
        splitCSV(line, f, c);
        if (c >= 3) {
            int id = 0;
            for (int i = 0; f[0][i] != '\0'; i++)
                id = id * 10 + (f[0][i] - '0');
            admin = Admin(id, f[1], f[2]);
            fin.close();
            return true;
        }
    }
    fin.close();
    return false;
}

// APPEND FUNCTIONS
// add ONE new record to the end of the file
// used when creating a new patient, appointment, bill etc
// ios::app means open in append mode — dont overwrite
void FileHandler::appendPatient(const Patient& p) {
    ofstream f("patient.txt", ios::app);
    f << p.getID()      << ","
      << p.getName()    << ","
      << p.getAge()     << ","
      << p.getGender()  << ","
      << p.getContact() << ","
      << p.getPassword()<< ","
      << p.getBalance() << "\n";
}

void FileHandler::appendDoctor(const Doctor& d) {
    ofstream f("doctor.txt", ios::app);
    f << d.getID()             << ","
      << d.getName()           << ","
      << d.getSpecialization() << ","
      << d.getContact()        << ","
      << d.getPassword()       << ","
      << d.getFee()            << "\n";
}

void FileHandler::appendAppointment(const Appointment& a) {
    ofstream f("appointments.txt", ios::app);
    f << a.getID()        << ","
      << a.getPatientID() << ","
      << a.getDoctorID()  << ","
      << a.getDate()      << ","
      << a.getTimeSlot()  << ","
      << a.getStatus()    << "\n";
}

void FileHandler::appendBill(const Bill& b) {
    ofstream f("bill.txt", ios::app);
    f << b.getID()            << ","
      << b.getPatientID()     << ","
      << b.getAppointmentID()<< ","
      << b.getAmount()        << ","
      << b.getStatus()        << ","
      << b.getDate()          << "\n";
}

void FileHandler::appendPrescription(const Prescription& p) {
    ofstream f("prescription.txt", ios::app);
    f << p.getID()            << ","
      << p.getAppointmentID()<< ","
      << p.getPatientID()     << ","
      << p.getDoctorID()      << ","
      << p.getDate()          << ","
      << p.getMedicines()     << ","
      << p.getNotes()         << "\n";
}

// SAVE ALL FUNCTIONS
// rewrites the ENTIRE file from scratch using what is in Storage
// used after any update (status change, balance change) or delete
void FileHandler::saveAllPatients(Storage<Patient>& s) {
    ofstream f("patient.txt");
    for (int i = 0; i < s.size(); i++) {
        Patient& p = s.getAll()[i];
        f << p.getID()      << ","
          << p.getName()    << ","
          << p.getAge()     << ","
          << p.getGender()  << ","
          << p.getContact() << ","
          << p.getPassword()<< ","
          << p.getBalance() << "\n";
    }
}

void FileHandler::saveAllDoctors(Storage<Doctor>& s) {
    ofstream f("doctor.txt");
    for (int i = 0; i < s.size(); i++) {
        Doctor& d = s.getAll()[i];
        f << d.getID()             << ","
          << d.getName()           << ","
          << d.getSpecialization() << ","
          << d.getContact()        << ","
          << d.getPassword()       << ","
          << d.getFee()            << "\n";
    }
}

void FileHandler::saveAllAppointments(Storage<Appointment>& s) {
    ofstream f("appointments.txt");
    for (int i = 0; i < s.size(); i++) {
        Appointment& a = s.getAll()[i];
        f << a.getID()        << ","
          << a.getPatientID() << ","
          << a.getDoctorID()  << ","
          << a.getDate()      << ","
          << a.getTimeSlot()  << ","
          << a.getStatus()    << "\n";
    }
}

void FileHandler::saveAllBills(Storage<Bill>& s) {
    ofstream f("bill.txt");
    for (int i = 0; i < s.size(); i++) {
        Bill& b = s.getAll()[i];
        f << b.getID()             << ","
          << b.getPatientID()      << ","
          << b.getAppointmentID()  << ","
          << b.getAmount()         << ","
          << b.getStatus()         << ","
          << b.getDate()           << "\n";
    }
}
// logSecurity
// writes a failed login attempt to security_log.txt
// format: timestamp,role,enteredID,result
void FileHandler::logSecurity(const char* role,const char* enteredID,const char* result) {
    ofstream f("security_log.txt", ios::app);

    // get current time as a string using ctime
    time_t t = time(nullptr);
    char* raw = ctime(&t);

    // raw is like "Tue May  5 14:32:10 2026\n"
    // we write it without the newline at the end
    // find where newline is and stop there
    char timestamp[30];
    int i = 0;
    while (raw[i] != '\n' && raw[i] != '\0' && i < 29) {
        timestamp[i] = raw[i];
        i++;
    }
    timestamp[i] = '\0';

    f << timestamp << ","
      << role      << ","
      << enteredID << ","
      << result    << "\n";
}

// getMaxID
// reads a file and finds the largest ID in the given column
// col=0 means first column (which is always the ID column)
// used to generate new IDs: newID = getMaxID(...) + 1
int FileHandler::getMaxID(const char* filename, int col) {
    ifstream fin(filename);
    if (!fin.is_open()) return 0;

    char line[1024];
    int maxID = 0;

    while (fin.getline(line, 1023)) {
        if (line[0] == '\0') continue;
        char f[10][256];
        int c;
        splitCSV(line, f, c);
        if (c > col) {
            int id = 0;
            for (int i = 0; f[col][i] != '\0'; i++)
                id = id * 10 + (f[col][i] - '0');
            if (id > maxID) maxID = id;
        }
    }
    fin.close();
    return maxID;
}

// archivePatient
// copies the patient and all their records into discharged.txt
// called before deleting them from the system
void FileHandler::archivePatient(const Patient& p,
                                  Storage<Appointment>& apps,
                                  Storage<Bill>& bills,
                                  Storage<Prescription>& pres) {
    ofstream f("discharged.txt", ios::app);

    // write patient row
    f << p.getID()      << ","
      << p.getName()    << ","
      << p.getAge()     << ","
      << p.getGender()  << ","
      << p.getContact() << ","
      << p.getPassword()<< ","
      << p.getBalance() << "\n";

    // write all their appointments
    for (int i = 0; i < apps.size(); i++) {
        Appointment& a = apps.getAll()[i];
        if (a.getPatientID() == p.getID()) {
            f << a.getID()        << ","
              << a.getPatientID() << ","
              << a.getDoctorID()  << ","
              << a.getDate()      << ","
              << a.getTimeSlot()  << ","
              << a.getStatus()    << "\n";
        }
    }

    // write all their bills
    for (int i = 0; i < bills.size(); i++) {
        Bill& b = bills.getAll()[i];
        if (b.getPatientID() == p.getID()) {
            f << b.getID()             << ","
              << b.getPatientID()      << ","
              << b.getAppointmentID()  << ","
              << b.getAmount()         << ","
              << b.getStatus()         << ","
              << b.getDate()           << "\n";
        }
    }

    // write all their prescriptions
    for (int i = 0; i < pres.size(); i++) {
        Prescription& pr = pres.getAll()[i];
        if (pr.getPatientID() == p.getID()) {
            f << pr.getID()            << ","
              << pr.getAppointmentID() << ","
              << pr.getPatientID()     << ","
              << pr.getDoctorID()      << ","
              << pr.getDate()          << ","
              << pr.getMedicines()     << ","
              << pr.getNotes()         << "\n";
        }
    }
}

// deletePatientRecords
// removes this patient from all 4 Storage objects
// then rewrites all 4 files
void FileHandler::deletePatientRecords(int patientID,
                                        Storage<Patient>& patients,
                                        Storage<Appointment>& apps,
                                        Storage<Bill>& bills,
                                        Storage<Prescription>& pres) {
    // remove from patients storage and save
    patients.removeByID(patientID);
    saveAllPatients(patients);

    // remove all appointments belonging to this patient
    // we loop and remove one at a time because removeByID
    // shifts the array so we restart after each removal
    bool removed = true;
    while (removed) {
        removed = false;
        for (int i = 0; i < apps.size(); i++) {
            if (apps.getAll()[i].getPatientID() == patientID) {
                apps.removeByID(apps.getAll()[i].getID());
                removed = true;
                break;
            }
        }
    }
    saveAllAppointments(apps);

    // remove all bills belonging to this patient
    removed = true;
    while (removed) {
        removed = false;
        for (int i = 0; i < bills.size(); i++) {
            if (bills.getAll()[i].getPatientID() == patientID) {
                bills.removeByID(bills.getAll()[i].getID());
                removed = true;
                break;
            }
        }
    }
    saveAllBills(bills);

    // prescriptions dont have removeByID by patientID
    // so we manually rebuild the storage without this patient
    Prescription temp[100];
    int count = 0;
    for (int i = 0; i < pres.size(); i++) {
        if (pres.getAll()[i].getPatientID() != patientID) {
            temp[count++] = pres.getAll()[i];
        }
    }
    pres.clear();
    for (int i = 0; i < count; i++) pres.add(temp[i]);

    // rewrite prescriptions file
    ofstream f("prescription.txt");
    for (int i = 0; i < pres.size(); i++) {
        Prescription& pr = pres.getAll()[i];
        f << pr.getID()            << ","
          << pr.getAppointmentID() << ","
          << pr.getPatientID()     << ","
          << pr.getDoctorID()      << ","
          << pr.getDate()          << ","
          << pr.getMedicines()     << ","
          << pr.getNotes()         << "\n";
    }
}