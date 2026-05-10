#include "validator.h"
#include <cstring>
#include <ctime>

// helper: manually convert a string of digits to integer
// example: "28" -> 28
// we multiply result by 10 each step and add next digit
// '0' is ASCII 48, so s[i]-'0' gives the actual digit value
static int toInt(const char* s) {
    int result = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        result = result * 10 + (s[i] - '0');
    }
    return result;
}

// helper: manually convert string like "1500.50" to double
// split at the dot, handle whole part then decimal part
static double toFloat(const char* s) {
    double result = 0;
    int i = 0;
    // whole number part before the dot
    while (s[i] != '\0' && s[i] != '.') {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    // decimal part after the dot
    if (s[i] == '.') {
        i++;
        double place = 0.1;
        while (s[i] != '\0') {
            result = result + (s[i] - '0') * place;
            place = place / 10;
            i++;
        }
    }

    return result;
}

// helper: get length of string without using strlen
static int strLen(const char* s) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

// toLowerManual
// 'A' is ASCII 65, 'a' is ASCII 97, difference is 32
// so adding 32 to any uppercase letter makes it lowercase
void Validator::toLowerManual(char* s) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] + 32;
        }
    }
}
// strEqualCI
// compares two strings ignoring case
// copies them first so originals are not changed
bool Validator::strEqualCI(const char* a, const char* b) {
    char copyA[100], copyB[100];
    strncpy(copyA, a, 99); copyA[99] = '\0';
    strncpy(copyB, b, 99); copyB[99] = '\0';
    toLowerManual(copyA);
    toLowerManual(copyB);
    return strcmp(copyA, copyB) == 0;
}

// isPositiveFloat
// valid:   "1500.00"  "500"  "0.5"
// invalid: "-100"  "abc"  "0"  "1.2.3"
bool Validator::isPositiveFloat(const char* s) {
    if (!s || s[0] == '\0') return false;
    if (s[0] == '-') return false;

    int dots = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '.') {
            dots++;
            if (dots > 1) return false;
        } else if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }

    return toFloat(s) > 0;
}
// isValidDate
// format must be DD-MM-YYYY
// day 01-31, month 01-12, year >= current year
bool Validator::isValidDate(const char* s) {
    if (!s || strLen(s) != 10) return false;
    if (s[2] != '-' || s[5] != '-') return false;

    // every other character must be a digit
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (s[i] < '0' || s[i] > '9') return false;
    }

    // manually extract DD MM YYYY
    char d[3], m[3], y[5];
    d[0]=s[0]; d[1]=s[1]; d[2]='\0';
    m[0]=s[3]; m[1]=s[4]; m[2]='\0';
    y[0]=s[6]; y[1]=s[7]; y[2]=s[8]; y[3]=s[9]; y[4]='\0';

    int day  = toInt(d);
    int mon  = toInt(m);
    int year = toInt(y);

    if (day < 1 || day > 31) return false;
    if (mon < 1 || mon > 12) return false;

    // year must be current year or later
    char today[12];
    getCurrentDate(today);
    // year starts at position 6 in DD-MM-YYYY
    char cy[5];
    cy[0]=today[6]; cy[1]=today[7]; cy[2]=today[8]; cy[3]=today[9]; cy[4]='\0';
    int currentYear = toInt(cy);

    if (year < currentYear) return false;

    return true;
}
// isValidTimeSlot
// only these 8 exact values are allowed
bool Validator::isValidTimeSlot(const char* s) {
    const char* slots[8] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"
    };
    for (int i = 0; i < 8; i++) {
        if (strcmp(s, slots[i]) == 0) return true;
    }
    return false;
}
// isValidContact
// exactly 11 digits, nothing else
bool Validator::isValidContact(const char* s) {
    if (!s || strLen(s) != 11) return false;
    for (int i = 0; i < 11; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}
// isValidPassword
// at least 6 characters
bool Validator::isValidPassword(const char* s) {
    if (!s) return false;
    return strLen(s) >= 6;
}

// isPositiveInt
// digits only and value > 0
bool Validator::isPositiveInt(const char* s) {
    if (!s || s[0] == '\0') return false;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
    }
    return toInt(s) > 0;
}

// isValidMenuChoice
// number must be between min and max inclusive
bool Validator::isValidMenuChoice(int choice, int min, int max) {
    return choice >= min && choice <= max;
}

// getCurrentDate
// ctime() returns a string like:
// "Tue May  5 14:32:10 2026\n"
//  0123456789012345678901234
// positions: day=8-9  month=4-6  year=20-23
// we rearrange into DD-MM-YYYY manually
void Validator::getCurrentDate(char* buf) {
    time_t t = time(nullptr);
    // ctime gives us a fixed format string we can parse
    char* raw = ctime(&t);
    // raw format: "Tue May  5 14:32:10 2026\n"
    // month is at positions 4,5,6
    // day   is at positions 8,9
    // year  is at positions 20,21,22,23

    // convert month name to number manually
    char monName[4];
    monName[0] = raw[4];
    monName[1] = raw[5];
    monName[2] = raw[6];
    monName[3] = '\0';

    int mon = 0;
    const char* months[12] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    for (int i = 0; i < 12; i++) {
        if (strcmp(monName, months[i]) == 0) {
            mon = i + 1;
            break;
        }
    }

    // day — position 8 and 9
    // if day < 10 then position 8 is a space, position 9 is the digit
    int day = 0;
    if (raw[8] == ' ') {
        day = raw[9] - '0';
    } else {
        day = (raw[8] - '0') * 10 + (raw[9] - '0');
    }

    // year — positions 20 to 23
    int year = (raw[20]-'0')*1000 + (raw[21]-'0')*100
             + (raw[22]-'0')*10   + (raw[23]-'0');

    // write into buf as DD-MM-YYYY
    // DD
    buf[0] = '0' + day / 10;
    buf[1] = '0' + day % 10;
    buf[2] = '-';
    // MM
    buf[3] = '0' + mon / 10;
    buf[4] = '0' + mon % 10;
    buf[5] = '-';
    // YYYY
    buf[6]  = '0' + year / 1000;
    buf[7]  = '0' + (year / 100) % 10;
    buf[8]  = '0' + (year / 10)  % 10;
    buf[9]  = '0' + year % 10;
    buf[10] = '\0';
}

// dateNotPast
// true if given date is today or in the future
bool Validator::dateNotPast(const char* date) {
    char today[12];
    getCurrentDate(today);
    return compareDates(date, today) >= 0;
}

// compareDates
// both in DD-MM-YYYY format
// compare year first, then month, then day
// returns negative if a < b, 0 if equal, positive if a > b
int Validator::compareDates(const char* a, const char* b) {
    // extract year (position 6-9)
    char ya[5], yb[5];
    ya[0]=a[6]; ya[1]=a[7]; ya[2]=a[8]; ya[3]=a[9]; ya[4]='\0';
    yb[0]=b[6]; yb[1]=b[7]; yb[2]=b[8]; yb[3]=b[9]; yb[4]='\0';
    int yearA = toInt(ya);
    int yearB = toInt(yb);
    if (yearA != yearB) return yearA - yearB;

    // extract month (position 3-4)
    char ma[3], mb[3];
    ma[0]=a[3]; ma[1]=a[4]; ma[2]='\0';
    mb[0]=b[3]; mb[1]=b[4]; mb[2]='\0';
    int monA = toInt(ma);
    int monB = toInt(mb);
    if (monA != monB) return monA - monB;

    // extract day (position 0-1)
    char da[3], db[3];
    da[0]=a[0]; da[1]=a[1]; da[2]='\0';
    db[0]=b[0]; db[1]=b[1]; db[2]='\0';
    int dayA = toInt(da);
    int dayB = toInt(db);
    return dayA - dayB;
}
// dateToSeconds
// converts date to a plain number (total days)
// used only to check if a bill is overdue by 7 days
// year*365 + month*30 + day is close enough for this purpose
double Validator::dateToSeconds(const char* date) {
    char da[3], ma[3], ya[5];
    da[0]=date[0]; da[1]=date[1]; da[2]='\0';
    ma[0]=date[3]; ma[1]=date[4]; ma[2]='\0';
    ya[0]=date[6]; ya[1]=date[7]; ya[2]=date[8]; ya[3]=date[9]; ya[4]='\0';

    int day  = toInt(da);
    int mon  = toInt(ma);
    int year = toInt(ya);

    return (double)(year * 365 + mon * 30 + day);
}