AS=yasm
SRC=addasm.s add.c hello.s
OBJ=addasm.o add.o hello.o
BIN=add hello

all: add hello

.SUFFIXES: .s
.s.o :
	$(AS) -f elf64 $< -o $@

hello: hello.o
	$(LD) $< -o $@

add: add.c addasm.o
	clang $? -o $@

.PHONY: clean
clean:
	rm $(OBJ) $(BIN)

