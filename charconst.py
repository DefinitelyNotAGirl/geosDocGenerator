#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Created Date: Saturday July 1st 2023
# Author: Lilith
# -----
# Last Modified: Saturday July 1st 2023 4:44:56 am
# Modified By: Lilith (definitelynotagirl115169@gmail.com)
# -----
# Copyright (c) 2023 DefinitelyNotAGirl@github
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
###

DATA = [
    #["directive_related","related"],
    #["directive_out","out"],
    #["directive_embed","embed"],
    #["directive_set","set"],
    #["directive_subcat","subcat"],
    #["directive_var","var"],
    #["directive_const","const"],
    #["directive_error","err","error","exit","fatal"],
    #["directive_info","msg","print","info","log","message"],
    #["directive_symbol","sym"],
    #["directive_arith2","add","sub","mul","div","mod","mov"],
    #["directive_arith1","inc","dec"],
    #["directive_cmp","cmp"],
    #["directive_jmp","jg","jge","jl","jle","je","jne","jnz","jz","jmp","jump","goto"],
    #["directive_title","title"]
    ["directive_link","link"]
]

#format
#   function
#   case
UINT32 = []
#format
#   function
#   case
#   bits to mask off
UINT64 = []
UINT64_FINAL = [[],[],[],[]]

def genCharConst(data,function):
    if(len(data) > 8):
        print("ERROR: \"",I,"\" is too long!")
    else:
        if(len(data) < 5):
            UINT32.append([function,"'"+str(data[::-1])+"'"])
        else:
            UINT64.append([function,"cat('"+str(data[4:len(data)][::-1])+"','"+str(data[0:4][::-1])+"')",8-len(data)])

for I in DATA:
    for II in I:
        if II == I[0]:
            continue
        genCharConst(II+" ",I[0])
        genCharConst(II+"\t",I[0])

# 64 bit masking shenanigans

for I in UINT64:
    UINT64_FINAL[I[2]].append([I[0],I[1]])

#print functions
for I in DATA:
    print("static inline void "+I[0]+"(char** source)\n{\n}\n\n")

# out
print("source++;")

# 32 bit out
print("switch(((uint32_t*)(source))[0])\n{")
for I in UINT32:
    print("REPLACEWITHTABcase("+I[1]+"):")
    print("REPLACEWITHTABREPLACEWITHTAB"+I[0]+"(&source);")
    print("REPLACEWITHTABREPLACEWITHTABgoto skipSwitch2;")
print("}")

# 64 bit out
print("switch(((uint64_t*)(source))[0])\n{")
for I in UINT64_FINAL[0]:
    print("REPLACEWITHTABcase("+I[1]+"):")
    print("REPLACEWITHTABREPLACEWITHTAB"+I[0]+"(&source);")
    print("REPLACEWITHTABREPLACEWITHTABgoto skipSwitch2;")
print("}")

print("switch(((uint64_t*)(source))[0] & 0x00FFFFFFFFFFFFFF)\n{")
for I in UINT64_FINAL[1]:
    print("REPLACEWITHTABcase("+I[1]+"):")
    print("REPLACEWITHTABREPLACEWITHTAB"+I[0]+"(&source);")
    print("REPLACEWITHTABREPLACEWITHTABgoto skipSwitch2;")
print("}")

print("switch(((uint64_t*)(source))[0] & 0x0000FFFFFFFFFFFF)\n{")
for I in UINT64_FINAL[2]:
    print("REPLACEWITHTABcase("+I[1]+"):")
    print("REPLACEWITHTABREPLACEWITHTAB"+I[0]+"(&source);")
    print("REPLACEWITHTABREPLACEWITHTABgoto skipSwitch2;")
print("}")


print("switch(((uint64_t*)(source))[0] & 0x000000FFFFFFFFFF)\n{")
for I in UINT64_FINAL[3]:
    print("REPLACEWITHTABcase("+I[1]+"):")
    print("REPLACEWITHTABREPLACEWITHTAB"+I[0]+"(&source);")
    print("REPLACEWITHTABREPLACEWITHTABgoto skipSwitch2;")
print("}")
