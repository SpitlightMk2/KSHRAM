/*
    This file is part of KSHRAM.

    KSHRAM: A command-style K-Shoot Mania chart editing toolpack.
    Copyright (C) 2024 Singular_Photon

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "command.h"

#include "src/misc/utilities.h"

using namespace std;

Command::Command(const std::string& str, int time)
{
    this->time_ = time;
    this->exec_delay_ = 0;
    this->original = str;
    // 这里会拆掉最外层的大括号。相应地，在CommandMap解析一行注释的时候不拆大括号。
    vector<string> cmd_split = BlankSplitWithBracket(str, true);
    if (!cmd_split.empty() && cmd_split[0][0] == '#')
    {
        // 处理延迟值。只有后面跟整数的情况视作延迟值。
        string delay = cmd_split[0].substr(1);
        if (IsDigit(delay))
        {
            this->exec_delay_ = stoi(delay);
            this->cmds = vector<string>(cmd_split.begin() + 1, cmd_split.end());
        }
        else {
            this->cmds = std::move(cmd_split);
        }
        
    }
    else
    {
        this->cmds = std::move(cmd_split);
    }

    for (auto& str : this->cmds)
    {
        ToLower(str);
    }
}

std::string Command::substring(int begin_i, int end_i) const
{
    vector<string>::const_iterator begin_iter = this->cmds.begin();
    begin_iter += begin_i + 1;
    vector<string>::const_iterator end_iter = this->cmds.begin();
    if (end_i < 0)
    {
        end_iter = this->cmds.end();
        end_iter += (1 + end_i);
    }
    else
    {
        end_iter += end_i;
    }

    string output(Assemble(vector<string>(begin_iter, end_iter), ' '));
    return output;
}