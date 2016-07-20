
.PHONY: all clean install uninstall

all: backlightctl

clean:
	-rm backlightctl.o
	-rm backlightctl

backlightctl: CFLAGS += -ggdb3
backlightctl: backlightctl.o

install: backlightctl
	strip backlightctl
	cp backlightctl /usr/bin/backlightctl
	chown root:root /usr/bin/backlightctl
	chmod 4755 /usr/bin/backlightctl

uninstall:
	-rm /usr/bin/backlightctl

