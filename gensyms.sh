#!/bin/sh

echo "/*\n    Auto-generated by gensyms.sh." > syms.gen
echo "    DO NOT EDIT\n*/\n\n#include \"src/symbols.h\"\n" >> syms.gen
echo "const symbol __kernel_symtab[] = {" >> syms.gen
nm $1 | grep -i " t " | awk '{ print "    { .addr = 0x"$1", .name = \""$3"\" }," }' | sort >> syms.gen
echo "    { .addr = 0xffffffffffffffff, .name = \"\" }\n};" >> syms.gen