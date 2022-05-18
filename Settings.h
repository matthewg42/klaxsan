#pragma once

#include <PersistentSetting.h>
#include "Config.h"

extern PersistentSetting<bool> ArmedSetting;
extern PersistentSetting<uint8_t> ProbabilitySetting;
extern PersistentSetting<uint16_t> MagicSetting; // Used to automatically detect invalid settings

void checkSettings();
void resetSettings();
void printSettings();


