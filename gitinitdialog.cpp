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
    ui->buttonBox->setFocus();
    ui->checkBox_quiet->setFocus();
}

GitInitDialog::~GitInitDialog()
{
    delete ui;
}

void GitInitDialog::on_pushButton_clicked()
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
