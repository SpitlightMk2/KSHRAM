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

#include "write_mark.h"

#include <string>

using namespace std;

namespace WriteMark_Core
{

MarkType MarkTypeFromString(const std::string& mark_str)
{
    // Command已经帮我们做过小写转化了
    if(mark_str == "bpm")
    {
        return MarkType::BPM;
    }
    else if(mark_str == "sig" || mark_str == "signature")
    {
        return MarkType::TimeSignature;
    }
    else if(mark_str == "fxlong_l" || mark_str == "fxlong_r")
    {
        return MarkType::FXLong;
    }
    else if(mark_str == "fxchip_l" || mark_str == "fxchip_r")
    {
        return MarkType::FXChip;
    }
    else if(mark_str == "laser2x_l" || mark_str == "laser2x_r")
    {
        return MarkType::Laser2x;
    }
    else if(mark_str == "filter")
    {
        return MarkType::Filter;
    }
    else if(mark_str == "slamsound")
    {
        return MarkType::SlamSound;
    }
    else if(mark_str == "knobvol")
    {
        return MarkType::KnobVolume;
    }
    else if(mark_str == "slamvol")
    {
        return MarkType::SlamVolume;
    }
    else if(mark_str == "zt" || mark_str == "zoomtop")
    {
        return MarkType::ZoomTop;
    }
    else if(mark_str == "zb" || mark_str == "zoombottom")
    {
        return MarkType::ZoomBottom;
    }
    else if(mark_str == "zs" || mark_str == "zoomside")
    {
        return MarkType::ZoomSide;
    }
    else if(mark_str == "tilt")
    {
        return MarkType::Tilt;
    }
    else if(mark_str == "stop")
    {
        return MarkType::Stop;
    }
    else if(mark_str == "split")
    {
        return MarkType::LaneSplit;
    }

    return MarkType::Error;
}

Side MarkSideFromString(const std::string& mark_str)
{
    // Command已经帮我们做过小写转化了
    if(mark_str == "fxlong_r" || mark_str == "fxchip_r" || mark_str == "laser2x_r")
    {
        return Side::R;
    }
    else {
        return Side::L;
    }
}

}

namespace core = WriteMark_Core;

vector<string> WriteMark::AcceptedCmdName() { return {"mark"}; }

bool WriteMark::CheckArgs(const Command& cmd)
{
    // mark [type] [start_val] ([end_val])
    return cmd.argLength() == 2 || cmd.argLength() == 3;
}

bool WriteMark::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    // 该命令不检查用户给的值是否合法，只是往上填。
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    MarkType type = core::MarkTypeFromString(cmd.arg(0));
    Side side = core::MarkSideFromString(cmd.arg(0));
    if(type == MarkType::Error)
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
    }


    auto& mark_list = chart.MarkList(type, side);

    // 特殊：laser2x肯定只会写2x
    if(type ==MarkType::Laser2x)
    {
        mark_list.insert(cmd.time(), "2x");
    }
    else if(cmd.argLength() == 2)
    {
        mark_list.insert(cmd.time(), cmd.arg(1));
    }
    else /* if(cmd.argLength() == 3) */
    {
        mark_list.insert(cmd.time(), cmd.arg(1), cmd.arg(2));
    }
    
    return true;
}