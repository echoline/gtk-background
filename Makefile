CFLAGS=`pkg-config --cflags gtk+-3.0` -g
LDFLAGS=`pkg-config --libs gtk+-3.0 libsoup-2.4` -lm -lX11 -g
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

$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean
