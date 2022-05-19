#include "Bluetooth.h"
#include "Rtc.h"
#include "TimePeriod.h"
#include "Button.h"
#include "Config.h"
#include "Settings.h"
#include <Arduino.h>
#include <Ds1302.h>
#include <Millis.h>
#include <MutilaDebug.h>
#include <SoftwareSerial.h>

#define IFAUTH(x)             \
    do {                      \
        if (isAuthorized()) { \
            response = x;     \
            setAuth();        \
        }                     \
    } while (0)

// Glboals
String         cmd;
millis_t       lastAuthMs     = 0;
TimePeriod     onTime[OnTimes];
bool           on = false;
millis_t       lastOnCheckMs = 0;

String cmdStat();

void onCheck()
{
    for (uint8_t i=0; i<OnTimes; ++i) {
        if (onTime[i].activeNow()) {
            on = true;
            return;
        }
    }
    on = false;
}

void txString(String s)
{
    DB("TX: \"");
    DB(s);
    DBLN('"');
    Bluetooth.println(s);
}

void setup()
{
    Serial.begin(115200);
    DBLN(F("\n\nsetup() start"));

    Rtc.init();
    Bluetooth.begin(9600); // Baud Rate for AT-command Mode.
    Button.begin();
    pinMode(PinRelay, OUTPUT);
    digitalWrite(PinRelay, LOW);

    for(uint8_t i=0; i<OnTimes; ++i) {
        onTime[i].dump();
    }

    checkSettings(); 
    DBLN(F("setup() end"));
    txString(F("KlakSAN up"));

}

void sound(millis_t ms)
{
    if (!ArmedSetting.get()) {
        return;
    }
    String msg("BEP[");
    msg += ms;
    msg += "]!";
    digitalWrite(PinRelay, HIGH);
    txString(msg);
    delay(ms);
    digitalWrite(PinRelay, LOW);
    BeepHitCounter.set(BeepHitCounter.get() + 1, true);
}

String nowStr()
{
    if (Rtc.isHalted()) {
        return "000000";
    }
    Ds1302::DateTime now;
    Rtc.getDateTime(&now);
    return timeToStr(now, true);
}

bool isAuthorized()
{
    return lastAuthMs > 0 && MillisSince(lastAuthMs) < AuthDurationMs;
}

void setAuth(bool on = true)
{
    lastAuthMs = on ? Millis() : 0;
}

String removeLeading(String s, uint8_t len)
{
    if (len >= s.length()) {
        return "";
    }
    return s.substring(len, s.length());
}

String cmdAuth(String authCode)
{
    DBLN(__func__);
    if (authCode == AUTH_CODE) {
        setAuth();
        return "k";
    }
    return "nuh";
}

// expect tm e.g. "DDHHMM"
String cmdStime(String tm)
{
    DBLN(__func__);
    Ds1302::DateTime dt;
    if (!strToTime(dt, tm)) {
        return "?t";
    }
    DB("Y=");
    DB(dt.year);
    DB(" M=");
    DB(dt.month);
    DB(" D=");
    DB(dt.day);
    DB(" h=");
    DB(dt.hour);
    DB(" m=");
    DBLN(dt.minute);
    Rtc.setDateTime(&dt);
    return "k";
}

String cmdList()
{
    String s;
    for (uint8_t i=0; i<OnTimes; i++) {
        s += i;
        s += " ";
        s += onTime[i].toString();
        if (onTime[i].activeNow()) {
            s += " *";
        }
        if (i < OnTimes-1) {
            s += "\n";
        }
    }
    return s;
}

// expect n=ddhhmm-ddhhmm
// n can be two digits.
String cmdSet(String times)
{
    uint8_t slot, t1_off, t2_off;
    if (times.length() == 15) {
        slot = times.substring(0, 1).toInt();
        t1_off = 2;
        t2_off = 9;
    } else if (times.length() == 16) {
        slot = times.substring(0, 2).toInt();
        t1_off = 3;
        t2_off = 10;
    } else {
        return "?l";
    }

    if (slot > OnTimes) {
        return "?#";
    }

    if (!onTime[slot].set(times.substring(t1_off,t1_off+6), times.substring(t2_off,t2_off+6))) {
        return "?v";
    }
    return "k";
}

void clrSlot(int n)
{
    if (n < OnTimes) {
        onTime[n].clear();
    }
}

String cmdClr(String slot)
{
    if (slot.length() == 0) {
        return "?s";
    }

    if (slot == "*") {
        for(uint8_t i=0; i<OnTimes; ++i) {
            clrSlot(i);
        }
    } else {
        int n = slot.toInt();
        if (n < OnTimes) {
            clrSlot(slot.toInt());
        } else {
            return "?r";
        }
    }
    return "k" + slot;
}

String cmdDisarm()
{
    ArmedSetting.set(false, true); // set value to false and saveIt=true
    return "k";
}

String cmdArm()
{
    ArmedSetting.set(true, true); // set value to true and saveIt=true
    return "k";
}

String cmdBeep(String period)
{
    millis_t ms = period.toInt();
    if (ms > 1000) {
        return "?l";
    }
    sound(ms);
    return "k";
}

String cmdStat()
{
    String s = nowStr();
    s += " armed=";
    s += ArmedSetting.get() ? 'Y' : 'n';
    s += " on=";
    s += on ? 'Y' : 'n';
    s += " prob=";
    s += ProbabilitySetting.get();
    s += "%\nbutton=";
    s += ButtonCounter.get();
    s += " timehit=";
    s += TimeHitCounter.get();
    s += " beephit=";
    s += BeepHitCounter.get();
    return s;
}

String cmdProb(String prob)
{
    uint8_t percent = prob.toInt();
    if (percent < 1 || percent > 100) {
        return "?v";
    }
    ProbabilitySetting.set(percent, true);
    return "k";
}

void triggerCmd()
{
    if (cmd.length() == 0)
        return;
    DB(F("RX '"));
    DB(cmd);
    DBLN('\'');
    String response("?");
    if (cmd.startsWith("AUTH")) {
        response = cmdAuth(removeLeading(cmd, 4));
    } else if (cmd == "DEAUTH") {
        if (isAuthorized()) {
            response   = "k";
            lastAuthMs = 0;
        }
    } else if (cmd.startsWith("STIME")) {
        IFAUTH(cmdStime(removeLeading(cmd, 5)));
    } else if (cmd == "LIST") {
        IFAUTH(cmdList());
    } else if (cmd.startsWith("SET")) {
        IFAUTH(cmdSet(removeLeading(cmd, 3)));
    } else if (cmd.startsWith("CLR")) {
        IFAUTH(cmdClr(removeLeading(cmd, 3)));
    } else if (cmd == "DISARM") {
        IFAUTH(cmdDisarm());
    } else if (cmd == "ARM") {
        IFAUTH(cmdArm());
    } else if (cmd.startsWith("BEEP")) {
        IFAUTH(cmdBeep(removeLeading(cmd, 4)));
    } else if (cmd == "STAT") {
        IFAUTH(cmdStat());
    } else if (cmd.startsWith("PROB")) {
        IFAUTH(cmdProb(removeLeading(cmd, 4)));
    }
    txString(response);
    cmd = "";
}

void rxChar(int c)
{
    if (c == '\n' || c == '\r') {
        triggerCmd();
    } else if (c >= ' ' || c <= '~') {
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        cmd += (char)c;
    }
}

void loop()
{
    if (DoEvery(OnCheckMs, lastOnCheckMs)) {
        bool prevOn = on;
        onCheck();   
        if (prevOn != on) {
            String msg("on=");
            msg += on ? 'Y' : 'n';
        }
    }

    Button.update();
    if (Button.pushed()) {
        txString("BUT!");
        ButtonCounter.set(ButtonCounter.get() + 1, true);
        if (on) {
            TimeHitCounter.set(TimeHitCounter.get() + 1, true);
            if (random(1, 100) < ProbabilitySetting.get()) {
                sound(random(50, 650));
            }
        }
    }

    int c;
    while ((c = Bluetooth.read()) > 0) {
        rxChar(c);
    }
}
