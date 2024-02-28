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

#include "entry.h"
#include "src/misc/utilities.h"

#include <utility>
#include <iostream>

using namespace std;

/* #region Mark */

Mark::Mark(const string& str) {
	this->type = MarkType::Error;
	this->side = Side::L;
	auto split_pos = str.find_first_of('=');
	// 没找到'='的话就变成error类型
	if (split_pos == string::npos) {
		return;
	}
	std::string_view mark_id(str.data(), split_pos);
	std::string_view mark_val(str.data() + split_pos + 1);
	this->value = mark_val;

	if (mark_id == "t") {
		this->type = MarkType::BPM;
	}
	else if (mark_id == "beat") {
		this->type = MarkType::TimeSignature;
	}
	else if (mark_id == "fx-l") {
		this->type = MarkType::FXLong;
		this->side = Side::L;
	}
	else if (mark_id == "fx-r") {
		this->type = MarkType::FXLong;
		this->side = Side::R;
	}
	else if (mark_id == "fx-l_se") {
		this->type = MarkType::FXChip;
		this->side = Side::L;
	}
	else if (mark_id == "fx-r_se") {
		this->type = MarkType::FXChip;
		this->side = Side::R;
	}
	else if (mark_id == "laserrange_l") {
		this->type = MarkType::Laser2x;
		this->side = Side::L;
	}
	else if (mark_id == "laserrange_r") {
		this->type = MarkType::Laser2x;
		this->side = Side::R;
	}
	else if (mark_id == "filtertype") {
		this->type = MarkType::Filter;
	}
	else if (mark_id == "pfiltergain") {
		this->type = MarkType::KnobVolume;
	}
	else if (mark_id == "chokkakuse") {
		this->type = MarkType::SlamSound;
	}
	else if (mark_id == "chokkakuvol") {
		this->type = MarkType::SlamVolume;
	}
	else if (mark_id == "zoom_top") {
		this->type = MarkType::ZoomTop;
	}
	else if (mark_id == "zoom_bottom") {
		this->type = MarkType::ZoomBottom;
	}
	else if (mark_id == "zoom_side") {
		this->type = MarkType::ZoomSide;
	}
	else if (mark_id == "tilt") {
		this->type = MarkType::Tilt;
	}
	else if (mark_id == "stop") {
		this->type = MarkType::Stop;
	}
	else if (mark_id == "center_split") {
		this->type = MarkType::LaneSplit;
	}
	// for unknown type
	// else {
	//	this->type = MarkType::Error;
	// }
	
}

string Mark::ToString() const {
	string output = "";
	// 写入type
	output += MarkStr(this->type, this->side);
	output += "=";

	// 写入value

	output += this->value;

	return output;
}

bool Mark::IsSameType(MarkType type, Side side) const {
	if (type == MarkType::FXLong || type == MarkType::FXChip || type == MarkType::Laser2x) {
		return type == this->type && side == this->side;
	}
	else {
		return type == this->type;
	}
}

/* #endregion */

/* #region SpinEffect */

SpinEffect::SpinEffect(const string& str){
	this->params[0] = 0; this->params[1] = 0; this->params[2] = 0;

	std::string_view mark_id(str.data(), 2);
	if (mark_id == "@(") {
		this->spin = Spin::Circle;
		this->side = Side::L;
	}
	else if (mark_id == "@)") {
		this->spin = Spin::Circle;
		this->side = Side::R;
	}
	else if (mark_id == "@<") {
		this->spin = Spin::Side;
		this->side = Side::L;
	}
	else if (mark_id == "@>") {
		this->spin = Spin::Side;
		this->side = Side::R;
	}
	else if (mark_id == "S<") {
		this->spin = Spin::Shake;
		this->side = Side::L;
	}
	else if (mark_id == "S>") {
		this->spin = Spin::Shake;
		this->side = Side::R;
	}
	else {
		// default
		this->spin = Spin::None;
		this->side = Side::L;
	}

	std::vector<std::string> params_str = Split(str.substr(2), ';');
	this->length = stoi(params_str[0]);
	if (this->spin == Spin::Shake) {
		for (int i = 0; i < 3; ++i) {
			this->params[i] = stoi(params_str[i + 1]);
		}
	}
}

std::string SpinEffect::ToString() const {
	std::string output = "";
	output += SpinStr(this->spin, this->side);
	output += to_string(this->length);
	if (this->spin == Spin::Shake) {
		output += ";" + to_string(this->params[0]) + ";"
			+ to_string(this->params[1]) + ";" + to_string(this->params[2]);
	}

	return output;
}

/* #endregion */

/* #region Entry */

/* #region 构造 */

// 空构造
Entry::Entry() {
	this->marks = vector<Mark>();
	this->comments = "";
	this->notes = "0000|00|--";
	this->err_flag = false;
}

// 从ksh构造
Entry::Entry(const string& ksh) {
	this->marks = vector<Mark>();
	this->comments = "";
	this->notes = "0000|00|--";
	this->err_flag = false;

	this->ImportFromKsh(ksh);
}

/* #endregion */

/* #region 导入导出 */

// 导入ksh
void Entry::ImportFromKsh(const string& ksh) {
	vector<string> ksh_split = Split(ksh, '\n');
	bool read_anything = false;
	for (string& str : ksh_split) {
		Strip(str);
		if (str.empty()) {
			continue;
		}
		else if (str.substr(0, 2) == "//") {
			// comments
			this->comments = str;
			read_anything = true;
		}
		else if (isdigit(str.front())) {
			// notes
			this->notes = str.substr(0,10);
			read_anything = true;
			// spin
			if (str.length() > 10) {
				this->spin_effect = SpinEffect(str.substr(10));
			}
		}
		else {
			Mark mark(str);
			// marks
			if (mark.type != MarkType::Error) {
				this->marks.push_back(std::move(mark));
			}
			// 其他有的没的。虽然不处理但是写回时也要放回去。
			else {
				this->other_parts += str + "\n";
			}
			
			read_anything = true;
		}
	}

	if (!read_anything) {
		this->err_flag = true;
	}
}

// 导出ksh
string Entry::ExportToKsh() const {
	string output = "";
	for (const Mark& mark : this->marks) {
		output += mark.ToString() + "\n";
	}
	if (!this->comments.empty()) {
		output += this->comments + "\n";
	}
	output += this->other_parts;
	output += this->notes;
	if (this->spin_effect.spin != Spin::None) {
		output += this->spin_effect.ToString();
	}
	return output;
}

// 从流输入
istream& operator>> (istream& is, Entry& entry) {
	string temp;
	bool read_anything = false;
	while (true) {
		getline(is, temp);
		if (temp.empty()) {
			continue;
		}
		Strip(temp);
		if (temp.empty()) {
			continue;
		}
		else if (temp.substr(0, 2) == "//") {
			// comments
			entry.comments = temp;
			read_anything = true;
		}
		else if (isdigit(temp.front())) {
			// notes
			entry.notes = temp.substr(0,10);
			read_anything = true;
			// spin
			if (temp.length() > 10) {
				entry.spin_effect = SpinEffect(temp.substr(10));
			}
			break;
		}
		else {
			// marks
			Mark mark(temp);
			// marks
			if (mark.type != MarkType::Error) {
				entry.marks.push_back(std::move(mark));
			}
			// 其他有的没的。虽然不处理但是写回时也要放回去。
			else {
				entry.other_parts += temp + "\n";
			}
			read_anything = true;
		}
	}

	if (!read_anything) {
		entry.err_flag = true;
	}

	return is;
}

// 输出到流
ostream& operator<< (ostream& os, const Entry& entry) {
	os << "Marks:\n";
	for (const Mark& mark : entry.marks) {
		os << "\t" << mark.ToString() << "\n";
	}
	os << "Comments:\n\t" << entry.comments << "\n";
	os << "Other: \n\t" << entry.other_parts;
	os << "Notes:\n\t" << entry.notes;

	return os;
}

/* #endregion */

/* #region 编辑 */

inline KeyState BTState(char bt_ch) {
	if (bt_ch == '0') {
		return KeyState::None;
	}
	else if (bt_ch == '1') {
		return KeyState::Chip;
	}
	else if (bt_ch == '2') {
		return KeyState::Long;
	}
	else {
		return KeyState::None;
	}
}

inline char BTChar(KeyState state) {
	switch (state)
	{
	case KeyState::None:
		return '0';
	case KeyState::Chip:
		return '1';
	case KeyState::Long:
		return '2';
	default:
		// never reach here
		return '0';
	}
}

inline KeyState FXState(char bt_ch) {
	if (bt_ch == '0') {
		return KeyState::None;
	}
	else if (bt_ch == '1') {
		return KeyState::Long;
	}
	else if (bt_ch == '2') {
		return KeyState::Chip;
	}
	else {
		return KeyState::None;
	}
}

inline char FXChar(KeyState state) {
	switch (state)
	{
	case KeyState::None:
		return '0';
	case KeyState::Chip:
		return '2';
	case KeyState::Long:
		return '1';
	default:
		// never reach here
		return '0';
	}
}

KeyState Entry::GetBTState(BT bt) const {
	switch (bt)
	{
	case BT::A:
		return BTState(this->notes[0]);
	case BT::B:
		return BTState(this->notes[1]);
	case BT::C:
		return BTState(this->notes[2]);
	case BT::D:
		return BTState(this->notes[3]);
	default:
		// never reach here
		return KeyState::None;
	}
}

void Entry::SetBTState(BT bt, KeyState state) {
	switch (bt)
	{
	case BT::A:
		this->notes[0] = BTChar(state);
		break;
	case BT::B:
		this->notes[1] = BTChar(state);
		break;
	case BT::C:
		this->notes[2] = BTChar(state);
		break;
	case BT::D:
		this->notes[3] = BTChar(state);
		break;
	}
}

KeyState Entry::GetFXState(FX fx) const {
	switch (fx)
	{
	case FX::L:
		return FXState(this->notes[5]);
	case FX::R:
		return FXState(this->notes[6]);
	default:
		return KeyState::None;
	}
}

void Entry::SetFXState(FX fx, KeyState state) {
	switch (fx)
	{
	case FX::L:
		this->notes[5] = FXChar(state);
		break;
	case FX::R:
		this->notes[6] = FXChar(state);
		break;
	}
}

int Entry::GetKnobIndex(Knob knob) const {
	switch (knob)
	{
	case Knob::L:
		return ToKnobIndex(this->notes[8]);
	case Knob::R:
		return ToKnobIndex(this->notes[9]);
	default:
		// never reach here
		return -1;
	}
}

/// 获取指定旋钮的位置
double Entry::GetKnobPos(Knob knob) const {
	switch (knob)
	{
	case Knob::L:
		return ToKnobPos(this->notes[8]);
	case Knob::R:
		return ToKnobPos(this->notes[9]);
	default:
		// never reach here
		return NAN;
	}
}

/// 设置指定旋钮的位置
void Entry::SetKnobPos(Knob knob, int index) {
	switch (knob)
	{
	case Knob::L:
		this->notes[8] = ToKnobChar(index);
		break;
	case Knob::R:
		this->notes[9] = ToKnobChar(index);
		break;
	}
}

std::string Entry::GetFirstMarkVal(MarkType mark, Side side) const {
	auto found_mark = this->FindFirstMark(mark, side);
	if (found_mark == this->marks.cend()) {
		return "";
	}
	else {
		return found_mark->value;
	}
}

std::string Entry::GetLastMarkVal(MarkType mark, Side side) const {
	auto found_mark = this->FindLastMark(mark, side);
	if (found_mark == this->marks.crend()) {
		return "";
	}
	else {
		return found_mark->value;
	}
}

void Entry::SetFirstMarkVal(const std::string& val, MarkType mark, Side side) {
	auto found_mark = this->FindFirstMark(mark, side);
	if (found_mark == this->marks.end()) {
		// 没有的话就自己加一个
		Mark temp_mark;
		temp_mark.type = mark;
		temp_mark.side = side;
		temp_mark.value = val;
		this->marks.push_back(temp_mark);
	}
	else {
		found_mark->value = val;
	}
}

void Entry::SetLastMarkVal(const std::string& val, MarkType mark, Side side) {
	auto found_mark = this->FindLastMark(mark, side);
	if (found_mark == this->marks.rend()) {
		// 没有的话就自己加一个
		Mark temp_mark;
		temp_mark.type = mark;
		temp_mark.side = side;
		temp_mark.value = val;
		this->marks.push_back(temp_mark);
	}
	else {
		found_mark->value = val;
	}
}

/* #endregion */

/* #region 查找 */

bool Entry::HasMark(MarkType mark, Side side) const {
	return find_if(this->marks.begin(), this->marks.end(), [&mark, &side](const Mark& item) {
		return item.IsSameType(mark, side);
		}) != this->marks.cend();
}

bool Entry::HasMultipleMarks(MarkType mark, Side side) const {
	auto iter = find_if(this->marks.begin(), this->marks.end(), [&mark, &side](const Mark& item) {
		return item.IsSameType(mark, side);
		});

	if (iter == this->marks.cend()) {
		return false;
	}
	else {
		iter = find_if(iter + 1, this->marks.end(), [&mark, &side](const Mark& item) {
			return item.IsSameType(mark, side);
			});
		return iter != this->marks.cend();
	}


}

vector<Mark>::iterator Entry::FindFirstMark(MarkType mark, Side side) {
	return find_if(this->marks.begin(), this->marks.end(), [&mark, &side](Mark& item) {
		return item.IsSameType(mark, side); });
}

vector<Mark>::const_iterator Entry::FindFirstMark(MarkType mark, Side side) const {
	return find_if(this->marks.begin(), this->marks.end(), [&mark, &side](const Mark& item) {
		return item.IsSameType(mark, side); });
}

vector<Mark>::reverse_iterator Entry::FindLastMark(MarkType mark, Side side) {
	return find_if(this->marks.rbegin(), this->marks.rend(), [&mark, &side](Mark& item) {
		return item.IsSameType(mark, side); });
}

vector<Mark>::const_reverse_iterator Entry::FindLastMark(MarkType mark, Side side) const {
	return find_if(this->marks.rbegin(), this->marks.rend(), [&mark, &side](const Mark& item) {
		return item.IsSameType(mark, side); });
}

/* #endregion */

Entry Entry::InsertionEntry() const {
	Entry output;
	for (int ibt = 0; ibt <= 3; ++ibt) {
		if (this->notes[ibt] == '2') {
			output.notes[ibt] = '2';
		}
	}
	for (int ifx = 5; ifx <= 6; ++ifx) {
		if (this->notes[ifx] == '1') {
			output.notes[ifx] = '1';
		}
	}
	for (int ikn = 8; ikn <= 9; ++ikn) {
		if (this->notes[ikn] != '-') {
			output.notes[ikn] = ':';
		}
	}

	return output;
}

/* #endregion */