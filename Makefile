# Compiler Options
CXX := g++
CXXFLAGS := -g -std=c++20 -ltgui -lsfml-graphics -lsfml-window -lsfml-system 
# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(filter-out $(SRCDIR)/test_%.cpp,$(SRCS)))

# OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Binary name
TARGET = $(BINDIR)/tetris

all: $(TARGET) $(TESTTARGET)

$(TARGET): $(OBJS) $(SHDRS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./bin/tetris

clean:
	rm -rf $(OBJDIR) $(BINDIR)

