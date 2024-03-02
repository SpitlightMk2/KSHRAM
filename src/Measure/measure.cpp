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

#include "measure.h"
#include "src/misc/utilities.h"

using namespace std;

/* #region Measure */

Measure::Measure() {
	this->time_sig_numer = 4;
	this->time_sig_denom = 4;
	this->start_time = 0;
	this->entry_timespan = 0;
}

Measure Measure::BlankMeasure(int numer, int denom) {
	Measure output;
	for (int loop = 0; loop < numer; ++loop) {
		output.entries.push_back(Entry());
	}
	output.time_sig_numer = numer;
	output.time_sig_denom = denom;

	output.UpdateEntryTimespan();

	return output;
}

void Measure::InitializeAfterImport() {
	// 如果第一个entry有拍号信息，修改自身的拍号
	Entry& first = this->entries.front();
	if (first.HasMark(MarkType::TimeSignature)) {
		string time_sig = first.GetLastMarkVal(MarkType::TimeSignature);
		auto [numer, denom] = ReadRatioI(time_sig);
        this->time_sig_numer = numer; this->time_sig_denom = denom;
	}

	// 计算timespan
	int time = this->time_sig_numer * 192 / this->time_sig_denom;
	if (time % this->entries.size() != 0) {
		// ERROR: invalid entry count
		exit(-1);
	}

	this->entry_timespan = time / static_cast<int>(this->entries.size());
}

void Measure::ImportFromKsh(const std::string& ksh) {
	if (!this->entries.empty()) {
		this->Clear();
	}

	std::string::const_iterator iter = ksh.begin();
	std::string::const_iterator end = ksh.end();
	std::string::const_iterator last_iter = iter;
	while (iter != end) {
		// 找到一行数据行
		while (*iter != '\n' || (iter[1] < '0' || iter[1] > '2')) {
			++iter;
			if (iter == end) { break; }
		}
		if (iter == end) { break; }

		++iter;
		// 找到数据行的末尾
		while (*iter != '\n') {
			++iter;
			if (iter == end) { break; }
		}

		if (iter == end) { break; }

		// 录入一个entry
		std::string entry_ksh(last_iter, iter);
		Entry temp_entry;
		temp_entry.ImportFromKsh(entry_ksh);
		if (!temp_entry.Error()) {
			this->entries.push_back(std::move(temp_entry));
		}

		// 准备下一轮循环
		last_iter = iter + 1;

		if (iter + 1 == end) { break;  }
	}

	if (!this->entries.empty()) {
		this->InitializeAfterImport();
	}
}

std::string Measure::ExportToKsh() {
	std::string output = "";
	for (Entry& entry : this->entries) {
		if (&entry != this->entries.data()) {
			output += CRLF();
		}
		output.append(std::move(entry.ExportToKsh()));
		
	}

	return output;
}

void Measure::ExpandBy(int amp) {
	if (entry_timespan % amp != 0) {
		// ERROR: wrong amp input
		exit(-1);
	}

	const int length = static_cast<int>(this->entries.size());
	vector<Entry> new_entries(length * amp);

	for (int i = 0; i < length; ++i) {
		Entry& current_entry = this->entries[i];
		string ksh_inter = "";
		Entry insertion_entry = current_entry.InsertionEntry();
		if (i < length - 1) {
			Entry& next_entry = this->entries[i + 1];

			if (next_entry.GetKnobIndex(Knob::L) == -1) {
				insertion_entry.SetKnobPos(Knob::L, -1);
			}
			if (next_entry.GetKnobIndex(Knob::R) == -1) {
				insertion_entry.SetKnobPos(Knob::R, -1);
			}
		}
		new_entries[i * amp] = std::move(current_entry);
		for (int loop = 1; loop < amp; ++loop) {
			new_entries[i * amp + loop] = insertion_entry;
		}
	}

	this->entries = std::move(new_entries);
	this->entry_timespan /= amp;
}

/* #endregion */

/* #region Iterator */

Measure::Iterator::Iterator(Measure& m, int index) {
	this->p_measure = &m;
	this->step = m.EntryTimespan();
	this->time_index = index * step;
}

/* #endregion */

/* #region ConstIterator */

Measure::ConstIterator::ConstIterator(const Measure& m, int index) {
	this->p_measure = &m;
	this->step = m.EntryTimespan();
	this->time_index = index * step;
}

/* #endregion */

/* #region IO */

istream& operator>>(istream& is, Measure& measure) {
	while (!is.eof() && is.peek() != '-') {
		Entry& temp = measure.entries.emplace_back();
		is >> temp;
		if (temp.Error()) {
			measure.entries.pop_back();
		}
	}

	if (!is.eof()) {
		// 把末尾的小节线"--\n"删去
		string dummy;
		getline(is, dummy);
	}

	if (!measure.entries.empty()) {
		measure.InitializeAfterImport();
	}

	return is;
}

ostream& operator<<(ostream& os, const Measure& measure) {
	for (const Entry& entry : measure.entries) {
		os << entry.ExportToKsh() << CRLF();
	}

	// 小节线
	os << "--";

	return os;
}

/* #endregion */
