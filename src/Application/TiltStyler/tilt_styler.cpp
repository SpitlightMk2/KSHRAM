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

#include "tilt_styler.h"

#include <functional>
#include <string>

#include "TiltStyler/tilt_style.h"
#include "TiltStyler/style_config.h"

#include "src/Application/Shared/knob_query.h"


using namespace std;

namespace TiltStyler_Core
{

/* #region assist func */

inline CommandMap::Iterator FindEndCommand(const Command& cmd, CommandMap& cmd_map)
{
    int start_time = cmd.time();
    int delay = cmd.delay();
    string end_cmd = "end " + cmd.cmd();
    auto iter = cmd_map.FindNearestCommand(end_cmd, start_time, delay);

    return iter;
}

inline void RemoveEndCommand(CommandMap& cmd_map, CommandMap::Iterator iter) { cmd_map.erase(iter); }

/* #endregion */

}  // namespace TiltStyler_Core

namespace core = TiltStyler_Core;

vector<string> TiltStyler::AcceptedCmdName() { return {"tiltstyle"}; }

bool TiltStyler::CheckArgs(const Command& cmd)
{
    // tiltstyle [style] ([amp])
    return cmd.argLength() >= 1;
}

void TiltStyler::RegisterTiltStyleCmd(const Command& cmd, bool with_style)
{
    // 假定cmd已经通过了语法检测
    // 提取数据
    double amp = 1.0;
    vector<string> styles;
    int cmd_last_index = cmd.argLength() - 1;
    if (cmd.argIsDouble(cmd_last_index))
    {
        amp = cmd.argAsDouble(cmd_last_index);
        cmd_last_index -= 1;
    }
    for (int i = 0; i <= cmd_last_index; ++i)
    {
        styles.push_back(cmd.arg(i));
    }

    // 生成style
    core::StyleConfig config;
    config.ConfigFromStr(styles, with_style);
    config.pleft->amp *= amp;
    config.pright->amp *= amp;

    style_map.insert_or_assign(cmd.time(), std::move(config));
}

bool TiltStyler::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    style_map.clear();
    // 起始和结束时间
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    auto end_cmd = core::FindEndCommand(cmd, cmd_map);
    if (end_cmd == cmd_map.end())
    {
        string end_cmd = "end " + cmd.cmd();
        err_collector.ErrorLog(ErrorMessage<ErrorType::EndCommandNotFound>() + "\n End command syntax: " + end_cmd);
        return false;
    }
    // 生成tilt用
    int start_time = cmd.time();
    int end_time = end_cmd->first;
    // 裁剪tilt用
    int start_clamp_time = start_time;
    int end_clamp_time = end_time;
    int delay = cmd.delay();

    // 登记首个tiltstyle命令
    RegisterTiltStyleCmd(cmd, true);
    // 将这中间所有的tiltstyle命令登记到表中
    auto cmd_iter = cmd_map.upper_bound(start_time);
    while (cmd_iter != end_cmd)
    {
        const Command& the_cmd = cmd_iter->second;
        if (the_cmd.matchesType("tiltstyle") && CheckArgs(the_cmd) && cmd.delay() == delay)
        {
            this->RegisterTiltStyleCmd(the_cmd);
            cmd_map.erase(cmd_iter);
        }
        else
        {
            ++cmd_iter;
        }
    }

    if(style_map.begin()->second.styler == nullptr)
    {
        style_map.begin()->second.styler = core::TiltStyle_KSM;
    }

    // 调整起始时间
    if (IsStart(chart, Knob::L, start_time) || IsStart(chart, Knob::R, start_time))
    {
        start_time = max(0, start_time - 96);
    }
    // 调整结束时间
    if(IsEnd(chart, Knob::L, end_time) || IsEnd(chart, Knob::R, end_time))
    {
        end_time += 96;
    }

    IndexList<double> tilt_guide = core::MakeTiltGuide(chart, start_time, end_time, style_map);

    IndexList<string> tilt = style_map.begin()->second.styler(tilt_guide, 6);

    // 裁剪，使得只在用户指定的范围内铺设tilt命令
    tilt = tilt.innerSublist(start_clamp_time, end_clamp_time);

    chart.ReplaceMark(MarkType::Tilt, tilt);

    core::RemoveEndCommand(cmd_map, end_cmd);

    return true;
}