EMF 2022 project: KlaxSAN
=========================


Concept
-------

KlaxSAN was created as a rogue installation for EMF Camp 2022. Free-standing hand sanitizing 
station that makes a loud "toot" when used. The hope is to surprise and amuse people. 

Only makes noises at specified times to prevent being too much of a nuisance ("Thank you for 
being responsible with your irresponsibility"). 

Remote controllable / reconfigurable using Bluetooth serial app from smartphone. 


Hardware
--------

* Pedestal fabricated from 4mm plate steel
* Brains: Arduino nano with firmware from this repo
* Switch: 30A DC relay module
* Remote: Bluetooth HC-05 module (plus 3.3/5V logic shifter for comms with BT module)
* Time: DS1302-based RTC module 
* Main power: 12V lead acid leisure battery
* Electronics power: Large USB power bank
* Noise maker: multi-tone car horn

Electronics was powered separately from horn to prevent inductive spikes from frying things. 


Comms
-----

Bluetooth module should be configured with pairing PIN prior to use with KlaxSAN. Pair and use
BT serial to send commands / receive responses. During development it looked as though BT module 
was buggy and didn't allow for change of PIN from the defaut of 1234.  Therefore there is a 
secondary AUTH command to perform authorization after connection, without which other commands
will not function.

```
ARM                         Arm KlaxSAN
AUTH<buildtimecode>         Required before other commands will function (60 sec timeout)
BEEP#                       Manual toot for # milliseconds
COOL#                       Set cool down setting (seconds)
CLRn                        Clear active slot n (0-9 or * for all)
DEAUTH                      Revoke authorization immediately
DISARM                      Disarm KlaxSAN
LIST                        Print list of active time slots
PROB#                       Set probability (percent) of toot per press
SETn=ddhhmm-ddhhmm          Set slot n to be active between specified day, hour and min
STAT                        Print current status
STIMEddhhmm                 Set RTC to day hour and min
```

Response will generally be "k" (for OK), and "?" with some second letter suffix to indicate 
problems.


Building the Firmware
---------------------

Build with Arduino Makefile.

```
export AUTH_CODE=passphrase
make
make upload
```

Alternatively, edit `Config.h`, and add some `#define AUTH_CODE` value and then build with Aruino
IDE thing.

