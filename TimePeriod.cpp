#include "TimePeriod.h"
#include "Rtc.h"
#include <MutilaDebug.h>

bool strToTime(Ds1302::DateTime& dest, String src)
{
    int d, h, m;
    if (src.length() != 6) {
        return false;
    }
    d = src.substring(0, 2).toInt();
    h = src.substring(2, 4).toInt();
    m = src.substring(4, 6).toInt();
    if (d < 1 || d > 30 || h < 0 || h > 23 || m < 0 || m > 59) {
        return false;
    }
    dest.year   = 22;
    dest.month  = 6;
    dest.day    = d;
    dest.hour   = h;
    dest.minute = m;
    dest.second = 0;
    return true;
}

static String intToDecStrPad(int n)
{
    String result = String(n);
    while (true) {
        if (result.length() >= 2) {
            break;
        }
        result = '0' + result;
    }
    return result;
}

String timeToStr(Ds1302::DateTime& src, bool human)
{
    String result;

    result += intToDecStrPad(src.day);
    if (human)
        result += '/';

    result += intToDecStrPad(src.hour);
    if (human)
        result += ':';

    result += intToDecStrPad(src.minute);
    if (human) {
        result += ':';
        result += intToDecStrPad(src.second);
    }

    return result;
}

TimePeriod::TimePeriod()
    : fromTime(6, "000000")
    , toTime(6, "000000")
{
}

TimePeriod::TimePeriod(String from, String to)
    : fromTime(6, "000000")
    , toTime(6, "000000")
{
    this->set(from, to);
}

bool TimePeriod::valid()
{
    Ds1302::DateTime dt;
    return strToTime(dt, fromTime.get()) && strToTime(dt, toTime.get());
}

bool TimePeriod::activeNow()
{
    Ds1302::DateTime dt;
    Rtc.getDateTime(&dt);
    String now = timeToStr(dt);
    if (now < fromTime.get()) {
        return false;
    }
    if (now > toTime.get()) {
        return false;
    }
    return true;
}

String TimePeriod::toString()
{
    Ds1302::DateTime dt;
    String           s;
    String           ret;
    if (!strToTime(dt, fromTime.get())) {
        return "-";
    }
    ret += timeToStr(dt, true);
    if (!strToTime(dt, toTime.get())) {
        return "-";
    }
    ret += " - ";
    ret += timeToStr(dt, true);
    return ret;
}

void TimePeriod::clear()
{
    fromTime = "000000";
    toTime = "000000";
    fromTime.save();
    toTime.save();
}

void TimePeriod::dump(const char* msg)
{
    DB(msg);
    DB(F("from="));
    DBLN(fromTime.get());
    fromTime.dump();
    DB(F("to="));
    DBLN(toTime.get());
    toTime.dump();
}

bool TimePeriod::set(String from, String to)
{
    bool ok = true;
    Ds1302::DateTime dt;
    if(!strToTime(dt, from)) {
        from = "000000";
        ok = false;
    }
    if(!strToTime(dt, to)) {
        to = "000000";
        ok = false;
    }
    if (ok) {
        if (from > to) {
            String tmp = from;
            from       = to;
            to         = tmp;
        }
    } else {
        from = "000000";
        to = "000000";
    }
    fromTime.set(from, true);
    toTime.set(to, true);
    return ok;
}
