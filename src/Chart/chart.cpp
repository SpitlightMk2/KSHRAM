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

#include "chart.h"

#include <fstream>
#include <sstream>

using namespace std;

/* #region Chart */

/* #region 导入导出 */

/// 从字符串导入
bool Chart::ImportFromString(const string& ksh) {
	istringstream ksh_stream(ksh);

	// 移除开头的BOM
	if (ksh_stream.peek() == 0xef) {
		char remove_bom[4];
		ksh_stream.read(remove_bom, 3);
	}

	ksh_stream >> header;

	int numer = 4, denom = 4;
	int start_time = 0;
	while (!ksh_stream.eof() && !ksh_stream.fail() && ksh_stream.peek() != '#') {
		// 空行，跳过
		if(ksh_stream.peek() == '\n')
		{
			ksh_stream.get();
			continue;
		}
		
		Measure& temp = this->measures.emplace_back();
		// 向后传递小节的拍号
		temp.SetTimeSignature(numer, denom);
		temp.StartTime() = start_time;
		ksh_stream >> temp;

		if (temp.Empty()) {
			this->measures.pop_back();
		}
		else {
			// 如果读到了拍号变更，记录下来
			temp.GetTimeSignature(numer, denom);
			start_time = temp.EndTime();
		}
	}

	custom_fx.clear();
	while (!ksh_stream.eof() && !ksh_stream.fail()) {
		string temp;
		getline(ksh_stream, temp);
		custom_fx += temp + CRLF();
	}

	if(ksh_stream.fail() && !ksh_stream.eof()) // 没正常读完
	{
		return false;
	}

	// 登记各小节起始时间
	for (int i = 0; i < this->measures.size(); ++i) {
		int time = this->measures[i].StartTime();
		this->measure_at_time.insert(time, i);
	}

	return true;
}

inline void WriteBOM(ostream& os) {
	os << static_cast<char>(0xef) << static_cast<char>(0xbb) << static_cast<char>(0xbf);
}

/// 导出到字符串
string Chart::ExportToString() {
	ostringstream output_stream;
	WriteBOM(output_stream);
	output_stream << header << CRLF();
	for (const Measure& measure : this->measures) {
		output_stream << measure << CRLF();
	}

	output_stream << custom_fx;

	return output_stream.str();
}

/* #endregion */

/* #region 索引表 */

IndexList<int> Chart::BTIndexList(BT bt) {
	IndexList<int> key_index;

	// 键盘状态记录
	bool holding = false;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		KeyState state = entry.GetBTState(bt);

		if (state == KeyState::Chip) {
			key_index.insert(kTime, 1); // 统一为note = 1, long = 2
		}
		else if (state == KeyState::Long && !holding) {
			key_index.insert(kTime, 2); // 统一为note = 1, long = 2
			holding = true;
		}
		else if (state == KeyState::None && holding) {
			key_index.insert(kTime, 0);
			holding = false;
		}
	}

	return key_index;
}

IndexList<int> Chart::FXIndexList(FX fx) {
	IndexList<int> key_index;

	// 键盘状态记录
	bool holding = false;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		KeyState state = entry.GetFXState(fx);

		if (state == KeyState::Chip) {
			key_index.insert(kTime, 1); // 统一为note = 1, long = 2
		}
		else if (state == KeyState::Long && !holding) {
			key_index.insert(kTime, 2); // 统一为note = 1, long = 2
			holding = true;
		}
		else if (state == KeyState::None && holding) {
			key_index.insert(kTime, 0);
			holding = false;
		}
	}

	return key_index;
}

IndexList<int> Chart::KnobIndexList(Knob side) {
	IndexList<int> knob_index;
	knob_index.insert(0, -1);

	// 旋钮状态记录
	bool knob_on = false;
	int last_key_time = 0;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		int knob_pos = entry.GetKnobIndex(side);

		// 无旋钮
		if (knob_pos == -1) {
			if (knob_on) {
				knob_index[last_key_time].setSecond(-1);
				knob_on = false;
			}
			// else no-op
		}
		// 前后连接
		else if (knob_pos == 128) {
			// no-op
		}
		// 关键点
		else {
			// 旋钮起始
			if (!knob_on) {
				knob_index.insert(kTime, knob_pos);
				knob_on = true;
			}
			// 直角
			else if (kTime <= knob_index.last().first + 6 &&
			 	knob_pos != knob_index.last().second.second()) 
			{
				knob_index.last().second.setSecond(knob_pos);
				knob_index.insert(kTime, knob_pos);
			}
			// 非直角
			else {
				knob_index.insert(kTime, knob_pos);
			}

			last_key_time = kTime;
		}
	}

	return knob_index;
}

IndexList<std::string> Chart::MarkIndexList(MarkType mark, Side side) {
	IndexList<std::string> mark_index;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		bool has_mark = entry.HasMark(mark, side);
		bool double_mark = entry.HasMultipleMarks(mark, side);

		if (double_mark) {
			mark_index.insert(
				kTime,
				entry.FindFirstMark(mark, side)->value,
				entry.FindLastMark(mark, side)->value
			);
		}
		else if(has_mark) {
			mark_index.insert(
				kTime,
				entry.FindFirstMark(mark, side)->value
			);
		}
	}

	// 对于BPM表特殊处理：插入0位置的值
	if (mark == MarkType::BPM && !mark_index.hasKey(0)) {
		std::string init_bpm = this->header.GetMarkValue("t");
		mark_index.insert(0, init_bpm);
	}

	return mark_index;
}

IndexList<SpinEffect> Chart::SpinEffectList() {
	IndexList<SpinEffect> spin_index;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		SpinEffect& spin_effect = entry.GetSpinEffect();

		if (spin_effect.spin != Spin::None) {
			spin_index.insert(kTime, spin_effect);
		}
	}

	return spin_index;
}

IndexList<std::string> Chart::CommentIndexList() {
	IndexList<std::string> comment_index;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		std::string& comment = entry.Comments();

		if (!comment.empty()) {
			comment_index.insert(kTime, comment);
		}
	}

	return comment_index;
}

IndexList<std::string> Chart::OtherItemIndexList() {
	IndexList<std::string> comment_index;

	const Chart::EntryIterator iter_end = this->end();
	for (Chart::EntryIterator iter = this->begin(); iter != iter_end; ++iter) {
		Entry& entry = *iter;
		const int kTime = iter.Time();
		std::string& other_item = entry.OtherItems();

		if (!other_item.empty()) {
			comment_index.insert(kTime, other_item);
		}
	}

	return comment_index;
}

/* #endregion */

/* #endregion */

/* #region EntryIterator */

Chart::EntryIterator::EntryIterator(Chart& chart, int measure_id, int entry_id) {
	this->p_chart = &chart;
	this->measure_id = measure_id;
	this->entry_id = entry_id;
}

Chart::EntryIterator& Chart::EntryIterator::operator++() {
	if (this->measure_id >= this->p_chart->measures.size()) {
		this->entry_id += 1;
	}
	else if (this->measure_id < 0) {
		if (this->entry_id == 0) {
			this->measure_id += 1;
		}
		else {
			this->entry_id += 1;
		}
	}
	else {
		Measure& measure = this->p_chart->measures[this->measure_id];
		if (this->entry_id == measure.Length() - 1) {
			this->entry_id = 0;
			this->measure_id += 1;
		}
		else {
			this->entry_id += 1;
		}
	}

	return *this;
}

Chart::EntryIterator& Chart::EntryIterator::operator--() {
	if (this->measure_id < 0) {
		this->entry_id -= 1;
	}
	else if (this->measure_id >= this->p_chart->measures.size()) {
		if (this->entry_id == 0) {
			this->measure_id -= 1;
			Measure& measure = this->p_chart->measures.back();
			this->entry_id = measure.Length() - 1;
		}
		else {
			this->entry_id -= 1;
		}
	}
	else {
		Measure& measure = this->p_chart->measures[this->measure_id];
		if (this->entry_id == measure.Length() - 1) {
			this->entry_id = 0;
			this->measure_id += 1;
		}
		else {
			this->entry_id += 1;
		}
	}

	return *this;
}

Chart::EntryIterator::value_type& Chart::EntryIterator::operator*() {
	if (this->measure_id >= this->p_chart->measures.size() || this->measure_id < 0) {
		// ERROR
		exit(-1);
	}
	Measure& measure = this->p_chart->measures[this->measure_id];
	return measure[this->entry_id];
}

/* #endregion */

