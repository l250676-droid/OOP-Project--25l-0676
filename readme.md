# MediCore Hospital Management System

A console-based Hospital Management System built in C++ for a fictional hospital called MediCore.  
Supports three user roles: **Patient**, **Doctor**, and **Admin**.  
All data is persisted in `.txt` files and restored on every startup.

---

## GitHub Repository

> https://github.com/YOUR_USERNAME/medicore-hospital

---

## Features

### Patient
- Book appointments by searching doctor specialization
- Cancel appointments with automatic refund
- View appointments sorted by date
- View medical records and prescriptions
- View and pay bills
- Top up account balance

### Doctor
- View today's appointments sorted by time
- Mark appointments as completed or no-show
- Write prescriptions for completed appointments
- View patient medical history (own patients only)

### Admin
- Add and remove doctors
- View all patients, doctors, appointments
- View unpaid and overdue bills
- Discharge patients (archives all their records)
- View security log of failed logins
- Generate daily report

---

## File Structure

```
medicore/
├── Person.h / Person.cpp            # Abstract base class
├── Patient.h / Patient.cpp          # Patient class
├── Doctor.h / Doctor.cpp            # Doctor class
├── Admin.h / Admin.cpp              # Admin class
├── Appointment.h / Appointment.cpp  # Appointment class
├── Bill.h / Bill.cpp                # Bill class
├── Prescription.h / Prescription.cpp# Prescription class
├── Storage.h                        # Generic template container
├── Exceptions.h                     # Custom exception classes
├── Validator.h / Validator.cpp      # Input validation
├── FileHandler.h / FileHandler.cpp  # All file I/O
├── HospitalSystem.h                 # Main system declaration
├── HospitalSystem.cpp               # Patient + Doctor menus
├── AdminMenu.cpp                    # Admin menu functions
├── main.cpp                         # Entry point
├── admin.txt                        # Admin credentials
├── doctor.txt                       # Doctor records
├── patient.txt                      # Patient records
├── appointments.txt                 # Appointment records
├── bill.txt                         # Bill records
├── prescription.txt                 # Prescription records
├── security_log.txt                 # Failed login attempts
└── discharged.txt                   # Archived discharged patients
```

---

## How to Compile

Make sure you have **g++** installed (MinGW on Windows or GCC on Linux/Mac).

Open a terminal in the project folder and run:

```
g++ main.cpp Patient.cpp Doctor.cpp Admin.cpp Appointment.cpp Bill.cpp Prescription.cpp Validator.cpp FileHandler.cpp HospitalSystem.cpp AdminMenu.cpp -o medicore
```

---

## How to Run

**Windows:**
```
.\medicore.exe
```

**Linux / Mac:**
```
./medicore
```

---

## Sample Login Credentials

| Role    | ID | Password  |
|---------|----|-----------|
| Admin   | 1  | admin123  |
| Patient | 1  | pass123   |
| Doctor  | 1  | doc456    |

---

## Data File Format

**patient.txt**
```
id,name,age,gender,contact,password,balance
1,Ahmed Ali,28,M,03001234567,pass123,5000.00
```

**doctor.txt**
```
id,name,specialization,contact,password,fee
1,Dr. Sara Khan,Cardiology,03111234567,doc456,1500.00
```

**appointments.txt**
```
id,patientID,doctorID,date,timeSlot,status
1,1,1,15-04-2025,09:00,completed
```

**bill.txt**
```
id,patientID,appointmentID,amount,status,date
1,1,1,1500.00,paid,15-04-2025
```

**prescription.txt**
```
id,appointmentID,patientID,doctorID,date,medicines,notes
1,1,1,1,15-04-2025,Paracetamol 500mg,Take after meals
```

---

## Notes

- After 3 failed login attempts the account is locked and the attempt is logged to `security_log.txt`
- Patients cannot be discharged if they have unpaid bills or pending appointments
- Doctors cannot be removed if they have pending appointments
- All sorting is implemented manually without library sort functions
- All string comparisons are done manually without library functions where required