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

#include "command_batch.h"

#include <fstream>
#include <string>

#include "src/FileSystem/path_manager.h"

using namespace std;

static bool isCommentLine(const std::string& line)
{
    string stripped = line;
    Strip(stripped);
    return (stripped.find("//") == 0);
}

static string readFile(const filesystem::path& path)
{
    fstream fs;
    fs.open(path, ios::in);

    if (!fs)
    {
        return "";
    }
    string output;
    string line;
    while(getline(fs, line))
    {
        if(!isCommentLine(line))
        {
            output += line;
        }
    }

    fs.close();
    return output;
}

/* #region METADATA */

vector<string> CommandBatch::AcceptedCmdName() { return {"batch"}; }

bool CommandBatch::CheckArgs(const Command& cmd)
{
    if (cmd.argLength() < 2)
    {
        return false;
    }

    if (cmd.arg(0) == "define")
    {
        return cmd.argLength() == 3;
    }
    else if (cmd.arg(0) == "call")
    {
        return cmd.argLength() == 2;
    }
    else
    {
        return true;
    }
}

/* #endregion METADATA */

/* #region 三大功能 */

bool CommandBatch::DefineBatch(
    const std::string& name, const std::string& content,
    ApplicationBus::ErrorStack& err_stack)
{
    ApplicationBus& bus = ApplicationBus::GetInstance();
    Command cmd_in(content);
    CommandMap cmds;

    cmds.ImportFromString(content);

    batch_map.insert_or_assign(name, std::move(cmds));
    return true;
}

CommandMap CommandBatch::LoadBatch(const std::string& name)
{
    auto iter = batch_map.find(name);
    if (iter != batch_map.end())
    {
        return iter->second;
    }
    else
    {
        return CommandMap();
    }
}

bool CommandBatch::ImportFromFile(const string& path)
{
    bool success = true;

    // 这里的path是可以直接读的。文件的搜寻交给更外面
    string file_content = PathManager::LoadFileWithoutComment(path);
    if (file_content.empty())
    {
        return false;
    }
    ToLower(file_content);

    // 拆分
    vector<string> content_split = BlankSplitWithBracket(file_content, true);
    // 每找到一个batch define就去执行
    const int content_split_size = content_split.size();
    for (int i = 0; i < content_split_size; ++i)
    {
        if (content_split[i] == "batch")
        {
            if (i <= content_split_size - 4 && content_split[i + 1] == "define")
            {
                ApplicationBus::ErrorStack err_stack;

                // EXECUTE
                string& name = content_split[i + 2];
                string& cmd_str = content_split[i + 3];
                success = this->DefineBatch(name, cmd_str, err_stack);
                if (!success)
                {
                    if (err_stack.empty())
                    {
                        GET_ERROR_COLLECTOR.ErrorLog(
                            "Failed to load this batch. Check if there is any syntax error or infinite recursion."
                        );
                    }
                    else
                    {
                        std::string all_invalid_cmds = "";
                        for (auto& cmd : err_stack)
                        {
                            all_invalid_cmds += "\n\t" + cmd;
                        }

                        GET_ERROR_COLLECTOR.ErrorLog(
                            "Failed to load this batch. Detected following command with invalid syntax:" + all_invalid_cmds
                        );
                    }
                }

                // 这一段都跳过
                i += 3;
            }
        }
    }

    return success;
}

/* #endregion 三大功能 */

/* #region 总入口 */

bool CommandBatch::ProcessCmd(const Command& cmd, CommandMap& cmd_map, IndexedChart& chart)
{
    bool success = true;
    const string& subtype = cmd.arg(0);

    if (subtype == "define")
    {
        ApplicationBus::ErrorStack err_stack;

        success = DefineBatch(cmd.arg(1), cmd.arg(2), err_stack);
        if (!success)
        {
            if (err_stack.empty())
            {
                GET_ERROR_COLLECTOR.ErrorLog(
                    "Failed to load this batch. Check if there is any syntax error or infinite recursion."
                );
            }
            else
            {
                std::string all_invalid_cmds = "";
                for (auto& cmd : err_stack)
                {
                    all_invalid_cmds += "\n\t" + cmd;
                }

                GET_ERROR_COLLECTOR.ErrorLog(
                    "Failed to load this batch. Detected following command with invalid syntax:" + all_invalid_cmds
                );
            }
        }
    }

    else if (subtype == "call")
    {
        auto& err_collector = GET_ERROR_COLLECTOR;
        CommandMap batch = LoadBatch(cmd.arg(1));
        if (batch.Empty())
        {
            err_collector.ErrorLog("Calling undefined batch: " + cmd.arg(1) + ".");
            success = false;
        }
        else
        {
            err_collector.AddToStack(cmd.toOriginalString());
            ApplicationBus::GetInstance().ExecuteBatch(batch, cmd.time());
            err_collector.PopStack();
        }
    }

    else if (subtype == "import")
    {
        string path_in_cmd = cmd.substring(1);
        string full_path = PathManager::GetInstance().FindFirstFileInPath(path_in_cmd);
        if (full_path.empty())
        {
            GET_ERROR_COLLECTOR.ErrorLog("Import file not found.");
            success = false;
        }
        else
        {
            success = ImportFromFile(full_path);
        }
    }
    else
    {
        success = false;
    }

    return success;
}

/* #endregion 总入口 */