CFLAGS=`pkg-config --cflags gtk+-3.0` -g
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
BIN=gtk-background

$(BIN): $(OBJS)
	gcc -o $(BIN) $(OBJS) `pkg-config --libs gtk+-3.0 gweather-3.0` -lm -lX11 -g

%.o: %.c %.h

weather.o: weather.c weather.h
	gcc -g -c -o weather.o weather.c $(CFLAGS) `pkg-config --cflags gweather-3.0`

clean:
	rm -f $(BIN) $(OBJS)
