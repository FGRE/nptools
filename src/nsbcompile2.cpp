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
#include "npafile.hpp"
#include "nsbcompile2.hpp"
#include "scriptfile.hpp"
#include "parser.hpp"

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

void Node::Compile(uint16_t Magic, uint16_t NumParams)
{
    Output.write((char*)&Counter, sizeof(uint32_t));
    Output.write((char*)&Magic, sizeof(uint16_t));
    Output.write((char*)&NumParams, sizeof(uint16_t));
    ++Counter;
}

void Argument::Compile()
{
    // Value
    if (Type == ARG_VARIABLE)
        Node::Compile(MAGIC_GET, 1);
    // Variable
    else
    {
        const char* StrType = ArgumentTypes[Type];
        uint32_t TypeSize = strlen(StrType);
        Node::Compile(MAGIC_LITERAL, 2);
        Output.write((char*)&TypeSize, sizeof(uint32_t));
        Output.write(StrType, TypeSize);
    }
    CompileRaw();
}

void Argument::CompileRaw()
{
    Data = NpaFile::FromUtf8(Data);
    uint32_t Size = Data.size();
    Output.write((char*)&Size, sizeof(uint32_t));
    Output.write(Data.c_str(), Size);
}

void Call::Compile()
{
    uint16_t NumParams = Arguments.size();
    uint32_t BuiltinMagic = Nsb::MagicifyString(Name.Data.c_str());

    // Parameters
    if (BuiltinMagic != MAGIC_PARSE_TEXT)
        for (auto i = Arguments.begin(); i != Arguments.end(); ++i)
            (*i)->Compile();

    // Builtin function
    if (BuiltinMagic != 0)
        Node::Compile(BuiltinMagic, NumParams);
    // Script function
    else
    {
        NumParams += 1;
        Node::Compile(MAGIC_CALL_FUNCTION, NumParams);
        Name.CompileRaw();
    }
    // Arguments
    for (auto i = Arguments.begin(); i != Arguments.end(); ++i)
        (*i)->CompileRaw();
}

void Block::Compile()
{
    Node::Compile(MAGIC_SCOPE_BEGIN, 0);
    for (auto i = Statements.begin(); i != Statements.end(); ++i)
        (*i)->Compile();
    Node::Compile(MAGIC_SCOPE_END, 0);
}

void Subroutine::CompilePrototype(uint16_t BeginMagic, uint32_t NumBeginParams)
{
    Node::Compile(BeginMagic, NumBeginParams);
    Name.CompileRaw();
}

void Subroutine::Compile()
{
    SubroutineBlock.Compile();
}

void Subroutine::CompileReturn(uint16_t EndMagic)
{
    Node::Compile(EndMagic, NumEndParams);
}

void Function::Compile()
{
    Name.Data = string("function.") + Name.Data;
    CompilePrototype(MAGIC_FUNCTION_BEGIN, Arguments.size() + 1);
    for (auto i = Arguments.begin(); i != Arguments.end(); ++i)
        (*i)->CompileRaw();
    Subroutine::Compile();
    CompileReturn(MAGIC_FUNCTION_END);
}

void Chapter::Compile()
{
    Name.Data = string("chapter.") + Name.Data;
    CompilePrototype(MAGIC_CHAPTER_DECLARATION, 1);
    Subroutine::Compile();
    CompileReturn(MAGIC_END_CHAPTER);
}

void Scene::Compile()
{
    Name.Data = string("scene.") + Name.Data;
    CompilePrototype(MAGIC_SCENE_DECLARATION, 1);
    Subroutine::Compile();
    CompileReturn(MAGIC_END_SCENE);
}

void Assignment::Compile()
{
    Rhs.Compile();
    Node::Compile(MAGIC_ASSIGN, 1);
    Name.CompileRaw();
}

void BinaryOperator::Compile()
{
    Lhs.Compile();
    Rhs.Compile();
    Node::Compile(Magic, 0);
}

void UnaryOperator::Compile()
{
    Rhs.Compile();
    Node::Compile(Magic, 0);
}

void Condition::Compile()
{
    Expr.Compile();
    Node::Compile(Magic, 0);
    ConditionBlock.Compile();
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " <output.nsb> <charset>" << std::endl;
        return 1;
    }

    NpaFile::SetLocale(argv[2]);
    yyparse();
    Output.open(argv[1], ios::binary);
    pRoot->Compile();
}
