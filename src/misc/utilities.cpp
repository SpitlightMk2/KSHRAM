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

#include "utilities.h"

#include "enums.h"

using namespace std;

/* #region string utils */

vector<std::string> Split(const string& str, const char split_char)
{
    string temp = "";
    vector<std::string> output;
    for (const char& ch : str)
    {
        if (ch != split_char)
        {
            temp.push_back(ch);
        }
        else
        {
            if (temp.length() > 0)
            {
                output.push_back(temp);
            }
            temp.clear();
        }
    }

    if (temp.length() > 0)
    {
        output.push_back(temp);
    }

    return output;
}

vector<string> SplitWithBracket(
    const std::string& str, const char split_char,
    bool remove_outmost_brackets,
    const char bracket_left, const char bracket_right)
{
    string temp = "";
    vector<std::string> output;
    unsigned int bracket_level = 0;
    for (const char& ch : str)
    {
        if (ch == split_char && bracket_level == 0)
        {
            if (temp.length() > 0)
            {
                output.push_back(temp);
            }
            temp.clear();
        }
        else if (ch == bracket_left)
        {
            if (bracket_level >= 1 || !remove_outmost_brackets)
            {
                temp.push_back(ch);
            }
            
            bracket_level += 1;
        }
        else if (ch == bracket_right)
        {
            if (bracket_level >= 1 || !remove_outmost_brackets)
            {
                temp.push_back(ch);
            }
            if (bracket_level >= 1)
            {
                bracket_level -= 1;
            }
        }
        else
        {
            temp.push_back(ch);
        }
    }

    if (temp.length() > 0)
    {
        output.push_back(temp);
    }

    return output;
}

vector<std::string> Split(const string& str)
{
    string temp = "";
    vector<std::string> output;
    for (const char& ch : str)
    {
        if (!isspace(ch))
        {
            temp.push_back(ch);
        }
        else
        {
            if (temp.length() > 0)
            {
                output.push_back(temp);
            }
            temp.clear();
        }
    }

    if (temp.length() > 0)
    {
        output.push_back(temp);
    }

    return output;
}

vector<string> BlankSplitWithBracket(
    const std::string& str,
    bool remove_outmost_brackets,
    const char bracket_left, const char bracket_right)
{
    string temp = "";
    vector<std::string> output;
    unsigned int bracket_level = 0;
    for (const char& ch : str)
    {
        if (isspace(ch) && bracket_level == 0)
        {
            if (temp.length() > 0)
            {
                output.push_back(temp);
            }
            temp.clear();
        }
        else if (ch == bracket_left)
        {
            if (bracket_level == 0)
            {
                if (temp.length() > 0)
                {
                    output.push_back(temp);
                }
                temp.clear();
            }

            if (bracket_level >= 1 || !remove_outmost_brackets)
            {
                temp.push_back(ch);
            }
            
            bracket_level += 1;
        }
        else if (ch == bracket_right)
        {
            if (bracket_level > 1 || !remove_outmost_brackets)
            {
                temp.push_back(ch);
            }

            if(bracket_level == 1)
            {
                if (temp.length() > 0)
                {
                    output.push_back(temp);
                }
                temp.clear();
            }

            if (bracket_level >= 1)
            {
                bracket_level -= 1;
            }
        }
        else
        {
            temp.push_back(ch);
        }
    }

    if (temp.length() > 0)
    {
        output.push_back(temp);
    }

    return output;
}

string Assemble(const vector<string>& split_str, const char split_char)
{
    string output = "";
    for (int i = 0; i < split_str.size(); ++i)
    {
        if (i > 0)
        {
            output += split_char;
        }
        output += split_str[i];
    }

    return output;
}

bool IsDigit(const string& str)
{
    if(str.empty())
    {
        return false;
    }

    for (auto iter = str.begin(); iter != str.end(); ++iter)
    {
        char ch = *iter;
        if (iter == str.begin())
        {
            if (!isdigit(ch) && ch != '-')
            {
                return false;
            }
        }
        else if (!isdigit(ch))
        {
            return false;
        }
    }

    return true;
}

bool IsFloat(const string& str)
{
    if(str.empty())
    {
        return false;
    }
    
    // flags
    bool sign_reject = false;
    bool has_dot = false, has_e = false, has_num_before_e = false, has_num_after_e = false;
    // result
    bool is_digit = true;
    for (char ch : str)
    {
        if (isdigit(ch))
        {
            // 在e出现之前需要至少出现一个数字
            if (!(has_e || has_num_before_e))
            {
                has_num_before_e = true;
            }
            // 在e出现之后需要至少出现一个数字
            if (has_e && !has_num_after_e)
            {
                has_num_after_e = true;
            }
            //更新flags
            sign_reject = true;
        }
        else if (ch == '+' || ch == '-')
        {
            // 符号只能出现在开头和e之后
            if (sign_reject)
            {
                is_digit = false;
                break;
            }
            //更新flags
            sign_reject = true;
        }
        else if (ch == 'e' || ch == 'E')
        {
            // e必须只出现一次
            if (has_e)
            {
                is_digit = false;
                break;
            }
            else
            {
                has_e = true;
            }
            //更新flags
            sign_reject = false;
        }
        else if (ch == '.')
        {
            //小数点必须只出现一次，在e之前
            if (has_dot || has_e)
            {
                is_digit = false;
                break;
            }
            else
            {
                has_dot = true;
            }
            //更新flags
            sign_reject = true;
        }
        else
        {
            is_digit = false;
            break;
        }
    }

    if (!has_num_before_e)
    {
        is_digit = false;
    }
    else if (has_e && !has_num_after_e)
    {
        is_digit = false;
    }

    return is_digit;
}

bool IsRatio(const string& str)
{
    size_t split_pos = str.find('/');
    if (split_pos == string::npos)
    {
        return false;
    }

    string numer = str.substr(0, split_pos);
    Strip(numer);
    string denom = str.substr(split_pos + 1);
    Strip(denom);

    return IsFloat(numer) && IsFloat(denom);
}

tuple<int, int> ReadRatioI(const std::string& str)
{
    size_t split_pos = str.find('/');

    int numer = stoi(str.substr(0, split_pos));
    int denom = stoi(str.substr(split_pos + 1));

    return {numer, denom};
}

tuple<double, double> ReadRatio(const std::string& str)
{
    size_t split_pos = str.find('/');

    double numer = stod(str.substr(0, split_pos));
    double denom = stod(str.substr(split_pos + 1));

    return {numer, denom};
}

/* #endregion */

/* #region knob params */

int ToKnobIndex(char knob_char)
{
    // 0 - 9
    if (knob_char >= '0' && knob_char <= '9')
    {
        // '0' = 0x30
        return knob_char - 0x30;
    }
    // 10 - 35
    else if (knob_char >= 'A' && knob_char <= 'Z')
    {
        // 'A' = 0x41
        return knob_char - 0x41 + 10;
    }
    // 36 - 50
    else if (knob_char >= 'a' && knob_char <= 'o')
    {
        // 'a' = 0x61
        return knob_char - 0x61 + 36;
    }
    // ":" = 128
    else if (knob_char == ':')
    {
        return 128;
    }
    // "-" 和default = -1
    else
    {
        return -1;
    }
}

double ToKnobPos(int knob_index, bool laser2x)
{
    if (laser2x)
    {
        if (knob_index == 12)
        {
            return 12.5f;
        }
        else if (knob_index == 37)
        {
            return 37.5f;
        }
        else
        {
            return knob_index;
        }
    }
    else
    {
        return knob_index;
    }
}

char ToKnobChar(int knob_index)
{
    if (knob_index >= 0 && knob_index <= 50)
    {
        return g_KshKnobList[knob_index];
    }
    else if (knob_index == 128)
    {
        return ':';
    }
    else
    {
        return '-';
    }
}

int ToKnobIndex(double knob_pos, bool laser2x)
{
    if (laser2x)
    {
        if (knob_pos > 11 && knob_pos < 11.75)
        {
            return 11;
        }
        else if (knob_pos >= 11.75 && knob_pos < 12.75)
        {
            return 12;
        }
        else if (knob_pos > 36 && knob_pos < 36.75)
        {
            return 36;
        }
        else if (knob_pos >= 36.75 && knob_pos < 37.75)
        {
            return 37;
        }
        else
        {
            return static_cast<int>(round(knob_pos));
        }
    }
    else
    {
        return static_cast<int>(round(knob_pos));
    }
}

/* #endregion */
