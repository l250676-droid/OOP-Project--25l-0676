#include "Prescription.h"

Prescription::Prescription()
    : prescriptionID(0), appointmentID(0), patientID(0), doctorID(0) {
    date[0] = '\0'; medicines[0] = '\0'; notes[0] = '\0';
}

Prescription::Prescription(int pid, int apid, int patid, int did,
                            const char* d, const char* m, const char* n)
    : prescriptionID(pid), appointmentID(apid),
      patientID(patid), doctorID(did) {
    strncpy(date,      d, 11);  date[11]       = '\0';
    strncpy(medicines, m, 499); medicines[499] = '\0';
    strncpy(notes,     n, 299); notes[299]     = '\0';
}

int         Prescription::getID()            const { return prescriptionID; }
int         Prescription::getAppointmentID() const { return appointmentID; }
int         Prescription::getPatientID()     const { return patientID; }
int         Prescription::getDoctorID()      const { return doctorID; }
const char* Prescription::getDate()          const { return date; }
const char* Prescription::getMedicines()     const { return medicines; }
const char* Prescription::getNotes()         const { return notes; }