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

#include "error_collector.h"

#include <iostream>

using namespace std;

std::ostream& operator<<(std::ostream& os, const CallStack& stack)
{
    os << "Batch callstack:\n";
    auto rev_end = stack.cmds.rend();
    for(auto rev_iter = stack.cmds.rbegin(); rev_iter != rev_end; ++rev_iter)
    {
        os << "\t" << *rev_iter << "\n";
    }
    return os;
}

inline void PrintPos(const TimeInfo& time, std::ostream& os)
{
	os << "[ measure #" << time.measure + 1 << ", " << time.entry_numer << " / " << time.entry_denom << " ]";
}

void ErrorCollector::LogMessage(const std::string& msg, const std::string& type, std::ostream& os)
{
	// 根命令时间
	os << "Command at ";
	PrintPos(this->root_cmd_time, os);
	os << "\n";
	// 执行时间
	os << "Execution Time: ";
	PrintPos(this->execution_time, os);
	os << "\n";
	
	// 命令本身
	os << "Command: " << this->root_command << "\n";

	// 错误消息
	os << type << ": " << msg << "\n";

	// 调用链（如果有）
	if(!this->call_stack.Empty())
	{
		os << this->call_stack;
	}

	// 最后空一行（顺便flush一下）
	os << endl;

}

void ErrorCollector::Reset()
{
    this->err_count = 0;
    this->warning_count = 0;
    this->root_cmd_time = {0, 0, 4};
    this->execution_time = {0, 0, 4};
    this->root_command = "";
    this->call_stack.Clear();
}