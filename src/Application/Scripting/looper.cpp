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

#include "looper.h"

#include <string>

using namespace std;

vector<string> Looper::AcceptedCmdName() { return {"loop"}; }

bool Looper::CheckArgs(const Command& cmd)
{
    // loop [count] [step] [subcmd...]
    return cmd.argLength() >= 3 && cmd.argIsInt(0) && (cmd.argIsDouble(1) || cmd.argIsRatio(1));
}

bool Looper::Compile(const Command& cmd, CommandMap& cmd_map, ErrorStack& err_stack)
{
    ApplicationBus& bus = ApplicationBus::GetInstance();
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    
    // 提取数据
    int count = cmd.argAsInt(0);
    int step = round(cmd.argAsDoubleOrRatio(1, 48.0));

    if(count <= 1)
    {
        err_collector.ErrorLog("Loop count is not greater than 1.");
        return false;
    }

    if(step <= 0)
    {
        err_collector.ErrorLog("Step is not positive.");
        return false;
    }

    // 提取子命令
    CommandMap subcmds;
    vector<string> command_str_split = SplitWithBracket(cmd.substring(2), ';', false);

    for (auto& cmdstr : command_str_split)
    {
        Strip(cmdstr);
        if(cmdstr.empty())
        {
            continue;
        }
        Command command(cmdstr, 0);
        
        subcmds.insert(0, command);
    }

    // 使用Compile先展开子命令
    subcmds = bus.Compile(subcmds, err_stack);
    if(subcmds.Empty())
    {
        return false;
    }

    // 写入
    int time = cmd.time();
    for (int i = 0; i < count; ++i)
    {
        for (auto& [inner_time,subcmd] : subcmds)
        {
            subcmd.time() = time + inner_time;
            cmd_map.insert(cmd.time() + inner_time, subcmd);
        }
        time += step;
    }

    return true;
}