#include "gitinitdialog.h"
#include "ui_gitinitdialog.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

GitInitDialog::GitInitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GitInitDialog)
{
    ui->setupUi(this);
    ui->lineEdit_directory->setText(MainWindow::getRootGitDir().filePath());
    setWindowTitle(QString("git init"));
    ui->checkBox_quiet->setFocus();

    MainWindow::SetButtonFormattedToolTip(ui->checkBox_quiet,
                QString("Only print error and warning messages, all other "
                        "output will be suppressed."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_bare,
                QString("Create a bare repository. If GIT_DIR environment is "
                        "not set, it is set to the current working "
                        "directory."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_template,
                QString("Specify the directory from which templates will be "
                        "used."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_separate_git_dir,
                QString("Instead of initializing the repository where it is "
                        "supposed to be, place a filesytem-agnostic Git "
                        "symbolic link there, pointing to the specified path, "
                        "and initialize a Git repository at the path. The "
                        "result is Git repository can be separated from working"
                        " tree. If this is reinitialization, the repository "
                        "will be moved to the specified path."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_shared_permissions,
                QString("Specify that the Git repository is to be shared "
                        "amongst several users. This allows users belonging to "
                        "the same group to push into that repository. When "
                        "specified, the config variable \"core."
                        "sharedRepository\" is set so that files and "
                        "directories under $GIT_DIR are created with the "
                        "requested permissions. When not specified, Git will "
                        "use permissions reported by umask(2)."));
}

GitInitDialog::~GitInitDialog()
{
    delete ui;
}

void GitInitDialog::on_buttonBox_accepted()
{
    const string cmd("git");
    const string execDir(ui->lineEdit_directory->text().toStdString());
    TStrVect args;
    args.push_back("init");
    if(ui->checkBox_quiet->isChecked())
    {
        args.push_back("--quiet");
    }
    if(ui->checkBox_bare->isChecked())
    {
        args.push_back("--bare");
    }
//    if(ui->checkBox_template->isChecked())
//    {
//    }
//    if(ui->checkBox_quiet->isChecked())
//    {
//    }
//    if(ui->checkBox_quiet->isChecked())
//    {
//    }
    TStrVect resultVect;
    MainWindow::GetProcessResults(cmd, execDir, args, resultVect);
    QMessageBox msgBox;
    QString msg;
    msg.append("git ");
    for(auto itr: args)
    {
        string str (itr);
        msg.append(QString(str.c_str())).append(" ");
    }
    msg.append(":\n");
    for(auto itr: resultVect)
    {
        string str (itr);
        msg.append(QString(str.c_str())).append(" ");
    }
    msgBox.setText(msg);
    msgBox.exec();

}

void GitInitDialog::on_btn_directory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                  MainWindow::getRootGitDir().filePath(),
                  QFileDialog::ShowDirsOnly
                  | QFileDialog::DontResolveSymlinks);
    if(!dir.isNull())
    {
        ui->lineEdit_directory->setText(dir);
    }
}
