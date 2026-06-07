# Variables substituted from configure.ac
KDIR = @KDIR@

# The actual source code files that need to be packaged in your tarball
EXTRA_DIST = src/kernel/kernel.c Kbuild

# 1. Build the module
# We call Kbuild. $(abs_srcdir) handles out-of-tree builds cleanly.
all-local:
	$(MAKE) -C $(KDIR) M=$(abs_srcdir) modules

# 2. Install the module
# Installs to /lib/modules/.../extra/ (standard for custom modules)
install-exec-local:
	$(MAKE) -C $(KDIR) M=$(abs_srcdir) modules_install

# 3. Clean up Kbuild artifacts
clean-local:
	$(MAKE) -C $(KDIR) M=$(abs_srcdir) clean