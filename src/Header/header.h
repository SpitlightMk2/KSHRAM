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
#include <string>
#include <iostream>

/// 谱面头（记录谱面基本信息）
///
/// 以map<string, string>的形式存储。
class Header {
private:
	std::map<std::string, std::string> items;

public:
	Header() = default;

	/// 从ksh片段导入
	void ImportFromKsh(const std::string& ksh);
	/// 导出为ksh片段
	std::string ExportToKsh() const;

	/// 是否有给定关键词的信息
	inline bool HasMark(const std::string& key) const;
	/// 是否有给定关键词的值
	inline std::string GetMarkValue(const std::string& key) const;

public:
	friend std::istream& operator>>(std::istream&, Header&);
	friend std::ostream& operator<<(std::ostream&, const Header&);
};

/// 从ksh片段导入
std::istream& operator>>(std::istream&, Header&);
/// 导出为ksh片段
std::ostream& operator<<(std::ostream&, const Header&);

#include "header_inline.h"