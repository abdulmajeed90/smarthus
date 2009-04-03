# makefile, written by guido socher
#MCU=atmega168
#DUDECPUTYPE=m168
MCU=atmega328p
DUDECPUTYPE=m328
#
LOADCMD=avrdude
LOADARG=-p $(DUDECPUTYPE) -c dt006 -e -U flash:w:
#
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
.PHONY: test0 test1 test2 all
#
all: test0.hex test1.hex test2.hex test_readSiliconRev.hex eth_rem_dev_tcp.hex
	@echo "done"
#
test0: test0.hex
	@echo "done"
#
test1: test1.hex 
	@echo "done"
#
test_readSiliconRev: test_readSiliconRev.hex
	@echo "done"
#
test2: test2.hex
	@echo "done"
#
#-------------------
help: 
	@echo "Usage: make all|test0.hex|test1.hex|test2.hex|load|load_pre|load_test0|load_test1|load_test2|rdfuses"
	@echo "or"
	@echo "Usage: make clean"
	@echo " "
	@echo "For new hardware with clock from enc38j60: make fuse"
#-------------------
pre: eth_rem_dev_tcp.hex
	cp eth_rem_dev_tcp.hex eth_rem_dev_tcp_pre.hex
eth_rem_dev_tcp.hex : eth_rem_dev_tcp.out 
	$(OBJCOPY) -R .eeprom -O ihex eth_rem_dev_tcp.out eth_rem_dev_tcp.hex 
	avr-size eth_rem_dev_tcp.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "

eth_rem_dev_tcp.out : main.o ip_arp_udp_tcp.o enc28j60.o 
	$(CC) $(CFLAGS) -o eth_rem_dev_tcp.out -Wl,-Map,eth_rem_dev_tcp.map main.o ip_arp_udp_tcp.o enc28j60.o
enc28j60.o : enc28j60.c avr_compat.h timeout.h enc28j60.h
	$(CC) $(CFLAGS) -Os -c enc28j60.c
ip_arp_udp_tcp.o : ip_arp_udp_tcp.c net.h avr_compat.h enc28j60.h
	$(CC) $(CFLAGS) -Os -c ip_arp_udp_tcp.c
main.o : main.c ip_arp_udp_tcp.h avr_compat.h enc28j60.h timeout.h net.h
	$(CC) $(CFLAGS) -Os -c main.c
#------------------
test0.hex : test0.out 
	$(OBJCOPY) -R .eeprom -O ihex test0.out test0.hex 
	avr-size test0.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "
test0.out : test0.o 
	$(CC) $(CFLAGS) -o test0.out -Wl,-Map,test0.map test0.o 
test0.o : test0.c 
	$(CC) $(CFLAGS) -Os -c test0.c
#------------------
test2.hex : test2.out 
	$(OBJCOPY) -R .eeprom -O ihex test2.out test2.hex 
	avr-size test2.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "
test2.out : test2.o enc28j60.o ip_arp_udp_tcp.o
	$(CC) $(CFLAGS) -o test2.out -Wl,-Map,test2.map test2.o enc28j60.o ip_arp_udp_tcp.o
test2.o : test2.c ip_arp_udp_tcp.h avr_compat.h enc28j60.h timeout.h net.h
	$(CC) $(CFLAGS) -Os -c test2.c
#------------------
test1.hex : test1.out 
	$(OBJCOPY) -R .eeprom -O ihex test1.out test1.hex 
	avr-size test1.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "
test1.out : test1.o enc28j60.o ip_arp_udp_tcp.o
	$(CC) $(CFLAGS) -o test1.out -Wl,-Map,test1.map test1.o enc28j60.o ip_arp_udp_tcp.o
test1.o : test1.c ip_arp_udp_tcp.h avr_compat.h enc28j60.h timeout.h net.h
	$(CC) $(CFLAGS) -Os -c test1.c
#------------------
test_readSiliconRev.hex : test_readSiliconRev.out 
	$(OBJCOPY) -R .eeprom -O ihex test_readSiliconRev.out test_readSiliconRev.hex 
	avr-size test_readSiliconRev.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "
test_readSiliconRev.out : test_readSiliconRev.o enc28j60.o ip_arp_udp_tcp.o
	$(CC) $(CFLAGS) -o test_readSiliconRev.out -Wl,-Map,test_readSiliconRev.map test_readSiliconRev.o enc28j60.o ip_arp_udp_tcp.o
test_readSiliconRev.o : test_readSiliconRev.c ip_arp_udp_tcp.h avr_compat.h enc28j60.h timeout.h net.h
	$(CC) $(CFLAGS) -Os -c test_readSiliconRev.c
#------------------
#------------------
load_test2: test2.hex
	$(LOADCMD) $(LOADARG)test2.hex
#
load_readSiliconRev: test_readSiliconRev.hex
	$(LOADCMD) $(LOADARG)test_readSiliconRev.hex
#
load_test1: test1.hex
	$(LOADCMD) $(LOADARG)test1.hex
#
load_test0: test0.hex
	$(LOADCMD) $(LOADARG)test0.hex
#------------------
load: eth_rem_dev_tcp.hex
	$(LOADCMD) $(LOADARG)eth_rem_dev_tcp.hex
#
load_pre: eth_rem_dev_tcp_pre.hex
	$(LOADCMD) $(LOADARG)eth_rem_dev_tcp_pre.hex
#-------------------
# Check this with make rdfuses
rdfuses:
	avrdude -p $(DUDECPUTYPE) -c dt006 -v -q
#
fuse:
	@echo "warning: run this command only if you have an external clock on xtal1"
	@echo "The is the last chance to stop. Press crtl-C to abort"
	@sleep 2
	avrdude -p  $(DUDECPUTYPE) -c dt006 -u -v -U lfuse:w:0x60:m
#-------------------
clean:
	rm -f *.o *.map *.out test*.hex eth_rem_dev_tcp.hex
#-------------------
