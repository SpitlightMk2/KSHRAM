#pragma once

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

inline IndexedChart::IndexedChart(Chart& chart) {
	this->ImportFromChart(chart);
}


inline void IndexedChart::UpdateTotalTime(int time) {
	if (this->total_time < time) {
		total_time = time;
	}
}

inline IndexList<int>& IndexedChart::BTList(BT bt) {
	int i = BTIndex(bt);
	return this->bt_lists[i];
}

inline IndexList<int>& IndexedChart::FXList(FX fx) {
	int i = FXIndex(fx);
	return this->fx_lists[i];
}

inline IndexList<int>& IndexedChart::KnobList(Knob knob) {
	int i = KnobIndex(knob);
	return this->knob_lists[i];
}

inline IndexList<std::string>& IndexedChart::MarkList(MarkType mark, Side side = Side::L) {
	int i = MarkIndex(mark, side);
	return this->mark_lists[i];
}

inline IndexList<SpinEffect>& IndexedChart::SpinEffectList() {
	return this->spin_effect_list;
}

inline IndexList<std::string>& IndexedChart::CommentList() {
	return this->comment_list;
}

inline IndexList<std::string>& IndexedChart::OtherItemsList() {
	return this->other_items_list;
}
