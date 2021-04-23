include config.mk

SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SRC))
EXEC=ft

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS += -g
debug: ft

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

.PHONY: install
install: $(EXEC)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(EXEC) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(EXEC)

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(EXEC)

.PHONY: clean
clean:
	rm -f $(OBJ) $(EXEC)
