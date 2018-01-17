# ESP8266_UniqueDeviceID

This library manages the generation and storage of a random
128-bit device identifier which will survive device reboots
and Arduino software uploads.

With this, you should be able to flash many devices with the
same code and they will all obtain a unique ID on their first
boot. There should be no need to run a specific configuration
per device.

This library was tested with the Adafruit Feather HUZZAH
ESP8266, but it should work with other ESP8266 devices. It
uses pin 0 for random entropy, pin 3 to blink the LED during
the random generation phase, and the ID is stored to device
flash.

## Pins

  - Pin #0: Must be disconnected and floating. Used by the random
number generator for entropy.
  - Pin #2: LED output. This is the blue LED on the HUZZAH

## RNG Theory

NOTE: this fails the "don't design your own crypto" rule.
I don't design crypto systems professionally, and this software
has not been statistically or formally tested. Use at your own risk.

A non-deterministic random number generator is used to create
unique IDs, such that two devices running the exact same code
will obtain completely different identifiers. Several rounds of
entropy data are mixed to produce each byte of the identifier,
using the following algorithm:

  1. A 32-bit random pool holds the generated random data.
  2. A byte is read from pin #0, which is floating and should
produce _some_ amount of entropy.
  3. The random pool is rotated 1 bit to the left.
  4. The entropy byte is xored with the random pool.
  5. Steps 2-4 are repeated through 2048 rounds.
  6. The least significant 8 bits of the pool are returned as
a random byte.
  7. The entropy in the random pool is not re-initialized, and is
instead reused for subsequent reads.


## TODO

  - Investigate combining pin entropy with a proper PRNG


## License

This library was written by Jason Striegel and provided
under the BSD license. See license.txt for details.

