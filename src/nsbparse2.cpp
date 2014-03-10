/* 
 * nsbparse: Nitroplus script decompiler
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
#include "nsbfile.hpp"
#include "nsbmagic.hpp"
using namespace std;

ofstream Output;
vector<string> Params;
int IndentLevel = 0;

void Indent()
{
    for (int i = 0; i < IndentLevel; ++i)
        Output << "    ";
}

string GenParams(vector<string>& Params)
{
    string ParamList = "(";
    for (int i = 0; i < Params.size(); ++i)
    {
        ParamList += Params[i];
        ParamList += ((i != (Params.size() - 1)) ? ", " : "");
    }
    ParamList += ")";
    return ParamList;
}

void BinaryOperator(string Op)
{
    Params[Params.size() - 2] = Params[Params.size() - 2] + " " + Op + " " + Params[Params.size() - 1];
    Params.resize(Params.size() - 1);
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " <input.nsb> <output.nss>" << std::endl;
        return 1;
    }

    NsbFile Script(argv[1]);
    Output.open(argv[2]);

    while (Line* pLine = Script.GetNextLine())
    {
        switch (pLine->Magic)
        {
            case MAGIC_SCOPE_BEGIN:
                Indent();
                Output << "{\n";
                ++IndentLevel;
                break;
            case MAGIC_SCOPE_END:
                --IndentLevel;
                Indent();
                Output << "}\n";
                break;
            case MAGIC_CLEAR_PARAMS:
                Output << ";\n";
                Params.clear();
                break;
            case MAGIC_GET:
                Params.push_back(pLine->Params[0]);
                break;
            case MAGIC_SET_PARAM:
                if (pLine->Params[0] == "STRING")
                    Params.push_back(string("\"" + pLine->Params[1] + "\""));
                else
                    Params.push_back(pLine->Params[1]);
                break;
            case MAGIC_CALL:
                Indent();
                Output << pLine->Params[0] << GenParams(Params);
                break;
            case MAGIC_FUNCTION_BEGIN:
            {
                pLine->Params[0][8] = ' ';
                Output << pLine->Params[0];
                pLine->Params.erase(pLine->Params.begin());
                Output << GenParams(pLine->Params) << '\n';
                break;
            }
            case MAGIC_SCENE_BEGIN:
                pLine->Params[0][5] = ' ';
                Output << pLine->Params[0] << '\n';
                break;
            case MAGIC_CHAPTER_BEGIN:
                pLine->Params[0][7] = ' ';
                Output << pLine->Params[0] << '\n';
                break;
            case MAGIC_FUNCTION_END:
            case MAGIC_SCENE_END:
            case MAGIC_CHAPTER_END:
                Output << '\n';
                break;
            case MAGIC_ADD:
                BinaryOperator("+");
                break;
            case MAGIC_SUBSTRACT:
                BinaryOperator("-");
                break;
            case MAGIC_DIVIDE:
                BinaryOperator("/");
                break;
            case MAGIC_MULTIPLY:
                BinaryOperator("*");
                break;
            case MAGIC_LOGICAL_EQUAL:
                BinaryOperator("==");
                break;
            case MAGIC_LOGICAL_GREATER:
                BinaryOperator(">");
                break;
            case MAGIC_LOGICAL_LESS:
                BinaryOperator("<");
                break;
            case MAGIC_LOGICAL_NOT_EQUAL:
                BinaryOperator("!=");
                break;
            case MAGIC_LOGICAL_GREATER_EQUAL:
                BinaryOperator(">=");
                break;
            case MAGIC_LOGICAL_LESS_EQUAL:
                BinaryOperator("<=");
                break;
            case MAGIC_LOGICAL_AND:
                BinaryOperator("&&");
                break;
            case MAGIC_LOGICAL_OR:
                BinaryOperator("||");
                break;
            case MAGIC_LOGICAL_NOT:
                Params[Params.size() - 1] = string("!") + Params[Params.size() - 1];
                break;
            case MAGIC_SET:
                Indent();
                if (!Params.empty())
                    Output << pLine->Params[0] << " = " << Params.back();
                else
                    cout << "WARNING: MAGIC_SET without parameters! (Perhaps builtin return?)" << endl;
                break;
            case MAGIC_IF:
                Indent();
                Output << "if " << GenParams(Params) << '\n';
                Params.clear();
                break;
            case MAGIC_WHILE:
                Indent();
                Output << "while " << GenParams(Params) << '\n';
                Params.clear();
                break;
            case MAGIC_PARSE_TEXT:
                Output << pLine->Params[2] << '\n';
                break;
            case MAGIC_NEGATIVE:
                Params[Params.size() - 1] = string("-") + Params[Params.size() - 1];
                break;
            case MAGIC_PLACEHOLDER_PARAM:
            case MAGIC_LABEL:
            case MAGIC_LOOP_JUMP:
                break;
            case MAGIC_INCREMENT:
                Indent();
                Output << Params.back() << "++";
                break;
            case MAGIC_CALL_CHAPTER:
            case MAGIC_CALL_SCENE:
                Indent();
                Output << NsbFile::StringifyMagic(pLine->Magic) << GenParams(pLine->Params) << ";\n";
                break;
            // Builtins which return value
            case MAGIC_UNK140:
            case MAGIC_UNK184:
            case MAGIC_TIME:
            case MAGIC_GET_MOVIE_TIME:
            case MAGIC_GET_SCRIPT_NAME:
                Params.push_back(string(NsbFile::StringifyMagic(pLine->Magic)) + GenParams(pLine->Params));
                break;
            case MAGIC_RETURN:
                Indent();
                Output << "return;\n";
                break;
            default:
                Indent();
                Output << NsbFile::StringifyMagic(pLine->Magic) << GenParams(Params);
                break;
        }
    }
}
