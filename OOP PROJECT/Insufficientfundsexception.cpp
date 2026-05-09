#include "InsufficientFundsException.h"
#include "Helpers.h"

InsufficientFundsException::InsufficientFundsException()
    : HospitalException("Insufficient funds in account.")
{
}

InsufficientFundsException::InsufficientFundsException(float balance, float required) {
    char balBuf[20], reqBuf[20];

    intToStr((int)balance, balBuf, 20);
    intToStr((int)required, reqBuf, 20);

    copyStr(message, "Insufficient funds. Balance: PKR ", 200);
    int len = strLength(message);
    copyStr(message + len, balBuf, 200 - len);
    len = strLength(message);
    copyStr(message + len, " | Required: PKR ", 200 - len);
    len = strLength(message);
    copyStr(message + len, reqBuf, 200 - len);
}