#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>

#include <iostream>
#include <sstream>

using namespace std;

MainWindow::MainWindow(const QString& cmdStr, QWidget *parent) :
    QMainWindow(parent),
    m_rootGitDir(cmdStr),
    mc_appDir(m_rootGitDir.path()),
    mc_cfgFileStr(mc_appDir.filePath() + "/rootGitDir.txt"),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Fred's qt-git");
    {
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

    ui->comboBox_stash->addItem("foo");
    ui->comboBox_stash->addItem("bar");
    ui->comboBox_stash->addItem("doo");
    ui->comboBox_stash->addItem("echo");
    OnGitStatus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

size_t MainWindow::GetProcessResults(const string& cmd, const string& execDir,
                                     const TStrVect& args, TStrVect& resultVect)
{
    // Setup
    resultVect.clear();
    QProcess proc;
    QStringList strList;
    for(TStrVectCItr itr = args.begin(); args.end() != itr; ++itr)
    {
        string str(*itr);
        strList.append(str.c_str());
    }
    if(0 < execDir.length())
    {
        proc.setWorkingDirectory(execDir.c_str());
    }

    // Execute
    proc.start(cmd.c_str(), strList);
//    proc.start(cmd.c_str(), QStringList() << args.c_str() << "-R");
    if (!proc.waitForStarted())
    {
        cout << "!proc.waitForStarted()" << endl;
        return 0;
    }

    proc.closeWriteChannel();
    if (!proc.waitForFinished())
    {
        cout << "!proc.waitForFinished()" << endl;
        return 0;
    }

    // Read results
    QByteArray result = proc.readAll();
    string tmpStr;
    tmpStr.clear();
    for(auto itr: result )
    {
        if('\n' != itr)
        {
            char c(itr);
            tmpStr += c;
        }
        else
        {
            resultVect.push_back(tmpStr);
            tmpStr.clear();
        }
    }
    return resultVect.size();
}

void MainWindow::OnGitStatus()
{
    const string cmd("git");
    const string execDir("/home/fred/dev/sandbox/qt-git");

    TStrVect args;
    args.push_back("status");
    TStrVect resultVect;
    GetProcessResults(cmd, execDir, args, resultVect);
    if(0 < resultVect.size())
    {
/*
2048> git status
On branch master
Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git checkout -- <file>..." to discard changes in working directory)

        modified:   mainwindow.cpp
        modified:   mainwindow.h
        modified:   mainwindow.ui
        modified:   qt-git.pro
        modified:   qt-git.pro.user

no changes added to commit (use "git add" and/or "git commit -a")
[fred][~/dev/sandbox/qt-git][Sat Jul 02@18:34:50]
*/
        QRegularExpression re("[ |\\t]+modified:   (.+)");
        for(auto itr: resultVect)
        {
            const string str0(itr);
            const QString str(str0.c_str());
            QRegularExpressionMatch match = re.match(str);
            if(match.hasMatch())
            {
                cout << "Match: [" << match.captured(1).toStdString() << "]" << endl;
            }
        }
    }
}


void MainWindow::on_btn_choose_git_root_clicked()
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

void MainWindow::on_comboBox_stash_currentIndexChanged(const QString &arg1)
{
//    QMessageBox msgBox;
//    msgBox.setText(arg1);
//    msgBox.exec();
    QString sbMsg("Stash \"");
    sbMsg.append(arg1).append("\" chosen");
    ui->statusBar->showMessage(sbMsg);
    ui->comboBox_stash->addItem("giddyup");
}
