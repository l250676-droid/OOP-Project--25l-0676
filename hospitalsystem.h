#ifndef HOSPITALSYSTEM_H
#define HOSPITALSYSTEM_H

#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "storage.h"
#include "filehandler.h"
#include "validator.h"
#include "exceptions.h"

class HospitalSystem {
private:

    // -----------------------------------------------------------
    // all data lives here in memory
    // loaded from files on startup
    // -----------------------------------------------------------
    Storage<Patient>      patients;
    Storage<Doctor>       doctors;
    Storage<Appointment>  appointments;
    Storage<Bill>         bills;
    Storage<Prescription> prescriptions;
    Admin                 admin;

    // whoever is logged in right now
    // nullptr means nobody is logged in
    Patient* currentPatient;
    Doctor*  currentDoctor;

    // the 8 fixed daily time slots
    static const char* TIME_SLOTS[8];

    // -----------------------------------------------------------
    // STARTUP
    // -----------------------------------------------------------
    void loadAllData();

    // -----------------------------------------------------------
    // LOGIN
    // each one tries 3 times before locking
    // returns true if login succeeded
    // -----------------------------------------------------------
    bool loginPatient();
    bool loginDoctor();
    bool loginAdmin();

    // -----------------------------------------------------------
    // PATIENT MENU FUNCTIONS
    // -----------------------------------------------------------
    void patientMenu();
    void bookAppointment();
    void cancelAppointment();
    void viewMyAppointments();
    void viewMyMedicalRecords();
    void viewMyBills();
    void payBill();
    void topUpBalance();

    // -----------------------------------------------------------
    // DOCTOR MENU FUNCTIONS
    // -----------------------------------------------------------
    void doctorMenu();
    void viewTodayAppointments();
    void markComplete();
    void markNoShow();
    void writePrescription();
    void viewPatientHistory();

    // -----------------------------------------------------------
    // ADMIN MENU FUNCTIONS
    // -----------------------------------------------------------
    void adminMenu();
    void addDoctor();
    void removeDoctor();
    void viewAllPatients();
    void viewAllDoctors();
    void viewAllAppointments();
    void viewUnpaidBills();
    void dischargePatient();
    void viewSecurityLog();
    void generateDailyReport();

    // -----------------------------------------------------------
    // HELPER FUNCTIONS
    // used by multiple menu functions
    // -----------------------------------------------------------

    // get doctor name from their ID
    const char* getDoctorName(int doctorID);

    // get patient name from their ID
    const char* getPatientName(int patientID);

    // get doctor fee from their ID
    double getDoctorFee(int doctorID);

    // get doctor specialization from their ID
    const char* getDoctorSpec(int doctorID);

    // check if a time slot is already booked
    // returns true if slot is taken
    bool slotTaken(int doctorID, const char* date, const char* slot);

    // find the bill ID that belongs to an appointment
    // returns -1 if not found
    int getBillForAppointment(int appointmentID);

    // count unpaid bills for a patient
    int countUnpaidBills(int patientID);

    // count pending appointments for a patient
    int countPendingAppointments(int patientID);

public:

    // constructor loads all data from files
    HospitalSystem();

    // main loop — shows main menu and handles everything
    void run();
};

#endif