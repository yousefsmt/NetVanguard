KDIR = @KDIR@
EXTRA_DIST = src/kernel/kernel.c Kbuild

all-local:
	@mkdir -p $(abs_builddir)/src/kernel
	@cp -p $(abs_srcdir)/Kbuild $(abs_builddir)/Kbuild
	@cp -p $(abs_srcdir)/src/kernel/kernel.c $(abs_builddir)/src/kernel/kernel.c
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules

install-exec-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) modules_install

clean-local:
	$(MAKE) -C $(KDIR) M=$(abs_builddir) clean