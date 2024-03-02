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

#include "header.h"

#include "src/misc/utilities.h"


using namespace std;

void Header::ImportFromKsh(const std::string& ksh)
{
    vector<string> lines = Split(ksh, '\n');

    for (string& line : lines)
    {
        Strip(line);
        auto split_pos = line.find_first_of('=');
        std::string mark_id(line.data(), split_pos);
        std::string_view mark_val(line.data() + split_pos + 1);

        this->items[mark_id] = mark_val;
    }
}

std::string Header::ExportToKsh() const
{
    std::string output = "";
    for (auto iter = this->items.begin(); iter != this->items.end(); ++iter)
    {
        if (iter != this->items.begin())
        {
            output += CRLF();
        }
        output += iter->first + "=" + iter->second;
    }

    output.pop_back();

    return output;
}

istream& operator>>(istream& is, Header& header)
{
    string line;
    getline(is, line);
    Strip(line);
    while (line != "--")
    {
        if (line.empty())
        {
            continue;
        }
        auto split_pos = line.find_first_of('=');
        std::string mark_id(line.data(), split_pos);
        std::string_view mark_val(line.data() + split_pos + 1);

        header.items[mark_id] = mark_val;

        if (is.eof())
        {
            break;
        }

        getline(is, line);
        Strip(line);
    }

    return is;
}
ostream& operator<<(ostream& os, const Header& header)
{
    for (auto& item : header.items)
    {
        os << item.first << "=" << item.second << CRLF();
    }

    // 第一小节的分隔线
    os << "--";

    return os;
}