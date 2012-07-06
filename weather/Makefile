CFLAGS=`pkg-config --cflags gtk+-3.0 libsoup-2.4` -g
BIN=weather.la

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

$(BIN): $(OBJS)
	ar rcs $(BIN) $(OBJS)

%.o: %.c %.h

clean:
	rm -f $(BIN) $(OBJS)
