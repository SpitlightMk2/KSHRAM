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
#include <cmath>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

/* #region common math functions */

constexpr double PI = 3.14159265358979323846;
constexpr double HALF_PI = PI * 0.5;
constexpr double E = 2.71828182845904523536;

/// 获取x的小数部分
inline double Tail(double x)
{
    return x >= 0 ? x - floor(x) : x - ceil(x);
}

inline double Remain(double x)
{
    return x - round(x);
}

/// 符号函数
inline int Sign(int x)
{
    if (x > 0)
    {
        return 1;
    }
    else if (x < 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/// 符号函数
inline double Sign(double x)
{
    if (x > 0)
    {
        return 1.0f;
    }
    else if (x < 0)
    {
        return -1.0f;
    }
    else
    {
        return 0.0f;
    }
}

inline int DivisorOf(int local_time)
{
    return 192 / std::gcd(local_time, 48);
}

/// 按比例（0-1）线性插值
/// - start 起始值
/// - end 结束值
/// - ratio 比例
inline double LinearInterpolation(double start, double end, double ratio)
{
    return start * (1.0 - ratio) + end * ratio;
}

/// 根据两端坐标线性插值
/// @param start_x 起始点x
/// @param start_y 起始点y
/// @param end_x 结束点x
/// @param end_y 结束点y
/// @param intp_x 待插值点的x
inline double LinearInterpolation(
    double start_x, double start_y,
    double end_x, double end_y, double intp_x)
{
    double ratio = (intp_x - start_x) / (end_x - start_x);
    return start_y * (1.0 - ratio) + end_y * ratio;
}

/* #endregion */

/* #region string utils */

/// 切去字符串两端的空白，会直接写入传入的字符串上。
inline void Strip(std::string& str)
{
    size_t start_id = str.find_first_not_of(" \t\n\r\f\v");
    if(start_id == std::string::npos)
    {
        str = "";
        return;
    }
    size_t end_id = str.find_last_not_of(" \t\n\r\f\v") + 1;
    str = str.substr(start_id,end_id - start_id);
}

/// 切去字符串两端的空白，以新字符串返回。
inline std::string Stripped(std::string& str)
{
    size_t start_id = str.find_first_not_of(" \t\n\r\f\v");
    if(start_id == std::string::npos)
    {
        return "";
    }
    size_t end_id = str.find_last_not_of(" \t\n\r\f\v") + 1;
    return str.substr(start_id,end_id - start_id);
}

/// 小写字母转为大写，会直接写入传入的字符串上。
inline void ToUpper(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}

/// 大写字母转为小写，会直接写入传入的字符串上。
inline void ToLower(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

/// 检查字符串是否以指定字串开头
inline bool BeginWith(const std::string& str, const std::string& token)
{
    return !str.empty() && search(str.begin(), str.end(), token.begin(), token.end()) == str.begin();
}

/// 用空白字符分割字符串
///
/// 空白字符包括：空格，\t，\n，\r，\f，\v
std::vector<std::string> Split(const std::string& str);

/// @brief 根据指定的字符，将字符串分割
/// @param str 待分割的字符串
/// @param split_char 指定的分隔符
std::vector<std::string> Split(const std::string& str, const char split_char);

/// @brief 根据指定的字符，将字符串分割。当内容外具有括号时，分割符将无效。
/// @param str 待分割的字符串
/// @param split_char 分割字符
/// @param bracket_left 左括号字符（默认为‘{’）
/// @param bracket_right 右括号字符（默认为‘}’）
std::vector<std::string> SplitWithBracket(
    const std::string& str, const char split_char,
    bool remove_outmost_brackets,
    const char bracket_left = '{', const char bracket_right = '}');

/// @brief 以空白符将字符串分割。当内容外具有括号时，分割符将无效。
/// @param str 待分割的字符串
/// @param bracket_left 左括号字符（默认为‘{’）
/// @param bracket_right 右括号字符（默认为‘}’）
std::vector<std::string> BlankSplitWithBracket(
    const std::string& str,
    bool remove_outmost_brackets,
    const char bracket_left = '{', const char bracket_right = '}');

/// @brief 根据指定的字符，将被分割的字符串连接在一起
/// @param split_str 已经被分割的字符串列表
/// @param split_char 分隔符
std::string Assemble(const std::vector<std::string>& split_str, const char split_char);

/// 检查字符串str是否可以被解释为整数
bool IsDigit(const std::string& str);

/// 检查字符串str是否可以被解释为浮点数
bool IsFloat(const std::string& str);

/// 检查字符串str是否可以被解释为"a/b"的格式
bool IsRatio(const std::string& str);

/// 将 "a/b" 格式的比例字符串读入两个int中
std::tuple<int, int> ReadRatioI(const std::string& str);

/// 将 "a/b" 格式的比例字符串读入两个double中
std::tuple<double, double> ReadRatio(const std::string& str);

/* #endregion */

/* #region knob params */

/// 从旋钮位置字符转换为下标
int ToKnobIndex(char knob_char);

/// @brief 从旋钮实际位置转换为下标
/// @param laser2x 是否为外扩
int ToKnobIndex(double knob_pos, bool laser2x = false);

/// @brief 从旋钮位置下标转换为实际位置（浮点）
/// @param laser2x 是否为外扩
double ToKnobPos(int knob_index, bool laser2x = false);

/// @brief 从旋钮位置字符转换为实际位置（浮点）
/// @param laser2x 是否为外扩
inline double ToKnobPos(char knob_char, bool laser2x = false)
{
    int knob_index = ToKnobIndex(knob_char);
    return ToKnobPos(knob_index, laser2x);
}

/// 从旋钮位置下标转为字符
char ToKnobChar(int knob_index);

/// @brief 从旋钮实际位置转为字符
/// @param laser2x 是否为外扩
inline char ToKnobChar(double knob_pos, bool laser2x = false)
{
    int knob_index = ToKnobIndex(knob_pos, laser2x);
    return ToKnobChar(knob_index);
}

/* #endregion */
