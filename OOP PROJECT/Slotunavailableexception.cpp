#include "Slotunavailableexception.h"
#include "Helpers.h"

SlotUnavailableException::SlotUnavailableException()
    : HospitalException("The selected time slot is unavailable.")
{
}

// Builds: "Slot <slot> on <date> is already booked."
SlotUnavailableException::SlotUnavailableException(const char* slot, const char* date) {
    copyStr(message, "Slot ", 200);
    int len = strLength(message);
    copyStr(message + len, slot, 200 - len);
    len = strLength(message);
    copyStr(message + len, " on ", 200 - len);
    len = strLength(message);
    copyStr(message + len, date, 200 - len);
    len = strLength(message);
    copyStr(message + len, " is already booked.", 200 - len);
}