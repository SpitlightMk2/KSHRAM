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

#include "swing.h"

#include <string>

#include "src/Application/Shared/knob_query.h"

using namespace std;

namespace Swing_Core
{

/* #region assist func */

inline CommandMap::Iterator FindEndCommand(const Command& cmd, CommandMap& cmd_map)
{
    int start_time = cmd.time();
    string end_cmd = "end " + cmd.cmd();
    auto iter = cmd_map.FindNearestCommand(end_cmd, start_time);

    return iter;
}

inline void RemoveEndCommand(CommandMap& cmd_map, CommandMap::Iterator iter) { cmd_map.erase(iter); }

/* #endregion */

/* #region Swing Exec */

template <typename TList, std::enable_if_t<std::is_void_v<typename TList::IsIndexList>, bool> = true,
          typename TIter = typename TList::Iterator>
void ExecSwing(TList& list, int start_time, int end_time, int div, int delay, int start_beat)
{
    int step = 192 / div;
    int double_step = 2 * step;

    TIter start_iter = list.nextItem(start_time - 1);
    TIter end_iter = list.nextItem(end_time - 1);
    for (TIter iter = start_iter; iter != end_iter; ++iter)
    {
        int time = iter->first;
        int local_time = time - start_beat;
        if (local_time % double_step == step)
        {
            list.changeKey(iter, time + delay);
        }
    }
}

void ExecSwing_Knob(IndexedChart::KnobListType& list, int start_time, int end_time, int div, int delay, int start_beat)
{
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    int step = 192 / div;
    int double_step = 2 * step;

    auto start_iter = list.nextItem(start_time - 1);
    auto end_iter = list.nextItem(end_time - 1);
    for (auto iter = start_iter; iter != end_iter; ++iter)
    {
        int time = iter->first;
        int local_time = time - start_beat;

        if (local_time % double_step == step)
        {
            if (IsSlam(iter))
            {
                // [iter] 直角 Time : A -> B
                // [iter + 1] 结束 Time + 6 : B (-> -1)
                // [iter + 2] 下一个旋钮的关键点
                auto iter_next = iter;
                ++iter_next;
                auto iter_next2 = iter_next;
                ++iter_next2;
                int time_end = iter_next->first;
                int time_next_knob = iter_next2 == list.end() ? INT32_MAX : iter_next2->first;
                // 要和下一个旋钮关键点撞车的情况
                if (time_end + delay + 1 >= time_next_knob)
                {
                    err_collector.WarningLog("knob slam here is not suitable for swinging.");
                }
                else
                {
                    list.changeKey(iter, time + delay);
                    list.changeKey(iter_next, time_end + delay);
                }
            }
            else
            {
                list.changeKey(iter, time + delay);
            }
        }
    }
}

void ExecSwing(CommandMap& cmd_map, int start_time, int end_time, int div, int delay, int start_beat)
{
    int step = 192 / div;
    int double_step = 2 * step;

    auto start_iter = cmd_map.lower_bound(start_time + 1);
    auto end_iter = cmd_map.lower_bound(end_time);
    for (auto iter = start_iter; iter != end_iter;)
    {
        int time = iter->first;
        int local_time = time - start_beat;
        if (local_time % double_step == step)
        {
            cmd_map.ChangeKey(iter, time + delay);  // 会自动自增iter
        }
        else
        {
            ++iter;
        }
    }
}

void ExecSwing(IndexedChart& chart, CommandMap& cmd_map, int start_time, int end_time, int div, int delay,
               int start_beat)
{
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;

    // BT
    for (int i = 0; i < 4; ++i)
    {
        auto& bt_list = chart.BTList(BTByIndex(i));
        ExecSwing(bt_list, start_time, end_time, div, delay, start_beat);
    }

    // FX
    for (int i = 0; i < 2; ++i)
    {
        auto& fx_list = chart.FXList(FXByIndex(i));
        ExecSwing(fx_list, start_time, end_time, div, delay, start_beat);
    }

    // Knob
    for (int i = 0; i < 2; ++i)
    {
        auto& knob_list = chart.KnobList(KnobByIndex(i));
        ExecSwing_Knob(knob_list, start_time, end_time, div, delay, start_beat);
    }

    // Mark
    for (int i = 0; i < MarkTypesCount; ++i)
    {
        auto& mark_list = chart.MarkList(MarkByIndex(i), MarkSideByIndex(i));
        ExecSwing(mark_list, start_time, end_time, div, delay, start_beat);
    }

    ExecSwing(chart.SpinEffectList(), start_time, end_time, div, delay, start_beat);
    ExecSwing(chart.CommentList(), start_time + 1, end_time - 1, div, delay, start_beat);
    ExecSwing(chart.OtherItemsList(), start_time, end_time, div, delay, start_beat);
    ExecSwing(cmd_map, start_time, end_time, div, delay, start_beat);
}

/* #endregion */

}  // namespace Swing_Core

namespace core = Swing_Core;

vector<string> Swing::AcceptedCmdName() { return {"swing"}; }

bool Swing::CheckArgs(const Command& cmd)
{
    // swing [target_div(int)] [delay(double / ratio)]
    return cmd.argLength() == 2 && cmd.argIsInt(0) && (cmd.argIsDouble(1) || cmd.argIsRatio(1));
}

bool Swing::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    // STEP 1 获取信息
    ErrorCollector& err_collector = GET_ERROR_COLLECTOR;
    int start_time = cmd.time();
    auto end_cmd_iter = core::FindEndCommand(cmd, cmd_map);

    if (end_cmd_iter == cmd_map.end())
    {
        string end_cmd = "end " + cmd.cmd();
        err_collector.ErrorLog(ErrorMessage<ErrorType::EndCommandNotFound>() + "\n End command syntax: " + end_cmd);
        return false;
    }

    int end_time = end_cmd_iter->first;

    int div = cmd.argAsInt(0);
    int delay = static_cast<int>(round(cmd.argAsDoubleOrRatio(1, 48.0)));
    if (delay >= 192 / div)
    {
        err_collector.ErrorLog("Param delay is too large for a swing");
        return false;
    }

    int ref_beat = 0;
    auto time_sig_list = chart.MarkList(MarkType::TimeSignature);
    auto iter = time_sig_list.prevItem(start_time);
    if (iter != time_sig_list.end())
    {
        ref_beat = iter->first;
    }

    // STEP 2 执行
    core::ExecSwing(chart, cmd_map, start_time, end_time, div, delay, ref_beat);

    // STEP 3 删除结束命令
    core::RemoveEndCommand(cmd_map, end_cmd_iter);

    return true;
}