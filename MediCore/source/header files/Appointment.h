#pragma once
#include <iostream>

//  Format: appointment_id,patient_id,doctor_id,date,time_slot,status
//  Status: "pending" | "completed" | "cancelled" | "no-show"
//  Overloads: == (conflict detection), <<


class Appointment {
private:
    int  appointmentId;
    int  patientId;
    int  doctorId;
    char date[15];      
    char timeSlot[10];  
    char status[20];     

public:
    Appointment();
    Appointment(int appointmentId, int patientId, int doctorId,
        const char* date, const char* timeSlot, const char* status);

    // Getters
    int         getId()         const;
    int         getPatientId()  const;
    int         getDoctorId()   const;
    const char* getDate()       const;
    const char* getTimeSlot()   const;
    const char* getStatus()     const;

    // Setters
    void setStatus(const char* s);

    // Status helpers
    bool isPending()   const;
    bool isCompleted() const;
    bool isCancelled() const;
    bool isNoShow()    const;

    bool operator==(const Appointment& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Appointment& a);
};