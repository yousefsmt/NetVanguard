## Process this file with automake to produce Makefile.in

## this is so that Automake includes the C compiling definitions, and
## includes the source files in the distribution.

## @kerneldir@ is the kernel build directory.
## @kernelext@ is the kernel module extension `ko`.
## @moduledir@ is the module installation directory.
## @depmod@ is the depmod program.

# Define the variable using Autoconf substitution
moduledir = @moduledir@

# Register the dummy program so it isn't installed, but is tracked
noinst_PROGRAMS = automake_dummy

# Assign the source files to the dummy program
automake_dummy_SOURCES = src/kernel/kernel.c 

# Keep your non-C files in EXTRA_DIST so they get packaged
EXTRA_DIST = Makefile.kernel

## there is no *just* object file support in automake.  This is close enough
module_DATA = mymodule.o
export_objs = mymodule.o

# where the kernel build is located
KERNEL_LOCATION=@kerneldir@

MYMODULE_TOP_SRCDIR = @abs_top_srcdir@
MYMODULE_TOP_BUILDDIR = @abs_top_builddir@
MYMODULE_BUILDDIR = @abs_builddir@

AM_CPPFLAGS = -I$(top_srcdir)/include
SUBDIRS = src

# some magic for using linux kernel settings
# when compiling module(s)
MYMODULE_EXTRA_CFLAGS = -DEXPORT_SYMTAB $(DEFS) -I$(MYMODULE_BUILDDIR) \
    -I$(MYMODULE_TOP_BUILDDIR) -I$(MYMODULE_TOP_SRCDIR)/include
export MYMODULE_EXTRA_CFLAGS KERNEL_LOCATION module_DATA export_objs

.PHONY: FORCE

$(automake_dummy_SOURCES): FORCE
    @test "$(srcdir)" == "." || test -e "$@" || ln -vs "$(srcdir)/$@" .

# Add FORCE in case the kernel has changed.
$(module_DATA): $(generated_sources) $(automake_dummy_SOURCES) FORCE
    mv Makefile Makefile.automake
    cp Makefile.kernel Makefile
    $(MAKE) -C $(KERNEL_LOCATION) SUBDIRS=$(MYMODULE_BUILDDIR) M=$(MYMODULE_BUILDDIR) modules
    mv Makefile.automake Makefile

install-moduleDATA: $(module_DATA)
    $(mkinstalldirs) $(DESTDIR)$(moduledir)
    @list='$(module_DATA:.o=.@kernelext@)'; for p in $$list; do \
      if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
      f="`echo $$p | sed -e 's|^.*/||'`"; \
      echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(moduledir)/$$f"; \
      $(INSTALL_DATA) $$d$$p $(DESTDIR)$(moduledir)/$$f; \
    done

uninstall-moduleDATA:
    @listun='$(module_DATA:.o=.@kernelext@)'; for p in $$listun; do \
      f="`echo $$p | sed -e 's|^.*/||'`"; \
      echo " rm -f $(DESTDIR)$(moduledir)/$$f"; \
      rm -f $(DESTDIR)$(moduledir)/$$f; \
    done

if SANDBOXED
else
install-data-local: install-moduleDATA
    -@depmod@ -a
endif

MOSTLYCLEANFILES = $(module_DATA) $(module_DATA:.o=.@kernelext@) \
    Modules.symvers Module.symvers Module.markers modules.order
CLEANFILES = $(module_DATA:.o=.mod.c) $(generated_sources)

clean-local:
    -rm -f .*.cmd .*.flags
    -rm -rf .tmp_versions
    -if test "$(srcdir)" != "."; then \
        for f in $(automake_dummy_SOURCES); do \
            if test "$$f" -ef "$(srcdir)/$$f"; then \
                rm -f "$$f"; \
            fi; \
        done; \
    fi

FORCE: