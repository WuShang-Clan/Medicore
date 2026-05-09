#include "Patient.h"
#include "Helpers.h"
#include <iostream>


Patient::Patient() : Person(), age(0), balance(0.0f) {
    gender[0] = '\0';
}

Patient::Patient(int id, const char* name, int age, const char* gender,
    const char* contact, const char* password, float balance)
    : Person(id, name, contact, password), age(age), balance(balance)
{
    copyStr(this->gender, gender, 10);
}

const char* Patient::getRole() const { return "Patient"; }

void Patient::displayInfo() const {
    std::cout << "\n=== Patient Profile ===\n";
    printBaseInfo();
    std::cout << "Age     : " << age << "\n";
    std::cout << "Gender  : " << gender << "\n";
    std::cout << "Balance : PKR " << balance << "\n";
}

int         Patient::getAge()     const { return age; }
const char* Patient::getGender()  const { return gender; }
float       Patient::getBalance() const { return balance; }

void Patient::setBalance(float b) { balance = b; }
void Patient::setAge(int a) { age = a; }

Patient& Patient::operator+=(float amount) { balance += amount; return *this; }
Patient& Patient::operator-=(float amount) { balance -= amount; return *this; }

bool Patient::operator==(const Patient& other) const { return id == other.id; }
bool Patient::operator==(int searchId)         const { return id == searchId; }

std::ostream& operator<<(std::ostream& os, const Patient& p) {
    os << "ID: " << p.id
        << " | Name: " << p.name
        << " | Age: " << p.age
        << " | Gender: " << p.gender
        << " | Contact: " << p.contact
        << " | Balance: PKR " << p.balance;
    return os;
}