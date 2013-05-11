###############################################################################
# Makefile for the project bc
###############################################################################

## General Flags
PROJECT = bc
MCU = at90can128
TARGET = bc.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                  -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=bc.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\Users\Lenovo\Desktop\bc\." 

## Objects that must be built in order to link
OBJECTS = c_startup.o lcd_diver.o touchscreen_driver.o clock.o ADC.o TWI.o gear.o f_meter.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = "C:\Users\Lenovo\Desktop\bc\avrx.o" 

## Build
all: $(TARGET) bc.hex bc.eep bc.lss size

## Compile
c_startup.o: ../c_startup.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd_diver.o: ../lcd_diver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

touchscreen_driver.o: ../touchscreen_driver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

clock.o: ../clock.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ADC.o: ../ADC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

TWI.o: ../TWI.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gear.o: ../gear.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

f_meter.o: ../f_meter.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) bc.elf dep/* bc.hex bc.eep bc.lss bc.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

