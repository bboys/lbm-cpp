CXX = g++
# CXXFLAGS = -Wall -std=c++11 -g
# with -MMD creates .d files
CXXFLAGS = -Wall -std=c++11 -MMD -g
CXXINCL =
LINKER = $(CXX)
LDFLAGS =
LDLIBS =

TARGET = main
SOURCES = main.cc $(wildcard **/*.cc)
OBJECTS = $(SOURCES:.cc=.o)

TEST_TARGET = test
TEST_SOURCES = test.cc $(wildcard **/*.cc)
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)

DEPS = $(OBJECTS:.o=.d)
ODIR = ./objects

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	$(LINKER) $(LDFLAGS) $(TEST_OBJECTS) $(LDLIBS) -o $@

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $(CXXINCL) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

-include $(DEPS)