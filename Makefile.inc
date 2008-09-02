
-include config

CC = /usr/bin/gcc  
MAKE = /usr/bin/make -s
#LD = /home/tirra/works/Jari/m-spring/binutils-2.18/ld/ld-new
LD = /usr/bin/ld
OBJDUMP = /usr/bin/objdump
OBJCOPY = /usr/bin/objcopy
AS = /usr/bin/as
ASFLAGS = --64
CFLAGS = -Wall -nostdinc -fomit-frame-pointer -fno-builtin 
LDFLAGS = -M 
Q = @
GREP = /bin/grep
ECHO = /bin/echo
LN = /bin/ln
GMAP = eza/gmap.py


ATSIGN ?= @

SYMTAB_SECTION=".section symtab.data, \"a\", $(ATSIGN)progbits;"
