#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QCheckBox>
#include <QLabel>
#include <QJsonObject>

#include "gitjsontext.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    static void ClearStaticMembers();

public:
    explicit MainWindow(const QString& cmdStr, QWidget *parent = 0);
    ~MainWindow();

    void ClearStash();
    static size_t GetProcessResults(const string& cmd, const string& execDir,
                                    const TStrVect& args, TStrVect& resultVect);

    void OnGitStatus();
    static const QFileInfo& getRootGitDir();
    static void SetButtonFormattedToolTip(QAbstractButton *pCB,
                                            const QString& tooltip,
                                            int width=120);
    static void ReadSettings();
    static void SaveSettings();
    static void SaveSettings(const QJsonObject& jsonObj,
                             const string& fileNameStr);

private slots:
    void on_btn_choose_git_root_clicked();

    void on_comboBox_stash_currentIndexChanged(const QString &arg1);

    void on_btn_git_init_clicked();

    void on_btn_git_clone_clicked();

    void on_btn_remote_repo_clicked();

    void on_btn_git_branch_clicked();

    void on_btn_git_checkout_clicked();

    void on_btn_git_merge_clicked();

    void on_btn_git_fetch_clicked();

    void on_btn_git_rebase_clicked();

    void on_btn_git_add_clicked();

    void on_btn_git_stash_clicked();

    void on_btn_git_commit_clicked();

    void on_btn_git_push_clicked();

    void on_btn_git_pull_clicked();

    void on_btn_git_diff_clicked();

    void on_btn_git_status_clicked();

    void on_btn_git_log_clicked();

private:
    static QJsonObject ms_jsonSettings;
    static QFileInfo ms_rootGitDir;
    static QString ms_settingsFileStr;
    static QString ms_remoteRepoFileStr;

    const QFileInfo mc_appDir;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
