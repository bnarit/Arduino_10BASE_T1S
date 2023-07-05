ifndef KDIR
    KDIR=/lib/modules/$(shell uname -r)/build
endif

obj-m := microchip_t1s.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
