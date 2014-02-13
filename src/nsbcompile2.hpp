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
#ifndef NSB_COMPILE_HPP
#define NSB_COMPILE_HPP

#include <vector>
#include <cstdint>
#include <string>
#include "nsbmagic.hpp"
using namespace std;

class Statement;
class Argument;
typedef std::vector<Statement*> StatementList;
typedef std::vector<Argument*> ArgumentList;

enum ArgumentType
{
    ARG_INT = 0,
    ARG_STRING,
    ARG_FLOAT,
    ARG_VARIABLE,
    ARG_FUNCTION
};

enum ConditionType
{
    COND_IF,
    COND_WHILE
};

struct Node
{
    virtual ~Node() {}
    void Compile(uint16_t Magic, uint16_t NumParams);
};

struct Statement : Node
{
    virtual void Compile() = 0;
};

struct Expression : Node
{
    virtual void Compile() = 0;
};

struct Argument : Expression
{
    Argument(const string& Data, ArgumentType Type) : Data(Data), Type(Type) {}
    void CompileRaw();
    virtual void Compile();

    string Data;
    const uint16_t SetParamMagic = MAGIC_SET_PARAM;
    const uint16_t GetVarMagic = MAGIC_GET;
    const uint16_t NumSetParamParams = 2;
    const uint16_t NumGetVarParams = 1;
    ArgumentType Type; // Todo: CallArgument : Argument?
};

struct Call : Statement
{
    Call(Argument& Name, ArgumentList& Arguments) : Name(Name), Arguments(Arguments) {}
    virtual void Compile();

    Argument& Name;
    const uint16_t Magic = MAGIC_CALL;
    ArgumentList Arguments;
};

struct Block : Statement
{
    Block() {}
    Block(StatementList& Statements) : Statements(Statements) {}
    virtual void Compile();

    StatementList Statements;
    const uint16_t BeginMagic = MAGIC_SCOPE_BEGIN;
    const uint16_t EndMagic = MAGIC_SCOPE_END;
    const uint16_t NumParams = 0;
};

struct Condition : Statement
{
    Condition(Block& ConditionBlock, Expression& Expr, ConditionType Type) : ConditionBlock(ConditionBlock), Expr(Expr), Type(Type) {}
    virtual void Compile();

    const uint16_t MagicIf = MAGIC_IF;
    const uint16_t MagicWhile = MAGIC_WHILE;
    Expression& Expr;
    Block& ConditionBlock;
    ConditionType Type;
};

struct Subroutine : Statement
{
    Subroutine(Argument& Name, Block& SubroutineBlock) : Name(Name), SubroutineBlock(SubroutineBlock) {}
    void CompilePrototype(uint16_t BeginMagic, uint32_t NumBeginParams);
    virtual void Compile();
    void CompileReturn(uint16_t EndMagic);

    const uint16_t NumEndParams = 0;
    Argument& Name;
    Block& SubroutineBlock;
};

struct Function : Subroutine
{
    Function(Argument& Name, ArgumentList& Arguments, Block& Statements) : Subroutine(Name, Statements), Arguments(Arguments) {}
    virtual void Compile();

    ArgumentList Arguments;
};

struct Chapter : Subroutine
{
    Chapter(Argument& Name, Block& Statements) : Subroutine(Name, Statements) {}
    virtual void Compile();
};

struct Scene : Subroutine
{
    Scene(Argument& Name, Block& Statements) : Subroutine(Name, Statements) {}
    virtual void Compile();
};

struct Assignment : Expression
{
    Assignment(Argument& Name, Expression& Rhs) : Name(Name), Rhs(Rhs) {}
    virtual void Compile();

    Argument& Name;
    Expression& Rhs;
    const uint16_t Magic = MAGIC_SET;
    const uint16_t NumParams = 1;
};

struct BinaryOperator : Expression
{
    BinaryOperator(Expression& Lhs, int Op, Expression& Rhs) : Lhs(Lhs), Op(Op), Rhs(Rhs) {}
    virtual void Compile();

    int Op;
    Expression& Lhs;
    Expression& Rhs;
    const uint16_t MagicAdd = MAGIC_ADD;
    const uint16_t MagicSub = MAGIC_SUBSTRACT;
    const uint16_t MagicDiv = MAGIC_DIVIDE;
    const uint16_t MagicMul = 0; // UNK
    const uint16_t MagicLess = 0; // UNK
    const uint16_t MagicGreater = 0; // UNK
    const uint16_t MagicEqual = 0; // UNK
    const uint16_t MagicNotEqual = 0; // UNK
    const uint16_t MagicGreaterEqual = 0; // UNK
    const uint16_t MagicLessEqual = 0; // UNK
    const uint16_t MagicAnd = 0; // UNK
    const uint16_t MagicOr = 0; // UNK
};

struct UnaryOperator : Expression
{
    UnaryOperator(int Op, Expression& Rhs) : Op(Op), Rhs(Rhs) {}
    virtual void Compile();

    int Op;
    Expression& Rhs;
    const uint16_t MagicNot = 0; // UNK
};

#endif
