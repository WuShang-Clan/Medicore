#pragma once
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Bill.h"
#include "Appointment.h"
#include "Prescription.h"

//  FileHandler — The ONLY class permitted to perform file I/O.
class FileHandler {
public:
    // Patients
    static bool loadPatients(Storage<Patient>& store, const char* filename);
    static bool appendPatient(const Patient& p, const char* filename);
    static bool rewritePatients(Storage<Patient>& store, const char* filename);

    // Doctors
    static bool loadDoctors(Storage<Doctor>& store, const char* filename);
    static bool appendDoctor(const Doctor& d, const char* filename);
    static bool rewriteDoctors(Storage<Doctor>& store, const char* filename);

    // Admin
    static bool loadAdmin(Admin& admin, const char* filename);

    // Bills
    static bool loadBills(Storage<Bill>& store, const char* filename);
    static bool appendBill(const Bill& b, const char* filename);
    static bool rewriteBills(Storage<Bill>& store, const char* filename);

    // Appointments
    static bool loadAppointments(Storage<Appointment>& store, const char* filename);
    static bool appendAppointment(const Appointment& a, const char* filename);
    static bool rewriteAppointments(Storage<Appointment>& store, const char* filename);

    // Prescriptions
    static bool loadPrescriptions(Storage<Prescription>& store, const char* filename);
    static bool appendPrescription(const Prescription& p, const char* filename);
    static bool rewritePrescriptions(Storage<Prescription>& store, const char* filename);

    // Security log
    static void logSecurityEvent(const char* role, const char* enteredId, const char* result, const char* filename);
    static void printSecurityLog(const char* filename);
};