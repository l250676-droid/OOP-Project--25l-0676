#include "Appointment.h"

Appointment::Appointment()
    : appointmentID(0), patientID(0), doctorID(0) {
    date[0] = '\0'; timeSlot[0] = '\0';
    strcpy(status, "pending");
}

Appointment::Appointment(int aid, int pid, int did, const char* d, const char* ts, const char* st)
    : appointmentID(aid), patientID(pid), doctorID(did) {
    strncpy(date,     d,  11); 
    date[11]     = '\0';
    strncpy(timeSlot, ts, 5);  
    timeSlot[5]  = '\0';
    strncpy(status,   st, 11); 
    status[11]   = '\0';
}

int Appointment::getID() const { 
    return appointmentID; 
}
int Appointment::getPatientID() const {
     return patientID; 
}
int Appointment::getDoctorID()  const { 
    return doctorID; 
}
const char* Appointment::getDate() const { 
    return date; 
}
const char* Appointment::getTimeSlot() const {
     return timeSlot; 
}
const char* Appointment::getStatus()const { 
    return status; 
}

void Appointment::setStatus(const char* s) {
    strncpy(status, s, 11);
    status[11] = '\0';
}

// conflict: same doctor + same date + same slot
// and neither appointment is cancelled
bool Appointment::operator==(const Appointment& other) const {
    if (strcmp(status, "cancelled") == 0) return false;
    if (strcmp(other.status, "cancelled") == 0) return false;
    return doctorID == other.doctorID
        && strcmp(date,other.date)     == 0
        && strcmp(timeSlot, other.timeSlot) == 0;
}

ostream& operator<<(ostream& os, const Appointment& a) {
    os << "ID: "  << a.appointmentID
       << " | Patient: " << a.patientID
       << " | Doctor: "  << a.doctorID
       << " | Date: "    << a.date
       << " | Time: "    << a.timeSlot
       << " | Status: "  << a.status;
    return os;
}