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

#include "indexed_chart.h"

#include <fstream>
#include <iostream>
#include <numeric>

using namespace std;

/* #region import & export */

void IndexedChart::ImportFromChart(Chart& chart)
{
    this->total_time = chart.TotalTime();
    // BT
    for (int i = 0; i < 4; ++i)
    {
        BT bt = BTByIndex(i);
        this->bt_lists[i] = chart.BTIndexList(bt);
    }

    // FX, Knob
    for (int i = 0; i < 2; ++i)
    {
        FX fx = FXByIndex(i);
        Knob knob = KnobByIndex(i);
        this->fx_lists[i] = chart.FXIndexList(fx);
        this->knob_lists[i] = chart.KnobIndexList(knob);
    }

    // Mark
    for (int i = 0; i < MarkTypesCount; ++i)
    {
        MarkType mark = MarkByIndex(i);
        Side side = MarkSideByIndex(i);
        this->mark_lists[i] = chart.MarkIndexList(mark, side);
    }

    // Spin Effect
    this->spin_effect_list = chart.SpinEffectList();

    // Comment
    this->comment_list = chart.CommentIndexList();

    // Other Items
    this->other_items_list = chart.OtherItemIndexList();
}

/* #region Export -> Chart 辅助函数 */

// 获取子列表的Divisor
template <typename TList, std::enable_if_t<std::is_void_v<typename TList::IsIndexList>, bool> = true,
          typename T = typename TList::Component>
int TimespanOfList(TList& map, int start_time, int length)
{
    int timespan = 48;
    auto iter = map.nextItem(start_time - 1);
    auto end = map.nextItem(start_time + length - 1);

    while (iter != end)
    {
        timespan = gcd(timespan, iter->first - start_time);
        ++iter;
    }

    return timespan;
}

inline int FinerTimespan(int timespan)
{
    return max(1, timespan / 2);
}

// 获取旋钮子列表的Divisor
// 会额外判断是否要追加细分
int TimespanOfKnobList(IndexList<int>& map, int start_time, int length)
{
    int timespan = 48;
    auto start = map.nextItem(start_time - 1);
    auto end = map.nextItem(start_time + length - 1);

    // 正常求timespan
    auto iter = start;
    while (iter != end)
    {
        timespan = gcd(timespan, iter->first - start_time);
        ++iter;
    }
    // 第二轮遍历：查当前timespan是否需要再细化一次
    iter = start;
    int last_iter_time = iter->first;
    bool last_iter_is_knob_end = (iter->second.second() == -1);
    while (iter != end)
    {
        ++iter;
        if (iter != map.end())
        {
            if (last_iter_is_knob_end && iter->first - last_iter_time == timespan)
            {
                timespan = FinerTimespan(timespan);
                break;
            }
            last_iter_time = iter->first;
            last_iter_is_knob_end = (iter->second.second() == -1);
        }
    }

    return timespan;
}

// 对于已经扩充Entry数目的单个小节，写入BT数据
void Write(Measure& measure, IndexList<int>& sub_map, BT bt, Side side = Side::L)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // chip
        if (sub_map.hasKey(map_time) && sub_map.startVal(map_time) == 1)
        {
            entry.SetBTState(bt, KeyState::Chip);
        }
        else
        {
            auto prev = sub_map.prevItem(map_time);
            // none: 没有记录
            if (prev == sub_map.end())
            {
                entry.SetBTState(bt, KeyState::None);
            }
            else
            {
                int last_state = prev->second.second();
                // long
                if (last_state == 2)
                {
                    entry.SetBTState(bt, KeyState::Long);
                }
                // none
                else
                {
                    entry.SetBTState(bt, KeyState::None);
                }
            }
        }
    }
}

// 对于已经扩充Entry数目的单个小节，写入FX数据
void Write(Measure& measure, IndexList<int>& sub_map, FX fx, Side side = Side::L)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // chip
        if (sub_map.hasKey(map_time) && sub_map.startVal(map_time) == 1)
        {
            entry.SetFXState(fx, KeyState::Chip);
        }
        else
        {
            auto prev = sub_map.prevItem(map_time);
            // none: 没有记录
            if (prev == sub_map.end())
            {
                entry.SetFXState(fx, KeyState::None);
            }
            else
            {
                int last_state = prev->second.second();
                // long
                if (last_state == 2)
                {
                    entry.SetFXState(fx, KeyState::Long);
                }
                // none
                else
                {
                    entry.SetFXState(fx, KeyState::None);
                }
            }
        }
    }
}

// 对于已经扩充Entry数目的单个小节，写入旋钮数据
void Write(Measure& measure, IndexList<int>& sub_map, Knob knob, Side side = Side::L)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // 关键点
        if (sub_map.hasKey(map_time))
        {
            int start_val = sub_map.startVal(map_time);
            entry.SetKnobPos(knob, start_val);
        }
        // 非关键点
        else
        {
            auto iter = sub_map.prevItem(map_time);
            if (iter == sub_map.end())
            {
                entry.SetKnobPos(knob, -1);
            }
            else
            {
                int last_val = iter->second.second();
                if (last_val == -1)
                {
                    entry.SetKnobPos(knob, -1);
                }
                else
                {
                    entry.SetKnobPos(knob, 128);
                }
            }
        }
    }
}

// 对于已经扩充Entry数目的单个小节，写入Mark数据
void Write(Measure& measure, IndexList<string>& sub_map, MarkType mark, Side side)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // 关键点
        if (sub_map.hasKey(map_time))
        {
            // 第一个记录
            const string& value = sub_map.startVal(map_time);
            Mark temp(mark, side, value);
            entry.AddMark(temp);

            // 第二个记录
            if (!sub_map.valIsUniform(map_time))
            {
                const string& value = sub_map.endVal(map_time);
                Mark temp(mark, side, value);
                entry.AddMark(temp);
            }
        }
    }
}

// 对于已经扩充Entry数目的单个小节，写入SpinEffect数据
void Write(Measure& measure, IndexList<SpinEffect>& sub_map, Spin, Side = Side::L)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // 关键点
        if (sub_map.hasKey(map_time))
        {
            entry.GetSpinEffect() = sub_map.startVal(map_time);
        }
    }
}

// 对于已经扩充Entry数目的单个小节，写入注释
void Write(Measure& measure, IndexList<string>& sub_map, Comment, Side)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // 关键点
        if (sub_map.hasKey(map_time))
        {
            entry.Comments() = sub_map.startVal(map_time);
        }
    }
}

// 对于已经扩充Entry数目的单个小节，写入其他内容
void Write(Measure& measure, IndexList<string>& sub_map, Others, Side)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    for (int time = 0; time < length; time += measure.EntryTimespan())
    {
        Entry& entry = measure.EntryByLocalTime(time);
        int map_time = start_time + time;
        // 关键点
        if (sub_map.hasKey(map_time))
        {
            entry.OtherItems() = sub_map.startVal(map_time);
        }
    }
}

template <typename TList, typename EnumType, std::enable_if_t<std::is_void_v<typename TList::IsIndexList>, bool> = true,
          typename T = typename TList::Component>
void WriteFromMap(Measure& measure, TList& item_map, EnumType write_type, Side side = Side::L)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    // 切子列表
    auto sub_map = item_map.surroundingSublist(start_time, start_time + length);

    // 获取divisor
    int timespan = TimespanOfList(sub_map, start_time, length);

    measure.ExpandToTimespan(timespan);

    // 写入
    Write(measure, sub_map, write_type, side);
}

// Knob偏特化
template <>
void WriteFromMap(Measure& measure, IndexList<int>& item_map, Knob write_type, Side side)
{
    int start_time = measure.StartTime();
    int length = measure.TotalTimespan();

    // 切子列表
    auto sub_map = item_map.surroundingSublist(start_time, start_time + length);

    // 获取divisor
    int timespan = TimespanOfKnobList(sub_map, start_time, length);

    measure.ExpandToTimespan(timespan);

    // 写入
    Write(measure, sub_map, write_type, side);
}

/* #endregion */

/* Note:
* step1 开小节
* 使用time_signature的数据开好整个谱面的小节
*
* step2 逐个小节写数据
* 列表：BT表，FX表，旋钮表，mark，comment，other_item
* 用小节的起始结束时间切割表
* 用切割的子表获取divisor
* 根据divisor拓宽小节记录数量
* 写入切割后表内的每个数据
*/

Chart IndexedChart::ExportToChart()
{
    Chart output;
    // STEP 1 根据time signature表创建各小节
    int time = 0, i = 0;
    int sig_numer = 4, sig_denom = 4;
    IndexList<string>& sig_list = this->mark_lists[MarkIndex(MarkType::TimeSignature)];
    while (time < this->total_time)
    {
        auto last_mark = sig_list.prevItem(time);
        if (last_mark != sig_list.end())
        {
            auto [numer, denom] = ReadRatioI(last_mark->second.first());
            sig_numer = numer; sig_denom = denom;
        }

        Measure new_measure = Measure::BlankMeasure(sig_numer, sig_denom);
        new_measure.SetTimeSignature(sig_numer, sig_denom);
        output.AppendMeasure(new_measure);
        output.measure_at_time.insert(std::pair{time, i});

        time += new_measure.TotalTimespan();
        i += 1;
    }

    // STEP 2 填入信息
    for (auto& measure : output.measures)
    {
        // BT
        for (int i = 0; i < 4; ++i)
        {
            BT bt = BTByIndex(i);
            WriteFromMap(measure, this->bt_lists[i], bt);
        }
        // FX
        WriteFromMap(measure, this->fx_lists[0], FX::L);
        WriteFromMap(measure, this->fx_lists[1], FX::R);
        // 旋钮
        WriteFromMap(measure, this->knob_lists[0], Knob::L);
        WriteFromMap(measure, this->knob_lists[1], Knob::R);

        // Marks
        for (int i = 0; i < MarkTypesCount; ++i)
        {
            MarkType mark = MarkByIndex(i);
            Side side = MarkSideByIndex(i);
            WriteFromMap(measure, this->mark_lists[i], mark, side);
        }

        // SpinEffect
        WriteFromMap(measure, this->spin_effect_list, Spin::None);
        // Comments
        WriteFromMap(measure, this->comment_list, Comment::Others);
        // SpinEffect
        WriteFromMap(measure, this->other_items_list, Others::Others);
    }

    return output;
}

/* #region Import <- File 辅助函数 */

/*
* BT-A ~ BT-D			0, 0~3
* FX-L					1, 0
* FX-R					1, 1
* Knob-L				2, 0
* Knob-R				2, 1
* 
* Marks					3, 0-17
* 
* Spin Effect			4, 0
* Comment				5, 0
* Other Items			6, 0
* 
*/

int modeToID(const std::string& mode)
{
    if (mode == "BT-A" || mode == "FX-L" || mode == "Knob-L" || mode == "t" || mode == "Spin Effect" ||
        mode == "Comment" || mode == "Other Items")
    {
        return 0;
    }
    else if (mode == "BT-B" || mode == "FX-R" || mode == "Knob-R" || mode == "beat")
    {
        return 1;
    }
    else if (mode == "BT-C" || mode == "fx-l")
    {
        return 2;
    }
    else if (mode == "BT-D" || mode == "fx-r")
    {
        return 3;
    }
    else if (mode == "fx-l_se")
    {
        return 4;
    }
    else if (mode == "fx-r_se")
    {
        return 5;
    }
    else if (mode == "laserrange_l")
    {
        return 6;
    }
    else if (mode == "laserrange_r")
    {
        return 7;
    }
    else if (mode == "filtertype")
    {
        return 8;
    }
    else if (mode == "pfiltergain")
    {
        return 9;
    }
    else if (mode == "chokkakuse")
    {
        return 10;
    }
    else if (mode == "chokkakuvol")
    {
        return 11;
    }
    else if (mode == "tilt")
    {
        return 12;
    }
    else if (mode == "zoom_top")
    {
        return 13;
    }
    else if (mode == "zoom_bottom")
    {
        return 14;
    }
    else if (mode == "zoom_side")
    {
        return 15;
    }
    else if (mode == "stop")
    {
        return 16;
    }
    else if (mode == "center_split")
    {
        return 17;
    }
    else
    {
        return -1;
    }
}

int modeToType(const std::string& mode)
{
    if (mode == "BT-A" || mode == "BT-B" || mode == "BT-C" || mode == "BT-D")
    {
        return 0;
    }
    else if (mode == "FX-L" || mode == "FX-R")
    {
        return 1;
    }
    else if (mode == "Knob-L" || mode == "Knob-R")
    {
        return 2;
    }
    else if (mode == "Spin Effect")
    {
        return 4;
    }
    else if (mode == "Comments")
    {
        return 5;
    }
    else if (mode == "Other Items")
    {
        return 6;
    }
    else
    {
        return 3;  // marks
    }
}

void IndexedChart::InsertData(int map_type, int map_id, int time, const std::string& val)
{
    if (map_type == 0)
    {
        // BT
        this->bt_lists[map_id].insert(time, stoi(val));
    }
    else if (map_type == 1)
    {
        // FX
        this->fx_lists[map_id].insert(time, stoi(val));
    }
    else if (map_type == 2)
    {
        // Knobs
        auto val_list = Split(val, ',');
        int begin = stoi(val_list[0]);
        if (val_list.size() > 1)
        {
            int end = stoi(val_list[1]);
            this->knob_lists[map_id].insert(time, begin, end);
        }
        else
        {
            this->knob_lists[map_id].insert(time, begin);
        }
    }
    else if (map_type == 3)
    {
        // Marks
        if (map_id == -1)
        {
            // ERROR
            return;
        }
        auto val_list = Split(val, ',');
        if (val_list.size() > 1)
        {
            this->mark_lists[map_id].insert(time, val_list[0], val_list[1]);
        }
        else
        {
            this->mark_lists[map_id].insert(time, val_list[0]);
        }
    }
    else if (map_type == 4)
    {
        // Spin Effect
        this->spin_effect_list.insert(time, val);
    }
    else if (map_type == 5)
    {
        // Comments
        this->comment_list.insert(time, val);
    }
    else if (map_type == 6)
    {
        // Other Items
        this->other_items_list.insert(time, val);
    }
}

/* #endregion */

bool IndexedChart::ImportFromString(const std::string& content)
{
    istringstream ss(content);

    int type = -1, map_id = -1;
    int split_pos = -1;

    while (!ss.eof())
    {
        std::string line;
        getline(ss, line);

        if (BeginWith(line, "total_time"))
        {
            this->total_time = stoi(line.substr(11));
        }
        else if (BeginWith(line, "section"))
        {
            std::string mode_str = line.substr(8);
            type = modeToType(mode_str);
            map_id = modeToID(mode_str);
        }
        else if ((split_pos = line.find(':')) != string::npos)
        {
            int time = stoi(line.substr(0, split_pos - 1));
            this->InsertData(type, map_id, time, line.substr(split_pos + 2));
        }
    }

    return true;
}

std::string IndexedChart::ExportToString()
{
    ostringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, IndexedChart ic)
{
    os << "total_time=" << ic.total_time << CRLF();
    os << "-----------" << CRLF();

    for (int i = 0; i < 4; ++i)
    {
        os << "section BT-" << static_cast<char>('A' + i) << CRLF();
        os << ic.bt_lists[i] << CRLF();
    }

    os << "section FX-L" << CRLF();
    os << ic.fx_lists[0] << CRLF();
    os << "section FX-R" << CRLF();
    os << ic.fx_lists[1] << CRLF();

    os << "section Knob-L" << CRLF();
    os << ic.knob_lists[0] << CRLF();
    os << "section Knob-R" << CRLF();
    os << ic.knob_lists[1] << CRLF();

    for (int i = 0; i < MarkTypesCount; ++i)
    {
        MarkType mark = MarkByIndex(i);
        Side side = MarkSideByIndex(i);
        os << "section " << MarkStr(mark, side) << CRLF();
        os << ic.mark_lists[i] << CRLF();
    }

    os << "section Spin Effect" << CRLF();
    os << ic.spin_effect_list << CRLF();

    os << "section Comments" << CRLF();
    os << ic.comment_list << CRLF();

    os << "section Other Items" << CRLF();
    os << ic.other_items_list;

    return os;
}

/* #endregion */

/* #region 数据统计 */

int IndexedChart::CalculateTotalTime() const
{
    int total_time = 0;
    // BT
    for (auto& lst : this->bt_lists)
    {
        total_time = max(total_time, lst.last().first);
    }

    // FX
    for (auto& lst : this->fx_lists)
    {
        total_time = max(total_time, lst.last().first);
    }

    // Knob
    for (auto& lst : this->knob_lists)
    {
        total_time = max(total_time, lst.last().first);
    }

    // Marks
    for (auto& lst : this->mark_lists)
    {
        total_time = max(total_time, lst.last().first);
    }

    // Spin Effects
    total_time = max(total_time, spin_effect_list.last().first);

    // Comments
    total_time = max(total_time, comment_list.last().first);

    // Others
    total_time = max(total_time, other_items_list.last().first);

    return total_time;
}

/* #endregion */

/* #region 衍生表计算 */

IndexList<double> IndexedChart::KnobPosList(Knob knob) const
{
    auto& knob_list = this->knob_lists[KnobIndex(knob)];
    auto& laser2x_list = this->mark_lists[MarkIndex(MarkType::Laser2x, ToSide(knob))];

    IndexList<double> output;

    bool continuing_knob = false;
    bool laser2x = false;

    for (auto& item : knob_list)
    {
        // 判断laser2x
        laser2x = (continuing_knob && laser2x) ||
                  (!continuing_knob && item.second.first() != -1 && laser2x_list.hasKey(item.first));
        continuing_knob = item.second.second() != -1;

        // 写入值
        double start_val = 0.0, end_val = 0.0;
        if (laser2x)
        {
            start_val = item.second.first() == -1 ? -1 : 2 * ToKnobPos(item.second.first(), true) - 25;
            end_val = item.second.second() == -1 ? -1 : 2 * ToKnobPos(item.second.second(), true) - 25;
        }
        else
        {
            start_val = item.second.first();
            end_val = item.second.second();
        }
        if (item.second.isSame())
        {
            output.insert(item.first, start_val);
        }
        else
        {
            output.insert(item.first, start_val, end_val);
        }
    }

    return output;
}

/* #endregion */

/* #region modify */

void IndexedChart::ReplaceBT(BT bt, const IndexList<int>& lst, int offset)
{
    // 空输入保护
    if(lst.empty())
    {
        return;
    }
    
    IndexList<int>& bt_list = this->bt_lists[BTIndex(bt)];
    int start_time = lst.first().first + offset;
    int end_time = lst.last().first + offset;
    this->UpdateTotalTime(end_time);

    bt_list.erase(start_time, end_time);

    if (offset == 0)
    {
        bt_list.insert(lst);
    }
    else
    {
        bt_list.insert(lst.offset(offset));
    }
}

void IndexedChart::ReplaceFX(FX fx, const IndexList<int>& lst, int offset)
{
    // 空输入保护
    if(lst.empty())
    {
        return;
    }
    
    IndexList<int>& fx_list = this->fx_lists[FXIndex(fx)];
    int start_time = lst.first().first + offset;
    int end_time = lst.last().first + offset;
    this->UpdateTotalTime(end_time);

    fx_list.erase(start_time, end_time);

    if (offset == 0)
    {
        fx_list.insert(lst);
    }
    else
    {
        fx_list.insert(lst.offset(offset));
    }
}

void IndexedChart::ReplaceKnob(Knob knob, const IndexList<int>& lst, int offset)
{
    // 空输入保护
    if(lst.empty())
    {
        return;
    }

    IndexList<int>& knob_list = this->knob_lists[KnobIndex(knob)];
    int start_time = lst.first().first + offset;
    int end_time = lst.last().first + offset;
    this->UpdateTotalTime(end_time);

    knob_list.erase(start_time, end_time);

    if (offset == 0)
    {
        knob_list.insert(lst);
    }
    else
    {
        knob_list.insert(lst.offset(offset));
    }
}

void IndexedChart::ReplaceMark(MarkType mark, const IndexList<string>& lst, int offset)
{
    // 空输入保护
    if(lst.empty())
    {
        return;
    }

    IndexList<string>& mark_list = this->mark_lists[MarkIndex(mark)];
    int start_time = lst.first().first + offset;
    int end_time = lst.last().first + offset;
    this->UpdateTotalTime(end_time);

    mark_list.erase(start_time, end_time);

    if (offset == 0)
    {
        mark_list.insert(lst);
    }
    else
    {
        mark_list.insert(lst.offset(offset));
    }
}

void IndexedChart::ReplaceMark(MarkType mark, Side side, const IndexList<string>& lst, int offset)
{
    // 空输入保护
    if(lst.empty())
    {
        return;
    }

    IndexList<string>& mark_list = this->mark_lists[MarkIndex(mark, side)];
    int start_time = lst.first().first + offset;
    int end_time = lst.last().first + offset;
    this->UpdateTotalTime(end_time);

    mark_list.erase(start_time, end_time);

    if (offset == 0)
    {
        mark_list.insert(lst);
    }
    else
    {
        mark_list.insert(lst.offset(offset));
    }
}

void IndexedChart::Offset(int offset_val)
{
    // BT
    for (int i = 0; i < 4; ++i)
    {
        this->bt_lists[i] = this->bt_lists[i].offset(offset_val);
    }
    // FX
    for (int i = 0; i < 2; ++i)
    {
        this->fx_lists[i] = this->fx_lists[i].offset(offset_val);
    }
    // Knob
    for (int i = 0; i < 2; ++i)
    {
        this->knob_lists[i] = this->knob_lists[i].offset(offset_val);
    }
    // Mark
    for (int i = 0; i < MarkTypesCount; ++i)
    {
        this->mark_lists[i] = this->mark_lists[i].offset(offset_val);
    }
    // misc
    this->comment_list = this->comment_list.offset(offset_val);
    this->spin_effect_list = this->spin_effect_list.offset(offset_val);
    this->other_items_list = this->other_items_list.offset(offset_val);

    this->total_time += offset_val;
}

/* #endregion */

/* #region slicing & merge */

IndexedChart IndexedChart::Slice(int start_time, int length) const
{
    IndexedChart output;

    int end_time = start_time + length;
    // BT
    for (int i = 0; i < 4; ++i)
    {
        output.bt_lists[i] = this->bt_lists[i].surroundingSublist(start_time, end_time);
    }
    // FX
    for (int i = 0; i < 2; ++i)
    {
        output.fx_lists[i] = this->fx_lists[i].surroundingSublist(start_time, end_time);
    }
    // Knob
    for (int i = 0; i < 2; ++i)
    {
        output.knob_lists[i] = this->knob_lists[i].surroundingSublist(start_time, end_time);
    }
    // Mark
    for (int i = 0; i < MarkTypesCount; ++i)
    {
        output.mark_lists[i] = this->mark_lists[i].surroundingSublist(start_time, end_time);
    }
    // misc
    output.comment_list = this->comment_list.surroundingSublist(start_time, end_time);
    output.spin_effect_list = this->spin_effect_list.surroundingSublist(start_time, end_time);
    output.other_items_list = this->other_items_list.surroundingSublist(start_time, end_time);

    output.total_time = start_time + length;

    return output;
}

void IndexedChart::ReplaceMerge(IndexedChart& chart, int offset)
{
    int length = chart.CalculateTotalTime();
    // BT
    for (int i = 0; i < 4; ++i)
    {
        this->ReplaceBT(BTByIndex(i), chart.bt_lists[i], offset);
    }
    // FX
    for (int i = 0; i < 2; ++i)
    {
        this->ReplaceFX(FXByIndex(i), chart.fx_lists[i], offset);
    }
    // Knob
    for (int i = 0; i < 2; ++i)
    {
        this->ReplaceKnob(KnobByIndex(i), chart.knob_lists[i], offset);
    }
    // Marks
    for (int i = 0; i < MarkTypesCount; ++i)
    {
        this->ReplaceMark(MarkByIndex(i), MarkSideByIndex(i), chart.mark_lists[i], offset);
    }
}

/* #endregion */
