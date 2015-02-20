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
HEADERFILES     = util/address.h util/sockutils.h \
                  proto/connreq.h proto/connres.h proto/connstatereq.h proto/connstateres.h proto/dcreq.h proto/dcres.h proto/header.h proto/hostinfo.h proto/knxnetip.h proto/tunnelreq.h proto/tunnelres.h proto/routingind.h \
                  net/tunnelclient.h net/routerclient.h
SOURCEFILES     = proto/connstateres.c proto/connreq.c proto/tunnelreq.c proto/connstatereq.c proto/connres.c proto/dcreq.c proto/hostinfo.c proto/knxnetip.c proto/header.c proto/tunnelres.c proto/dcres.c proto/routingind.c \
                  util/sockutils.c util/log.c \
                  net/tunnelclient.c net/routerclient.c

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

install: $(OUTPUT)
	$(INSTALL) -m755 -d $(INCLUDEDIR) $(LIBDIR)
	$(INSTALL) -m755 $(OUTPUT) $(LIBDIR)
	$(INSTALL) -m644 $(HEADEROBJS) $(INCLUDEDIR)

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
# Phony
.PHONY: all clean test
