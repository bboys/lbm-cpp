CC := g++
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR    := src
SPECDIR   := specs
BUILDDIR  := build
TARGETDIR := bin
TARGET    := main

# Compile all .cpp files in the src directory and put the object files in the build dir
SRCEXT  := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT) -not -wholename src/main.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
SPECS   := $(shell find $(SPECDIR) -type f -name *.$(SRCEXT))
SPEC_OBJECTS := $(patsubst $(SPECDIR)/%,$(BUILDDIR)/$(SPECDIR)/%,$(SPECS:.$(SRCEXT)=.o))
# SPECS   := $(shell find $(SPECDIR) $(SRCDIR) -type f -name *.$(SRCEXT) -not -wholename src/main.$(SRCEXT))
# SPEC_OBJECTS := $(patsubst $(SPECDIR)/%,$(BUILDDIR)/$(SPECS)/%,$(SPECS:.$(SRCEXT)=.o))
# SPEC_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SPEC_OBJECTS:.$(SRCEXT)=.o))
CFLAGS  := -std=c++11 -g -MMD
LIB     := -L lib/MulticoreBSP-for-C/lib -l mcbsp1.2.0 -l pthread
INC     := -I include

all: lbm

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGETDIR)/$(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/specs/%.o: $(SPECDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -I lib/bandit -c -o $@ $<

lbm: $(OBJECTS) build/main.o
	$(CC) $^ -o $(TARGETDIR)/$(TARGET) $(LIB)

# Tests
spec: $(SPEC_OBJECTS) $(OBJECTS)
	@echo "Linking spec runner..."
	$(CC) $(CFLAGS) $^ -o $(TARGETDIR)/spec $(LIB)

clean:
	@echo "Cleaning...";
	$(RM) -r $(BUILDDIR) $(TARGETDIR)/$(TARGET)

.PHONY: clean