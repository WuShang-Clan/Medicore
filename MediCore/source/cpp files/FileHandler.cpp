#include "FileHandler.h"
#include "Helpers.h"
#include <ctime>
#include <fstream>
#include <iostream>

//  PATIENTS
//  Format: patient_id,name,age,gender,contact,password,balance
bool FileHandler::loadPatients(Storage<Patient>& store, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[FileHandler] Cannot open: " << filename << "\n";
        return false;
    }

    char line[300];
    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue;

        int   pid = 0, age = 0;
        char  name[100] = {}, gender[10] = {}, contact[20] = {}, password[50] = {};
        float balance = 0.0f;
        char  token[200] = {};
        int   pos = 0, col = 0;

        while (line[pos] != '\0') {
            nextToken(line, ',', pos, token, 200);
            switch (col++) {
            case 0: pid = strToInt(token);      break;
            case 1: copyStr(name, token, 100);  break;
            case 2: age = strToInt(token);      break;
            case 3: copyStr(gender, token, 10);   break;
            case 4: copyStr(contact, token, 20);   break;
            case 5: copyStr(password, token, 50);   break;
            case 6: balance = strToFloat(token);    break;
            }
        }
        store.add(Patient(pid, name, age, gender, contact, password, balance));
    }
    file.close();
    return true;
}

bool FileHandler::appendPatient(const Patient& p, const char* filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;
    file << p.getId() << "," << p.getName() << ","
        << p.getAge() << "," << p.getGender() << ","
        << p.getContact() << "," << p.getPassword() << ","
        << p.getBalance() << "\n";
    file.close();
    return true;
}

bool FileHandler::rewritePatients(Storage<Patient>& store, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (int i = 0; i < store.size(); i++) {
        Patient& p = store[i];
        file << p.getId() << "," << p.getName() << ","
            << p.getAge() << "," << p.getGender() << ","
            << p.getContact() << "," << p.getPassword() << ","
            << p.getBalance() << "\n";
    }
    file.close();
    return true;
}

//  DOCTORS
//  Format: doctor_id,name,specialization,contact,password,fee
bool FileHandler::loadDoctors(Storage<Doctor>& store, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[FileHandler] Cannot open: " << filename << "\n";
        return false;
    }

    char line[300];
    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue;

        int   did = 0;
        char  name[100] = {}, spec[100] = {}, contact[20] = {}, password[50] = {};
        float fee = 0.0f;
        char  token[200] = {};
        int   pos = 0, col = 0;

        while (line[pos] != '\0') {
            nextToken(line, ',', pos, token, 200);
            switch (col++) {
            case 0: did = strToInt(token);          break;
            case 1: copyStr(name, token, 100);  break;
            case 2: copyStr(spec, token, 100);  break;
            case 3: copyStr(contact, token, 20);   break;
            case 4: copyStr(password, token, 50);   break;
            case 5: fee = strToFloat(token);        break;
            }
        }
        store.add(Doctor(did, name, spec, contact, password, fee));
    }
    file.close();
    return true;
}

bool FileHandler::appendDoctor(const Doctor& d, const char* filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;
    file << d.getId() << "," << d.getName() << ","
        << d.getSpecialization() << "," << d.getContact() << ","
        << d.getPassword() << "," << d.getFee() << "\n";
    file.close();
    return true;
}

bool FileHandler::rewriteDoctors(Storage<Doctor>& store, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (int i = 0; i < store.size(); i++) {
        Doctor& d = store[i];
        file << d.getId() << "," << d.getName() << ","
            << d.getSpecialization() << "," << d.getContact() << ","
            << d.getPassword() << "," << d.getFee() << "\n";
    }
    file.close();
    return true;
}

//  BILLS
//  Format: bill_id,patient_id,appointment_id,amount,status,date
bool FileHandler::loadBills(Storage<Bill>& store, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[FileHandler] Cannot open: " << filename << "\n";
        return false;
    }

    char line[300];
    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue;

        int   bid = 0, pid = 0, aid = 0;
        float amount = 0.0f;
        char  status[20] = {}, date[15] = {};
        char  token[200] = {};
        int   pos = 0, col = 0;

        while (line[pos] != '\0') {
            nextToken(line, ',', pos, token, 200);
            switch (col++) {
            case 0: bid = strToInt(token);       break;
            case 1: pid = strToInt(token);       break;
            case 2: aid = strToInt(token);       break;
            case 3: amount = strToFloat(token);     break;
            case 4: copyStr(status, token, 20);     break;
            case 5: copyStr(date, token, 15);     break;
            }
        }
        store.add(Bill(bid, pid, aid, amount, status, date));
    }
    file.close();
    return true;
}

bool FileHandler::appendBill(const Bill& b, const char* filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;
    file << b.getId() << "," << b.getPatientId() << ","
        << b.getAppointmentId() << "," << b.getAmount() << ","
        << b.getStatus() << "," << b.getDate() << "\n";
    file.close();
    return true;
}

bool FileHandler::rewriteBills(Storage<Bill>& store, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (int i = 0; i < store.size(); i++) {
        Bill& b = store[i];
        file << b.getId() << "," << b.getPatientId() << ","
            << b.getAppointmentId() << "," << b.getAmount() << ","
            << b.getStatus() << "," << b.getDate() << "\n";
    }
    file.close();
    return true;
}

//  ADMIN
//  Format: admin_id,name,password
bool FileHandler::loadAdmin(Admin& admin, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[FileHandler] Cannot open: " << filename << "\n";
        return false;
    }
    char line[200];
    if (file.getline(line, 200)) {
        int  aid = 0;
        char name[100] = {}, password[50] = {};
        char token[200] = {}; int pos = 0, col = 0;
        while (line[pos] != '\0') {
            nextToken(line, ',', pos, token, 200);
            switch (col++) {
            case 0: aid = strToInt(token);          break;
            case 1: copyStr(name, token, 100);  break;
            case 2: copyStr(password, token, 50);   break;
            }
        }
        admin = Admin(aid, name, password);
    }
    file.close();
    return true;
}

//  APPOINTMENTS
//  Format: appointment_id,patient_id,doctor_id,date,time_slot,status
bool FileHandler::loadAppointments(Storage<Appointment>& store, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[FileHandler] Cannot open: " << filename << "\n";
        return false;
    }
    char line[300];
    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue;
        int  aid = 0, pid = 0, did = 0;
        char date[15] = {}, slot[10] = {}, status[20] = {};
        char token[200] = {}; int pos = 0, col = 0;
        while (line[pos] != '\0') {
            nextToken(line, ',', pos, token, 200);
            switch (col++) {
            case 0: aid = strToInt(token);          break;
            case 1: pid = strToInt(token);          break;
            case 2: did = strToInt(token);          break;
            case 3: copyStr(date, token, 15);     break;
            case 4: copyStr(slot, token, 10);     break;
            case 5: copyStr(status, token, 20);     break;
            }
        }
        store.add(Appointment(aid, pid, did, date, slot, status));
    }
    file.close();
    return true;
}

bool FileHandler::appendAppointment(const Appointment& a, const char* filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;
    file << a.getId() << "," << a.getPatientId() << ","
        << a.getDoctorId() << "," << a.getDate() << ","
        << a.getTimeSlot() << "," << a.getStatus() << "\n";
    file.close();
    return true;
}

bool FileHandler::rewriteAppointments(Storage<Appointment>& store, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (int i = 0; i < store.size(); i++) {
        Appointment& a = store[i];
        file << a.getId() << "," << a.getPatientId() << ","
            << a.getDoctorId() << "," << a.getDate() << ","
            << a.getTimeSlot() << "," << a.getStatus() << "\n";
    }
    file.close();
    return true;
}

//  PRESCRIPTIONS
//  Format: prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes
bool FileHandler::loadPrescriptions(Storage<Prescription>& store, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[FileHandler] Cannot open: " << filename << "\n";
        return false;
    }
    char line[1200];
    while (file.getline(line, 1200)) {
        if (line[0] == '\0') continue;
        int  pid = 0, aid = 0, patid = 0, did = 0;
        char date[15] = {}, medicines[500] = {}, notes[300] = {};
        char token[600] = {}; int pos = 0, col = 0;
        while (line[pos] != '\0') {
            nextToken(line, ',', pos, token, 600);
            switch (col++) {
            case 0: pid = strToInt(token);            break;
            case 1: aid = strToInt(token);            break;
            case 2: patid = strToInt(token);            break;
            case 3: did = strToInt(token);            break;
            case 4: copyStr(date, token, 15);      break;
            case 5: copyStr(medicines, token, 500);     break;
            case 6: copyStr(notes, token, 300);     break;
            }
        }
        store.add(Prescription(pid, aid, patid, did, date, medicines, notes));
    }
    file.close();
    return true;
}
bool FileHandler::appendPrescription(const Prescription& p, const char* filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;
    file << p.getId() << "," << p.getAppointmentId() << ","
        << p.getPatientId() << "," << p.getDoctorId() << ","
        << p.getDate() << "," << p.getMedicines() << ","
        << p.getNotes() << "\n";
    file.close();
    return true;
}
bool FileHandler::rewritePrescriptions(Storage<Prescription>& store, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    for (int i = 0; i < store.size(); i++) {
        Prescription& p = store[i];
        file << p.getId() << "," << p.getAppointmentId() << ","
            << p.getPatientId() << "," << p.getDoctorId() << ","
            << p.getDate() << "," << p.getMedicines() << ","
            << p.getNotes() << "\n";
    }
    file.close();
    return true;
}

//  SECURITY LOG
//  Format: timestamp,role,entered_id,result
void FileHandler::logSecurityEvent(const char* role, const char* enteredId,
    const char* result, const char* filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return;

    //TAking time using ctime
    time_t now = time(nullptr);
    char timeBuf[30];
    tm tmNow = {}; safeLocaltime(&now, &tmNow); strftime(timeBuf, 30, "%d-%m-%Y %H:%M:%S", &tmNow);

    file << timeBuf << "," << role << "," << enteredId << "," << result << "\n";
    file.close();
}

void FileHandler::printSecurityLog(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "No security events logged.\n";
        return;
    }
    char line[300];
    bool empty = true;
    while (file.getline(line, 300)) {
        if (line[0] == '\0') continue;
        std::cout << line << "\n";
        empty = false;
    }
    if (empty) std::cout << "No security events logged.\n";
    file.close();
}