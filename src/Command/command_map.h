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

#include <map>

#include "command.h"
#include "src/IndexList/index_list.h"

/// 存储一系列命令的表。同时被用于表示谱面所有命令的总表和一个batch所包含的命令。
class CommandMap {

public:
	using Iterator = std::multimap<int, Command>::iterator;
	using ConstIterator = std::multimap<int, Command>::const_iterator;

public:
	std::multimap<int, Command> content;

public:
	inline CommandMap() = default;
	inline CommandMap(const CommandMap&) = default;

	/// 从IndexedChart的注释表构造
	inline CommandMap(const IndexList<std::string>& lst);

	/// 从单行Comment导入
	void ImportFromString(const std::string& str);
	/// 从IndexedChart的注释表导入
	void ImportFromComment(const IndexList<std::string>& lst);
	/// 导出到Comment字符串
	IndexList<std::string> ExportToComment() const;

	/// 插入一个新的command对象
	inline void insert(int time, Command cmd);
	/// 删除一个command对象，以迭代器导航。迭代器会被引导到下一个命令。
	inline void erase(Iterator& iter);

	/// 命令表是否为空
	inline bool Empty() const;
	/// 修改迭代器所指向的那个命令的时间。迭代器会自动向后递增。
	void ChangeKey(Iterator& iter, int new_key);
	/// 整张表所有命令的执行时间向后增加，以新的命令表返回。
	CommandMap Offset(int offset_time) const;

	/// @brief 从start_time开始，寻找最近的，完全匹配命令词的Command对象位置
	/// @param cmd_str 要匹配的命令词
	/// @param start_time 搜索起始时间
	/// @param delay 目标命令应当具有的延迟值
	inline Iterator FindNearestCommand(const std::string& cmd_str, int start_time, int delay = 0);

	/// @brief 寻找特定时间上存在的某个命令。注意同一时刻可以存在很多条命令。
	/// @param cmd_str 要匹配的命令词
	Iterator FindCommandAtTime(const std::string& cmd_str, int start_time);

	/// 起始迭代器
	inline Iterator begin();
	/// 终止迭代器
	inline Iterator end();
	/// 起始只读迭代器
	inline ConstIterator begin() const;
	/// 终止只读迭代器
	inline ConstIterator end() const;

	/// 同std::multimap的lower_bound
	inline Iterator lower_bound(int key);
	/// 同std::multimap的upper_bound
	inline Iterator upper_bound(int key);

	
	
	
};

/* INLINE FUNCTION */

#include "command_map_inline.h"