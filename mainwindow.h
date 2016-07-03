#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

#include <string>
#include <vector>

using namespace std;

typedef vector<string> TStrVect;
typedef TStrVect::const_iterator TStrVectCItr;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& cmdStr, QWidget *parent = 0);
    ~MainWindow();

    static size_t GetProcessResults(const string& cmd, const string& execDir,
                                    const TStrVect& args, TStrVect& resultVect);

    void OnGitStatus();

private slots:
    void on_btn_choose_git_root_clicked();

    void on_comboBox_stash_currentIndexChanged(const QString &arg1);

private:
    QFileInfo m_rootGitDir;
    const QFileInfo mc_appDir;
    const QString mc_cfgFileStr;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
