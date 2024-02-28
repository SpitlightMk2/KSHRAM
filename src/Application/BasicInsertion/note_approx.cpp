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

#include "note_approx.h"

using namespace std;

namespace NoteApprox_Core
{

/// 寻找最邻近点
int Approximate(double rate, int div = 48)
{
    double real_val = rate * div;
    return static_cast<int>(std::round(real_val));
}

bool AddNote(IndexedChart& chart, const string& note_id, const IndexList<int>& note)
{
    if (note_id == "a")
    {
        chart.ReplaceBT(BT::A, note);
        return true;
    }
    else if (note_id == "b")
    {
        chart.ReplaceBT(BT::B, note);
        return true;
    }
    else if (note_id == "c")
    {
        chart.ReplaceBT(BT::C, note);
        return true;
    }
    else if (note_id == "d")
    {
        chart.ReplaceBT(BT::D, note);
        return true;
    }
    else if (note_id == "l")
    {
        chart.ReplaceFX(FX::L, note);
        return true;
    }
    else if (note_id == "r")
    {
        chart.ReplaceFX(FX::R, note);
        return true;
    }
    else
    {
        return false;
    }
}

}  // namespace NoteApprox_Core

vector<string> NoteApprox::AcceptedCmdName()
{
    return {"note"};
}

bool NoteApprox::CheckArgs(const Command& cmd)
{
    // note [note_type] [start] ([end])
    bool valid = cmd.argLength() >= 2 && (cmd.argIsDouble(1) || cmd.argIsRatio(1));
    if (cmd.argLength() == 3)
    {
        valid &= cmd.argIsDouble(2) || cmd.argIsRatio(2);
    }
    else if (cmd.argLength() > 3)
    {
        valid = false;
    }

    return valid;
}

namespace core = NoteApprox_Core;

bool NoteApprox::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    bool success = true;
    // STEP 1 获取信息
    const string& note_id = cmd.arg(0);
    double start_time = cmd.argAsDoubleOrRatio(1);
    bool long_note = (cmd.argLength() == 3);
    double end_time = long_note ? cmd.argAsDoubleOrRatio(2) : NAN;

    if(end_time <= start_time)
    {
        GET_ERROR_COLLECTOR.ErrorLog("Note end time is ahead of start time.");
        return false;
    }

    // STEP 2 执行
    IndexList<int> new_note;
    if (long_note)
    {
        new_note.insert(cmd.time() + core::Approximate(start_time), 2);
        new_note.insert(cmd.time() + core::Approximate(end_time), 0);
    }
    else
    {
        new_note.insert(cmd.time() + core::Approximate(start_time), 1);
    }

    success = core::AddNote(chart, note_id, new_note);

    return success;
}