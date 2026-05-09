#pragma once
#include "Person.h"
#include <iostream>


class Patient : public Person {
private:
    int   age;
    char  gender[10];
    float balance;

public:
    Patient();
    Patient(int id, const char* name, int age, const char* gender,
        const char* contact, const char* password, float balance);

    const char* getRole()     const override;
    void        displayInfo() const override;

    int         getAge()     const;
    const char* getGender()  const;
    float       getBalance() const;

    void setBalance(float b);
    void setAge(int a);

    Patient& operator+=(float amount);
    Patient& operator-=(float amount);
    bool     operator==(const Patient& other) const;
    bool     operator==(int searchId)         const;

    friend std::ostream& operator<<(std::ostream& os, const Patient& p);
};