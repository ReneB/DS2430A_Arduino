# DS2430A_Arduino
Arduino library for Maxim DS2430A 1-Wire EEPROM

## Required libraries
* OneWire <https://github.com/PaulStoffregen/OneWire>

## Adapted from
Almost all the work in this library was done by [Tom Magnier](https://github.com/tommag/DS2431_Arduino). The only thing I did was adapt the library for the slightly older DS2430A EEPROM which uses different addressing and protocols for reading and writing.

Since copying the scratchpad on the DS2430A copies the *whole* scratchpad to memory, the only way to reliably "modify" its contents is to read the whole memory, make modifications locally, write the new data to the scratchpad and copy the scratchpad to the memory. The DS2431, in contrast, has a row-based (8-byte) scratchpad writing mechanism. Therefore, the implementation here is kept simple - read 32 bytes of memory, locally modify, write to scratchpad, copy scratchpad, done.

TODO: verification that what's in the scratchpad actually matches what we think should be in there, if requested from the `write()` method.
