#ifndef BILL_H
#define BILL_H

#include <cstring>

class Bill {
    int    billID;
    int    patientID;
    int    appointmentID;
    double amount;
    char   status[12]; // unpaid, paid, cancelled
    char   date[12];
public:
    Bill();
    Bill(int bid, int pid, int apid, double amt,
         const char* st, const char* d);

    int         getID()            const;
    int         getPatientID()     const;
    int         getAppointmentID() const;
    double      getAmount()        const;
    const char* getStatus()        const;
    const char* getDate()          const;
    void        setStatus(const char* s);
};

#endif