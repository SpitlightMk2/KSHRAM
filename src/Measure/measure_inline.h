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

#include "measure.h"

#include <numeric>

/* INLINE FUNCTIONS */

/* #region Measure */

inline void Measure::UpdateEntryTimespan() {
	this->entry_timespan = 192 * this->time_sig_numer / this->time_sig_denom / static_cast<int>(this->entries.size());
}

inline bool Measure::Empty() {
	return this->entries.empty();
}

inline int& Measure::StartTime() {
	return this->start_time;
}

inline int Measure::StartTime() const {
	return this->start_time;
}

inline int Measure::EndTime() const {
	return this->start_time + 192 * this->time_sig_numer / this->time_sig_denom;
}

inline int Measure::Length() const {
	return static_cast<int>(this->entries.size());
}

inline int Measure::EntryTimespan() const {
	return this->entry_timespan;
}

inline int Measure::TotalTimespan() const {
	return 192 * this->time_sig_numer / this->time_sig_denom;
}

inline int Measure::IndexByLocalTime(int local_time) const {
	return local_time / entry_timespan;
}

inline int Measure::LocalTimeByIndex(int index) const {
	return index * entry_timespan;
}

inline Entry& Measure::EntryByIndex(int index) {
	return this->entries[index];
}

inline const Entry& Measure::EntryByIndex(int index) const {
	return this->entries[index];
}

inline Entry& Measure::EntryByLocalTime(int local_time) {
	return this->entries[IndexByLocalTime(local_time)];
}

inline const Entry& Measure::EntryByLocalTime(int local_time) const {
	return this->entries[IndexByLocalTime(local_time)];
}

inline Entry& Measure::operator[](int index) {
	return this->entries[index];
}

inline const Entry& Measure::operator[](int index) const {
	return this->entries[index];
}

inline Entry& Measure::operator()(int local_time) {
	return this->entries[IndexByLocalTime(local_time)];
}

inline const Entry& Measure::operator()(int local_time) const {
	return this->entries[IndexByLocalTime(local_time)];
}

inline void Measure::Clear() {
	this->time_sig_numer = 4;
	this->time_sig_denom = 4;
	this->start_time = 0;
	this->entry_timespan = 0;
	this->entries.clear();
}

inline void Measure::ExpandToTimespan(int timespan) {
	if (192 % timespan != 0) {
		// ERROR
		exit(-1);
	}
	int amp = this->entry_timespan / std::gcd(this->entry_timespan, timespan);

	if (amp != 1) {
		this->ExpandBy(amp);
	}
}

inline void Measure::ExpandToDivisor(int divisor) {
	if (192 % divisor != 0) {
		// ERROR
		exit(-1);
	}
	int target_span = 192 / divisor;
	int amp = this->entry_timespan / std::gcd(this->entry_timespan, target_span);

	if (amp != 1) {
		this->ExpandBy(amp);
	}
}

inline Measure::Iterator Measure::begin() {
	return Measure::Iterator(*this, 0);
}

inline Measure::ConstIterator Measure::begin() const {
	return Measure::ConstIterator(*this, 0);
}

inline Measure::Iterator Measure::end() {
	return Measure::Iterator(*this, this->Length());
}

inline Measure::ConstIterator Measure::end() const {
	return Measure::ConstIterator(*this, this->Length());
}

inline void Measure::SetTimeSignature(int numer, int denom) {
	this->time_sig_numer = numer;
	this->time_sig_denom = denom;
}

inline void Measure::GetTimeSignature(int& numer, int& denom) const {
	numer = this->time_sig_numer;
	denom = this->time_sig_denom;
}

/* #endregion */

/* #region Iterator */

inline Measure::Iterator& Measure::Iterator::operator++() {
	this->time_index += this->step;
	return *this;
}

inline Measure::Iterator Measure::Iterator::operator++(int) {
	Iterator backup(*this);
	this->time_index += this->step;
	return backup;
}

inline Measure::Iterator& Measure::Iterator::operator--() {
	this->time_index -= this->step;
	return *this;
}

inline Measure::Iterator Measure::Iterator::operator--(int) {
	Iterator backup(*this);
	this->time_index -= this->step;
	return backup;
}

inline Measure::Iterator& Measure::Iterator::operator+=(int n) {
	this->time_index += this->step * n;
	return *this;
}

inline Measure::Iterator& Measure::Iterator::operator-=(int n) {
	this->time_index -= this->step * n;
	return *this;
}

inline Measure::Iterator Measure::Iterator::operator+(int n) const {
	Iterator copy(*this);
	copy += n;
	return copy;
}

inline Measure::Iterator Measure::Iterator::operator-(int n) const {
	Iterator copy(*this);
	copy -= n;
	return copy;
}

inline int Measure::Iterator::operator-(const Iterator& other) const {
	return (this->time_index - other.time_index) / this->step;
}

inline bool Measure::Iterator::operator==(const Iterator& other) const {
	return this->time_index == other.time_index;
}

inline bool Measure::Iterator::operator!=(const Iterator& other) const {
	return this->time_index != other.time_index;
}

inline bool Measure::Iterator::operator<(const Iterator& other) const {
	return this->time_index < other.time_index;
}

inline bool Measure::Iterator::operator<=(const Iterator& other) const {
	return this->time_index <= other.time_index;
}

inline bool Measure::Iterator::operator>(const Iterator& other) const {
	return this->time_index > other.time_index;
}

inline bool Measure::Iterator::operator>=(const Iterator& other) const {
	return this->time_index >= other.time_index;
}

inline Entry& Measure::Iterator::operator[](int n) const {
	return (*this->p_measure)(this->time_index + n * this->step);
}

inline Entry& Measure::Iterator::operator*() const {
	return (*this->p_measure)(this->time_index);
}

inline int Measure::Iterator::Time() const {
	return this->p_measure->start_time + this->time_index;
}

inline int Measure::Iterator::LocalTime() const {
	return this->time_index;
}

/* #endregion */

/* #region ConstIterator */

inline Measure::ConstIterator& Measure::ConstIterator::operator++() {
	this->time_index += this->step;
	return *this;
}

inline Measure::ConstIterator Measure::ConstIterator::operator++(int) {
	ConstIterator backup(*this);
	this->time_index += this->step;
	return backup;
}

inline Measure::ConstIterator& Measure::ConstIterator::operator--() {
	this->time_index -= this->step;
	return *this;
}

inline Measure::ConstIterator Measure::ConstIterator::operator--(int) {
	ConstIterator backup(*this);
	this->time_index -= this->step;
	return backup;
}

inline Measure::ConstIterator& Measure::ConstIterator::operator+=(int n) {
	this->time_index += this->step * n;
	return *this;
}

inline Measure::ConstIterator& Measure::ConstIterator::operator-=(int n) {
	this->time_index -= this->step * n;
	return *this;
}

inline Measure::ConstIterator Measure::ConstIterator::operator+(int n) const {
	ConstIterator copy(*this);
	copy += n;
	return copy;
}

inline Measure::ConstIterator Measure::ConstIterator::operator-(int n) const {
	ConstIterator copy(*this);
	copy -= n;
	return copy;
}

inline int Measure::ConstIterator::operator-(const ConstIterator& other) const {
	return (this->time_index - other.time_index) / this->step;
}

inline bool Measure::ConstIterator::operator==(const ConstIterator& other) const {
	return this->time_index == other.time_index;
}

inline bool Measure::ConstIterator::operator!=(const ConstIterator& other) const {
	return this->time_index != other.time_index;
}

inline bool Measure::ConstIterator::operator<(const ConstIterator& other) const {
	return this->time_index < other.time_index;
}

inline bool Measure::ConstIterator::operator<=(const ConstIterator& other) const {
	return this->time_index <= other.time_index;
}

inline bool Measure::ConstIterator::operator>(const ConstIterator& other) const {
	return this->time_index > other.time_index;
}

inline bool Measure::ConstIterator::operator>=(const ConstIterator& other) const {
	return this->time_index >= other.time_index;
}

inline const Entry& Measure::ConstIterator::operator[](int n) const {
	return (*this->p_measure)(this->time_index + n * this->step);
}

inline const Entry& Measure::ConstIterator::operator*() const {
	return (*this->p_measure)(this->time_index);
}

inline int Measure::ConstIterator::Time() const {
	return this->p_measure->start_time + this->time_index;
}

inline int Measure::ConstIterator::LocalTime() const {
	return this->time_index;
}

/* #endregion */