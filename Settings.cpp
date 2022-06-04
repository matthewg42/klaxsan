#include <stdint.h>
#include <MutilaDebug.h>
#include "Config.h"
#include "Settings.h"

// Params: min, max, default
PersistentSetting<bool> ArmedSetting(false, true, false);
PersistentSetting<uint16_t> CoolDownSetting(0, 600, 15, 16);
PersistentSetting<uint8_t> ProbabilitySetting(1, 100, 100);
PersistentSetting<uint16_t> ButtonCounter(0, UINT16_MAX, 0, 16);
PersistentSetting<uint16_t> TimeHitCounter(0, UINT16_MAX, 0, 16);   
PersistentSetting<uint16_t> BeepHitCounter(0, UINT16_MAX, 0, 16);
PersistentSetting<uint16_t> ManualCounter(0, UINT16_MAX, 0, 16);
PersistentSetting<uint16_t> MagicSetting(0, UINT16_MAX, SettingsMagicNumber);

void checkSettings()
{
    // Check if we have valid settings
    if (MagicSetting.get() != SettingsMagicNumber) {
        // Save all the settings with their default values
        DBLN(F("EEPROM magic mismatch - setting defaults"));
        resetSettings();
        MagicSetting.reset(true);
    } else {
        DBLN(F("Settings loaded from EEPROM"));
        printSettings();
    }
}

void resetSettings()
{
    DBLN(F("resetting defaults & save to EEPROM"));
    ArmedSetting.reset(true);
    CoolDownSetting.reset(true);
    ProbabilitySetting.reset(true);
    ButtonCounter.reset(true);
    TimeHitCounter.reset(true);
    BeepHitCounter.reset(true);
    ManualCounter.reset(true);
}

void printSettings()
{
    DBLN(F("Settings:"));
    DB(F("- ArmedSetting = "));
    DBLN(ArmedSetting.get() ? 'Y' : 'n');
    DB(F("- CoolDownSetting = "));
    DBLN(CoolDownSetting.get());
    DB(F("- ProbabilitySetting = "));
    DB(ProbabilitySetting.get());
    DBLN('%');
    DB(F("- ButtonCounter = "));
    DBLN(ButtonCounter.get());
    DB(F("- TimeHitCounter = "));
    DBLN(TimeHitCounter.get());
    DB(F("- BeepHitCounter = "));
    DBLN(BeepHitCounter.get());
    DB(F("- ManualCounter = "));
    DBLN(ManualCounter.get());
}

