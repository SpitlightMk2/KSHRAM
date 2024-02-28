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

#include "smooth_camera_increment.h"

#include <vector>

#include "src/misc/enums.h"
#include "src/misc/utilities.h"
#include "../Shared/curve_generator.h"

using namespace std;
using namespace CurveGen;

namespace SmoothCameraIncrement_Core
{

enum class ProcessType
{
    Normal,
    Additive
};

/* #region assist func */

MarkType CmdMarkType(const std::string& str)
{
    if (str == "ztadd")
    {
        return MarkType::ZoomTop;
    }
    else if (str == "zbadd")
    {
        return MarkType::ZoomBottom;
    }
    else if (str == "zsadd")
    {
        return MarkType::ZoomSide;
    }
    else if (str == "tiltadd")
    {
        return MarkType::Tilt;
    }
    else if (str == "splitadd")
    {
        return MarkType::LaneSplit;
    }
    else
    {
        return MarkType::Error;
    }
}

/* #endregion */

}  // namespace SmoothCameraIncrement_Core

namespace core = SmoothCameraIncrement_Core;

vector<string> SmoothCameraIncrement::AcceptedCmdName()
{
    return {
        "ztadd", "zbadd", "zsadd", "tiltadd", "splitadd"};
}

bool SmoothCameraIncrement::CheckArgs(const Command& cmd)
{
    // ztadd [mode] [length(float, ratio)] [divisor(int)] [offset(float)] (curvetype) (amp(double))
    if (cmd.argLength() >= 4 && cmd.argIsInt(2) && cmd.argIsDouble(3) && (cmd.argIsDouble(1) || cmd.argIsRatio(1)))
    {
        if (cmd.argLength() == 6)
        {
            return cmd.argIsDouble(5);
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

bool SmoothCameraIncrement::ProcessCmd(const Command& cmd, CommandMap& /*cmd_map*/, IndexedChart& chart)
{
    // STEP 1 判断类型
    MarkType mark_type = core::CmdMarkType(cmd.cmd());

    // STEP 2 获取信息
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    int start_time = cmd.time();
    auto& mark_list = chart.MarkList(mark_type);

    auto iter = mark_list.prevItem(start_time);
    if (iter == mark_list.end())
    {
        // 如果找不到，说明此前没有任何标记，那么就添加默认值0
        mark_list.insert(start_time, "0");
    }
    else if (!IsFloat(iter->second.second()))
    {
        err_collector.ErrorLog(ErrorMessage<ErrorType::ParamIsNotNumber>());
        return false;
    }

    int length = static_cast<int>(round(cmd.argAsDoubleOrRatio(1) * 48.0));

    const string& mode = cmd.arg(0);
    if(!CameraCurve_Additive_TypeCheck(mode))
    {
        err_collector.ErrorLog("Unknown mode enum at args[0].");
        return false;
    }
    int step = 192 / cmd.argAsInt(2);
    double offset = cmd.argAsDouble(3);
    bool roundInt = (mark_type != MarkType::Tilt);

    unique_ptr<ICurveFunctor> curve_functor = nullptr;

    if (cmd.argLength() >= 5)
    {
        const string& curve_type = cmd.arg(4);
        curve_functor = NewCurveFunctor(curve_type);
        if (curve_functor == nullptr)
        {
            err_collector.ErrorLog(ErrorMessage<ErrorType::InvalidArgs>());
            return false;
        }
    }
    else
    {
        curve_functor = make_unique<SqrType1>();
    }

    if (cmd.argLength() == 6)
    {
        curve_functor->amp = cmd.argAsDouble(5);
    }

    // STEP 3 生成
    DoubleCurveData data = CameraCurve_Additive(mode, *curve_functor, offset, length, step, roundInt);

    // STEP 4 组装
    auto curve_map = AssembleIncrementCameraData(data, start_time, mark_list);

    chart.ReplaceMark(mark_type, curve_map);

    return true;
}