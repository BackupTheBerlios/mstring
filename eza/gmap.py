#!/usr/bin/env python
#
# (c) Copyright MString core development team <http://mstring.berlios.de>
# (c) Copyright Tirra <tirra.newly@gmail.com> 
#
# gmap.py: generates binary map for linking muikernel
# it's idea was in my brains - but I don't know 
# it's my own idea or not ... 
#

import sys
import struct
import re

MAXSTRING=63
symtabfmt="<Q%ds" % (MAXSTRING+1)

funcline=re.compile(r'([0-9a-f]+)\s+[lg]\s+.\s+\.text\s+([0-9a-f]+)\s+(.*)$')
bssline=re.compile(r'([0-9a-f]+)\s+[lg]\s+[a-zA-Z]\s+\.bss\s+([0-9a-f]+)\s+(.*)$')
dataline=re.compile(r'([0-9a-f]+)\s+[lg]\s+[a-zA-Z]\s+\.data\s+([0-9a-f]+)\s+(.*)$')
fileexp=re.compile(r'([^\s]+):\s+file format')

def read_objdump(inp):
    funcs={}
    data={}
    bss={}
    fname=''
    for line in inp:
        line=line.strip()
        res=funcline.match(line)
        if res:
            funcs.setdefault(fname,[]).append((int(res.group(1),16),res.group(3)))
            continue
        res=bssline.match(line)
        if res:
            start=int(res.group(1),16)
            end=int(res.group(2),16)
            if end:
                bss.setdefault(fname,[]).append((start,res.group(3)))
        res=dataline.match(line)
        if res:
            start=int(res.group(1),16)
            end=int(res.group(2),16)
            if end:
                data.setdefault(fname,[]).append((start,res.group(3)))
        res=fileexp.match(line)
        if res:
            fname=res.group(1)
            continue
    
    return {
        'text':funcs,
        'bss':bss,
        'data':data
        }
        
startfile=re.compile(r'\.(text|bss|data)\s+(0x[0-9a-f]+)\s+0x[0-9a-f]+\s+(.*)$')

def generator(kmapf,objmapf,out):
    objdump=read_objdump(objmapf)

    def sorter(x,y):
        return cmp(x[0],y[0])
    
    for line in kmapf:
        line=line.strip()
        res=startfile.match(line)

        if res and objdump[res.group(1)].has_key(res.group(3)):
            offset=int(res.group(2),16)
            fname=res.group(3)
            symbols=objdump[res.group(1)][fname]
            symbols.sort(sorter)
            for addr,symbol in symbols:
                value=fname+':'+symbol
                data=struct.pack(symtabfmt,addr+offset,value[:MAXSTRING])
                out.write(data)

    out.write(struct.pack(symtabfmt,0,''))

def main():

    if len(sys.argv)!=4:
        print "Using: %s <mui map> <name map> <generated map>" % sys.argv[0]
        sys.exit(1)

    kmapf=open(sys.argv[1],'r')
    objmapf=open(sys.argv[2],'r')
    out=open(sys.argv[3],'w')

    generator(kmapf,objmapf,out)

    kmapf.close()
    objmapf.close()
    out.close()

if __name__=='__main__':
    main()

