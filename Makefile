CFLAGS=`pkg-config --cflags gtk+-3.0` -g
LDFLAGS=`pkg-config --libs gtk+-3.0` -lm -lX11 -g
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
BIN=gtk-background

$(BIN): $(OBJS)
	gcc -o $(BIN) $(OBJS) $(LDFLAGS)

%.o: %.c %.h

clean:
	rm -f $(BIN) $(OBJS)
