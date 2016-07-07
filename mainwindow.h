#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QCheckBox>
#include <QLabel>

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

    void ClearAll();
    static size_t GetProcessResults(const string& cmd, const string& execDir,
                                    const TStrVect& args, TStrVect& resultVect);

    void OnGitStatus();
    static const QFileInfo& getRootGitDir();
    static void SetButtonFormattedToolTip(QAbstractButton *pCB,
                                            const QString& tooltip,
                                            int width=120);
    static const QString& ReadSettingFromFile(const QString& settingFileStr,
                                              const QString& altTextStr,
                                              QLabel* settingLabel,
                                              QString& settingStr);

private slots:
    void on_btn_choose_git_root_clicked();

    void on_comboBox_stash_currentIndexChanged(const QString &arg1);

    void on_btn_git_init_clicked();

    void on_btn_git_clone_clicked();

    void on_btn_remote_repo_clicked();

    void on_btn_git_branch_clicked();

    void on_btn_git_checkout_clicked();

private:
    static QFileInfo ms_rootGitDir;
    const QFileInfo mc_appDir;
    const QString mc_cfgFileStr;
    const QString mc_remoteRepoFileStr;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
