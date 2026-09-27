CC = gcc

SRC = src
INCLUDE = include
OBJDIR = build

CFLAGS = -Wall -Wextra -g -I$(INCLUDE)

BIN_PPMHIST = ppmhist
BIN_QUANTIF = quantif

OBJ_CORE = $(OBJDIR)/image.o $(OBJDIR)/histogram.o

OBJ_PPMHIST = $(OBJDIR)/ppmhist.o

OBJ_QUANTIF = $(OBJDIR)/quantification.o $(OBJDIR)/mapping.o $(OBJDIR)/main.o

.PHONY: all clean
all: $(BIN_PPMHIST) $(BIN_QUANTIF)

$(BIN_PPMHIST): $(OBJ_CORE) $(OBJ_PPMHIST)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_QUANTIF): $(OBJ_CORE) $(OBJ_QUANTIF)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRC)/%.c $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BIN_PPMHIST) $(BIN_QUANTIF)
