# Propeller Game Engine (C) 2013 Marco Maccaferri
# TERMS OF USE: MIT License

# basic gnu tools
CC := propeller-elf-gcc
CXX := propeller-elf-g++
LD := propeller-elf-ld
AS := propeller-elf-as
AR := propeller-elf-ar
OBJCOPY := propeller-elf-objcopy
LOADER := propeller-load

# BSTC program
BSTC := bstc
SPINDIR := .

NAME := frogger
MODEL := lmm
CFLAGS := -Os -Wall -fno-exceptions -fdata-sections -ffunction-sections
LDFLAGS := -s -Wl,--script=propeller.ld -Wl,--gc-sections

OBJS := \
    main.o \
    graphics.o \
    sound.o \
    graphics_renderer.o \
    graphics_renderer_firmware.o \
    vga_driver.o \
    vga_driver_firmware.o \
    ay3891.o \
    ay3891_firmware.o \
    nes_gamepad_ctrl.o 

all: $(NAME).elf $(NAME).binary

$(NAME).elf: $(OBJS) Makefile
	$(CC) -m$(MODEL) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)


%.o: %.c
	$(CC) -m$(MODEL) $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CC) -m$(MODEL) $(CXXFLAGS) -o $@ -c $<

%.o: %.s
	$(CC) -o $@ -c $<


%.binary: %.elf
	$(LOADER) -s $<

%.dat: $(SPINDIR)/%.spin
	$(BSTC)  -Ox -c -o $(basename $@) $<

%_firmware.o: %.dat
	$(OBJCOPY) -I binary -B propeller -O $(CC) $< $@

#
# cleanup
#
clean:
	rm -f *.o *.elf *.a *.dat *.cog *.ecog *.binary

#
# how to run
#
run: $(NAME).elf
	$(LOADER) $(BOARDFLAG) $(NAME).elf -r -p /dev/ttyUSB0

#
# how to write to eeprom
#
burn: $(NAME).elf
	$(LOADER) $(BOARDFLAG) $(NAME).elf -r -e -p /dev/ttyUSB0
