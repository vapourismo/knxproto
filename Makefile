# Utilities
RM              = rm -rf
MKDIR           = mkdir -p
LINK            = ln -sf
EXEC            = exec
INSTALL         = install
DEBUGGER        = gdb -ex run
MEMCHECKER      = valgrind -v --leak-check=full

# Install Directories
BASENAME        := knxproto
PREFIX          ?= /usr/local
INCLUDEDIR      ?= $(PREFIX)/include/$(BASENAME)
LIBDIR          ?= $(PREFIX)/lib

# Local Directories
DISTDIR         = dist
SOURCEDIR       = src
TESTDIR         = test

# Artifacts
HEADERFILES     = proto/connreq.h proto/connres.h proto/connstatereq.h proto/connstateres.h \
                  proto/dcreq.h proto/dcres.h proto/hostinfo.h proto/proto.h proto/tunnelreq.h \
                  proto/tunnelres.h proto/routingind.h proto/descreq.h proto/cemi.h proto/ldata.h \
                  proto/tpdu.h proto/data.h proto/descres.h util/address.h
SOURCEFILES     = proto/connstateres.c proto/connreq.c proto/tunnelreq.c proto/connstatereq.c \
                  proto/connres.c proto/dcreq.c proto/hostinfo.c proto/proto.c proto/tunnelres.c \
                  proto/dcres.c proto/routingind.c proto/descreq.c proto/cemi.c proto/ldata.c \
                  proto/tpdu.c proto/data.c proto/descres.c

TESTFILES       = $(wildcard $(TESTDIR)/*.c)
HEADEROBJS      = $(HEADERFILES:%=$(SOURCEDIR)/%)
SOURCEOBJS      = $(SOURCEFILES:%.c=$(DISTDIR)/%.o)
TESTOBJS        = $(TESTFILES:%.c=%.o)
SOURCEDEPS      = $(SOURCEFILES:%.c=$(DISTDIR)/%.d)
TESTDEPS        = $(TESTFILES:%.c=%.d)

SOVERSION       = 1
SOBASE          = lib$(BASENAME).so
SONAME          = $(SOBASE).$(SOVERSION)

SOOUTPUT        = $(DISTDIR)/$(SONAME)
TESTOUTPUT      = $(DISTDIR)/$(BASENAME)-test

# On Debug
ifeq ($(DEBUG), 1)
	DEBUGCFLAGS = -O0 -DDEBUG -g
endif

# Compiler
CC              ?= clang
BASECFLAGS      := -std=c99 -O2 -D_POSIX_SOURCE -D_GNU_SOURCE $(DEBUGCFLAGS) \
                   -fmessage-length=0 -Wall -Wextra -pedantic -Wno-unused-parameter
CFLAGS          += $(BASECFLAGS) -fPIC
LDFLAGS         += -shared -Wl,-soname,$(SONAME)
LDLIBS          := -lm

TESTCFLAGS      = $(BASECFLAGS)
TESTLDFLAGS     =

ifeq ($(LTO), 1)
	TESTLDFLAGS += -flto
	LDFLAGS += -flto
endif

# Default Targets
all: $(SOOUTPUT)

clean:
	$(RM) $(SOURCEDEPS) $(SOURCEOBJS)
	$(RM) $(TESTDEPS) $(TESTOBJS)
	$(RM) $(SOOUTPUT) $(DISTDIR)

test: $(TESTOUTPUT)
	$(EXEC) $(TESTOUTPUT)

gdb: $(TESTOUTPUT)
	$(DEBUGGER) $(TESTOUTPUT)

valgrind: $(TESTOUTPUT)
	$(MEMCHECKER) $(TESTOUTPUT)

docs:
	doxygen

gh-pages: docs
	git add docs
	git commit -m "docs: $$(date)"
	git subtree push --prefix docs/html origin gh-pages

# Targets
-include $(SOURCEDEPS)
-include $(TESTDEPS)

# Shared Object
$(SOOUTPUT): $(SOURCEOBJS) Makefile
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o$@ $(SOURCEOBJS) $(LDLIBS)

$(DISTDIR)/%.o: $(SOURCEDIR)/%.c Makefile
	@$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

# Test
$(TESTOUTPUT): $(TESTOBJS) $(SOURCEOBJS) Makefile
	@$(MKDIR) $(dir $@)
	$(CC) $(TESTLDFLAGS) -o$@ $(TESTOBJS) $(SOURCEOBJS) $(LDLIBS)

$(TESTDIR)/%.o: $(TESTDIR)/%.c Makefile
	@$(MKDIR) $(dir $@)
	$(CC) -c $(TESTCFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

# Install
install: $(LIBDIR)/$(SOBASE) $(LIBDIR)/$(SONAME) $(foreach h, $(HEADERFILES), $(INCLUDEDIR)/$h)

uninstall:
	$(RM) $(LIBDIR)/$(SOBASE) $(LIBDIR)/$(SONAME) $(foreach h, $(HEADERFILES), $(INCLUDEDIR)/$h)

$(LIBDIR)/$(SOBASE): $(LIBDIR)/$(SONAME)
	$(LINK) $(SONAME) $@

$(LIBDIR)/%: $(DISTDIR)/%
	$(INSTALL) -m755 -D $< $@

$(INCLUDEDIR)/%: $(SOURCEDIR)/%
	$(INSTALL) -m644 -D $< $@

# Phony
.PHONY: all clean test install docs
