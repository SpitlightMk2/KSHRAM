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

#include <algorithm>
#include "entry.h"

/* INLINE FUNCTION */

/* #region Mark */

inline Mark::Mark() {
	this->type = MarkType::Error;
	this->side = Side::L;
}

inline Mark::Mark(MarkType type, Side side, std::string value, std::string param) {
	this->type = type;
	this->side = side;
	this->value = value;
	this->param = param;
}

/* #endregion */

/* #region SpinEffect */

inline SpinEffect::SpinEffect() {
	this->spin = Spin::None;
	this->side = Side::L;
	this->length = 192;
	this->params[0] = 0; this->params[1] = 0; this->params[2] = 0;
}

inline std::ostream& operator <<(std::ostream& os, SpinEffect se) {
	os << se.ToString();
	return os;
}

/* #endregion */


/* #region Entry */

inline bool Entry::Error() {
	return this->err_flag;
}

inline SpinEffect& Entry::GetSpinEffect() {
	return this->spin_effect;
}

inline const SpinEffect& Entry::GetSpinEffect() const {
	return this->spin_effect;
}

inline std::string& Entry::Comments() {
	return this->comments;
}

inline const std::string& Entry::Comments() const {
	return this->comments;
}

inline std::string& Entry::OtherItems() {
	return this->other_parts;
}

inline const std::string& Entry::OtherItems() const {
	return this->other_parts;
}

inline void Entry::AddMark(const Mark& mark) {
	this->marks.push_back(mark);
}

inline void Entry::DeleteMarks(MarkType mark, Side side) {
	this->marks.erase(
		std::remove_if(this->marks.begin(), this->marks.end(), [&mark, &side](Mark& item) {
			return item.IsSameType(mark, side);
			}
		)
	);
}

/* #endregion */
