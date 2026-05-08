#ifndef VALIDATOR_H
#define VALIDATOR_H

class Validator {
public:
    static bool isPositiveFloat(const char* s);
    static bool isValidDate(const char* s);
    static bool isValidTimeSlot(const char* s);
    static bool isValidContact(const char* s);
    static bool isValidPassword(const char* s);
    static bool isPositiveInt(const char* s);
    static bool isValidMenuChoice(int choice, int min, int max);
    static void toLowerManual(char* s);
    static bool strEqualCI(const char* a, const char* b);
    static void getCurrentDate(char* buf);
    static bool dateNotPast(const char* date);
    static int compareDates(const char* a, const char* b);
    static double dateToSeconds(const char* date);
};

#endif