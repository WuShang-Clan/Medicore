#pragma once
#include <iostream>

class Bill {
private:
    int   billId;
    int   patientId;
    int   appointmentId;
    float amount;
    char  status[20];
    char  date[15];   

public:
    Bill();
    Bill(int billId, int patientId, int appointmentId,
        float amount, const char* status, const char* date);

    // Getters
    int         getId()            const;
    int         getPatientId()     const;
    int         getAppointmentId() const;
    float       getAmount()        const;
    const char* getStatus()        const;
    const char* getDate()          const;

    // Setters
    void setStatus(const char* s);
    void setAmount(float a);

    // Status helpers
    bool isUnpaid()    const;
    bool isPaid()      const;
    bool isCancelled() const;

    friend std::ostream& operator<<(std::ostream& os, const Bill& b);
};