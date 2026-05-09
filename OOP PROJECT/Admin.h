#pragma once
#include "Person.h"

//  Loaded from admin.txt on startup.
class Admin : public Person {
public:
    Admin();
    Admin(int id, const char* name, const char* password);

    const char* getRole()     const override;
    void        displayInfo() const override;
};