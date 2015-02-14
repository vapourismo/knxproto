# Utilities
RM              = rm -rf
MKDIR           = mkdir -p
EXEC            = exec

# Directories
BUILDDIR        = dist
SRCDIR          = src
DESTDIR         ?= /usr/local

# Configuration
SONAME          = libknxclient.so.0
OUTPUT          = $(BUILDDIR)/$(SONAME)

# Source Artifacts
SRCFILES        = $(shell find $(SRCDIR) -iname "*.c")
SRCOBJS         = $(SRCFILES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
SRCDEPS         = $(SRCFILES:$(SRCDIR)/%.c=$(BUILDDIR)/%.d)

# Compiler
CC              ?= clang
CFLAGS          += -std=c99 -fmessage-length=0 -Wall -Wextra -pedantic -O2 -fPIC -pthread -DDEBUG -D_POSIX_SOURCE
LDFLAGS         += -flto -pthread -shared

# Default Targets
all test: $(OUTPUT)

clean:
	$(RM) $(SRCDEPS) $(SRCOBJS) $(OUTPUT)

# File Targets
-include $(SRCDEPS)

$(OUTPUT): $(SRCOBJS)
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o$@ $(SRCOBJS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

# Phony
.PHONY: all clean test
