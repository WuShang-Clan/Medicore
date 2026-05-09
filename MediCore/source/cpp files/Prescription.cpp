#include "Prescription.h"
#include "Helpers.h"
#include <iostream>

Prescription::Prescription()
    : prescriptionId(0), appointmentId(0), patientId(0), doctorId(0)
{
    date[0] = medicines[0] = notes[0] = '\0';
}

Prescription::Prescription(int prescriptionId, int appointmentId, int patientId,
    int doctorId, const char* date,
    const char* medicines, const char* notes)
    : prescriptionId(prescriptionId), appointmentId(appointmentId),
    patientId(patientId), doctorId(doctorId)
{
    copyStr(this->date, date, 15);
    copyStr(this->medicines, medicines, 500);
    copyStr(this->notes, notes, 300);
}

int         Prescription::getId()            const { return prescriptionId; }
int         Prescription::getAppointmentId() const { return appointmentId; }
int         Prescription::getPatientId()     const { return patientId; }
int         Prescription::getDoctorId()      const { return doctorId; }
const char* Prescription::getDate()          const { return date; }
const char* Prescription::getMedicines()     const { return medicines; }
const char* Prescription::getNotes()         const { return notes; }

void Prescription::setMedicines(const char* m) { copyStr(medicines, m, 500); }
void Prescription::setNotes(const char* n) { copyStr(notes, n, 300); }

std::ostream& operator<<(std::ostream& os, const Prescription& p) {
    os << "Prescription ID: " << p.prescriptionId
        << " | Appt ID: " << p.appointmentId
        << " | Date: " << p.date
        << " | Medicines: " << p.medicines
        << " | Notes: " << p.notes;
    return os;
}