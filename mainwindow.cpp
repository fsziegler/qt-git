#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>
#include <QDesktopWidget>
#include "gitinitdialog.h"

#include <iostream>
#include <sstream>

using namespace std;

QFileInfo MainWindow::ms_rootGitDir;

MainWindow::MainWindow(const QString& cmdStr, QWidget *parent) :
   QMainWindow(parent),
   mc_appDir(QFileInfo(cmdStr).path()),
   mc_cfgFileStr(mc_appDir.filePath() + "/rootGitDir.txt"),
   ui(new Ui::MainWindow)
{
   ms_rootGitDir = cmdStr;
   ui->setupUi(this);
   const QRect screenRect(QApplication::desktop()->availableGeometry());
   if((screenRect.width() >= width()) &&
         (screenRect.height() >= height()))
   {
      setGeometry(
         QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QApplication::desktop()->availableGeometry()));
   }
   setWindowTitle("Fred's qt-git");
   {
      QFile cfgFile(mc_cfgFileStr);
      if(cfgFile.exists())
      {
         if(cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
         {
            QByteArray line = cfgFile.readLine();
            ms_rootGitDir = QFileInfo(QString(line));
            ui->label_git_root->setText(ms_rootGitDir.filePath());
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

void MainWindow::ClearAll()
{
   ui->comboBox_stash->clear();
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
   ui->comboBox_stash->clear();
   const string cmd("git");
   const string execDir(ms_rootGitDir.filePath().toStdString());
   //git status --porcelain

   TStrVect args;
   args.push_back("status");
   args.push_back("--porcelain");
   TStrVect resultVect;
   GetProcessResults(cmd, execDir, args, resultVect);
   if(0 < resultVect.size())//Hello
   {
      /*
          2006> git status --porcelain
              AM qt/cs-ipc/Client.cpp
              AM qt/cs-ipc/EventMessage.cpp
              AM qt/cs-ipc/Server.cpp
              A  qt/cs-ipc/ZiegVersion.h
              AM qt/cs-ipc/cs-ipc.pro
              A  qt/cs-ipc/csipc/Client.h
              A  qt/cs-ipc/csipc/EventMessage.h
              A  qt/cs-ipc/csipc/Server.h
              A  qt/cs-ipc/csipc/defines.h
              AM qt/cs-ipc/internals.cpp
              A  qt/cs-ipc/internals.h
              AM qt/cs-ipc/main.cpp
              ?? qt/build-cs-ipc-Desktop_Qt_5_4_2_GCC_32bit-Debug/
              ?? qt/cs-ipc/cs-ipc.pro.user
              ?? qt/cs-ipc/ziegversion.sh
          [fred][~/dev/sandbox/cs-ipc][Sat Jul 02@19:31:13]
      */
      QRegularExpression reModified("AM (.+)");
      for(auto itr: resultVect)
      {
         const string str0(itr);
         const QString str(str0.c_str());
         QRegularExpressionMatch match = reModified.match(str);
         if(match.hasMatch())
         {
            cout << "Match: [" << match.captured(1).toStdString() << "]" << endl;
            ui->comboBox_stash->addItem(match.captured(1));
         }
      }
   }
}

const QFileInfo& MainWindow::getRootGitDir()
{
    return ms_rootGitDir;
}

void MainWindow::on_btn_choose_git_root_clicked()
{
   QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                 ms_rootGitDir.filePath(),
                 QFileDialog::ShowDirsOnly
                 | QFileDialog::DontResolveSymlinks);
   if(!dir.isNull())
   {
      ClearAll();
      ms_rootGitDir = dir;
      ui->label_git_root->setText(dir);
      QFile cfgFile(mc_cfgFileStr);
      if(cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
      {
         QTextStream out(&cfgFile);
         out << ms_rootGitDir.filePath();
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
//    ui->comboBox_stash->addItem("giddyup");
}

void MainWindow::on_btn_git_init_clicked()
{
   GitInitDialog initDlg;
   initDlg.exec();
}
