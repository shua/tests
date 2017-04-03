YASM=yasm
SRC=boot.s phex.s

boot: $(SRC)
	$(YASM) -f bin -o $@ $<
