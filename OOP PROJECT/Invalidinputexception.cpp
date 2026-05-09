#include "InvalidInputException.h"

InvalidInputException::InvalidInputException()
    : HospitalException("Invalid input provided.")
{
}

InvalidInputException::InvalidInputException(const char* msg)
    : HospitalException(msg)
{
}