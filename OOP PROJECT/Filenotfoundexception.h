#pragma once
#include "HospitalException.h"

class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException();
    FileNotFoundException(const char* filename);
};