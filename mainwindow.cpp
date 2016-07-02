#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <iostream>

using namespace std;

MainWindow::MainWindow(const QString& cmdStr, QWidget *parent) :
    QMainWindow(parent),
    m_rootGitDir(cmdStr),
    mc_appDir(m_rootGitDir.path()),
    mc_cfgFileStr(mc_appDir.filePath() + "/rootGitDir.txt"),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile cfgFile(mc_cfgFileStr);
    if(cfgFile.exists())
    {
        if(cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QByteArray line = cfgFile.readLine();
            m_rootGitDir = QFileInfo(QString(line));
            ui->label_git_root->setText(m_rootGitDir.filePath());
        }
    }
    else
    {
        ui->label_git_root->setText("[Git root unknown]");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 m_rootGitDir.filePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(!dir.isNull())
    {
        m_rootGitDir = dir;
        ui->label_git_root->setText(dir);
        QFile cfgFile(mc_cfgFileStr);
        if(cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&cfgFile);
            out << m_rootGitDir.filePath();
            cfgFile.close();
        }
    }
}
