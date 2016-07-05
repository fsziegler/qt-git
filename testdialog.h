#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <map>

using namespace std;

class GitCheckBox;
typedef pair<string, GitCheckBox*> TStrCBPair;
typedef map<string, GitCheckBox*> TStrCBMap;
typedef TStrCBMap::const_iterator TStrCBMapCItr;

namespace Ui {
class TestDialog;
}

class TestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestDialog(QWidget *parent = 0);
    ~TestDialog();

    void SetDirectory(const QString& dir);
    void SetCommand(const QString& cmd, const QString& arg0);
    void SetTitle(const QString& title);
    void AddCheckbox(const QString& cbTitle, const QString& cbTooltip,
                     int row, int column, bool disabled = false);
    void ExecuteLayout();

private slots:
    void accept();
    void CBStateChanged(int i);

private:
    Ui::TestDialog *ui;
    TStrCBMap m_strCBMap;
    QString m_execDir;
    QString m_cmd;
    QString m_arg0;
    QGridLayout m_gridLayout;
    QDialogButtonBox m_okCancelBtns;
    int m_lastRow;
};

#endif // TESTDIALOG_H
