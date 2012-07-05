CFLAGS=`pkg-config --cflags gtk+-3.0` -g
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
BIN=gtk-background

$(BIN): $(OBJS)
	gcc -o $(BIN) $(OBJS) `pkg-config --libs gtk+-3.0` -lm -lX11 -g

%.o: %.c %.h

clean:
	rm -f $(BIN) $(OBJS)
