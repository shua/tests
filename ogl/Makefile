SRC=glxtest.c
LDFLAGS=-lX11 -lGL -lGLU

.PHONY: run
run: glxtest
	./$<
	
glxtest: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

	
