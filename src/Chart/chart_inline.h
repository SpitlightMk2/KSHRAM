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

#include "chart.h"
#include "src/FileSystem/path_manager.h"

/* #region Chart */

inline bool Chart::ImportFromFile(const std::string& path)
{
    std::string content = PathManager::LoadFile(path);
    if (content.empty())
    {
        return false;
    }
    else
    {
        return this->ImportFromString(content);
    }
}

inline void Chart::ExportToFile(const std::string& path)
{
	PathManager::SaveFile(path, this->ExportToString());
}

inline Header& Chart::GetHeader()
{
    return this->header;
}

inline CustomFX& Chart::GetCustomFX()
{
    return this->custom_fx;
}

inline Measure& Chart::operator[](int n)
{
    return this->measures[n];
}

inline Entry& Chart::operator()(int measure_id, int entry_id)
{
    return this->measures[measure_id][entry_id];
}

inline int Chart::MeasureIDAtTime(int time)
{
    return this->measure_at_time.prevItem(time)->second.first();
}

inline Measure& Chart::MeasureAtTime(int time)
{
    return this->measures[this->MeasureIDAtTime(time)];
}

inline int Chart::EntryIDAtTime(int time)
{
    Measure& measure = this->MeasureAtTime(time);
    int dt = time - measure.StartTime();
    return dt / measure.EntryTimespan();
}

inline Entry& Chart::EntryAtTime(int time)
{
    Measure& measure = this->MeasureAtTime(time);
    int dt = time - measure.StartTime();
    return measure.EntryByLocalTime(dt);
}

inline std::tuple<int, int> Chart::MeasureAndEntryIDAtTime(int time)
{
    int measure_id = this->MeasureIDAtTime(time);
    Measure& measure = this->measures[measure_id];
    int dt = time - measure.StartTime();
    int entry_id = dt / measure.EntryTimespan();
    return std::tuple<int, int>{measure_id, entry_id};
}

inline int Chart::TimeAtID(int measure_id, int entry_id)
{
    Measure& measure = this->measures[measure_id];
    if (entry_id >= measure.Length())
    {
        // ERROR
        return -1;
    }
    return measure.StartTime() + entry_id * measure.EntryTimespan();
}

inline int Chart::TotalTime()
{
    if (this->measures.empty())
    {
        return 0;
    }
    else
    {
        return this->measures.back().EndTime();
    }
}

inline void Chart::AppendMeasure(Measure& measure)
{
    int start_time = this->TotalTime();
    this->measures.push_back(measure);
    this->measures.back().StartTime() = start_time;
}

/* #endregion */

/* #region EntryIterator */

inline Chart::EntryIterator Chart::EntryIterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

inline Chart::EntryIterator Chart::EntryIterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

inline bool Chart::EntryIterator::operator==(const EntryIterator& other) const
{
    return this->p_chart == other.p_chart &&
           this->measure_id == other.measure_id &&
           this->entry_id == other.entry_id;
}

inline bool Chart::EntryIterator::operator!=(const EntryIterator& other) const
{
    return !(*this == other);
}

inline int Chart::EntryIterator::Time() const
{
    if (this->measure_id >= this->p_chart->measures.size())
    {
        return this->p_chart->TotalTime();
    }
    else if (this->measure_id < 0)
    {
        return -1;
    }
    else
    {
        Measure& measure = this->p_chart->measures[measure_id];
        return measure.StartTime() + this->entry_id * measure.EntryTimespan();
    }
}

inline Chart::EntryIterator Chart::begin()
{
    return Chart::EntryIterator(*this, 0, 0);
}

inline Chart::EntryIterator Chart::end()
{
    return Chart::EntryIterator(*this, static_cast<int>(this->measures.size()), 0);
}

/* #endregion */
