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

#include "command.h"
#include "src/misc/utilities.h"

inline bool Command::empty() const
{
    return (this->cmds.size() == 0);
}

inline int Command::argLength() const
{
    return this->cmds.size() > 0 ? static_cast<int>(this->cmds.size() - 1) : 0;
}

inline std::string& Command::cmd()
{
    return this->cmds[0];
}

inline const std::string& Command::cmd() const
{
    return this->cmds[0];
}

inline int& Command::time()
{
    return this->time_;
}

inline int Command::time() const
{
    return this->time_;
}

inline unsigned int& Command::delay()
{
    return this->exec_delay_;
}

inline unsigned int Command::delay() const
{
    return this->exec_delay_;
}

inline std::string& Command::arg(int index)
{
    return this->cmds[index + 1];
}

inline const std::string& Command::arg(int index) const
{
    return this->cmds[index + 1];
}

inline bool Command::argIsInt(int index) const
{
    return IsDigit(this->cmds[index + 1]);
}

inline int Command::argAsInt(int index) const
{
    return stoi(this->cmds[index + 1]);
}

inline bool Command::argIsDouble(int index) const
{
    return IsFloat(this->cmds[index + 1]);
}

inline double Command::argAsDouble(int index) const
{
    return stod(this->cmds[index + 1]);
}

inline bool Command::argIsBool(int index) const
{
    const std::string& str = this->cmds[index + 1];
    if (str == "t" || str == "f" || str == "true" || str == "false")
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline bool Command::argAsBool(int index) const
{
    const std::string& str = this->cmds[index + 1];
    return str == "t" || str == "true";
}

inline bool Command::argIsRatio(int index) const
{
    return IsRatio(this->cmds[index + 1]);
}

inline double Command::argAsRatio(int index, double amp) const
{
    // double numer, denom;
    auto [numer, denom] = ReadRatio(this->cmds[index + 1]);

    return numer * amp / denom;
}

inline double Command::argAsDoubleOrRatio(int index, double amp) const
{
    if (this->argIsDouble(index))
    {
        return this->argAsDouble(index) * amp;
    }
    else
    {
        return this->argAsRatio(index, amp);
    }
}

inline std::string Command::toString() const
{
    return Assemble(this->cmds, ' ');
}

inline std::string Command::toOriginalString() const
{
    if (!this->original.empty())
    {
        return this->original;
    }
    else
    {
        return this->toString();
    }
}

inline bool Command::matchesCommand(const std::string& str, int delay) const
{
    std::string str_copy = str;
    Strip(str_copy);

    return this->toString() == str_copy && this->exec_delay_ == delay;
}

inline bool Command::matchesType(const std::string& str) const
{
    return this->cmd() == str;
}