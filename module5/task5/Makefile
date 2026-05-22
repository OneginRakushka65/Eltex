obj-m += serv_nl_module.o

KDIR := /home/slarkfish/WSL2-Linux-Kernel

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean