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

#include "curve_knob.h"

#include <vector>

#include "../Shared/curve_generator.h"
#include "Shared/knob_query.h"
#include "src/misc/enums.h"

using namespace std;
using namespace CurveGen;

std::vector<std::string> CurveKnob::AcceptedCmdName()
{
    return {
        "cl", "curvel", "cr", "curver"};
}

bool CurveKnob::CheckArgs(const Command& cmd)
{
    // cl curvetype reverse [mode] [amp(float)]
    if (cmd.argLength() >= 2 && cmd.argLength() <= 4 && cmd.argIsBool(1))
    {
        if (cmd.argLength() == 4)
        {
            return cmd.argIsDouble(3);
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool CurveKnob::ProcessCmd(const Command& cmd, CommandMap& /*cmd_map*/, IndexedChart& chart)
{
    // STEP 1 判断命令类型
    Knob knob;
    if (cmd.cmd() == "cl" || cmd.cmd() == "curvel")
    {
        knob = Knob::L;
    }
    else if (cmd.cmd() == "cr" || cmd.cmd() == "curver")
    {
        knob = Knob::R;
    }
    else
    {
        return false;
    }

    // STEP 2 获取信息
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    int start_time = cmd.time();
    auto& knob_list = chart.KnobList(knob);
    bool slam = IsSlam(knob_list, start_time);
    if (slam)
    {
        start_time += 6;
    }

    auto iter = knob_list.nextItem(start_time);
    if (iter == knob_list.end())
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::ObjectNotFound>());
        return false;
    }

    int end_time = iter->first;

    auto iter2 = knob_list.prevItem(start_time);
    if (iter2 == knob_list.end())
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::ObjectNotFound>());
        return false;
    }

    int start_pos = iter2->second.second();
    int end_pos = iter->second.first();

    if (start_pos == -1 || end_pos == -1)
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::ObjectNotFound>());
        return false;
    }
    if (start_pos == end_pos)
    {
        err_collector.ErrorLog("Knob is straight(no position change).");
        return false;
    }

    int length = end_time - start_time;
    bool laser2x = IsLaser2x(chart, knob, start_time);
    bool reverse = cmd.argAsBool(1);

    string mode = "";
    if (cmd.argLength() >= 3)
    {
        mode = cmd.arg(2);
    }

    // STEP 3 生成曲线
    if (reverse)
    {
        std::swap(start_pos, end_pos);
    }

    unique_ptr<ICurveFunctor> pfunc(NewCurveFunctor(cmd.arg(0)));
    if (pfunc == nullptr)
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
        return false;
    }

    if (cmd.argLength() >= 4)
    {
        pfunc->amp = cmd.argAsDouble(3);
    }

    IntCurveData data;

    if (mode == "s" || mode == "static")
    {
        if (length < 16)
        {
            err_collector.ErrorLog("Knob line is too short to generate a decent curve.");
            return false;
        }

        if (slam && (length - 42) % 8 == 0)
        {
            data = KnobCurve_AfterSlam(*pfunc, start_pos, end_pos, length, reverse, 8, laser2x);
        }

        else if (length == 42)
        {
            data = KnobCurve_Static(*pfunc, start_pos, end_pos, length, reverse, 7, laser2x);
        }

        else
        {
            data = KnobCurve_Static(*pfunc, start_pos, end_pos, length, reverse, 8, laser2x);
        }
    }
    // 历史原因，支持d2标记，但是文档里面不提供。
    else if (mode == "d" || mode == "d2" || mode == "dynamic")
    {
        if (length < 42 && length % 8 == 0)
        {
            err_collector.WarningLog(
                "Knob length is so short that dynamic mode will hardly perform better than static mode.");
        }
        data = KnobCurve_Dynamic(*pfunc, start_pos, end_pos, length, reverse, laser2x);
    }
    else if (mode == "g" || mode == "global")
    {
        if (length < 42)
        {
            err_collector.WarningLog(
                "Knob length is so short that global mode will hardly perform better than static mode.");
        }
        data = KnobCurve_Global(*pfunc, start_pos, end_pos, length, reverse, laser2x);
    }
    else if (mode.empty())
    {
        if (length <= 48)
        {
            if (length < 16)
            {
                err_collector.ErrorLog("Knob line is too short to generate a decent curve.");
                return false;
            }
            if (length % 7 == 0 || length % 7 == 1)
            {
                data = KnobCurve_Static(*pfunc, start_pos, end_pos, length, reverse, 7, laser2x);
            }
            else if (length % 8 == 0 || length % 8 == 1)
            {
                data = KnobCurve_Static(*pfunc, start_pos, end_pos, length, reverse, 8, laser2x);
            }
            else
            {
                data = KnobCurve_Dynamic(*pfunc, start_pos, end_pos, length, reverse, laser2x);
            }
        }
        else if (length <= 96)
        {
            data = KnobCurve_Dynamic(*pfunc, start_pos, end_pos, length, reverse, laser2x);
        }
        else
        {
            data = KnobCurve_Global(*pfunc, start_pos, end_pos, length, reverse, laser2x);
        }
    }
    else
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
        return false;
    }

    if (reverse)
    {
        for (auto& item : data)
        {
            item.time = length - item.time;
        }
    }

    // STEP 4 组装和修改
    auto curve_map = AssembleCurveData(data, start_time);

    chart.ReplaceKnob(knob, curve_map);

    return true;
}