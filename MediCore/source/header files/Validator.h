#pragma once

class Validator {
public:
    static bool validateId(int id);
    static bool validateDate(const char* date);
    static bool validateTimeSlot(const char* slot);
    static bool validateContact(const char* contact);
    static bool validatePassword(const char* password);
    static bool validatePositiveFloat(float value);
    static bool validateMenuChoice(int choice, int min, int max);
    static bool validateName(const char* name, int maxLen);
};