DEVICE     = atmega328p
CLOCK      = 16000000
BAUDRATE   = 57600
PORT       = /dev/ttyUSB0

SRC_DIR    = ./src
HDR_DIR    = ./hdr
OBJ_DIR    = ./obj
TGT_DIR    = ./tgt

PROG = spidebug
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC = avr-gcc
CFLAGS  = -Wall -Os -DF_CPU=$(CLOCK) -DBAUD=$(BAUDRATE) -mmcu=$(DEVICE) -I$(HDR_DIR)
#CFLAGS += -DSPI_MASTER
LIBS = -mmcu=$(DEVICE)

all : $(TGT_DIR)/$(PROG).hex
	@rm -f $(OBJ_DIR)/*
	@cp $^ ~/tmp
	@cp $(TGT_DIR)/FIRMWARE.BIN ~/tmp

flash : $(TGT_DIR)/$(PROG).hex
	@rm -f $(OBJ_DIR)/*
	avrdude -F -V -c arduino -p $(DEVICE) -P $(PORT) -b $(BAUDRATE) -U flash:w:$^
	@rm -f $(OBJ_DIR)/*

$(TGT_DIR)/$(PROG).elf: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

$(TGT_DIR)/$(PROG).hex: $(TGT_DIR)/$(PROG).elf
	rm -f $@
	avr-objcopy -j .text -j .data -O ihex $^ $@
	avr-objcopy -I ihex -O binary $@ $(TGT_DIR)/FIRMWARE.BIN
	avr-size --format=sysv $^

$(OBJS) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@rm -f $(TGT_DIR)/*
	@rm -f $(OBJ_DIR)/*
