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

#include "path_manager.h"

#include "src/misc/utilities.h"

#ifdef QT_IMPL
#include <QFileInfo>
#include <QDir>
#else
#include <fstream>
#endif

#if defined(WIN32) || defined(WIN64)
#    include <io.h>
#elif defined(LINUX)
#    include <unistd.h>
#endif

using namespace std;

PathManager& PathManager::GetInstance()
{
    static PathManager singleton;
    return singleton;
}

/* #region 静态函数 */

static bool isCommentLine(const std::string& line)
{
    string stripped = line;
    Strip(stripped);
    return (stripped.find("//") == 0);
}

#ifdef QT_IMPL

std::string PathManager::LoadFile(const std::string& path)
{
    // QFile实现
    QFile file(QString::fromStdString(path));
    if (!file.exists())
    {
        return "";
    }

    file.open(QIODeviceBase::ReadOnly);
    QString content = file.readAll();
    file.close();
    return content.toStdString();
}

std::string PathManager::LoadFileWithoutComment(const std::string& path)
{
    // QFile实现
    QFile file(QString::fromStdString(path));
    if (!file.exists())
    {
        return "";
    }

    file.open(QIODeviceBase::ReadOnly);
    QString output;
    while (!file.atEnd())
    {
        QString line = file.readLine();
        if (!isCommentLine(line.toStdString()))
        {
            output += line;
        }
    }

    file.close();
    return output.toStdString();
}

bool PathManager::SaveFile(const std::string& path, const std::string& content)
{
    // QFile实现
    QFile file(QString::fromStdString(path));
    file.open(QIODeviceBase::WriteOnly);

    file.write(content.c_str());
    file.close();
    return true;
}

#else

std::string PathManager::LoadFile(const std::string& path)
{
    // fstream实现
    ifstream fs;
    fs.open(path);

    if (!fs)
    {
        return "";
    }

    istreambuf_iterator<char> start(fs), end;
    string output(start, end);

    fs.close();
    return output;
}

std::string PathManager::LoadFileWithoutComment(const std::string& path)
{
    // fstream实现
    ifstream fs;
    fs.open(path);

    if (!fs)
    {
        return "";
    }
    string output;
    string line;
    while (getline(fs, line))
    {
        if (!isCommentLine(line))
        {
            output += line;
        }
    }

    fs.close();
    return output;
}

bool PathManager::SaveFile(const std::string& path, const std::string& content)
{
    // fstream实现
    ofstream fs;
    fs.open(path);
    if (!fs)
    {
        return false;
    }

    fs << content;
    fs.close();
    return true;
}

#endif

/* #endregion 静态函数 */

/* #region 成员函数 */

#ifdef QT_IMPL

string PathManager::FindFirstFileInPath(const std::string& file_name) const
{
    string output;

    for (const string& p : paths)
    {
        QFileInfo file_info(QString::fromStdString(p) + QDir::separator() + QString::fromStdString(file_name));
        // 如果文件存在
        if (file_info.exists())
        {
            output = file_info.filePath().toStdString();
            break;
        }
    }

    return output;
}

#else

string PathManager::FindFirstFileInPath(const std::string& file_name) const
{
    // std::filesystem实现
    filesystem::path output;

    for (const filesystem::path& p : paths)
    {
        filesystem::path file_path = p / file_name;
        // 如果文件存在
        if (access(file_path.string().c_str(), 0) == 0)
        {
            output = file_path;
            break;
        }
    }

    return output.string();
}

#endif

/* #endregion 成员函数 */
