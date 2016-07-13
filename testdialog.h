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
typedef pair<string, bool> TStrBoolPair;
typedef map<string, bool> TStrBoolMap;

namespace Ui {
class TestDialog;
}

enum ToolTipLineType
{
    HelpTextLine,
    BlankLine,
    OptionLine,
    NextSectionLine,
};

class TestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestDialog(QWidget *parent = 0);
    ~TestDialog();

    // SetDirectory() sets the directory for executing git commands
    void SetDirectory(const QString& dir);

    // DIALOG BOX FORMATTING AND CREATION
    // GetToolTipLineType() determines the tool tip line type
    ToolTipLineType GetToolTipLineType(string& lineStr);
    // HandleSpecialOptionLine() handles special option lines, such as
    // "git stash save ..."
    bool HandleSpecialOptionLine(
            string& optionStr,  // The current checkbox string
            string& tooltipStr, // The current tool tip string
            int& row, int &col);
    size_t FindNextOptionDelim(size_t startPos, string& optionStr) const;
    // HandleOptionLine() creates a checkbox from optionStr and tooltipStr if
    // both are populated; otherwise it continues to populate an incomplete
    // optionStr.
    void HandleOptionLine(
            const string& nextLineStr,  // The next line to process
            string& optionStr,          // The current checkbox string
            string& tooltipStr,         // The current tool tip string
            int& row, int &col);
    // SetCommand() constructs the dialog box from the git command help,
    // parsing all commands under the OPTIONS section
    void SetCommand(const QString& cmd, const QString& arg0);

    void SetTitle(const QString& title);
    void AddCheckbox(const QString& cbTitle, const QString& cbTooltip,
                     int& row, int& col, bool disabled = false);
    void ExecuteLayout();
    bool HasPrefix(char prefix, const string& text) const;

private slots:
    void accept();
    void CBStateChanged(int i);

    void StripChar(char c, string& text);
    QString GetXORChoiceDialog(string title, bool& ok, string& option);
    void ChangeEnabledState(const GitCheckBox* cb);
private:
    Ui::TestDialog*     ui;
    TStrCBMap           m_strCBMap;
    TStrBoolMap         m_strBoolMap;
    QString             m_execDir;
    QString             m_cmd;
    QString             m_arg0;
    QGridLayout         m_gridLayout;
    QDialogButtonBox    m_okCancelBtns;
    int m_lastRow;
};

#endif // TESTDIALOG_H
