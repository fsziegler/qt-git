#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

using namespace std;

MainWindow::MainWindow(const QString& cmdStr, QWidget *parent) :
    QMainWindow(parent),
    m_rootGitDir(cmdStr),
    m_appDir(cmdStr),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    QMessageBox msgBox;
    msgBox.setText(dir);
    msgBox.exec();
}
