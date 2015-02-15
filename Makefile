# Utilities
RM              = rm -rf
MKDIR           = mkdir -p
EXEC            = exec
INSTALL         = install

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
HEADERFILES     = address.h connreq.h connres.h \
                  connstatereq.h connstateres.h dcreq.h \
                  dcres.h header.h \
                  hostinfo.h knx.h msgbuilder.h \
                  outqueue.h pkgqueue.h tunnelreq.h \
                  tunnelres.h
HEADEROBJS      = $(HEADERFILES:%=$(SOURCEDIR)/%)
SOURCEFILES     = connreq.c connres.c connstatereq.c connstateres.c \
                  dcreq.c dcres.c dgramsock.c header.c \
                  hostinfo.c knx.c log.c msgbuilder.c \
                  outqueue.c pkgqueue.c tunnelclient.c \
                  tunnelreq.c tunnelres.c
SOURCEOBJS      = $(SOURCEFILES:%.c=$(DISTDIR)/%.o)
SOURCEDEPS      = $(SOURCEFILES:%.c=$(DISTDIR)/%.d)
TESTFILES       = $(wildcard $(TESTDIR)/*.c)
TESTOBJS        = $(TESTFILES:%.c=%.o)
TESTDEPS        = $(TESTFILES:%.c=%.d)
LIBNAME         = lib$(BASENAME).so.0
OUTPUT          = $(DISTDIR)/$(LIBNAME)
TESTOUTPUT      = $(DISTDIR)/$(BASENAME)-test

# Compiler
CC              ?= clang
BASECFLAGS      = -std=c99 -O2 -pthread \
                  -fmessage-length=0 -Wall -Wextra -pedantic \
                  -DDEBUG -D_POSIX_SOURCE
CFLAGS          += $(BASECFLAGS) -fPIC
LDFLAGS         += -flto -pthread -shared
TESTCFLAGS      += $(BASECFLAGS)
TESTLDFLAGS     += -L$(DISTDIR) -l:$(LIBNAME)

# Default Targets
all: $(OUTPUT)

clean:
	$(RM) $(SOURCEDEPS) $(SOURCEOBJS) $(OUTPUT) $(DISTDIR)

test: $(TESTOUTPUT)
	LD_LIBRARY_PATH=$(DISTDIR) $(TESTOUTPUT)

install: $(OUTPUT)
	$(INSTALL) -m755 -d $(INCLUDEDIR) $(LIBDIR)
	$(INSTALL) -m755 $(OUTPUT) $(LIBDIR)
	$(INSTALL) -m644 $(HEADEROBJS) $(INCLUDEDIR)

# Targets
-include $(SOURCEDEPS)
-include $(TESTDEPS)

$(OUTPUT): $(SOURCEOBJS)
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o$@ $(SOURCEOBJS)

$(DISTDIR)/%.o: $(SOURCEDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

$(TESTOUTPUT): $(TESTOBJS) $(OUTPUT)
	@$(MKDIR) $(dir $@)
	$(CC) $(TESTLDFLAGS) -o$@ $(TESTOBJS)

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) -c $(TESTCFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<
# Phony
.PHONY: all clean test
