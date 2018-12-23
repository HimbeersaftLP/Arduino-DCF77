# Arduino-DCF77
Get time from DCF77 using an Arduino

If you just salvaged one of those time receiver modules from your alarm clock (e.g. mine is labled as RC8001 V2), this is the right repositiory for you!

Just connect it like this:

| Module    | Arduino   |
| --------- | --------- |
| VCC       | 5V or 3V3 |
| TCO       | D2        |
| PON       | GND       |
| GND       | GND       |

(you can pull PON high to disable the module to save energy)

(the pin for TCO can be changed by editing the variable dataPin)
