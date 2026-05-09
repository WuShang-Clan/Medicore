#include "Appointment.h"
#include "Helpers.h"
#include <iostream>


Appointment::Appointment()
    : appointmentId(0), patientId(0), doctorId(0)
{
    date[0] = timeSlot[0] = status[0] = '\0';
}

Appointment::Appointment(int appointmentId, int patientId, int doctorId,
    const char* date, const char* timeSlot, const char* status)
    : appointmentId(appointmentId), patientId(patientId), doctorId(doctorId)
{
    copyStr(this->date, date, 15);
    copyStr(this->timeSlot, timeSlot, 10);
    copyStr(this->status, status, 20);
}

// Getters
int         Appointment::getId()        const { return appointmentId; }
int         Appointment::getPatientId() const { return patientId; }
int         Appointment::getDoctorId()  const { return doctorId; }
const char* Appointment::getDate()      const { return date; }
const char* Appointment::getTimeSlot()  const { return timeSlot; }
const char* Appointment::getStatus()    const { return status; }

// Setters
void Appointment::setStatus(const char* s) { copyStr(status, s, 20); }

// Status helpers
bool Appointment::isPending()   const { return strEquals(status, "pending"); }
bool Appointment::isCompleted() const { return strEquals(status, "completed"); }
bool Appointment::isCancelled() const { return strEquals(status, "cancelled"); }
bool Appointment::isNoShow()    const { return strEquals(status, "no-show"); }

// == : Two appointments conflict if they have the same doctor
bool Appointment::operator==(const Appointment& other) const {
    if (doctorId != other.doctorId)             return false;
    if (!strEquals(date, other.date))       return false;
    if (!strEquals(timeSlot, other.timeSlot))   return false;
    if (isCancelled() || other.isCancelled())   return false;
    return true;
}

std::ostream& operator<<(std::ostream& os, const Appointment& a) {
    os << "Appt ID: " << a.appointmentId
        << " | Patient: " << a.patientId
        << " | Doctor: " << a.doctorId
        << " | Date: " << a.date
        << " | Slot: " << a.timeSlot
        << " | Status: " << a.status;
    return os;
}