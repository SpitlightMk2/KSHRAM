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

#include <string>
#include <vector>

/// KSHRAM命令。基本格式是 命令 参数 参数... 各项之间用空格分隔。
/// 
/// 使用大括号括起来的部分会被视为一个整体。
///
/// 构造时命令会被转成全小写。
class Command
{
private:
	// 命令所在时间（有时是谱面时间，有时是相对时间）
	int time_ = 0;
	// 命令延迟权重。延迟权重高的命令会比低的更晚执行。同权重命令再按时间顺序执行。
	unsigned int exec_delay_ = 0;
	// 原始字符串（方便错误处理的时候打印）
	std::string original;
	// 拆分后的字符串
	std::vector<std::string> cmds;
public:
	inline Command() = default;
	/// 从字符串构造命令。拆分后转为全小写。
	Command(const std::string& str, int time = 0);

	inline Command(const Command&) = default;

public:
	/// 命令是否是空的（空字符串）
	inline bool empty() const;
	/// 命令参数的长度，不包括命令词本身。
	inline int argLength() const;
	/// 命令词
	inline std::string& cmd();
	/// 获取命令词
	inline const std::string& cmd() const;
	/// 执行时间
	inline int& time();
	/// 获取执行时间
	inline int time() const;
	/// 延迟权重
	inline unsigned int& delay();
	/// 获取延迟权重
	inline unsigned int delay() const;

	/// 获取or修改给定下标的参数
	inline std::string& arg(int index);
	/// 获取给定下标的参数
	inline const std::string& arg(int index) const;
	/// 检查指定参数是否是整数
	inline bool argIsInt(int index) const;
	/// 将指定参数读取为整数
	inline int argAsInt(int index) const;
	/// 检查指定参数是否是数字（整数or小数）
	inline bool argIsDouble(int index) const;
	/// 将指定参数读取为数字
	inline double argAsDouble(int index) const;
	/// 检查指定参数是否是布尔值
	/// - 可用的True值：true, t
	/// - 可用的False值：false, f
	inline bool argIsBool(int index) const;
	/// 将指定参数读取为布尔值
	inline bool argAsBool(int index) const;
	/// 检查指定参数是否是分数(a/b的形式，不能带空格)
	inline bool argIsRatio(int index) const;

	/// @brief 将指定参数读取为分数，并且可以预先放大指定倍数（以获取到一个准确的整数值）。
	/// @param amp 放大倍数。返回值会是实际值乘以这个数。
	inline double argAsRatio(int index, double amp = 1.0) const;

	/// @brief 将指定参数读取为小数or分数，并且可以预先放大指定倍数。即使参数是小数也会被乘以倍数。
	/// @param amp 放大倍数。返回值会是实际值乘以这个数。
	inline double argAsDoubleOrRatio(int index, double amp = 1.0) const;

	/// 获取命令内容（从处理后全小写的vector<string>拼合而来）
	inline std::string toString() const;
	/// 获取命令原始内容。如果要打印，建议选这个。
	inline std::string toOriginalString() const;
	
	/// @brief 将命令参数的一部分拼合为一个字符串。可以用于取得子命令。
	/// @param begin_i 起始参数下标（注意命令词不算在内）
	/// @param end_i 结束参数下标。负值表示从末尾往前数。
	std::string substring(int begin_i, int end_i = -1) const;

	/// 检查该命令的命令词与延迟权重是否与输入参数匹配。
	inline bool matchesCommand(const std::string&, int delay = 0) const;
	/// 检查该命令的命令词是否与输入参数匹配。
	inline bool matchesType(const std::string&) const;
};

/* INLINE FUNCTION */

#include "command_inline.h"