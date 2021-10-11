# clmclk

This is a tool for DOS to adjust Cirrus Logic CL-GD542x memory clock.

This tool works on ISA and VL graphics cards with the CL-GD542, CL-GD5426, CL-GD5428 or CL-GD5429 graphics chips, as well as mainboards having one of these chips on board. If you invoke the tool without parameters, it prints the current memory clock. If you pass it parameters, the first parameter is interpreted as the desired memory clock in MHz. The value is rounded to the next possible value, the memory clock gets updated, and the actual frequency is printed to the screen.

The program accepts memory clock values between 30 and 80 MHz. The lower limit is arbitrary, but using lower clocks doesn't seem to make any sense at all. The upper limit is to prevent a 16-bit integer overflows in the code. It does *not* mean a frequency of 80 MHz is safe, recommended or even endorsed for any card. In fact, the 5424, 5426 and 5428 chips are specified up to 50 MHz, and the 5429 chip is specified up to 60 MHz. *Use higher frequencies at your own risk!*

The source code is meant to be compiled with Borland / Turbo C, any memory model will work.
