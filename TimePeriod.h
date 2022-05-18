#pragma once

#include <Arduino.h>
#include <PersistentSetting.h>
#include <Ds1302.h>


// Take src in "DDHHMM\0" format and return a DateTime structure with everything 
// filled in (except the DOW). Note: year is assumed to be 2022, and month is June
// Because this is for EMF 2022, which is June 2-5 2022.
bool strToTime(Ds1302::DateTime& dest, String src);

// Convert a DateTime to a String in format "DDHHMM"
String timeToStr(Ds1302::DateTime& src, bool human=false);

// Stores from and to times as "DDHHMM\0".
class TimePeriod {
public:
    TimePeriod();                           // load values from EEPROM
    TimePeriod(String from, String to);     // create with specifid values and save to EEPROM
    bool valid();
    bool activeNow();
    String toString();  // human readable string
    bool set(String from, String to);
    void clear();
    void dump(const char* msg="");

private:
    PersistentString fromTime;
    PersistentString toTime;

};


