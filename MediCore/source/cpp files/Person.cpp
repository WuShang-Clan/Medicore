#include "Person.h"
#include "Helpers.h"
#include <iostream>

Person::Person() : id(0) {
    name[0] = contact[0] = password[0] = '\0';
}

Person::Person(int id, const char* name, const char* contact, const char* password)
    : id(id)
{
    copyStr(this->name, name, 100);
    copyStr(this->contact, contact, 20);
    copyStr(this->password, password, 50);
}

int         Person::getId()       const { return id; }
const char* Person::getName()     const { return name; }
const char* Person::getContact()  const { return contact; }
const char* Person::getPassword() const { return password; }

void Person::setContact(const char* c) { copyStr(contact, c, 20); }
void Person::setPassword(const char* p) { copyStr(password, p, 50); }

void Person::printBaseInfo() const {
    std::cout << "ID      : " << id << "\n";
    std::cout << "Name    : " << name << "\n";
    std::cout << "Role    : " << getRole() << "\n";
    std::cout << "Contact : " << contact << "\n";
}