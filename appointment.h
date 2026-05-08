#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>
#include <cstring>
using namespace std;

class Appointment {
    int  appointmentID;
    int  patientID;
    int  doctorID;
    char date[12];
    char timeSlot[6];
    char status[12]; // pending, completed, cancelled, no-show
public:
    Appointment();
    Appointment(int aid, int pid, int did,
                const char* d, const char* ts, const char* st);

    int         getID()       const;
    int         getPatientID()const;
    int         getDoctorID() const;
    const char* getDate()     const;
    const char* getTimeSlot() const;
    const char* getStatus()   const;
    void        setStatus(const char* s);

    // checks scheduling conflict
    bool operator==(const Appointment& other) const;
    friend ostream& operator<<(ostream& os, const Appointment& a);
};

#endif