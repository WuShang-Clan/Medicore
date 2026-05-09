#include "HospitalException.h"
#include "Helpers.h"

HospitalException::HospitalException() {
    copyStr(message, "An unknown hospital error occurred.", 200);
}

HospitalException::HospitalException(const char* msg) {
    copyStr(message, msg, 200);
}

const char* HospitalException::what() const {
    return message;
}