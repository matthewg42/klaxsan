#include <stdint.h>
#include <MutilaDebug.h>
#include "Config.h"
#include "Settings.h"

// Params: min, max, default
PersistentSetting<bool> ArmedSetting(false, true, false);
PersistentSetting<uint8_t> ProbabilitySetting(1, 100, 100);
PersistentSetting<uint16_t> MagicSetting(0, UINT16_MAX, SettingsMagicNumber);

void checkSettings()
{
    // Check if we have valid settings
    if (MagicSetting.get() != SettingsMagicNumber) {
        // Save all the settings with their default values
        DBLN(F("EEPROM nomagic - setting defaults"));
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
    ProbabilitySetting.reset(true);
}

void printSettings()
{
	DB(F("- ArmedSetting = "));
	DB(ArmedSetting.get() ? 'Y' : 'n');
	DB(F("- ProbabilitySetting = "));
	DB(ProbabilitySetting.get());
    DBLN('%');
}

