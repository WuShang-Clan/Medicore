#pragma once
#include "Person.h"
#include <iostream>

class Doctor : public Person {
private:
    char  specialization[100];
    float fee;

public:
    Doctor();
    Doctor(int id, const char* name, const char* specialization,
        const char* contact, const char* password, float fee);

    // Pure virtual implementations
    const char* getRole()     const override;
    void        displayInfo() const override;

    // Getters
    const char* getSpecialization() const;
    float       getFee()            const;

    // Setters
    void setFee(float f);
    void setSpecialization(const char* s);

    // Operator overloads
    bool operator==(const Doctor& other) const;
    bool operator==(int searchId)        const;

    friend std::ostream& operator<<(std::ostream& os, const Doctor& d);
};
