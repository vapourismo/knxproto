# Utilities
RM              = rm -rf
MKDIR           = mkdir -p
EXEC            = exec
INSTALL         = install
DEBUGGER        = gdb -ex run
MEMCHECKER      = valgrind -v --leak-check=full

# Install Directories
BASENAME        := knxclient
PREFIX          ?= /usr/local
INCLUDEDIR      ?= $(PREFIX)/include/$(BASENAME)
LIBDIR          ?= $(PREFIX)/lib

# Local Directories
DISTDIR         = dist
SOURCEDIR       = src
TESTDIR         = test

# Artifacts
HEADERFILES     = proto/knxnetip/connreq.h proto/knxnetip/connres.h proto/knxnetip/connstatereq.h proto/knxnetip/connstateres.h proto/knxnetip/dcreq.h proto/knxnetip/dcres.h proto/knxnetip/hostinfo.h proto/knxnetip/knxnetip.h proto/knxnetip/tunnelreq.h proto/knxnetip/tunnelres.h proto/knxnetip/routingind.h \
                  proto/cemi/cemi.h proto/cemi/ldata.h proto/cemi/tpdu.h \
                  util/address.h util/sockutils.h \
                  tunnelclient.h routerclient.h
SOURCEFILES     = proto/knxnetip/connstateres.c proto/knxnetip/connreq.c proto/knxnetip/tunnelreq.c proto/knxnetip/connstatereq.c proto/knxnetip/connres.c proto/knxnetip/dcreq.c proto/knxnetip/hostinfo.c proto/knxnetip/knxnetip.c proto/knxnetip/tunnelres.c proto/knxnetip/dcres.c proto/knxnetip/routingind.c \
                  proto/cemi/cemi.c proto/cemi/ldata.c proto/cemi/tpdu.c \
                  util/address.c util/sockutils.c util/log.c \
                  tunnelclient.c routerclient.c

TESTFILES       = $(wildcard $(TESTDIR)/*.c)
HEADEROBJS      = $(HEADERFILES:%=$(SOURCEDIR)/%)
SOURCEOBJS      = $(SOURCEFILES:%.c=$(DISTDIR)/%.o)
TESTOBJS        = $(TESTFILES:%.c=%.o)
SOURCEDEPS      = $(SOURCEFILES:%.c=$(DISTDIR)/%.d)
TESTDEPS        = $(TESTFILES:%.c=%.d)

LIBNAME         = lib$(BASENAME).so.0
OUTPUT          = $(DISTDIR)/$(LIBNAME)
TESTOUTPUT      = $(DISTDIR)/$(BASENAME)-test

# Compiler
CC              ?= clang
BASECFLAGS      = -std=c99 -O2 -pthread \
                  -fmessage-length=0 -Wall -Wextra -pedantic \
                  -DDEBUG -D_POSIX_SOURCE -D_GNU_SOURCE -g
CFLAGS          += $(BASECFLAGS) -fPIC
LDFLAGS         += -flto -pthread -shared
TESTCFLAGS      += $(BASECFLAGS)
TESTLDFLAGS     += -L$(DISTDIR) -l:$(LIBNAME)

# Default Targets
all: $(OUTPUT)

clean:
	$(RM) $(SOURCEDEPS) $(SOURCEOBJS)
	$(RM) $(TESTDEPS) $(TESTOBJS)
	$(RM) $(OUTPUT) $(DISTDIR)

test: $(TESTOUTPUT)
	LD_LIBRARY_PATH=$(DISTDIR) $(TESTOUTPUT)

gdb: $(TESTOUTPUT)
	LD_LIBRARY_PATH=$(DISTDIR) $(DEBUGGER) $(TESTOUTPUT)

valgrind: $(TESTOUTPUT)
	LD_LIBRARY_PATH=$(DISTDIR) $(MEMCHECKER) $(TESTOUTPUT)

# Targets
-include $(SOURCEDEPS)
-include $(TESTDEPS)

$(OUTPUT): $(SOURCEOBJS) Makefile
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o$@ $(SOURCEOBJS)

$(DISTDIR)/%.o: $(SOURCEDIR)/%.c Makefile
	@$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

$(TESTOUTPUT): $(TESTOBJS) $(OUTPUT) Makefile
	@$(MKDIR) $(dir $@)
	$(CC) $(TESTLDFLAGS) -o$@ $(TESTOBJS)

$(TESTDIR)/%.o: $(TESTDIR)/%.c Makefile
	@$(MKDIR) $(dir $@)
	$(CC) -c $(TESTCFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

# Install
install: $(LIBDIR)/$(LIBNAME) $(foreach h, $(HEADERFILES), $(INCLUDEDIR)/$h)

$(LIBDIR)/%: $(DISTDIR)/%
	$(INSTALL) -m755 -D $< $@

$(INCLUDEDIR)/%: $(SOURCEDIR)/%
	$(INSTALL) -m644 -D $< $@

# Phony
.PHONY: all clean test
