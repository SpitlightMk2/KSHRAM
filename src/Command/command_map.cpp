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

#include "command_map.h"

using namespace std;

void CommandMap::ImportFromString(const string& str)
{
    // 这里不拆最外层的大括号。相应地，在Command解析每个命令的时候会拆掉一层大括号。
    vector<string> command_str_split = SplitWithBracket(
        str, ';', false);

    for (auto& cmd : command_str_split)
    {
        Strip(cmd);
        Command command(cmd, 0);
        this->content.insert({0, command});
    }
}

void CommandMap::ImportFromComment(const IndexList<string>& lst)
{
    auto iter_end = lst.end();
    for (auto iter = lst.begin(); iter != iter_end; ++iter)
    {
        // 组装指令
        int time = iter->first;
        // 这里不拆最外层的大括号。相应地，在Command解析每个命令的时候会拆掉一层大括号。
        vector<string> command_str_split = SplitWithBracket(
            iter->second.first().substr(2), ';', false);

        for (auto& cmd : command_str_split)
        {
            Strip(cmd);
            Command command(cmd, time);
            this->content.insert({time, command});
        }
    }
}

IndexList<string> CommandMap::ExportToComment() const
{
    IndexList<string> output;
    if (this->content.size() == 0)
    {
        return output;
    }

    auto iter_end = this->content.end();
    int time = this->content.begin()->first;
    vector<string> cmds;
    for (auto iter = this->content.begin(); iter != iter_end; ++iter)
    {
        int current_time = iter->first;

        // 循环到下一个时刻的命令，将上一时刻的写入output
        if (current_time != time)
        {
            output.insert(time, "//" + Assemble(cmds, ';'));
            cmds.clear();
            time = current_time;
        }

        cmds.push_back(iter->second.toOriginalString());
    }

    output.insert(time, "//" + Assemble(cmds, ';'));

    return output;
}

void CommandMap::ChangeKey(Iterator& iter, int new_key)
{
    Iterator backup = iter;
    Command cmd = backup->second;
    cmd.time() = new_key;
    ++iter;
    this->erase(backup);
    this->content.insert({new_key, cmd});
}

CommandMap CommandMap::Offset(int offset_time) const
{
    CommandMap output;
    for (const auto& [time, cmd] : *this)
    {
        Command offset_cmd = cmd;
        offset_cmd.time() += offset_time;
        output.insert(time + offset_time, offset_cmd);
    }

    return output;
}