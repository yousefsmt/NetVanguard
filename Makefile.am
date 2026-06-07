KDIR = @KDIR@
EXTRA_DIST = src/kernel/kernel.c Kbuild

all-local:
	$(MAKE) -C $(KDIR) M=$(abs_srcdir) modules

install-exec-local:
	$(MAKE) -C $(KDIR) M=$(abs_srcdir) modules_install

clean-local:
	$(MAKE) -C $(KDIR) M=$(abs_srcdir) clean