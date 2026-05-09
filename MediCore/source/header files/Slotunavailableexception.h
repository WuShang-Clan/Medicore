#pragma once
#include "HospitalException.h"

class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException();
    SlotUnavailableException(const char* slot, const char* date);
};