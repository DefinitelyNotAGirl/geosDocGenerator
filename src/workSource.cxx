/*
 * Created Date: Saturday July 1st 2023
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

#include <master.h>
#include <iostream>
#include <dict.hxx>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <gdml.hxx>
#include <algorithm> 
#include <cctype>
#include <locale>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void strip(std::string &s) {
    rtrim(s);
    ltrim(s);
}

struct DOC_TEXT
{
    uint64_t size;
};

struct DOCUMENT
{
    DOC_TEXT text;
};

static DOCUMENT doc;

static void* GlobalData = malloc(sizeof(GDML_HEADER));
static void* GlobalCode = nullptr;

static uint64_t codeSize = 0;
static uint64_t dataSize = sizeof(GDML_HEADER);

static uint64_t dataAddr = sizeof(GDML_HEADER);

static const char* HEXDATA[256] = {"0x00","0x01","0x02","0x03","0x04","0x05","0x06","0x07","0x08","0x09","0x0a","0x0b","0x0c","0x0d","0x0e","0x0f","0x10","0x11","0x12","0x13","0x14","0x15","0x16","0x17","0x18","0x19","0x1a","0x1b","0x1c","0x1d","0x1e","0x1f","0x20","0x21","0x22","0x23","0x24","0x25","0x26","0x27","0x28","0x29","0x2a","0x2b","0x2c","0x2d","0x2e","0x2f","0x30","0x31","0x32","0x33","0x34","0x35","0x36","0x37","0x38","0x39","0x3a","0x3b","0x3c","0x3d","0x3e","0x3f","0x40","0x41","0x42","0x43","0x44","0x45","0x46","0x47","0x48","0x49","0x4a","0x4b","0x4c","0x4d","0x4e","0x4f","0x50","0x51","0x52","0x53","0x54","0x55","0x56","0x57","0x58","0x59","0x5a","0x5b","0x5c","0x5d","0x5e","0x5f","0x60","0x61","0x62","0x63","0x64","0x65","0x66","0x67","0x68","0x69","0x6a","0x6b","0x6c","0x6d","0x6e","0x6f","0x70","0x71","0x72","0x73","0x74","0x75","0x76","0x77","0x78","0x79","0x7a","0x7b","0x7c","0x7d","0x7e","0x7f","0x80","0x81","0x82","0x83","0x84","0x85","0x86","0x87","0x88","0x89","0x8a","0x8b","0x8c","0x8d","0x8e","0x8f","0x90","0x91","0x92","0x93","0x94","0x95","0x96","0x97","0x98","0x99","0x9a","0x9b","0x9c","0x9d","0x9e","0x9f","0xa0","0xa1","0xa2","0xa3","0xa4","0xa5","0xa6","0xa7","0xa8","0xa9","0xaa","0xab","0xac","0xad","0xae","0xaf","0xb0","0xb1","0xb2","0xb3","0xb4","0xb5","0xb6","0xb7","0xb8","0xb9","0xba","0xbb","0xbc","0xbd","0xbe","0xbf","0xc0","0xc1","0xc2","0xc3","0xc4","0xc5","0xc6","0xc7","0xc8","0xc9","0xca","0xcb","0xcc","0xcd","0xce","0xcf","0xd0","0xd1","0xd2","0xd3","0xd4","0xd5","0xd6","0xd7","0xd8","0xd9","0xda","0xdb","0xdc","0xdd","0xde","0xdf","0xe0","0xe1","0xe2","0xe3","0xe4","0xe5","0xe6","0xe7","0xe8","0xe9","0xea","0xeb","0xec","0xed","0xee","0xef","0xf0","0xf1","0xf2","0xf3","0xf4","0xf5","0xf6","0xf7","0xf8","0xf9","0xfa","0xfb","0xfc","0xfd","0xfe","0xff"};

static inline void write8(void* addr,  uint8_t  value){*((uint8_t *)(addr)) = value;}
static inline void write16(void* addr, uint16_t value){*((uint16_t*)(addr)) = value;}
static inline void write32(void* addr, uint32_t value){*((uint32_t*)(addr)) = value;}
static inline void write64(void* addr, uint64_t value){*((uint64_t*)(addr)) = value;}

static inline void rtmx(uint64_t& value, uint64_t x)
{
    if(x == 0)return;

    uint64_t remainder = value % x;
    if(remainder == 0)return;
    value = value + x - remainder;
}

static void alignBlock(void*& src, uint64_t& srclen, uint64_t align)
{
    rtmx(srclen, align);
    src = realloc(src, srclen);
}

static void addCode(void* src, uint64_t bytes)
{
    if(bytes == 0)return;
    //std::cout << "\033[1;34m";
    //for(uint64_t i = 0; i < bytes; i++)
    //    std::cout << HEXDATA[((uint8_t*)src)[i]] << ' ';
    //std::cout <<"\033[0m"<< std::endl;
    if(GlobalCode == nullptr)
    {
        GlobalCode = malloc(bytes);
        memcpy(GlobalCode,src,bytes);
        return;
    }
    GlobalCode = realloc(GlobalCode,codeSize + bytes);
    memcpy(GlobalCode + codeSize,src,bytes);
    codeSize+=bytes;
}

static void addData(void* src, uint64_t bytes)
{
    if(bytes == 0)return;

    //16-byte align data
    alignBlock(src, bytes, 16);

    //write data
    //std::cout << "\033[1;32m";
    //for(uint64_t i = 0; i < bytes; i++)
    //    std::cout << HEXDATA[((uint8_t*)src)[i]] << ' ';
    //std::cout <<"\033[0m"<< std::endl;
    if(GlobalData == nullptr)
    {
        GlobalData = malloc(bytes);
        memcpy(GlobalData,src,bytes);
        return;
    }
    GlobalData = realloc(GlobalData,dataSize + bytes);
    memcpy(GlobalData + dataSize,src,bytes);
    dataSize+=bytes;
    dataAddr+=bytes;
}


static void addDataSTR(void* ogsrc, uint64_t bytes)
{
    if(bytes == 0)return;

    void* src = malloc(bytes);
    strcpy(src,ogsrc);

    //16-byte align data
    alignBlock(src, bytes, 16);

    //write data
    //std::cout << "\033[1;32m";
    //for(uint64_t i = 0; i < bytes; i++)
    //    std::cout << HEXDATA[((uint8_t*)src)[i]] << ' ';
    //std::cout <<"\033[0m"<< std::endl;
    if(GlobalData == nullptr)
    {
        GlobalData = malloc(bytes);
        memcpy(GlobalData,src,bytes);
        return;
    }
    GlobalData = realloc(GlobalData,dataSize + bytes);
    memcpy(GlobalData + dataSize,src,bytes);
    dataSize+=bytes;
    dataAddr+=bytes;
}


bool isWhiteSpace(char c)
{
    switch(c)
    {
        case(' '):
        case('\t'):
        case('\n'):
            return true;
        default:
            return false;
    }
    return false;
}

static char correctWhiteSpace(char c)
{
    switch(c)
    {
        case('\n'):
            return c;
        case('\t'):
            return c;
    }
    return ' ';
}

static std::vector<std::string> whiteSpaceSplit(char* data, char terminator)
{
    std::vector<std::string> result;
    std::string working;

    while(*data!=0x00&&*data!=terminator)
    {
        while(!isWhiteSpace(*data)&&*data!=0x00&&*data!=terminator)
            working.push_back(*data++);
        result.push_back(working);
        working.clear();
        while(isWhiteSpace(*data)&&*data!=0x00&&*data!=terminator)  
            *data++;
    }
    return result;
}

static bool isFirstInLine(char* source)
{
    source--;
    while(isWhiteSpace(*source) && *source != '\n')
        source--;
    if(*source == '\n')
        return true;
    return false;
}

static inline uint64_t proccessObject(dictionary* dict, bool isFirst = true)
{
    std::vector<uint64_t> subaddrs;

    void* data = malloc(0x20);

    write64(data+0x00,dataAddr);
    addDataSTR(dict->title.c_str(),dict->title.size()+1);

    write64(data+0x08,dataAddr);
    strip(dict->content);
    addDataSTR(dict->content.c_str(),dict->content.size()+1);

    //std::cout << "dict: " << dict->title << " cnt: " << dict->content << std::endl;

    for(dictionary* I : dict->subs)
        subaddrs.push_back(proccessObject(I,false));

    write64(data+0x10,subaddrs.size());

    void* data2 = malloc(8*subaddrs.size());

    uint64_t i = 0;
    for(uint64_t I : subaddrs)
        ((uint64_t*)data2)[i++] = I;

    write64(data+0x18,dataAddr);
    addData(data2,8*subaddrs.size());

    uint64_t objAddr = dataAddr;
    addData(data,0x20);

    if(isFirst)
    {
        //generate instruction
        void* code = malloc(10);
        write16(code+0x00,GDML_OPCODE_OBJ);
        write64(code+0x02,objAddr);
        //std::cout << "obj add data" << std::endl;
        addCode(code,10);
    }
    return (uint64_t)objAddr;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"

static inline void directive_link(char** source)
{
    //
    // link instruction
    //
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    if(tokens.size() != 3)
    {
        std::cout << "ERROR: invalid number of arguments to '#link' directive" << std::endl;
        exit(-1);
    }

    void* code = malloc(18);
    write16(code+0x00,GDML_OPCODE_LINK);
    write64(code+0x02,dataAddr);
    addDataSTR(tokens[1].c_str(),tokens[1].size()+1);
    write64(code+0x0A,dataAddr);
    addDataSTR(tokens[2].c_str(),tokens[2].size()+1);
    addCode(code,18);
}


static inline void directive_related(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');

    if(tokens[1] == "repo")
    {
        //
        //related repo instruction
        //

        void* code = malloc(11);
        write16(code,GDML_OPCODE_RELATED);
        write8(code+2,GDML_OBJT_REPO);
        write64(code+3,dataAddr);
        addDataSTR(tokens[2].c_str(),tokens[2].size()+1);

        addCode(code,11);
    }
    else if(tokens[1] == "file")
    {
        //related file instruction
        if(tokens.size() != 4)
        {
            std::cout << "ERROR: invalid number of arguments to '#related file' directive" << std::endl;
            exit(-1);
        }

        void* code = malloc(19);
        write16(code,GDML_OPCODE_RELATED);
        write8(code+2,GDML_OBJT_REPOFILE);
        write64(code+3,dataAddr);
        addDataSTR(tokens[2].c_str(),tokens[2].size()+1);
        write64(code+11,dataAddr);
        addDataSTR(tokens[3].c_str(),tokens[3].size()+1);
        addCode(code,19);
    }
}


static inline void directive_out(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    if(tokens.size() != 2)
    {
        std::cout << "ERROR: invalid number of arguments to '#out' directive" << std::endl;
        exit(-1);
    }

    write64(GlobalData+0x1C,dataAddr);
    addDataSTR(tokens[1].c_str(),tokens[1].size()+1);
}


static inline void directive_embed(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    if(tokens[1] == "repo")
    {
        //
        //related repo instruction
        //

        void* code = malloc(11);
        write16(code,GDML_OPCODE_EMBED);
        write8(code+2,GDML_OBJT_REPO);
        write64(code+3,dataAddr);
        addDataSTR(tokens[2].c_str(),tokens[2].size()+1);

        addCode(code,11);
    }
    else if(tokens[1] == "file")
    {
        //related file instruction
        if(tokens.size() != 4)
        {
            std::cout << "ERROR: invalid number of arguments to '#embed file' directive" << std::endl;
            exit(-1);
        }

        void* code = malloc(19);
        write16(code,GDML_OPCODE_EMBED);
        write8(code+2,GDML_OBJT_REPOFILE);
        write64(code+3,dataAddr);
        addDataSTR(tokens[2].c_str(),tokens[2].size()+1);
        write64(code+11,dataAddr);
        addDataSTR(tokens[3].c_str(),tokens[3].size()+1);
        addCode(code,19);
    }
}


static inline void directive_set(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    if(tokens.size() != 4)
    {
        std::cout << "ERROR: invalid number of arguments to '#set' directive" << std::endl;
        exit(-1);
    }

    if(tokens[1] == "text")
    {
        if(tokens[2] == "size")
        {
            if(tokens[3] == "XL" || tokens[3] == "extra-large")
                doc.text.size = 5;
            else if(tokens[3] == "L" || tokens[3] == "large")
                doc.text.size = 4;
            else if(tokens[3] == "M" || tokens[3] == "medium")
                doc.text.size = 3;
            else if(tokens[3] == "S" || tokens[3] == "small")
                doc.text.size = 2;
            else if(tokens[3] == "XS" || tokens[3] == "extra-small")
                doc.text.size = 1;
            else
            {
                std::cout << "ERROR: invalid text size" << tokens[3] << std::endl;
                exit(-1);
            }
        }
        else
        {
            std::cout << "ERROR: invalid value for category text: " << tokens[2] << std::endl;
            exit(-1);
        }
    }
    else
    {
        std::cout << "ERROR: invalid category: " << tokens[1] << std::endl;
        exit(-1);
    }
}


static inline void directive_subcat(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    if(tokens.size() != 2)
    {
        std::cout << "ERROR: invalid number of arguments to '#subcat' directive" << std::endl;
        exit(-1);
    }

    void* code = malloc(10);
    write16(code,GDML_OPCODE_SUBCAT);
    write64(code+2,dataAddr);
    std::cout << "subcat add data" << std::endl;
    addCode(code,10);
    addDataSTR(tokens[1].c_str(),tokens[1].size()+1);
}

static inline void directive_error(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    std::cerr << tokens[1] << std::endl;
    exit(-1);
}

static inline void directive_info(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    tokens.erase(tokens.begin());
    for(std::string I : tokens)
        std::cout << I << " ";
    std::cout << std::endl;
}

static inline void directive_title(char** source)
{
    std::vector<std::string> tokens = whiteSpaceSplit(*source,'\n');
    if(tokens.size() != 2)
    {
        std::cout << "ERROR: invalid number of arguments to '#title' directive" << std::endl;
        exit(-1);
    }

    write64(GlobalData+0x24,dataAddr);
    addDataSTR(tokens[1].c_str(),tokens[1].size()+1);
}

#pragma GCC diagnostic pop

void interpretSource(char* source)
{
    uint64_t blockCommentCounter = 0;
    bool isLineComment = false;

    void* codeE = malloc(2);
    write16(codeE,GDML_OPCODE_ENTRY);
    addCode(codeE,2);
    addCode(codeE,2);

    while(1)
    {
        if(*source == 0x00)
            break;
        //
        // skip over comments
        //
        if(blockCommentCounter > 0)
        {
            switch(*source)
            {
                case('*'):
                    if(((uint16_t*)(source))[0] == '/*')
                        blockCommentCounter--;
                    break;
            }
            goto skipSwitch;
        }
        if(isLineComment)
        {
            while(*source != '\n' && *source!= 0x00)
                source++;

            if(*source == 0x00)
                goto epilogue;

            isLineComment = false;
        }

        //
        // main switch
        //
        switch(*source)
        {
            case(0x00):
                goto epilogue;
            case('/'):
                if(((uint16_t*)(source))[0] == '//')
                    isLineComment = true;
                else if(((uint16_t*)(source))[0] == '*/')
                    blockCommentCounter++;
                break;
            case('#'):
                if(!isFirstInLine(source))
                    goto msDefaultCase;
                source++;
                switch(((uint32_t*)(source))[0])
                {
                    case(' tuo'):
                        directive_out(&source);
                        goto skipSwitch2;
                    case('\ttuo'):
                        directive_out(&source);
                        goto skipSwitch2;
                    case(' tes'):
                        directive_set(&source);
                        goto skipSwitch2;
                    case('\ttes'):
                        directive_set(&source);
                        goto skipSwitch2;
                    case(' rre'):
                        directive_error(&source);
                        goto skipSwitch2;
                    case('\trre'):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(' gsm'):
                        directive_info(&source);
                        goto skipSwitch2;
                    case('\tgsm'):
                        directive_info(&source);
                        goto skipSwitch2;
                    case(' gol'):
                        directive_info(&source);
                        goto skipSwitch2;
                    case('\tgol'):
                        directive_info(&source);
                        goto skipSwitch2;
                }
                switch(((uint64_t*)(source))[0])
                {
                    case(cat(' det','aler')):
                        directive_related(&source);
                        goto skipSwitch2;
                    case(cat('\tdet','aler')):
                        directive_related(&source);
                        goto skipSwitch2;
                    case(cat(' ega','ssem')):
                        directive_info(&source);
                        goto skipSwitch2;
                    case(cat('\tega','ssem')):
                        directive_info(&source);
                        goto skipSwitch2;
                }
                switch(((uint64_t*)(source))[0] & 0x00FFFFFFFFFFFFFF)
                {
                    case(cat(' ta','cbus')):
                        directive_subcat(&source);
                        goto skipSwitch2;
                    case(cat('\tta','cbus')):
                        directive_subcat(&source);
                        goto skipSwitch2;
                }
                switch(((uint64_t*)(source))[0] & 0x0000FFFFFFFFFFFF)
                {
                    case(cat(' d','ebme')):
                        directive_embed(&source);
                        goto skipSwitch2;
                    case(cat('\td','ebme')):
                        directive_embed(&source);
                        goto skipSwitch2;
                    case(cat(' r','orre')):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(cat('\tr','orre')):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(cat(' l','ataf')):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(cat('\tl','ataf')):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(cat(' t','nirp')):
                        directive_info(&source);
                        goto skipSwitch2;
                    case(cat('\tt','nirp')):
                        directive_info(&source);
                        goto skipSwitch2;
                    case(cat(' e','ltit')):
                        directive_title(&source);
                        goto skipSwitch2;
                    case(cat('\te','ltit')):
                        directive_title(&source);
                        goto skipSwitch2;
                }
                switch(((uint64_t*)(source))[0] & 0x000000FFFFFFFFFF)
                {
                    case(cat(' ','tixe')):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(cat('\t','tixe')):
                        directive_error(&source);
                        goto skipSwitch2;
                    case(cat(' ','ofni')):
                        directive_info(&source);
                        goto skipSwitch2;   
                    case(cat('\t','ofni')):
                        directive_info(&source);
                        goto skipSwitch2;
                    case(cat(' ','knil')):
                        directive_link(&source);
                        goto skipSwitch2;
                    case(cat('        ','knil')):
                        directive_link(&source);
                        goto skipSwitch2;

                }
                skipSwitch2:;
                break;
            case('<'):
            {
                if(!isFirstInLine(source))
                    goto msDefaultCase;
                proccessObject(makeDictionary(source));
                break;
            }
            default:
                msDefaultCase:;
                break;
        }

        //
        // loop epilogue
        //
        skipSwitch:;
        source++;
    }

    epilogue:;
    void* code = malloc(2);
    write16(code,GDML_OPCODE_EXIT);
    addCode(code,2);

    alignBlock(GlobalData,dataSize,4096);

    //std::cout << "data size: " << dataSize << std::endl;
    //std::cout << "code size: " << codeSize << std::endl;

    void* outdata = malloc(codeSize+dataSize+1);
    memcpy(outdata,GlobalData,dataSize); 
    memcpy(outdata+dataSize,GlobalCode,codeSize);
    write32(outdata+0x00,'gdml');
    write64(outdata+0x04,0);
    write64(outdata+0x0C,codeSize+dataSize);
    write64(outdata+0x14,dataSize);

    FILE* file = fopen(output.c_str(),"wb");
    if(file == NULL)
    {
        std::cerr << "ERROR: unable to open output file: " << output << std::endl;
        exit(-1);
    }

    if(!fwrite(outdata,codeSize+dataSize,1,file))
    {
        std::cerr << "ERROR: unable to write to output file: " << output << std::endl;
        exit(-1);
    }
}
