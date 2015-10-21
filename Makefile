CC := g++
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR    := src
BUILDDIR  := build
TARGETDIR := bin
TARGET    := main

# Compile all .cpp files in the src directory and put the object files in the build dir
SRCEXT  := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS  := -std=c++11 -Wall -g -MMD
LIB     := -L lib/MulticoreBSP-for-C/lib -l mcbsp1.2.0 -l pthread
INC     := -I include

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $^ -o $(TARGETDIR)/$(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning...";
	$(RM) -r $(BUILDDIR) $(TARGETDIR)/$(TARGET)

# TODO: add tests
# # Tests
# tester:
# 	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# # Spikes
# ticket:
# 	$(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean