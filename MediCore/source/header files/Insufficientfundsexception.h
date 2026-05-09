#pragma once
#include "HospitalException.h"

//  InsufficientFundsException — Thrown when patient balance is less than the required amount.

class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException();
    InsufficientFundsException(float balance, float required);
};