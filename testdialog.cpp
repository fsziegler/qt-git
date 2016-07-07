#include "testdialog.h"
#include "ui_testdialog.h"
#include "mainwindow.h"
#include "gitcheckbox.h"

#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>
#include <QInputDialog>

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

void TestDialog::SetCommand(const QString& cmd, const QString& arg0)
{
    m_cmd = cmd;
    m_arg0 = arg0;
}

void TestDialog::SetTitle(const QString& title)
{
    setWindowTitle(title);
}

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

void TestDialog::AddCheckbox(const QString& cbTitle, const QString& cbTooltip,
                             int row, int column, bool disabled)
{
    GitCheckBox* cb = new GitCheckBox(cbTitle, this);
    m_strCBMap.insert(TStrCBPair(cbTitle.toStdString(), cb));
//    cb->setDisabled(disabled);
    MainWindow::SetButtonFormattedToolTip(cb, cbTooltip);
    m_gridLayout.addWidget(cb, row, column);

    for(auto itr: paramSpecs)
    {
        //QRegularExpression
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
