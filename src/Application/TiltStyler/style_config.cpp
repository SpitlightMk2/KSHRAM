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

#include "style_config.h"

#include "TiltStyler/tilt_guide.h"


using namespace std;
using namespace TiltStyler_Core;

void StyleConfig::ConfigFromStr(const vector<string>& conf, bool with_style)
{
    // 设置默认值
    if(with_style)
    {
        styler = TiltStyle_KSM;
    }
    
    for (const string& str : conf)
    {
        // tilt pos
        if (str == "side")
        {
            pleft = std::make_unique<MaxTiltL>();
            pright = std::make_unique<MaxTiltR>();
        }
        else if (str == "mid")
        {
            pleft = std::make_unique<MidTilt>();
            pright = std::make_unique<MidTilt>();
        }
        else if(str == "keep")
        {
            pleft = std::make_unique<MaxTiltL_Keep>();
            pright = std::make_unique<MaxTiltR_Keep>();
        }

        // laser2x
        if (str == "2x")
        {
            laser2x_extend = true;
        }

        // 仅单侧输入
        if (str == "left")
        {
            zero_right = true;
            zero_left = false;
        }
        else if (str == "right")
        {
            zero_left = true;
            zero_right = false;
        }

        // tilt style
        if (with_style)
        {
            if (str == "ksm")
            {
                styler = TiltStyle_KSM;
            }
            else if (str == "uniform")
            {
                styler = TiltStyle_Uniform;
            }
            else if (str == "sudden")
            {
                styler = TiltStyle_Null;
            }
        }
    }
}

IndexList<double> TiltStyler_Core::MakeTiltGuide(
    IndexedChart& chart, int start, int end, std::map<int, StyleConfig>& func_map)
{
    IndexList<double> output;

    int start_time = 0, end_time = 0;
    auto iter_end = func_map.end();
    auto iter_last = iter_end;
    --iter_last;
    for (auto iter = func_map.begin(); iter != iter_end; ++iter)
    {
        // 确定开始和结束时间
        if (iter == func_map.begin())
        {
            start_time = start;
        }
        else
        {
            start_time = iter->first;
        }
        if (iter == iter_last)
        {
            end_time = end;
        }
        else
        {
            auto iter_next = iter;
            ++iter_next;
            end_time = iter_next->first;
        }

        StyleConfig& config = iter->second;

        // 获取旋钮位置
        IndexList<double> knob_left, knob_right;

        if (config.laser2x_extend)
        {
            knob_left = chart.KnobPosList(Knob::L);
            knob_right = chart.KnobPosList(Knob::R);
        }
        else
        {
            knob_left = chart.KnobList(Knob::L);
            knob_right = chart.KnobList(Knob::R);
        }

        // 生成各段的tiltguide
        IndexList<double> tilt_left = config.zero_left ? IndexList<double>() : 
            MakeTiltGuide(knob_left, start_time, end_time, *config.pleft);
        IndexList<double> tilt_right = config.zero_right ? IndexList<double>() : 
            MakeTiltGuide(knob_right, start_time, end_time, *config.pright);

        IndexList<double> tilt_guide = MergeTiltGuide(tilt_left, tilt_right);
        tilt_guide = tilt_guide.clamp(start_time, end_time);

        // 拼接到output上面
        auto first_entry = tilt_guide.first();
        if (output.hasKey(first_entry.first))
        {
            output[first_entry.first].setSecond(first_entry.second.second());
        }
        else
        {
            output.insert(first_entry);
        }
        tilt_guide.erase(first_entry.first);
        output.insert(tilt_guide);
    }

    RegulateTiltGuide(output);

    return output;
}