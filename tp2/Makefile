CARD_NUMB	= 25
ROUTER          = 132.227.102.36
LOGIN           = aggoun-phan
LAB             = tp2

MODULE1         = module
MODULE2         = ledbp

CROSSDIR        = /users/enseig/franck/peri
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE1).o
obj-m           += $(MODULE2).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules
clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
upload:;        scp -P50$(CARD_NUMB) $(MODULE1).ko $(MODULE2).ko *.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
