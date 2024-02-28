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
#include <iostream>

/*
用途：
在执行任何语句时，收集语句内容和执行的命令点位。
如果语句执行过程出现错误，那么使用这些内容与错误信息一起形成报错信息。
*/

struct TimeInfo
{
    /// 小节号
    int measure;
    /// 小节内的位置（分子）
    int entry_numer;
    /// 小节内的位置（分母）
    int entry_denom;
};

/// 用于记录batch的调用链
class CallStack
{
private:
    std::vector<std::string> cmds;

public:
    inline CallStack() = default;
    inline ~CallStack() = default;
    inline CallStack(const CallStack&) = default;

    /// 查看调用链长度
    inline int Size() const;

	/// 是否为空
	inline bool Empty() const;

    /// 增加一个函数
    inline void Append(const std::string& cmd);

    /// 减少一层
    inline void Pop();

	/// 清空
	inline void Clear();

    /// 把调用链信息打印到指定流中
    friend std::ostream& operator<<(std::ostream& os, const CallStack& stack);
};


/// 错误信息收集类。在单例ApplicationBus当中有一个实例。
class ErrorCollector {
private:
	// 错误信息计数器
	int err_count, warning_count;
	// 根命令时间（不计入任何调用的）
	TimeInfo root_cmd_time;
	// 当前调用发生时间
	TimeInfo execution_time;
	// 当前调用链
	CallStack call_stack;
	// 当前根命令（不算调用链上的那些）
	std::string root_command;
public:
	// 构造
	inline ErrorCollector();
	ErrorCollector(const ErrorCollector&) = default;
	ErrorCollector& operator=(const ErrorCollector&) = default;

private:
	// 打印消息
	void LogMessage(const std::string& msg, const std::string& type, std::ostream& os = std::cout);

public:
	/// 设置根命令时间
	inline void SetRootCmdTime(const TimeInfo& time);
	/// 设置当前调用时间
	inline void SetExecTime(const TimeInfo& time);
	/// 设置当前命令
	inline void SetCommand(const std::string& cmd);
	/// 向调用链增加一层内容
	inline void AddToStack(const std::string& cmd);
	/// 调用链减少一层
	inline void PopStack();
	/// 获取错误数量
	inline int ErrorCount() const;
	/// 获取警告数量
	inline int WarningCount() const;
	/// 重置
	void Reset();

	/// 打印错误
	inline void ErrorLog(const std::string& msg, std::ostream& os = std::cout);
	/// 打印警告
	inline void WarningLog(const std::string& msg, std::ostream& os = std::cout);

};

/* #region log assist */

enum class ErrorType {
	Unknown,
	InvalidArgs,
	ObjectNotFound,
	ParamIsNotNumber,
	KnobTooShort,
	EndCommandNotFound
};

template <ErrorType type>
inline std::string ErrorMessage() {
	return "Unidentified error.";
}

template <>
inline std::string ErrorMessage<ErrorType::InvalidArgs>() {
	return "Command args invalid.";
}

template <>
inline std::string ErrorMessage<ErrorType::ObjectNotFound>() {
	return "Object required to execute the command was missing or incomplete.";
}

template <>
inline std::string ErrorMessage<ErrorType::ParamIsNotNumber>() {
	return 
		"Object parameters required to execute the command is not a number.\n"
		"Possibly a non-digit tilt value?";
}

template <>
inline std::string ErrorMessage<ErrorType::KnobTooShort>() {
	return "Knob line is too short to generate a decent curve.";
}

template <>
inline std::string ErrorMessage<ErrorType::EndCommandNotFound>() {
	return "The end command of this command is not found.";
}

/* #endregion */

#include "error_collector_inline.h"