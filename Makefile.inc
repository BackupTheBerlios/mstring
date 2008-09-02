
-include config

CC = /usr/bin/gcc 
MAKE = /usr/bin/make -s
LD = /opt/x86-64/bin/x86_64-unknown-linux-ld
OBJDUMP = /opt/x86-64/bin/x86_64-unknown-linux-objdump
OBJCOPY = /usr/bin/objcopy
AS = /opt/x86-64/bin/x86_64-unknown-linux-as
ASFLAGS = 
CFLAGS = -Wall -nostdlib -nostdinc -fno-builtin -fomit-frame-pointer
LDFLAGS = -M
Q = @
GREP = /bin/grep
ECHO = /bin/echo
LN = /bin/ln
GMAP = eza/gmap.py


ATSIGN ?= @

SYMTAB_SECTION=".section symtab.data, \"a\", $(ATSIGN)progbits;"
