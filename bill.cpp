#include "Bill.h"

Bill::Bill() : billID(0), patientID(0), appointmentID(0), amount(0) {
    strcpy(status, "unpaid");
    date[0] = '\0';
}

Bill::Bill(int bid, int pid, int apid, double amt,
           const char* st, const char* d)
    : billID(bid), patientID(pid), appointmentID(apid), amount(amt) {
    strncpy(status, st, 11); status[11] = '\0';
    strncpy(date,   d,  11); date[11]   = '\0';
}

int         Bill::getID()            const { return billID; }
int         Bill::getPatientID()     const { return patientID; }
int         Bill::getAppointmentID() const { return appointmentID; }
double      Bill::getAmount()        const { return amount; }
const char* Bill::getStatus()        const { return status; }
const char* Bill::getDate()          const { return date; }

void Bill::setStatus(const char* s) {
    strncpy(status, s, 11);
    status[11] = '\0';
}