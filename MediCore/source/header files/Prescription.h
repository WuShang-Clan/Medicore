#pragma once
#include <iostream>

//  Format: prescription_id,appointment_id,patient_id,
//          doctor_id,date,medicines,notes
class Prescription {
private:
    int  prescriptionId;
    int  appointmentId;
    int  patientId;
    int  doctorId;
    char date[15];        
    char medicines[500];  
    char notes[300];

public:
    Prescription();
    Prescription(int prescriptionId, int appointmentId, int patientId,
        int doctorId, const char* date,
        const char* medicines, const char* notes);

    int         getId()            const;
    int         getAppointmentId() const;
    int         getPatientId()     const;
    int         getDoctorId()      const;
    const char* getDate()          const;
    const char* getMedicines()     const;
    const char* getNotes()         const;

    void setMedicines(const char* m);
    void setNotes(const char* n);

    friend std::ostream& operator<<(std::ostream& os, const Prescription& p);
};