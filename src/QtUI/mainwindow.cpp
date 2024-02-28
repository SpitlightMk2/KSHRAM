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

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "src/QtUI/kshram_qt_bridge.h"
#include "src/FileSystem/path_manager.h"
#include "versions.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PathManager::GetInstance().AddPath(QDir::currentPath().toStdString());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    static constexpr int offset = 129;
    int window_height = this->height();
    ui->txtboxLog->resize(ui->txtboxLog->width(), window_height - offset);
}

void MainWindow::on_buttonSelectInput_clicked()
{
    QString recent_path = settings.value("recent_path").toString();
    QString path = QFileDialog::getOpenFileName(
        this,tr("Select Input File"), recent_path, tr("K-Shoot Mania chart(*.ksh);;All Files(*)"));
    if(path.isEmpty())
    {
        ui->txtboxInputFile->setText("");
    }
    else
    {
        ui->txtboxInputFile->setText(path);
        settings.setValue("recent_path", path);
    }
}


void MainWindow::on_buttonSelectOutput_clicked()
{
    QString recent_path = settings.value("recent_path").toString();
    QString path = QFileDialog::getSaveFileName(
        this,tr("Select Output File"), recent_path, tr("K-Shoot Mania chart(*.ksh);;All Files(*)"));
    if(path.isEmpty())
    {
        ui->txtboxOutputFile->setText("");
    }
    else
    {
        ui->txtboxOutputFile->setText(path);
        settings.setValue("recent_path", path);
    }
}


void MainWindow::on_buttonClearLog_clicked()
{
    ui->txtboxLog->setPlainText("");
}

inline QString AddPostfix(const QString& ksh)
{
    QString output = ksh;
    output.insert(ksh.lastIndexOf("."), "_out");

    return output;
}

void MainWindow::on_buttonRun_clicked()
{
    QFileInfo in_file(ui->txtboxInputFile->text());
    QString out_file_s = ui->txtboxOutputFile->text();
    QDir in_dir = in_file.dir();

    // 登记输入谱面的位置
    PathManager::GetInstance().AddPath(in_dir.path().toStdString());

    // 处理输出路径
    QFileInfo out_file;
    if(out_file_s.isEmpty())
    {
        out_file.setFile(in_dir.filePath(AddPostfix(in_file.fileName())));
    }
    else
    {
        out_file.setFile(out_file_s);
    }

    if(out_file.isRelative())
    {
        out_file.setFile(in_dir.filePath(out_file.filePath()));
    }

    std::string log = "";
    KSHRAM::ExecuteCommand(in_file.filePath().toStdString(), out_file.filePath().toStdString(),log);

    ui->txtboxLog->setPlainText(QString::fromStdString(log));

    PathManager::GetInstance().RemoveLastPath();
}


void MainWindow::on_buttonAbout_clicked()
{
    ui->txtboxLog->setPlainText(
        "KSHRAM_Qt " PROJECT_VERSION ", Compiled with MinGW.\n"
        "\tCopyright (C) 2024 Singular_Photon\n"
        "===================================================================\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 3 of the License, or\n"
        "(at your option) any later version.\n\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
        "===================================================================\n"
        );
}

