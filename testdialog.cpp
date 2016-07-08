#include "testdialog.h"
#include "ui_testdialog.h"
#include "mainwindow.h"
#include "gitcheckbox.h"
#include "optionparser.h"

#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>
#include <QInputDialog>

#include <iostream>

using namespace std;

TestDialog::TestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestDialog),
    m_okCancelBtns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel),
    m_lastRow(0)
{
    ui->setupUi(this);
    connect(&m_okCancelBtns, SIGNAL(accepted()), this, SLOT(accept()));
    connect(&m_okCancelBtns, SIGNAL(rejected()), this, SLOT(reject()));
}

TestDialog::~TestDialog()
{
    delete ui;
}

void TestDialog::SetDirectory(const QString& dir)
{
    m_execDir = dir;
}

int stripLeadingWS(string& str)
{
    int cnt(0);
    while(' ' == str[0])
    {
        str.erase(0, 1);
        ++cnt;
    }
    return cnt;
}

void stripTrailingLF(string& str)
{
    while('\n' == str[str.length() - 1])
    {
        const size_t len(str.length());
        str.erase(len - 2, len - 1);
    }
}

enum ToolTipLineType
{
    HelpTextLine,
    BlankLine,
    OptionLine,
    NextSectionLine,
};

//            short command: ^(-[a-zA-Z]+)
//             long command: (--[a-zA-Z-]+)
//                parameter: <(.+)>
//         equals parameter: =<(.+)>
//       optional parameter: \[<(.+)>\]
//optional equals parameter: \[=<(.+)>\]
const QString longCmdSpec("(--[a-zA-Z-]+)");
const QString shortCmdSpec("^(-[a-zA-Z]+)");

const QString onlyparamSpec("^<(.+)>$");
const QString paramSpec("(.+)<(.+)>");
const QString equalsParamSpec("(.+=)<(.+)>");
const QString optionalParamSpec("(.+)\\[<(.+)>\\]");
const QString optionalEqualsParamSpec("(.+)\\[=<(.+)>\\]");

vector<QString> paramSpecs =
{
    optionalEqualsParamSpec,
    optionalParamSpec,
    equalsParamSpec,
    paramSpec,
    onlyparamSpec,
};

const QRegularExpression reIsSection("^[A-Z][A-Z \-]+$");
ToolTipLineType GetToolTipLineType(string& lineStr)
{
    if(0 == lineStr.length())
    {
        return BlankLine;
    }
    QRegularExpressionMatch match = reIsSection.match(lineStr.c_str());
    if(match.hasMatch())
    {
        return NextSectionLine;
    }
//    "       "
    if(7 == stripLeadingWS(lineStr))
    {
        return OptionLine;
    }
    return HelpTextLine;
}

void ParseSingleOption(const string& option, TStrVect& parsedOpts)
{
    int nested(0);
    size_t ptr(0);
    while(option.length() > ptr)
    {
        nested += ('[' == option[ptr] ? 1 : 0);
        ++ptr;
    }
    if(2 < nested)
    {
        throw;
    }

    // Remove outer square brackets
    string optnCpy(option);
    ptr = 0;
    while('[' != optnCpy[0])
    {
        ++ptr;
    }
    optnCpy.erase(0, ptr + 1);
    ptr = optnCpy.length() - 1;
    while(']' != optnCpy[ptr])
    {
        --ptr;
    }
    optnCpy.erase(ptr, optnCpy.length() - 1);

    if(1 == nested)
    {
        parsedOpts.push_back(optnCpy);
    }
    if(2 == nested)
    {
        size_t ptr_l = optnCpy.find_first_of('[');
        size_t ptr_r = optnCpy.find_first_of(']');

        string optnWith(optnCpy);
        optnWith.erase(ptr_l, 1);
        optnWith.erase(ptr_r-1, 1);
        parsedOpts.push_back(optnWith);

        string optnWithOut(optnCpy);
        optnWithOut.erase(ptr_l, ptr_r - ptr_l + 1);
        parsedOpts.push_back(optnWith);
    }
}

//const QRegularExpression reSubOption("^([^\\\[]+)(.*)");
//const QRegularExpression reSubOption("^([^\\\[]+)(.*)|^([a-z\-]+) (.*)");
const QRegularExpression reSubOption("^([a-z\-]+)(.*)");
const QRegularExpression reSubOptionParam("\\\[([^\\\]]+)");
void TestDialog::HandleSubOptionLine(string& optionStr, string& tooltipStr,
                                     int& row, int &col)
{
    QRegularExpressionMatch optionMatch = reSubOption.match(optionStr.c_str());
    if(optionMatch.hasMatch())
    {
        stripTrailingLF(tooltipStr);
        string option = optionMatch.captured(1).toStdString();
        string subOptions = optionMatch.captured(2).toStdString();
        QRegularExpressionMatch optionParamMatch =
                reSubOptionParam.match(subOptions.c_str());
        if(!optionParamMatch.hasMatch())
        {
            AddCheckbox(option.c_str(), tooltipStr.c_str(), row, col);
            row = (1 == col ? ++row : row);
            col = (0 == col ? 1 : 0);
        }
        else
        {
            OptionParser op(subOptions);
            const TStrVect& optnVect = op.getOptnVect();
            for(TStrVectCItr itr = optnVect.begin(); optnVect.end() != itr;
                ++itr)
            {
                string chkBoxStr(option);
                chkBoxStr.append(*itr);
                AddCheckbox(chkBoxStr.c_str(), tooltipStr.c_str(), row, col);
                row = (1 == col ? ++row : row);
                col = (0 == col ? 1 : 0);
            }
        }
    }
}

void TestDialog::HandleOptionLine(const string& lineStr, string& optionStr,
                                  string& tooltipStr, int& row, int &col)
{
    if((0 < tooltipStr.length())
            && (('-' == optionStr[0])
                || ('<' == optionStr[0])
                || (0 == optionStr.compare(0, 4, "[--]"))))
    {
        stripTrailingLF(tooltipStr);
        AddCheckbox(optionStr.c_str(), tooltipStr.c_str(), row, col);
        row = (1 == col ? ++row : row);
        col = (0 == col ? 1 : 0);
        optionStr.clear();
        tooltipStr.clear();
    }
    else if(0 < tooltipStr.length())
    {
        HandleSubOptionLine(optionStr, tooltipStr, row, col);
        optionStr.clear();
        tooltipStr.clear();
        cout << "No '-'?: " << optionStr << endl;
    }
    optionStr.append(0 < optionStr.length() ? " " : "");
    optionStr.append(lineStr);
}

void TestDialog::SetCommand(const QString& cmd, const QString& arg0)
{
    m_cmd = cmd;
    m_arg0 = arg0;

    const string cmdStr("git");
    const string execDir(".");
    TStrVect args;
    args.push_back(arg0.toStdString());
    args.push_back("--help");
    TStrVect resultVect;
    MainWindow::GetProcessResults(cmdStr, execDir, args, resultVect);

    string title("git-");
    title.append(arg0.toStdString());
    SetTitle(title.c_str());
    for(TStrVectCItr itr = resultVect.begin(); resultVect.end() != itr; ++itr)
    {
       string lineStr(*itr);
       QRegularExpressionMatch match = reIsSection.match(lineStr.c_str());
       if(match.hasMatch())
       {
           if(0 == lineStr.compare("OPTIONS"))
           {
               int row(0), col(0);
               string optionStr;
               string tooltipStr;
               do
               {
                   ++itr;
                   try
                   {
                       lineStr = *itr;
                       switch(GetToolTipLineType(lineStr))
                       {
                           case HelpTextLine:
                               if((0 < tooltipStr.length()) &&
                                       ('\n' != tooltipStr[
                                        tooltipStr.length() - 1]))
                               {
                                   tooltipStr.append(" ");
                               }
                               tooltipStr.append(lineStr);
                               break;
                           case BlankLine:
                               tooltipStr.append("\n\n");
                               break;
                           case OptionLine:
                               HandleOptionLine(lineStr, optionStr, tooltipStr,
                                                row, col);
                               break;
                           case NextSectionLine:
                               break;
                           default:
                               throw;
                       }
                   }
                   catch(...)
                   {
                       int j;
                   }
               } while(NextSectionLine != GetToolTipLineType(lineStr));
               HandleOptionLine(lineStr, optionStr, tooltipStr, row, col);
           }
       }
    }

}

void TestDialog::SetTitle(const QString& title)
{
    setWindowTitle(title);
}

void TestDialog::AddCheckbox(const QString& cbTitle, const QString& cbTooltip,
                             int row, int column, bool disabled)
{
    GitCheckBox* cb = new GitCheckBox(cbTitle, this);
    m_strCBMap.insert(TStrCBPair(cbTitle.toStdString(), cb));
    cb->setDisabled(disabled);
    MainWindow::SetButtonFormattedToolTip(cb, cbTooltip);
    m_gridLayout.addWidget(cb, row, column);

    for(auto itr: paramSpecs)
    {
        const QRegularExpressionMatch match =
                QRegularExpression(itr).match(QString(cbTitle.toStdString().c_str()));
        if(match.hasMatch())
        {
            cb->Connect(this);
            break;
        }
    }

    m_lastRow = (row > m_lastRow ? row : m_lastRow);
}

void TestDialog::ExecuteLayout()
{
    m_gridLayout.addWidget(&m_okCancelBtns, m_lastRow + 1, 0, m_lastRow + 1, 1);
    setLayout(&m_gridLayout);
    show();
}

void TestDialog::accept()
{
    TStrVect args;
    args.push_back(m_arg0.toStdString());
    for(TStrCBMapCItr itr = m_strCBMap.begin(); m_strCBMap.end() != itr; ++itr)
    {
        const QCheckBox* cb = (*itr).second;
        if(cb->isChecked())
        {
            const string& cmdSpec = (*itr).first;
            QRegularExpressionMatch match =
                    QRegularExpression(
                        longCmdSpec).match(QString(cmdSpec.c_str()));
            if(match.hasMatch())
            {
                args.push_back(match.captured(1).toStdString());

                QMessageBox msgBox;
                QString msg("Long Command: ");
                msg.append(match.captured(1));
                msgBox.setText(msg);
                msgBox.exec();
            }
            else
            {
                match = QRegularExpression(
                            shortCmdSpec).match(QString(cmdSpec.c_str()));
                if(match.hasMatch())
                {
                    args.push_back(match.captured(1).toStdString());

                    QMessageBox msgBox;
                    QString msg("Short Command: ");
                    msg.append(match.captured(1));
                    msgBox.setText(msg);
                    msgBox.exec();
                }
            }
        }
    }

/*
    if(ui->checkBox_quiet->isChecked())
    {
        args.push_back("--quiet");
    }
*/
    TStrVect resultVect;
    MainWindow::GetProcessResults(
                m_cmd.toStdString(), m_execDir.toStdString(), args, resultVect);
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

    QDialog::accept();
}

void TestDialog::CBStateChanged(int i)
{
    if(2 == i)  // Case of checked
    {
        for(TStrCBMapCItr itr = m_strCBMap.begin(); m_strCBMap.end() != itr;
            ++itr)
        {
            GitCheckBox* cb = (*itr).second;
            const char* title = (*itr).first.c_str();
            // Find the check box who just got checked
            if(cb->isChecked() && (cb->getOrigText() == cb->text()))
            {
                for(auto itr: paramSpecs)
                {
                    const QRegularExpressionMatch match =
                            QRegularExpression(itr).match(QString(title));
                    if(match.hasMatch())
                    {
                        QString newText(match.captured(1));
                        if(optionalEqualsParamSpec == itr)
                        {
                            newText.append("=");
                        }
                        bool ok;
                        const QString userValue("[USER VALUES]");
                        QString text =
                                QInputDialog::getText(this,
                                                      tr("Input:"),
                                                      newText,
                                                      QLineEdit::Normal,
                                                      userValue, &ok);
                        if (onlyparamSpec == itr)
                        {
                            newText.clear();
                        }

                        if (ok && !text.isEmpty())
                        {
                            newText.append(text);
                        }
                        else
                        {
                            newText.append("[USER VALUES]");
                        }
                        cb->setText(newText);
                        break;
                    }
                }
            }
        }
    }
    else    // Case of unchecked
    {
        for(TStrCBMapCItr itr = m_strCBMap.begin(); m_strCBMap.end() != itr;
            ++itr)
        {
            GitCheckBox* cb = (*itr).second;
            if(!cb->isChecked() && (cb->text() != cb->getOrigText()))
            {
                cb->clearParamText();
                cb->setText(cb->getOrigText());
            }
        }
    }
}
