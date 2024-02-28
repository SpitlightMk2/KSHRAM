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

#include "error_collector.h"

inline int CallStack::Size() const
{
    return this->cmds.size();
}

inline bool CallStack::Empty() const
{
    return this->cmds.size() == 0;
}

inline void CallStack::Append(const std::string& cmd)
{
    this->cmds.push_back(cmd);
}

inline void CallStack::Pop()
{
    if(!this->cmds.empty())
    {
        this->cmds.pop_back();
    }
}

inline void CallStack::Clear()
{
    this->cmds.clear();
}

inline ErrorCollector::ErrorCollector() 
: err_count(0), warning_count(0), 
root_cmd_time({0, 0, 4}),
execution_time({0, 0, 4})
{
}

inline void ErrorCollector::SetRootCmdTime(const TimeInfo& time)
{
    this->root_cmd_time = time;
}

inline void ErrorCollector::SetExecTime(const TimeInfo& time)
{
    this->execution_time = time;
}

inline void ErrorCollector::SetCommand(const std::string& cmd)
{
    this->root_command = cmd;
}

inline void ErrorCollector::AddToStack(const std::string& cmd)
{
    this->call_stack.Append(cmd);
}

inline void ErrorCollector::PopStack()
{
    this->call_stack.Pop();
}

inline int ErrorCollector::ErrorCount() const
{
    return this->err_count;
}

inline int ErrorCollector::WarningCount() const
{
    return this->warning_count;
}

// 这个os还得传递到每个调用点上，所以其实没啥用，不如用rdbuf改cout指向的流。

inline void ErrorCollector::ErrorLog(const std::string& msg, std::ostream& os)
{
    this->LogMessage(msg, "Error", os);
    this->err_count += 1;
}

inline void ErrorCollector::WarningLog(const std::string& msg, std::ostream& os)
{
    this->LogMessage(msg, "Warning", os);
    this->warning_count += 1;
}