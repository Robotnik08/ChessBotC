# Compiler setup
CC = gcc
CFLAGS = -Iinclude -Werror -Wno-format -fPIC

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build
TEMPDIR = temp
TARGET = $(BUILDDIR)/main.exe
DLL = chesslib.dll

# Source and object files
SOURCES := $(wildcard $(SRCDIR)/*.c) main.c
OBJECTS := $(patsubst %.c,$(TEMPDIR)/%.o,$(notdir $(SOURCES)))

# Linker flags
LIBS = -L. -lchesslib

# Main target
all: $(BUILDDIR) $(TEMPDIR) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(addprefix $(TEMPDIR)/, $(notdir $(OBJECTS))) $(LIBS) -lm
ifeq ($(OS),Windows_NT)
	copy /Y $(DLL) $(BUILDDIR)\$(DLL)
else
	cp $(DLL) $(BUILDDIR)/$(DLL)
endif

$(TEMPDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEMPDIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(BUILDDIR):
ifeq ($(OS),Windows_NT)
	if not exist $(BUILDDIR) mkdir $(BUILDDIR)
else
	mkdir -p $(BUILDDIR)
endif

$(TEMPDIR):
ifeq ($(OS),Windows_NT)
	if not exist $(TEMPDIR) mkdir $(TEMPDIR)
else
	mkdir -p $(TEMPDIR)
endif

# Clean generated files
clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q $(TEMPDIR)
	rmdir /s /q $(BUILDDIR)
else
	rm -rf $(TEMPDIR) $(BUILDDIR)
endif

# Run the built program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
