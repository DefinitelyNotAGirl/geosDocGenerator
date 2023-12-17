/*
 * Created Date: Friday June 30th 2023
 * Author: Lilith
 * -----
 * Last Modified: Tuesday July 18th 2023 4:48:24 pm
 * Modified By: Lilith (definitelynotagirl115169@gmail.com)
 * -----
 * Copyright (c) 2023 DefinitelyNotAGirl@github
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <libCargparse.hxx>

#include <master.h>

static std::string input;

void cargHandler_output(CARGPARSE_HANDLER_ARGS)
{
    if(args.front().back() == '/')
    {
        std::cerr << "the output file name must not end with a '/' character" << std::endl;
        exit(-1);
    }
    output = args.front();
}

void cargHandler_MD(CARGPARSE_HANDLER_ARGS)
{
    generateDepFile = true;
}

void cargHandler_unknown(CARGPARSE_UNKOWNHANDLER_ARGS)
{
    if(arg.back() == '/')
    {
        std::cerr << "the input file name must not end with a '/' character" << std::endl;
        exit(-1);
    }
    input = arg;
}

int main(int argc, char** argv)
{
    cargparse::instance carg;

    carg.unknownHandler = &cargHandler_unknown;
    carg.addParameter(false,1,"-o",&cargHandler_output);
    carg.addParameter(false,0,"-MD",&cargHandler_MD);

    carg.parse(argc,argv);

    if(input.empty())
    {
        std::cerr << "ERROR: no input file specified" << std::endl;
        exit(-1);
    }

    FILE* f = fopen(input.c_str(), "rb");
    if(f == NULL)
    {
        std::cerr << "ERROR: could not open input file " << input << std::endl;
        exit(-1);
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char* content = malloc(fsize + 1);
    fread(content, fsize, 1, f);
    fclose(f);
    content[fsize] = 0x00;//terminate content string

    interpretSource(content);

    return 0;
}