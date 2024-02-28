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

#include "knob_approx.h"

using namespace std;

namespace KnobApprox_Core
{

/// 寻找最邻近点
int Approximate(double rate, int div = 48)
{
    double real_val = rate * div;
    return static_cast<int>(std::round(real_val));
}

Knob ToKnobSide(const string& side)
{
    if (side == "l")
    {
        return Knob::L;
    }
    else // side == "r" or whatever
    {
        return Knob::R;
    }
}

}  // namespace KnobApprox_Core

namespace core = KnobApprox_Core;

/* #region KnobApprox */

vector<string> KnobApprox::AcceptedCmdName()
{
    return {"knob"};
}

bool KnobApprox::CheckArgs(const Command& cmd)
{
    // knob [side] [start_time] [end_time] [start_pos] [end_pos]
    return cmd.argLength() == 5 &&
           (cmd.argIsDouble(1) || cmd.argIsRatio(1)) &&
           (cmd.argIsDouble(2) || cmd.argIsRatio(2)) &&
           cmd.argIsInt(3) && cmd.argIsInt(4);
}

bool KnobApprox::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    bool success = true;
    // STEP 1 获取信息
    const string& side_str = cmd.arg(0);
    if (side_str != "l" && side_str != "r")
    {
        GET_ERROR_COLLECTOR.ErrorLog("arg[0] is invalid: Unknown side.");
        return false;
    }
    Knob knob_side = core::ToKnobSide(side_str);
    auto& knob_lst = chart.KnobList(knob_side);

    double start_time = cmd.argAsDoubleOrRatio(1);
    int approx_start_time = cmd.time() + core::Approximate(start_time);
    double end_time = cmd.argAsDoubleOrRatio(2);
    int approx_end_time = cmd.time() + core::Approximate(end_time);
    if (approx_end_time <= approx_start_time)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Knob end time is ahead of start time.");
        return false;
    }

    int start_pos = cmd.argAsInt(3);
    int end_pos = cmd.argAsInt(4);

    if (approx_end_time - approx_start_time <= 6 && start_pos != end_pos)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Knob is so short that it will be translated as a slam in ksh format.");
        return false;
    }

    // STEP 2 检查旋钮状态
    auto last_entry = knob_lst.prevItem(approx_end_time);
    while (last_entry != knob_lst.end() && last_entry != knob_lst.begin() &&
     last_entry->second.first() == -1 && last_entry->second.second() == -1)
    {
        --last_entry;
    }

    if (last_entry != knob_lst.end() &&
        (last_entry->first > approx_start_time ||  // 中间有旋钮关键点
         last_entry->second.second() != -1 ||               // 最后一个旋钮关键点不是结束
         (last_entry->first >= approx_start_time - 1 &&
          last_entry->second.first() != start_pos)  // 起始位置不一致
         ))
    {
        GET_ERROR_COLLECTOR.ErrorLog("Cannot insert a knob where knob already exists.");
        return false;
    }

    // STEP 3 执行
    knob_lst.insert(approx_start_time, start_pos);
    knob_lst.insert(approx_end_time, end_pos, -1);
    chart.UpdateTotalTime(approx_end_time);

    return success;
}

/* #endregion KnobApprox */

/* #region KnobAddApprox */

vector<string> KnobAddApprox::AcceptedCmdName()
{
    return {"knobadd"};
}

bool KnobAddApprox::CheckArgs(const Command& cmd)
{
    // knobadd [side] [end_time] [end_pos]
    return cmd.argLength() == 3 &&
           (cmd.argIsDouble(1) || cmd.argIsRatio(1)) &&
           cmd.argIsInt(2);
}

bool KnobAddApprox::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    bool success = true;
    // STEP 1 获取信息
    const string& side_str = cmd.arg(0);
    if (side_str != "l" && side_str != "r")
    {
        GET_ERROR_COLLECTOR.ErrorLog("arg[0] is invalid: Unknown side.");
        return false;
    }
    Knob knob_side = core::ToKnobSide(side_str);
    auto& knob_list = chart.KnobList(knob_side);

    double end_time = cmd.argAsDoubleOrRatio(1);
    int approx_end_time = cmd.time() + core::Approximate(end_time);
    auto last_entry = knob_list.prevItem(approx_end_time - 1);
    const auto not_found = knob_list.end();
    while (last_entry != not_found && last_entry->second.first() == -1 && last_entry->second.second() == -1)
    {
        --last_entry;
    }

    if (last_entry == not_found)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Knob start position not found.");
        return false;
    }
    if (last_entry->second.second() != -1)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Cannot insert a knob where knob already exists.");
        return false;
    }
    int start_time = last_entry->first;
    int start_pos = last_entry->second.first();
    int end_pos = cmd.argAsInt(2);

    if (approx_end_time - start_time <= 6 && start_pos != end_pos)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Knob is so short that it will be translated as a slam in ksh format.");
        return false;
    }

    // STEP 2 执行
    last_entry->second.setSecondAsFirst();
    knob_list.insert(approx_end_time, end_pos, -1);
    chart.UpdateTotalTime(approx_end_time);

    return success;
}

/* #endregion KnobAddApprox */

/* #region KnobSlamApprox */

vector<string> KnobSlamApprox::AcceptedCmdName()
{
    return {"slam"};
}

bool KnobSlamApprox::CheckArgs(const Command& cmd)
{
    // slam [side] [time] [start_pos] [end_pos] ([duration])
    bool valid = cmd.argLength() >= 4 &&
                 (cmd.argIsDouble(1) || cmd.argIsRatio(1)) &&
                 cmd.argIsInt(2) && cmd.argIsInt(3);
    if (valid && cmd.argLength() == 5)
    {
        valid &= cmd.argIsDouble(4) || cmd.argIsRatio(4);
    }

    return valid;
}

bool KnobSlamApprox::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    bool success = true;
    // STEP 1 获取信息
    const string& side_str = cmd.arg(0);
    if (side_str != "l" && side_str != "r")
    {
        GET_ERROR_COLLECTOR.ErrorLog("arg[0] is invalid: Unknown side.");
        return false;
    }
    Knob knob_side = core::ToKnobSide(side_str);
    auto& knob_lst = chart.KnobList(knob_side);

    double start_time = cmd.argAsDoubleOrRatio(1);
    int approx_start_time = cmd.time() + core::Approximate(start_time);
    int duration = cmd.argLength() == 5 ? static_cast<int>(cmd.argAsDoubleOrRatio(4, 48)) : 6;
    if (duration > 6)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Duration is too long. Should be smaller than 1/8 beat.");
        return false;
    }
    int end_time = approx_start_time + duration;

    int start_pos = cmd.argAsInt(2);
    int end_pos = cmd.argAsInt(3);
    if(start_pos == end_pos)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Start position and end position are the same.");
        return false;
    }

    // STEP 2 检查旋钮状态
    auto last_entry = knob_lst.prevItem(end_time);
    while (last_entry != knob_lst.end() && last_entry != knob_lst.begin() &&
     last_entry->second.first() == -1 && last_entry->second.second() == -1)
    {
        --last_entry;
    }

    if (last_entry != knob_lst.end() &&
        (last_entry->first > approx_start_time ||  // 中间有旋钮关键点
         last_entry->second.second() != -1 ||               // 最后一个旋钮关键点不是结束
         (last_entry->first >= approx_start_time - 1 &&
          last_entry->second.first() != start_pos)  // 起始位置不一致
         ))
    {
        GET_ERROR_COLLECTOR.ErrorLog("Cannot insert a knob where knob already exists.");
        return false;
    }

    // STEP 3 执行
    knob_lst.insert(approx_start_time, start_pos, end_pos);
    knob_lst.insert(end_time, end_pos, -1);
    chart.UpdateTotalTime(end_time);
    return success;
}

/* #endregion KnobSlamApprox */