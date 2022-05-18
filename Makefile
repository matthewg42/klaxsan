ifdef AUTH_CODE
	CPPFLAGS += -DAUTH_CODE="\"$(AUTH_CODE)\""
endif

CPPFLAGS += -Wno-deprecated-declarations
CPPFLAGS += -DDEBUG
MONITOR_PORT = /dev/ttyUSB0
ARDUINO_LIBS += Adafruit_NeoPixel Mutila SoftwareSerial Ds1302 EEPROM

# For a modern nano:
BOARD_TAG = nano
BOARD_SUB = atmega328


ifndef ARDUINO_MAKEFILE
	# git clone https://github.com/sudar/Arduino-Makefile.git /opt/Arduino-Makefile
	ARDUINO_MAKEFILE := /opt/Arduino-Makefile/Arduino.mk
endif

include $(ARDUINO_MAKEFILE)

