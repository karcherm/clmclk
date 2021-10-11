#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

unsigned read_mclk_value(void)
{
	int result;
	disable();
	outportb(0x3c4, 0x1F);
	result = inportb(0x3c5);
	enable();
	return result;
}

void write_mclk_value(int new_value)
{
	outport(0x3c4, 0x1f | (new_value << 8));
}

#define REFCLK 1432U

unsigned to_1e4hz(unsigned input)
{
	return input * (REFCLK / 8);
}

unsigned from_mhz(unsigned mhz)
{
	return (mhz*800 + REFCLK/2) / REFCLK;
}

void ensure_cirrus(void)
{
	unsigned char oldval;
	unsigned char readback1, readback2;
	int crtcbase;

	// Probe for magic behaviour of the Cirrus unlock register.
	// write xxx1x010 to unlock, read back 0x12 if unlocked
	// write anything else to lock, read back 0x0F if locked
	disable();
	outportb(0x3c4, 6);
	oldval = inportb(0x3c5);
	outportb(0x3c5, 0);
	readback1 = inportb(0x3c5);
	outportb(0x3c5, 0xF2);
	readback2 = inportb(0x3c5);
	outportb(0x3c5, oldval);
	enable();
	if (readback1 != 0xF || readback2 != 0x12)
	{
		printf("This doesn't seem to be a cirrus chip "
		       "(diag: %02X/%02X)!\n", readback1, readback2);
		exit(1);
	}

	// read cirrus chip ID register
	disable();
	outport(0x3c4, 0x1206);
	crtcbase = (inportb(0x3cc) & 1) ? 0x3d4 : 0x3b4;
	outportb(crtcbase, 0x27);
	readback1 = inportb(crtcbase+1);
	enable();

	if((readback1 & 0xF0) != 0x90)
	{
		printf("Only CL-GD5424/26/28/29 supported. Chip ID = %02x\n",
		       readback1);
		exit(1);
	}
}

void lock_cirrus(void)
{
	outport(0x3c4, 0x0F06);
}

int main(int argc, char* argv[])
{
	unsigned freq, val;
	ensure_cirrus();
	if (argc == 1)
	{
		val = read_mclk_value();
	}
	else
	{
		unsigned long mhz;
		char *endptr;
		mhz = strtoul(argv[1], &endptr, 10);
		if (*endptr != 0 ||
		    mhz < 30 ||
		    mhz > 80)
		{
			printf("ERROR: Bad new MCLK value\n"
			       "\"%s\" is not an integer between 30 and 80\n", argv[1]);
			return 1;
		}
		val = from_mhz((unsigned)mhz);
		write_mclk_value(val);
	}
	freq = to_1e4hz(val);
	printf("Current MCLK: %d.%02d MHz\n", freq / 100, freq % 100);
	return 0;
}
