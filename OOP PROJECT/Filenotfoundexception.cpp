#include "FileNotFoundException.h"
#include "Helpers.h"


FileNotFoundException::FileNotFoundException()
    : HospitalException("Required file could not be opened.")
{
}

FileNotFoundException::FileNotFoundException(const char* filename) {
    // Build message: "Cannot open file: <filename>"
    copyStr(message, "Cannot open file: ", 200);
    int len = strLength(message);
    copyStr(message + len, filename, 200 - len);
}