#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& cmdStr, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    QFileInfo m_rootGitDir;
    const QFileInfo mc_appDir;
    const QString mc_cfgFileStr;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
