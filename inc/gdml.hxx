/*
 * Created Date: Friday June 30th 2023
 * Author: Lilith
 * -----
 * Last Modified: Friday June 30th 2023 11:10:47 pm
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
#ifndef GDML_FULL_H
#define GDML_FULL_H

#define gdml_packed __attribute__((__packed__))
#include <string>
#include <vector>

#include <cstdint>

#define GDML_OBJT_INVALID 0x00
#define GDML_OBJT_REPO 0x01
#define GDML_OBJT_REPOFILE 0x02
#define GDML_OBJT_FUNCTION 0x03
#define GDML_OBJT_CLASS 0x04

/*
    stop parsing code when reading this instruction

    format:
    uint16_t opcode;
*/
#define GDML_OPCODE_EXIT 0xFFFF

/*
    do nothing

    format:
    uint16_t opcode;
*/
#define GDML_OPCODE_NOOP 0x0001

/*
    do nothing

    format:
    uint16_t opcode;
*/
#define GDML_OPCODE_ENTRY 0x1234

/*
    put comment in output code

    format:
    uint16_t opcode;
    uint64_t text-address;
*/
#define GDML_OPCODE_COMMENT 0x0003


/*
    embed object in output

    format:
    uint16_t opcode;
    uint8_t object-type;
    for object-type GDML_OBJT_REPO:
        uint64_t repo-name-address
    for object-type GDML_OBJT_REPOFILE:
        uint64_t repo-name-address
        uint64_t file-path-address;
*/
#define GDML_OPCODE_EMBED 0x0008

/*
    note a related object in the output

    format:
    uint16_t opcode
    uint8_t object-type
    for object-type GDML_OBJT_REPO:
        uint64_t repo-name-address
    for object-type GDML_OBJT_REPOFILE:
        uint64_t repo-name-address
        uint64_t file-path-address;
*/
#define GDML_OPCODE_RELATED 0x0009

/*
    object in output

    format:
    uint16_t opcode
    uint64_t object-address
*/
#define GDML_OPCODE_OBJ 0x000A

/*
    sub category in output

    format:
    uint16_t opcode
    uint64_t name-address
*/
#define GDML_OPCODE_SUBCAT 0x000B

/*
    link for type

    format:
    uint16_t opcode
    uint64_t name-address
    uint64_t link-address
*/
#define GDML_OPCODE_LINK 0x000C

/*
    valid magic: 'gdml'

    revision: increments by 10.000 for every full version of gdml.
*/
struct GDML_HEADER
{
    /* +0x00 */ uint32_t magic;
    /* +0x04 */ uint64_t revision;
    /* +0x0C */ uint64_t filesize;
    /* +0x14 */ uint64_t codeStart;
    /* +0x1C */ uint64_t output;
    /* +0x24 */ uint64_t title;
    /* +0x2C */ uint32_t __padding1;
} gdml_packed;//this struct must be a multiple of 16 bytes long

struct GDML_OBJ
{
    /* +0x00 */uint64_t nameaddress;
    /* +0x08 */uint64_t contentaddress;
    /* +0x10 */uint64_t subobjs;
    /* +0x18 */uint64_t subaddrs;
} gdml_packed;

struct GDML_TYPE
{
    std::string name;
    std::string link;
};

/*
    the following functions are to be implemented by the user:
*/
extern "C" std::string content;
extern "C" void gdml_embed_repo(std::string link);
extern "C" void gdml_embed_repofile(std::string link, std::string path);
extern "C" void gdml_related_repo(std::string link);
extern "C" void gdml_related_repofile(std::string link, std::string path);
extern "C" void gdml_function_open(std::string name);
extern "C" void gdml_function_close();
extern "C" void gdml_function_overload_open();
extern "C" void gdml_function_overload_close();
extern "C" void gdml_parameter_open(std::string name);
extern "C" void gdml_parameter_close();
extern "C" void gdml_member_open(std::string name);
extern "C" void gdml_member_close();
extern "C" void gdml_class_open(std::string name);
extern "C" void gdml_class_close();
extern "C" void gdml_type_open(std::string name,std::string link);
extern "C" void gdml_type_close();
extern "C" void gdml_addAttribute(std::string name);
extern "C" void gdml_addComment(std::string text,std::string author, std::string date, std::string time);
extern "C" void gdml_comment(std::string text);
extern "C" void gdml_addIssue(std::string link);
extern "C" void gdml_addDescription(std::string text);
extern "C" void gdml_setNamespace(std::vector<std::string> text);
extern "C" uint64_t gdml_main(int argc, char** argv,std::string title);
extern "C" void gdml_subcat(std::string name);
extern "C" void gdml_end();

#endif