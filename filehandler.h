#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "storage.h"

class FileHandler {
public:
    // Load all records from files into Storage on startup
    static void loadPatients(Storage<Patient>& s);
    static void loadDoctors(Storage<Doctor>& s);
    static void loadAppointments(Storage<Appointment>& s);
    static void loadBills(Storage<Bill>& s);
    static void loadPrescriptions(Storage<Prescription>& s);
    static bool loadAdmin(Admin& admin);

    // Add a single new record to end of file
    static void appendPatient(const Patient& p);
    static void appendDoctor(const Doctor& d);
    static void appendAppointment(const Appointment& a);
    static void appendBill(const Bill& b);
    static void appendPrescription(const Prescription& p);

    // Rewrite entire file (used after any update or delete)
    static void saveAllPatients(Storage<Patient>& s);
    static void saveAllDoctors(Storage<Doctor>& s);
    static void saveAllAppointments(Storage<Appointment>& s);
    static void saveAllBills(Storage<Bill>& s);

    // Write a failed login attempt to security_log.txt
    static void logSecurity(const char* role,
                            const char* enteredID,
                            const char* result);

    // Copy patient data to discharged.txt then delete from all file
    static void archivePatient(const Patient& p,
                               Storage<Appointment>& apps,
                               Storage<Bill>& bills,
                               Storage<Prescription>& pres);
    static void deletePatientRecords(int patientID,
                                     Storage<Patient>& patients,
                                     Storage<Appointment>& apps,
                                     Storage<Bill>& bills,
                                     Storage<Prescription>& pres);

    // Reads a file and finds the highest ID in a given column
    // Used to generate new IDs
    
    static int getMaxID(const char* filename, int col);

private:
    // Convert one CSV line into an object
    static Patient      parsePatient(const char* line);
    static Doctor       parseDoctor(const char* line);
    static Appointment  parseAppointment(const char* line);
    static Bill         parseBill(const char* line);
    static Prescription parsePrescription(const char* line);

    // Split "1,Ahmed,28" into fields[0]="1" fields[1]="Ahmed" etc
    static void splitCSV(const char* line,
                         char fields[][256],
                         int& count);
};

#endif