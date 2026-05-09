#include "Bill.h"
#include "Helpers.h"
#include <iostream>


Bill::Bill() : billId(0), patientId(0), appointmentId(0), amount(0.0f) {
    status[0] = date[0] = '\0';
}

Bill::Bill(int billId, int patientId, int appointmentId,
    float amount, const char* status, const char* date)
    : billId(billId), patientId(patientId),
    appointmentId(appointmentId), amount(amount)
{
    copyStr(this->status, status, 20);
    copyStr(this->date, date, 15);
}

int         Bill::getId()            const { return billId; }
int         Bill::getPatientId()     const { return patientId; }
int         Bill::getAppointmentId() const { return appointmentId; }
float       Bill::getAmount()        const { return amount; }
const char* Bill::getStatus()        const { return status; }
const char* Bill::getDate()          const { return date; }

void Bill::setStatus(const char* s) { copyStr(status, s, 20); }
void Bill::setAmount(float a) { amount = a; }

bool Bill::isUnpaid()    const { return strEquals(status, "unpaid"); }
bool Bill::isPaid()      const { return strEquals(status, "paid"); }
bool Bill::isCancelled() const { return strEquals(status, "cancelled"); }

std::ostream& operator<<(std::ostream& os, const Bill& b) {
    os << "Bill ID: " << b.billId
        << " | Appt ID: " << b.appointmentId
        << " | Amount: PKR " << b.amount
        << " | Status: " << b.status
        << " | Date: " << b.date;
    return os;
}