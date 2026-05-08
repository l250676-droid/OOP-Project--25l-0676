#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include <cstring>

class Prescription {
    int  prescriptionID;
    int  appointmentID;
    int  patientID;
    int  doctorID;
    char date[12];
    char medicines[500];
    char notes[300];
public:
    Prescription();
    Prescription(int pid, int apid, int patid, int did,
                 const char* d, const char* m, const char* n);

    int         getID()            const;
    int         getAppointmentID() const;
    int         getPatientID()     const;
    int         getDoctorID()      const;
    const char* getDate()          const;
    const char* getMedicines()     const;
    const char* getNotes()         const;
};

#endif