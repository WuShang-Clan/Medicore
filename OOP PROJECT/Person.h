#pragma once
#include <iostream>

class Person {
protected:
    int  id;
    char name[100];
    char contact[20];
    char password[50];

public:
    Person();
    Person(int id, const char* name, const char* contact, const char* password);
    virtual ~Person() {}

    virtual const char* getRole()     const = 0;
    virtual void        displayInfo() const = 0;

    int         getId()       const;
    const char* getName()     const;
    const char* getContact()  const;
    const char* getPassword() const;

    void setContact(const char* c);
    void setPassword(const char* p);

    void printBaseInfo() const;
};