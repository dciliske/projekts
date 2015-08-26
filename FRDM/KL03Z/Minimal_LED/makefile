APPNAME=blink
ELF=$(APPNAME).elf
IMAGE=$(APPNAME).bin
SREC=$(APPNAME).srec
ARM_GCC_BASE=arm-none-eabi
ARM_GCC_FLAGS=-mthumb -mtune=cortex-m0plus
ARM_AS=$(ARM_GCC_BASE)-as
ARM_AS_FLAGS=-mthumb -march=armv6-m
ARM_OBJ_COPY=$(ARM_GCC_BASE)-objcopy
ARM_OBJ_COPY_FLAGS= --strip-all --image-base 0x20000000

AS_SRCS = blink.s

all: bin srec elf target bin_src

target:
	$(ARM_AS) $(ARM_AS_FLAGS) -o $(APPNAME) $(AS_SRCS)

image: bin

elf: target $(AS_SRCS)
	$(ARM_OBJ_COPY) $(ARM_OBJ_COPY_FLAGS) $(APPNAME) $(ELF)

bin: target $(AS_SRCS)
	$(ARM_OBJ_COPY) $(ARM_OBJ_COPY_FLAGS) -O binary $(APPNAME) $(IMAGE)

srec: target $(AS_SRCS)
	$(ARM_OBJ_COPY) $(ARM_OBJ_COPY_FLAGS) -O srec $(APPNAME) $(SREC)

bin_src: bin
	compfile $(IMAGE) blink_code blink_code_len $(APPNAME)_bin.c
