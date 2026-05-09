#include "Helpers.h"
#include "Validator.h"
#include <ctime>

bool Validator::validateId(int id) {
    return id > 0;
}

// Validate DD-MM-YYYY strictly
bool Validator::validateDate(const char* date) {
    if (!date) return false;

    // Must be exactly 10 chars: DD-MM-YYYY
    if (strLength(date) != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;

    // Extract day, month, year manually
    char dayBuf[3] = { date[0], date[1], '\0' };
    char monBuf[3] = { date[3], date[4], '\0' };
    char yearBuf[5] = { date[6], date[7], date[8], date[9], '\0' };

    if (!isNumeric(dayBuf) || !isNumeric(monBuf) || !isNumeric(yearBuf))
        return false;

    int day = strToInt(dayBuf);
    int mon = strToInt(monBuf);
    int year = strToInt(yearBuf);

    if (day < 1 || day > 31)  return false;
    if (mon < 1 || mon > 12)  return false;

    // Year must be current year or later
    time_t now = time(nullptr);
    tm tmNow = {}; safeLocaltime(&now, &tmNow); tm* t = &tmNow;
    int currentYear = t->tm_year + 1900;

    if (year < currentYear) return false;

    return true;
}

// Validate one of the 8 fixed daily slots
bool Validator::validateTimeSlot(const char* slot) {
    if (!slot) return false;
    const char* validSlots[] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"
    };
    for (int i = 0; i < 8; i++)
        if (strEquals(slot, validSlots[i])) return true;
    return false;
}

// Contact must be exactly 11 digits, all numeric
bool Validator::validateContact(const char* contact) {
    if (!contact) return false;
    if (strLength(contact) != 11) return false;
    return isNumeric(contact);
}

// Password minimum 6 characters
bool Validator::validatePassword(const char* password) {
    if (!password) return false;
    return strLength(password) >= 6;
}

// Positive float > 0
bool Validator::validatePositiveFloat(float value) {
    return value > 0.0f;
}

// Menu choice within [min, max]
bool Validator::validateMenuChoice(int choice, int min, int max) {
    return choice >= min && choice <= max;
}

// Name not empty and within maxLen
bool Validator::validateName(const char* name, int maxLen) {
    if (!name || name[0] == '\0') return false;
    return strLength(name) <= maxLen;
}