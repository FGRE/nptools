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
#include <boost/lexical_cast.hpp>
#include "scriptfile.hpp"
#include "npafile.hpp"
#include "nsbmagic.hpp"
using namespace std;

// TODO:
// - Move to libnpa
// - Use game-specific constants (e.g. steins-gate/src/nsbconstants.cpp)
const vector<string> NsbConstants =
{
    "null",
    "NULL",
    "true",
    "false",

    // MAGIC_REQUEST
    "Stop",
    "Play",
    "Disused",
    "AddRender",
    "Lock",
    "Erase",
    "UnClickable",
    "UnLock",
    "Start",
    "Enter",
    "Passive",
    "Smoothing",
    "NoLog",
    "PushText",
    "NoIcon",
    "Hideable",
    "Pause",
    "Resume",
    "EntrustSuspend",
    "CompulsorySuspend",
    "SubRender"
};

Line* pLine;
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

void WriteDefault()
{
    Indent();
    Output << Nsb::StringifyMagic(pLine->Magic) << GenParams(Params);
}

void WriteToParams()
{
    Params.resize(Params.size() - pLine->Params.size());
    Params.push_back(string(Nsb::StringifyMagic(pLine->Magic)) + GenParams(pLine->Params));
}

void BinaryOperator(string Op)
{
    Params[Params.size() - 2] = Params[Params.size() - 2] + " " + Op + " " + Params[Params.size() - 1];
    Params.resize(Params.size() - 1);
}

void AssignOperator(string Op)
{
    Indent();
    Output << pLine->Params[0] << " " << Op << " " << Params.back();
    Params.resize(Params.size() - 1);
}

int main(int argc, char** argv)
{
    if (argc < 3 || argc > 4)
    {
        std::cout << "usage: " << argv[0] << " <input.nsb> <output.nss> [charset]" << std::endl;
        return 1;
    }

    if (argc == 4)
        NpaFile::SetLocale(argv[3]);
    else
        NpaFile::SetLocale("ja_JP.CP932");

    ScriptFile Script(argv[1], ScriptFile::NSB);
    Output.open(argv[2]);

    uint32_t SourceIter = 0;
    while (pLine = Script.GetLine(SourceIter++))
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
            case MAGIC_VARIABLE:
                Params.push_back(pLine->Params[0]);
                break;
            case MAGIC_LITERAL:
                if (pLine->Params[0] == "STRING" &&
                    find(NsbConstants.begin(), NsbConstants.end(), pLine->Params[1]) == NsbConstants.end())
                    Params.push_back(string("\"" + pLine->Params[1] + "\""));
                else
                    Params.push_back(pLine->Params[1]);
                break;
            case MAGIC_CALL_FUNCTION:
                Indent();
                Output << pLine->Params[0] << GenParams(Params);
                break;
            case MAGIC_FUNCTION_DECLARATION:
            {
                pLine->Params[0][8] = ' ';
                Output << pLine->Params[0];
                pLine->Params.erase(pLine->Params.begin());
                Output << GenParams(pLine->Params) << '\n';
                break;
            }
            case MAGIC_SCENE_DECLARATION:
                pLine->Params[0][5] = ' ';
                Output << pLine->Params[0] << '\n';
                break;
            case MAGIC_CHAPTER_DECLARATION:
                pLine->Params[0][7] = ' ';
                Output << pLine->Params[0] << '\n';
                break;
            case MAGIC_END_FUNCTION:
            case MAGIC_END_SCENE:
            case MAGIC_END_CHAPTER:
                Output << '\n';
                break;
            case MAGIC_ADD_ASSIGN:
                AssignOperator("+=");
                break;
            case MAGIC_SUB_ASSIGN:
                AssignOperator("-=");
                break;
            case MAGIC_ADD_EXPRESSION:
                BinaryOperator("+");
                break;
            case MAGIC_SUB_EXPRESSION:
                BinaryOperator("-");
                break;
            case MAGIC_DIV_EXPRESSION:
                BinaryOperator("/");
                break;
            case MAGIC_MUL_EXPRESSION:
                BinaryOperator("*");
                break;
            case MAGIC_MOD_EXPRESSION:
                BinaryOperator("%");
                break;
            case MAGIC_CMP_EQUAL:
                BinaryOperator("==");
                break;
            case MAGIC_CMP_GREATER:
                BinaryOperator(">");
                break;
            case MAGIC_CMP_LESS:
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
            case MAGIC_CMP_LOGICAL_AND:
                BinaryOperator("&&");
                break;
            case MAGIC_CMP_LOGICAL_OR:
                BinaryOperator("||");
                break;
            case MAGIC_LOGICAL_NOT:
                Params[Params.size() - 1] = string("!") + Params[Params.size() - 1];
                break;
            case MAGIC_ASSIGN:
                Indent();
                if (!Params.empty())
                {
                    if (pLine->Params[0] == "__array_variable__")
                        Output << Params[0];
                    else
                        Output << pLine->Params[0];
                    Output << " = " << Params.back();
                }
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
            case MAGIC_BREAK:
                Indent();
                Output << "break;\n";
                Params.clear();
                break;
            case MAGIC_PARSE_TEXT:
                Output << pLine->Params[2] << '\n';
                break;
            case MAGIC_NEGA_EXPRESSION:
                Params[Params.size() - 1] = string("-") + Params[Params.size() - 1];
                break;
            // Currently ignored builtins
            case MAGIC_PLACEHOLDER_PARAM:
            case MAGIC_JUMP: // TODO: Check for else/else if
            case MAGIC_WHILE_END:
            case MAGIC_CASE_END:
            case MAGIC_SELECT_END:
            case MAGIC_SELECT_BREAK_END:
                break;
            case MAGIC_INCREMENT:
                Indent();
                Output << Params.back() << "++";
                break;
            case MAGIC_DECREMENT:
                Indent();
                Output << Params.back() << "--";
                break;
            case MAGIC_CALL_CHAPTER:
                Indent();
                Output << "call_chapter " << pLine->Params[0] << ";\n";
                break;
            case MAGIC_CALL_SCENE:
                Indent();
                Output << "call_scene " << pLine->Params[0] << ";\n";
                break;
            // Builtins which return value
            case MAGIC_STR_STR:
            case MAGIC_IMAGE_HORIZON:
            case MAGIC_IMAGE_VERTICAL:
            case MAGIC_SOUND_AMPLITUDE:
            case MAGIC_COUNT:
            case MAGIC_SCROLLBAR_VALUE:
            case MAGIC_EXIST_SAVE:
            case MAGIC_STRING:
            case MAGIC_UNK140:
            case MAGIC_READ_FILE:
            case MAGIC_UNK184:
            case MAGIC_TIME:
            case MAGIC_REMAIN_TIME:
            case MAGIC_DURATION_TIME:
            case MAGIC_PASSAGE_TIME:
            case MAGIC_GET_MODULE_FILE_NAME:
                WriteToParams();
                break;
            case MAGIC_SELECT:
                Indent();
                Output << "select\n";
                break;
            case MAGIC_CASE:
                Indent();
                Output << "case " << pLine->Params[0] << ":\n";
                break;
            case MAGIC_RETURN:
                Indent();
                Output << "return;\n";
                break;
            case MAGIC_ARRAY_READ:
            {
                string Param = pLine->Params[0];
                int Size = boost::lexical_cast<int>(pLine->Params[1]);
                for (int i = Params.size() - Size; i < Params.size(); ++i)
                    Param += "[" + Params[i] + "]";
                Params.resize(Params.size() - Size);
                Params.push_back(Param);
                break;
            }
            case MAGIC_VARIABLE_VALUE:
                if (pLine->Params.size() == 2)
                    WriteToParams();
                else
                    WriteDefault();
                break;
            default:
                WriteDefault();
                break;
        }
    }
}
