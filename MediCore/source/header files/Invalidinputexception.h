#pragma once
#include "HospitalException.h"


//  InvalidInputException — Thrown when user input fails validation.
class InvalidInputException : public HospitalException {
public:
    InvalidInputException();
    InvalidInputException(const char* msg);
};