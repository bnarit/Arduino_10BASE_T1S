ifndef KDIR
#KDIR=/lib/modules/$(shell uname -r)/build
    KDIR=/home/i17164/Projects/arduino/linux
endif

obj-m := microchip_t1s.o
obj-m += smsc95xx_t1s.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
