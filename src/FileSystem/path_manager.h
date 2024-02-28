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

#include <filesystem>
#include <string>
#include <vector>

/*
由于国内windows坑b操作系统的问题，搞了半天也没搞出来不依赖外部库就把gbk或者shift-jis转utf8之后读地址的方法。
以后有闲工夫的话看看能不能接一个icu进来吧。
Qt还是强啊，直接一个toLocal8Bit()就编程utf-8了。
所以目前整个程序只能读纯ascii的路径。
*/


/// 负责登记路径和查找文件。
///
/// 由于需要为console和Qt准备两套不同的文件读写实现，所以这部分也归它。
class PathManager
{
private:
    PathManager() = default;
    PathManager(const PathManager&) = delete;
    PathManager& operator=(const PathManager&) = delete;
public:
    // SINGLETON
    static PathManager& GetInstance();

private:
    std::vector<std::string> paths;

// STATIC
public:
    /// 打开文件，将全部内容读到一个字符串中。
    static std::string LoadFile(const std::string& path);
    /// 打开文件，去除其中的注释，其余部分读到一个字符串中。
    static std::string LoadFileWithoutComment(const std::string& path);
    /// 将字符串内的全部内容写入文件
    static bool SaveFile(const std::string& path, const std::string& content);

public:
    /// 添加一个新的路径
    inline void AddPath(const std::string& path);
    /// 删除最近添加的路径
    inline void RemoveLastPath();

    /// 按照路径添加顺序，逐个搜索特定的文件。
    /// 如果返回了一个地址，该地址肯定能开。如果文件没找到，那么返回空路径。
    std::string FindFirstFileInPath(const std::string& file_name) const;

};

/* INLINE FUNCTIONS */
#include "path_manager_inline.h"
