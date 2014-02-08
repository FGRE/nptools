/* 
 * nsbcompile: Nitroplus script compiler
 * Copyright (C) 2014 Mislav Blažević <krofnica996@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
#include <iostream>
#include <fstream>
#include <cstring>
#include "nsbcompile2.hpp"
#include "nsbfile.hpp"

class Block;
extern Block* pRoot;
extern int yyparse();
static uint32_t Counter = 1;
static ofstream Output;

const char* ArgumentTypes[] =
{
    "INT",
    "STRING",
    "FLOAT"
};

void Node::Compile()
{
    Output.write((char*)&Counter, sizeof(uint32_t));
    ++Counter;
}

void Argument::Compile()
{
    uint32_t Size = Data.size();
    Output.write((char*)&Size, sizeof(uint32_t));
    Output.write(Data.c_str(), Size);
}

void Call::Compile()
{
    uint16_t NumParams = Arguments.size() + 1;

    // Parameters
    for (auto i = Arguments.begin(); i != Arguments.end(); ++i)
    {
        Node::Compile();
        // Value
        if ((*i)->Type == ARG_VARIABLE)
        {
            Output.write((char*)&GetVarMagic, sizeof(uint16_t));
            Output.write((char*)&NumGetVarParams, sizeof(uint16_t));
        }
        // Variable
        else
        {
            const char* Type = ArgumentTypes[(*i)->Type];
            uint32_t TypeSize = strlen(Type);
            Output.write((char*)&SetParamMagic, sizeof(uint16_t));
            Output.write((char*)&NumSetParamParams, sizeof(uint16_t));
            Output.write((char*)&TypeSize, sizeof(uint32_t));
            Output.write(Type, TypeSize);
        }
        (*i)->Compile();
    }

    // Call
    Node::Compile();

    // Builtin function
    if (uint32_t BuiltinMagic = NsbFile::MagicifyString(Name.Data.c_str()))
    {
        Output.write((char*)&BuiltinMagic, sizeof(uint16_t));
        Output.write((char*)&NumParams, sizeof(uint16_t));
    }
    // Script function
    else
    {
        Output.write((char*)&Magic, sizeof(uint16_t));
        Output.write((char*)&NumParams, sizeof(uint16_t));
        Name.Compile();
    }
    // Arguments
    for (auto i = Arguments.begin(); i != Arguments.end(); ++i)
        (*i)->Compile();
}

void Block::Compile()
{
    Node::Compile();
    Output.write((char*)&BeginMagic, sizeof(uint16_t));
    Output.write((char*)&NumParams, sizeof(uint16_t));
    for (auto i = Statements.begin(); i != Statements.end(); ++i)
        (*i)->Compile();
    Node::Compile();
    Output.write((char*)&EndMagic, sizeof(uint16_t));
    Output.write((char*)&NumParams, sizeof(uint16_t));
}

void Subroutine::CompilePrototype(uint16_t BeginMagic, uint32_t NumBeginParams)
{
    Node::Compile();
    Output.write((char*)&BeginMagic, sizeof(uint16_t));
    Output.write((char*)&NumBeginParams, sizeof(uint16_t));
    Name.Compile();
}

void Subroutine::Compile()
{
    SubroutineBlock.Compile();
}

void Subroutine::CompileReturn(uint16_t EndMagic)
{
    Node::Compile();
    Output.write((char*)&EndMagic, sizeof(uint16_t));
    Output.write((char*)&NumEndParams, sizeof(uint16_t));
}

void Function::Compile()
{
    Name.Data = string("function.") + Name.Data;
    CompilePrototype(MAGIC_FUNCTION_BEGIN, Arguments.size() + 1);
    for (auto i = Arguments.begin(); i != Arguments.end(); ++i)
        (*i)->Compile();
    Subroutine::Compile();
    CompileReturn(MAGIC_FUNCTION_END);
}

void Chapter::Compile()
{
    Name.Data = string("chapter.") + Name.Data;
    CompilePrototype(MAGIC_CHAPTER_BEGIN, 1);
    Subroutine::Compile();
    CompileReturn(MAGIC_CHAPTER_END);
}

void Scene::Compile()
{
    Name.Data = string("scene.") + Name.Data;
    CompilePrototype(MAGIC_SCENE_BEGIN, 1);
    Subroutine::Compile();
    CompileReturn(MAGIC_SCENE_END);
}

int main(int argc, char** argv)
{
    if (argc != 2)
        return 1;

    yyparse();
    Output.open(argv[1], ios::binary);
    pRoot->Compile();
}
