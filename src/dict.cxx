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

#include <dict.hxx>
#include <stack>
#include <cstring>

extern bool isWhiteSpace(char c);

/*
    FYI: im fucking tripping as im writing this code, my screen looks like its perpetually comming closer LOL;
    regards, Lilith;
*/

dictionary* makeDictionary(char*& data)
{
    //std::cout << "creating dict, src: \n" << data << "\n" << std::endl;
    dictionary* lastPopped = nullptr;
    std::stack<dictionary*> dicts;

    //std::cout << "data: " << std::endl;
    while(*data != 0x00)
    {
        //std::cout << *data;
        if(*data == '<')
        {
            if(*(data+1) == '/')
            {
                //closing tag
                data+=2;
                while(*data!=0x00&&*data!='>')
                {
                    //std::cout << *data;
                    data++;
                }
                //std::cout << *data;

                //std::cout <<std::endl<< "stack size: " << dicts.size() << std::endl;

                //std::cout <<std::endl<< "closing tag: " << dicts.top()->title << std::endl;
                lastPopped = dicts.top();
                dicts.pop();
                if(dicts.size() == 0)
                    goto epi;
            }
            else
            {
                //opening tag
                data+=1;
                std::string title = "";
                while(*data!=0x00&&*data!='>')
                {
                    //std::cout << *data;
                    title.push_back(*data++);
                }
                //std::cout << *data;
                
                dictionary* newDict = new dictionary;
                newDict->title = title;
                if(!dicts.empty())
                    *dicts.top() << newDict;
                dicts.push(newDict);

                //std::cout <<std::endl<< "opening tag: " << dicts.top()->title << std::endl;
            }
        }
        else
        {
            dicts.top()->content.push_back(*data);
            //std::cout << "("<< dicts.top()->title <<")content add: \"" << *data << "\"" << std::endl;
        }

        data++;
    }

    epi: ;
    //std::cout << std::endl << "done" << std::endl;
    return lastPopped;
}

dictionary::dictionary(){}
dictionary::~dictionary() 
{
    for(uint64_t i = 0; i < this->subs.size();i++)
        free(this->subs[i]);
}

dictionary* dictionary::operator[](std::string index)
{
    for(uint64_t i = 0; i < this->subs.size();i++)
        if(this->subs[i]->title == index)
            return this->subs[i];
    return nullptr;
}

dictionary* dictionary::operator[](uint64_t index){return this->subs[index];}

const char* dictionary::getContent()
{
    return this->content.c_str();
}

void dictionary::operator<<(dictionary* dict)
{
    this->subs.push_back(dict);
}
