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

#include "delay.h"

#include <string>

using namespace std;

vector<string> Delay::AcceptedCmdName() { return {"delay"}; }

bool Delay::CheckArgs(const Command& cmd)
{
    // delay [step] [subcmds...]
    return cmd.argLength() >= 2 && (cmd.argIsDouble(0) || cmd.argIsRatio(0));
}

bool Delay::Compile(const Command& cmd, CommandMap& cmd_map, ErrorStack& err_stack)
{
    ApplicationBus& bus = ApplicationBus::GetInstance();
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;

    // 提取数据
    int delay_step = round(cmd.argAsDoubleOrRatio(0, 48.0));

    if (delay_step <= 0)
    {
        err_collector.ErrorLog("delay_step is not positive.");
        return false;
    }

    // 提取子命令
    CommandMap subcmds;
    vector<string> command_str_split = SplitWithBracket(cmd.substring(1), ';', false);

    for (auto& cmdstr : command_str_split)
    {
        Strip(cmdstr);
        Command command(cmdstr, 0);
        subcmds.insert(0, command);
    }

    // 使用Compile先展开子命令
    subcmds = bus.Compile(subcmds, err_stack);
    if (subcmds.Empty())
    {
        return false;
    }

    // 写入
    for (auto& [local_time, subcmd] : subcmds)
    {
        subcmd.time() = cmd.time()  + delay_step + local_time;
        cmd_map.insert(cmd.time() + local_time, subcmd);
    }

    return true;
}