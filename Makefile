# Utilities
RM              = rm -rf
MKDIR           = mkdir -p
EXEC            = exec

# Directories
DISTPATH        = dist
SRCPATH         = src

# Source Artifacts
SRCFILES        = $(shell find $(SRCPATH) -iname "*.c")
SRCOBJS         = $(SRCFILES:$(SRCPATH)/%.c=$(DISTPATH)/%.o)
SRCDEPS         = $(SRCFILES:$(SRCPATH)/%.c=$(DISTPATH)/%.d)

# Compiler
CC              = clang
CFLAGS          = -std=c11 -fmessage-length=0 -Wall -Wextra -pedantic -O2
LDFLAGS         = -flto

# Executable
EXENAME         = hpknxd
OUTPUT          = $(DISTPATH)/$(EXENAME)

# Default Targets
all: $(OUTPUT)

clean:
	$(RM) $(SRCDEPS) $(SRCOBJS)

test: $(OUTPUT)
	$(EXEC) $(OUTPUT)

# File Targets
-include $(SRCDEPS)

$(OUTPUT): $(SRCOBJS)
	@$(MKDIR) $(dir $@)
	$(CC) $(LDFLAGS) -o$@ $(SRCOBJS)

$(DISTPATH)/%.o: $(SRCPATH)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -MF$(@:%.o=%.d) -MT$@ -o$@ $<

# Phony
.PHONY: all clean test
