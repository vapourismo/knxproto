# Utilities
RM              = rm -rf
MKDIR           = mkdir -p
EXEC            = exec
INSTALL         = install

# Install Directories
PREFIX          ?= /usr/local
INCLUDEDIR      ?= $(PREFIX)/include/knxclient
LIBDIR          ?= $(PREFIX)/lib

# Local Directories
DISTDIR         = dist
SOURCEDIR       = src

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
LIBNAME         = libknxclient.so.0
OUTPUT          = $(DISTDIR)/$(LIBNAME)

# Compiler
CC              ?= clang
CFLAGS          += -std=c99 -O2 -fPIC -pthread \
                   -fmessage-length=0 -Wall -Wextra -pedantic \
                   -DDEBUG -D_POSIX_SOURCE
LDFLAGS         += -flto -pthread -shared

# Default Targets
all test: $(OUTPUT)

clean:
	$(RM) $(SOURCEDEPS) $(SOURCEOBJS) $(OUTPUT) $(DISTDIR)

install: $(OUTPUT)
	$(INSTALL) -m755 -d $(INCLUDEDIR) $(LIBDIR)
	$(INSTALL) -m755 $(OUTPUT) $(LIBDIR)
	$(INSTALL) -m644 $(HEADEROBJS) $(INCLUDEDIR)

# Targets
-include $(SOURCEDEPS)

$(OUTPUT): $(SOURCEOBJS)
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o$@ $(SOURCEOBJS)

$(DISTDIR)/%.o: $(SOURCEDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

# Phony
.PHONY: all clean test
