#
# SDCC Makefile for mcs51
#
# ------------------------------------------------------
# PATH

INCDIR  = ./inc
SRCDIR  =./src
OBJDIR  = ./obj
HEXDIR = ./hex

LIBDIR  =  
# ------------------------------------------------------
# Target and Source
TARGET = $(HEXDIR)/hello_world
ICP_ADDR = 10.2.0.30

#C_SRC = $(SRCDIR)/helloworld.c
C_SRC = $(wildcard  $(SRCDIR)/*.c)
ASM_SRC = 

C_SRC_FILE = $(notdir $(C_SRC))
C_OBJ_FILE = $(C_SRC_FILE:%.c=%.c.rel)

#OBJ = $(OBJDIR)/helloworld.c.rel      
OBJ = $(addprefix $(OBJDIR)/, $(C_OBJ_FILE)) 

# ------------------------------------------------------
# Usually SDCC's small memory model is the best choice.  If
# you run out of internal RAM, you will need to declare
# variables as "xdata", or switch to largeer model

# Memory Model (small, medium, large, huge)
MODEL  = small
# ------------------------------------------------------
# Memory Layout
# PRG Size = 4K Bytes
CODE_SIZE = --code-loc 0x0000 --code-size 4096
# INT-MEM Size = 256 Bytes
IRAM_SIZE = --idata-loc 0x0000  --iram-size 256
# EXT-MEM Size = 4K Bytes
XRAM_SIZE = --xram-loc 0x0000 --xram-size 768



# ------------------------------------------------------
# SDCC
BINPATH = /Users/dciliske/Applications/sdcc/bin
CC = sdcc
AS = sdas8051

MCU_MODEL = mcs51

#LIBS    = 
#LIBPATH = -L $(LIBDIR)

#DEBUG = --debug
AFLAGS = -l -s
CFLAGS = -I/Users/dciliske/Documents/projekts/8051/N76E003_BSP_Keil_C51_V1.0.6/Include -I$(INCDIR) -m$(MCU_MODEL) --model-$(MODEL) --out-fmt-ihx --no-xinit-opt $(DEBUG)
LFLAGS = $(LIBPATH) $(LIBS) -m$(MCU_MODEL) --model-$(MODEL) $(CODE_SIZE) $(IRAM_SIZE) $(XRAM_SIZE) --out-fmt-ihx  $(DEBUG)

# ------------------------------------------------------
# S = @

.PHONY: all clean load

all: $(TARGET).bin

$(HEXDIR)/%.bin: $(HEXDIR)/%.hex
	$(S) $(BINPATH)/makebin -p $^ > $@

$(HEXDIR)/%.hex: $(OBJDIR)/%.ihx
	$(S) $(BINPATH)/packihx $^ > $@

$(OBJDIR)/%.ihx: $(OBJ)
	$(S) $(BINPATH)/$(CC) -o $@ $(LFLAGS) $^

$(OBJDIR)/%.c.rel: $(SRCDIR)/%.c
	$(S) $(BINPATH)/$(CC) -o $@ $(CFLAGS) -c $^

$(OBJDIR)/%.asm.rel: $(SRCDIR)/%.asm
	$(S) $(BINPATH)/$(AS) $(AFLAGS) -o $@ $^ 

clean:
	$(S) rm -rf $(OBJDIR)/*
	$(S) rm -rf $(TARGET).hex

load: all
	$(S) ftp -u ftp://$(ICP_ADDR)/APROM.bin $(TARGET).bin
