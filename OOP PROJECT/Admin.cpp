#include "Admin.h"
#include <iostream>

Admin::Admin() : Person() {}
Admin::Admin(int id, const char* name, const char* password)
    : Person(id, name, "", password)
{
}

const char* Admin::getRole() const { return "Admin"; }

void Admin::displayInfo() const {
    std::cout << "\n=== Admin Profile ===\n";
    printBaseInfo();
}