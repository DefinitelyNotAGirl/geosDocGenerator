/**
 * Created Date: Sunday July 2nd 2023
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

#include <ios>
#include <iostream>
#include <gdml.hxx>
#include <cstring>
#include <vector>

std::vector<GDML_TYPE> linkedTypes;

uint64_t DATA_MEM_OFFSET = 0;

static inline uint64_t read64(void* ptr){return ((uint64_t*)ptr)[0];}
static inline uint32_t read32(void* ptr){return ((uint32_t*)ptr)[0];}
static inline uint16_t read16(void* ptr){return ((uint16_t*)ptr)[0];}
static inline uint8_t read8(void* ptr){return ((uint8_t*)ptr)[0];}

static bool isWhiteSpace(char c)
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

static std::vector<std::string> whiteSpaceSplit(char* data, char terminator)
{
    data+=DATA_MEM_OFFSET;
    std::vector<std::string> result;
    std::string working;

    while(isWhiteSpace(*data)&&*data!=0x00&&*data!=terminator)  
            *data++;
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

static std::vector<std::string> createNamespacelist(std::string& str)
{
    std::vector<std::string> result;
    std::string working;
    for(uint64_t I = 0;I<str.size();I++)
    {
        if(str[I] == ':')
        {
            if(I==str.size()-1)
                goto not_a_namespace;
            if(str[I+1] == ':')
            {
                result.push_back(working);
                working.clear();
                I++;
                goto namespacecheck_end;
            }
            not_a_namespace:;
        }
        working.push_back(str[I]);
        namespacecheck_end:;
    }
    if(!working.empty())
        result.push_back(working);

    return result;
}

static std::string readStringRAW(void* ptr)
{
    return std::string((char*)ptr);
}

static std::string readString(void* ptr)
{
    return std::string((char*)(ptr+DATA_MEM_OFFSET));
}

static void strToLowerCase(std::string& str)
{
    uint64_t i = 0;
    for(char I : str)
    {
        if(I >= 'A' && I <= 'Z')
            str[i] = I + 0x30;
        i++;
    }
}

static std::string objTryFindStr(void* data,std::string str)
{
    GDML_OBJ* obj = (GDML_OBJ*)(data+DATA_MEM_OFFSET);
    GDML_OBJ** subs = obj->subaddrs+DATA_MEM_OFFSET;
    for(uint64_t i = 0; i < obj->subobjs;i++)
    {
        GDML_OBJ* subobj = ((uint64_t)subs[i])+DATA_MEM_OFFSET;
        std::string name = readString(subobj->nameaddress);
        strToLowerCase(name);
        if(name == str)
            return readString(subobj->contentaddress);
    }
    return "";
}

static void proccessObject(void* data)
{
    GDML_OBJ* obj = (GDML_OBJ*)(data+DATA_MEM_OFFSET);
    std::string name = readString(obj->nameaddress);
    strToLowerCase(name);
    uint64_t closeID = 0;
    if(name == "function")
    {
        std::string fname = objTryFindStr(data,"name");
        gdml_function_open(fname);
        closeID = 0x0001;
    }
    else if(name == "desc")
    {
        std::string content = readString(obj->contentaddress);
        gdml_addDescription(content);
    }
    else if(name == "overload")
    {
        gdml_function_overload_open();
        closeID = 0x0002;
    }
    else if(name == "attributes")
    {
        std::vector<std::string> attribs = whiteSpaceSplit(obj->contentaddress,0x00);
        for(std::string attrib : attribs)
            gdml_addAttribute(attrib);
    }
    else if(name == "param")
    {
        std::string pname = objTryFindStr(data,"name");
        gdml_parameter_open(pname);
        closeID = 0x0003;
    }
    else if(name == "member")
    {
        std::string pname = objTryFindStr(data,"name");
        gdml_member_open(pname);
        closeID = 0x0005;
    }
    else if(name == "namespace")
    {
        std::string content = readString(obj->contentaddress);
        std::vector<std::string> namespacelist = createNamespacelist(content);
        gdml_setNamespace(namespacelist);
    }
    else if(name == "issue")
    {
        std::string content = readString(obj->contentaddress);
        gdml_addIssue(content);
    }
    else if(name == "comment")
    {
        std::string content = readString(obj->contentaddress);
        std::string author = objTryFindStr(data,"author");
        std::string date = objTryFindStr(data,"date");
        std::string time = objTryFindStr(data,"time");
        gdml_addComment(content,author,date,time);
    }
    else if(name == "class")
    {
        std::string cname = objTryFindStr(data,"name");
        gdml_class_open(cname);
        closeID = 0x0004;
    }
    else if(name == "name"){/*do nothing on name tags*/}
    else if(name == "author"){/*do nothing on author tags*/}
    else if(name == "date"){/*do nothing on date tags*/}
    else if(name == "time"){/*do nothing on time tags*/}
    else if(name == "type")
    {
        std::string tname = objTryFindStr(data,"name");
        closeID = 0x0006;
        for(GDML_TYPE& type : linkedTypes)
        {
            if(type.name == tname)
            {
                gdml_type_open(tname,type.link);
                goto typeopencalled;
            }
        }
        gdml_type_open(tname,"");
        typeopencalled:;
    }
    else
        std::cout << "Unknown tag type: \"" << name <<"\""<< std::endl;

    GDML_OBJ** subs = obj->subaddrs+DATA_MEM_OFFSET;
    for(uint64_t i = 0; i < obj->subobjs;i++)
        proccessObject(subs[i]);

    switch(closeID)
    {
        case(0x0001):
            return gdml_function_close();
        case(0x0002):
            return gdml_function_overload_close();
        case(0x0003):
            return gdml_parameter_close();
        case(0x0004):
            return gdml_class_close();
        case(0x0005):
            return gdml_member_close();
        case(0x0006):
            return gdml_type_close();
    }
}

void runGDML(int argc, char** argv, void* data)
{
    DATA_MEM_OFFSET = (uint64_t)data;
    uint32_t magic = read32(data);
    void* rip = read64(data+0x14)+data;
    void* titleAddr = read64(data+0x24);
    std::string title = readString(titleAddr);

    //call users main function
    if(gdml_main(argc,argv,title) != 0)
    {
        std::cerr << "ERROR: gdml_main failed!" << std::endl;
        exit(-1);
    }

    //std::cout << std::hex << "starting rip: " << rip-(uint64_t)data << std::endl;
    while(read16(rip)!=GDML_OPCODE_EXIT)
    {
        switch(read16(rip))
        {
            case(GDML_OPCODE_ENTRY):
                //std::cout << "entry opcode!" << std::endl;
                rip+=2;
                break;
            case(GDML_OPCODE_NOOP):
                //std::cout << "NOOP opcode!" << std::endl;
                rip+=2;
                break;
            case(GDML_OPCODE_OBJ):
                //std::cout << "object opcode!" << std::endl;
                proccessObject(read64(rip+2));
                rip+=10;
                break;
            case(GDML_OPCODE_COMMENT):
            {
                //std::cout << "comment opcode!" << std::endl;
                std::string comment = readString(read64(rip+2));
                gdml_comment(comment);
                rip+=10;
                break;
            }
            case(GDML_OPCODE_EMBED):
                //std::cout << "embed opcode!" << std::endl;
                if(read8(rip+2) == GDML_OBJT_REPO)
                {
                    //std::cout << "embed repo!" << std::endl;
                    std::string rURL = readString(read64(rip+3));
                    gdml_embed_repo(rURL);
                    rip+=11;
                }
                else if(read8(rip+2) == GDML_OBJT_REPOFILE)
                {
                    //std::cout << "embed repo file!" << std::endl;
                    std::string rURL = readString(read64(rip+3));
                    std::string rFile = readString(read64(rip+11));
                    gdml_embed_repofile(rURL,rFile);
                    rip+=19;
                }
                else
                {
                    std::cerr << "INVALID OPCODE EXCEPTION" << std::endl;
                    std::cerr << "opcode: " << std::hex << read16(rip) << std::endl;
                    std::cerr << "rip: " << std::hex << rip << std::endl;
                }
                break;
            case(GDML_OPCODE_RELATED):
                //std::cout << "related opcode!" << std::endl;
                if(read8(rip+2) == GDML_OBJT_REPO)
                {
                    //std::cout << "related repo!" << std::endl;
                    std::string rURL = readString(read64(rip+3));
                    gdml_embed_repo(rURL);
                    rip+=11;
                }
                else if(read8(rip+2) == GDML_OBJT_REPOFILE)
                {
                    //std::cout << "related repo file!" << std::endl;
                    std::string rURL = readString(read64(rip+3));
                    std::string rFile = readString(read64(rip+11));
                    gdml_embed_repofile(rURL,rFile);
                    rip+=19;
                }
                else
                {
                    std::cerr << "INVALID OPCODE EXCEPTION" << std::endl;
                    std::cerr << "opcode: " << std::hex << read16(rip) << std::endl;
                    std::cerr << "rip: " << std::hex << rip << std::endl;
                }
                break;
            case(GDML_OPCODE_SUBCAT):
            {
                //std::cout << "subcat opcode!" << std::endl;
                std::string subcat = readString(read64(rip+2));
                gdml_subcat(subcat);
                rip+=10;
                break;
            }
            case(GDML_OPCODE_LINK):
            {
                //std::cout << "link opcode!" << std::endl;
                std::string name = readString(read64(rip+2));
                std::string link = readString(read64(rip+10));
                GDML_TYPE type;
                type.name = name;
                type.link = link;
                linkedTypes.push_back(type);
                rip+=18;
                break;
            }
            default:
                std::cerr << "INVALID OPCODE EXCEPTION" << std::endl;
                std::cerr << "opcode: " << std::hex << read16(rip) << std::endl;
                std::cerr << "rip: " << std::hex << rip-(uint64_t)DATA_MEM_OFFSET << std::endl;
                exit(-1);
        }
    }

    gdml_end();
}