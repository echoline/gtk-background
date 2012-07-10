CFLAGS=`pkg-config --cflags gtk+-3.0` -g
LDFLAGS=`pkg-config --libs gtk+-3.0 libsoup-2.4 vte-2.90` -lm -lX11 -g
BIN=gtk-background
DIRS=weather

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CLEANDIRS=$(DIRS:%=clean-%)

.PHONY: subdirs $(DIRS)

$(BIN): $(OBJS) weather
	gcc -o $(BIN) $(OBJS) weather/weather.la $(LDFLAGS)

%.o: %.c %.h

subdirs: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

clean: $(CLEANDIRS)
	rm -f $(BIN) $(OBJS)

launcher.o: launcher.c launcher.h
	gcc -c -g launcher.c $(CFLAGS) `pkg-config --cflags vte-2.90`

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean
