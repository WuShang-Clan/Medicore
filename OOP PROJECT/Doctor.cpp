#include "Doctor.h"
#include "Helpers.h"
#include <iostream>

Doctor::Doctor() : Person(), fee(0.0f) {
    specialization[0] = '\0';
}

Doctor::Doctor(int id, const char* name, const char* specialization,
    const char* contact, const char* password, float fee)
    : Person(id, name, contact, password), fee(fee)
{
    copyStr(this->specialization, specialization, 100);
}

const char* Doctor::getRole() const { return "Doctor"; }

void Doctor::displayInfo() const {
    std::cout << "\n=== Doctor Profile ===\n";
    printBaseInfo();
    std::cout << "Specialization : " << specialization << "\n";
    std::cout << "Fee            : PKR " << fee << "\n";
}

const char* Doctor::getSpecialization() const { return specialization; }
float       Doctor::getFee()            const { return fee; }

void Doctor::setFee(float f) { fee = f; }
void Doctor::setSpecialization(const char* s) { copyStr(specialization, s, 100); }

bool Doctor::operator==(const Doctor& other) const { return id == other.id; }
bool Doctor::operator==(int searchId)        const { return id == searchId; }

std::ostream& operator<<(std::ostream& os, const Doctor& d) {
    os << "ID: " << d.id
        << " | Name: " << d.name
        << " | Specialization: " << d.specialization
        << " | Contact: " << d.contact
        << " | Fee: PKR " << d.fee;
    return os;
}