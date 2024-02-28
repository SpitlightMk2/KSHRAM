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

#include "command_map.h"

inline CommandMap::CommandMap(const IndexList<std::string>& lst) {
    this->ImportFromComment(lst);
}

inline void CommandMap::insert(int time, Command cmd) {
    this->content.insert({ time, cmd });
}

inline void CommandMap::erase(Iterator& iter) {
    Iterator backup = iter;
    ++iter;
    this->content.erase(backup);
}

inline CommandMap::Iterator CommandMap::FindNearestCommand(const std::string& cmd_str, int start_time, int delay) {
    Iterator start = this->content.lower_bound(start_time);
    Iterator end = this->content.end();

    return std::find_if(start, end, [&cmd_str, delay](const std::pair<const int, Command>& item) {
        return item.second.matchesCommand(cmd_str, delay);
        });
}

inline CommandMap::Iterator CommandMap::lower_bound(int key) {
    return this->content.lower_bound(key);
}
inline CommandMap::Iterator CommandMap::upper_bound(int key) {
    return this->content.upper_bound(key);
}

inline CommandMap::Iterator CommandMap::begin() {
    return this->content.begin();
}
inline CommandMap::Iterator CommandMap::end() {
    return this->content.end();
}

inline CommandMap::ConstIterator CommandMap::begin() const {
    return this->content.begin();
}
inline CommandMap::ConstIterator CommandMap::end() const {
    return this->content.end();
}

inline bool CommandMap::Empty() const
{
    return this->content.empty();
}

inline CommandMap::Iterator CommandMap::FindCommandAtTime(const std::string& cmd_str, int start_time) {
    Iterator start = this->content.lower_bound(start_time);
    Iterator end = this->content.upper_bound(start_time);

    Iterator found = std::find_if(start, end, [&cmd_str](const std::pair<const int, Command>& item) {
        return item.second.matchesType(cmd_str);
        });

    if (found->first == start_time) {
        return found;
    }
    else {
        return this->end();
    }
}