.PHONY: all

all: hexc decc

hexc: hex.c
	gcc -o hexc hex.c

decc: hex.c
	gcc -o decc -DINPUT_HEX hex.c

clean: 
	rm -f hexc
	rm -f decc
