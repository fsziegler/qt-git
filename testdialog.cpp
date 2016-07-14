#include "testdialog.h"
#include "ui_testdialog.h"
#include "gitcheckbox.h"
#include "optionparser.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QRadioButton>
#include <QJsonArray>

#include <iostream>

using namespace std;

const int addColumnThreshold(15);
const int minCols(2);

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
    if(0 == str.compare("\n\n"))
    {
        str.clear();
    }
    else
    {
        while('\n' == str[str.length() - 1])
        {
            const size_t len(str.length());
            str.erase(len - 2, len - 1);
        }
    }
}

const QString longCmdSpec("(--[a-zA-Z-]+)");
const QString shortCmdSpec("^(-[a-zA-Z]+)");

const QString onlyparamSpec("^<(.+)>$");
const QString paramSpec("(.+)<(.+)>");
const QString equalsParamSpec("(.+=)<(.+)>");
const QString optionalParamSpec("(.+)\\[<(.+)>\\]");
const QString optionalSlashParamSpec("(.+)\\[/<(.+)>\\]");
const QString optionalEqualsParamSpec("(.+)\\[=<(.+)>\\]");
const QString multipleParamSpec("<(.+)>\\.\\.\\.");

vector<QString> paramSpecs =
{
    optionalEqualsParamSpec,
    optionalSlashParamSpec,
    optionalParamSpec,
    equalsParamSpec,
    paramSpec,
    onlyparamSpec,
    multipleParamSpec,
};

const QString choiceSpec("^.+\\|.+$");

const QRegularExpression reIsSection("^[A-Z][A-Z \\-]+$");
ToolTipLineType TestDialog::GetToolTipLineType(string& lineStr)
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
    if(7 == stripLeadingWS(lineStr))
    {
        const char c = lineStr[0];
        if(0 < c)
        {
            return OptionLine;
        }
        else
        {
            cout << lineStr[0] << endl;
        }
    }
    return HelpTextLine;
}

const QString reSubOptionStr(
        "(^[a-zA-Z\\-<>_]+(?:[\\.]{3})?)(.*)|(\\[--\\])(.*)");
const QRegularExpression reSubOption(reSubOptionStr);
const QRegularExpression reWithNoOptParam("^(--)\\[no-\\](.+)");  // --[no-].+
bool TestDialog::HandleSpecialOptionLine(string& optionStr, string& tooltipStr,
                                         CheckBoxItemVect& cbItemVect)
{
    QRegularExpressionMatch subOptMatch = reSubOption.match(optionStr.c_str());
    const string som0 = subOptMatch.captured(0).toStdString();
    const string som1 = subOptMatch.captured(1).toStdString();
    const string som2 = subOptMatch.captured(2).toStdString();
    const string som3 = subOptMatch.captured(3).toStdString();
    const string som4 = subOptMatch.captured(4).toStdString();

    if(subOptMatch.hasMatch())
    {
        stripTrailingLF(tooltipStr);
        string option = subOptMatch.captured(1).toStdString();
        string parameters = subOptMatch.captured(2).toStdString();
        if('=' == parameters[0])
        {
            option.append("=");
            parameters.erase(0, 1);
        }

        TStrVect optnStrVect;
        QRegularExpressionMatch noOptParamMatch =
                reWithNoOptParam.match(optionStr.c_str());
        if(noOptParamMatch.hasMatch())
        {
            const string s1(noOptParamMatch.captured(1).toStdString());
            const string s2(noOptParamMatch.captured(2).toStdString());
            optnStrVect.push_back(string(s1 + s2));
            optnStrVect.push_back(string(s1 + "no-" + s2));
        }
        else
        {
            optnStrVect.push_back(option);
        }

        for(auto itr: optnStrVect)
        {
            itr.c_str();
            if((0 == parameters.length()) || noOptParamMatch.hasMatch())
            {
                CheckBoxItem cbItem;
                cbItem.tooltip = tooltipStr;
                cbItem.text = itr.c_str();
                cbItemVect.push_back(cbItem);
                cout << itr << endl;
            }
            else
            {
                OptionParser op(parameters);
                const TStrVect& optnVect = op.getOptnVect();
                for(TStrVectCItr itr2 = optnVect.begin(); optnVect.end() != itr2;
                    ++itr2)
                {
                    string chkBoxStr(itr);
                    if(' ' == optionStr[chkBoxStr.length()])
                    {
                        chkBoxStr.append(' ' != chkBoxStr[
                                chkBoxStr.length() - 1] ? " " : "");
                    }
                    chkBoxStr.append(*itr2);

                    CheckBoxItem cbItem;
                    cbItem.tooltip = tooltipStr;
                    cbItem.text = chkBoxStr;
                    cbItemVect.push_back(cbItem);
                    cout << chkBoxStr << endl;
                }
            }
        }
        return true;
    }
    return false;
}

typedef pair<char, char> TCharPair;
typedef vector<TCharPair> TCharPairVect;
TCharPairVect::const_iterator TChrPairVectCItr;
TCharPairVect groupDelim =
{
    {'[',']'},
    {'<','>'},
    {'(',')'},
    {'{','}'},
};

size_t TestDialog::FindNextOptionDelim(size_t startPos, string& optionStr) const
{
    int net(0);
    size_t pos(startPos);
    while(optionStr.length() > pos + 1)
    {
        TCharPairVect::const_iterator itr2;
        for(TCharPairVect::const_iterator itr = groupDelim.begin();
            groupDelim.end() != itr; ++itr)
        {
            net += ((*itr).first == optionStr[pos] ? 1 : 0);
            net -= ((*itr).second == optionStr[pos] ? 1 : 0);
        }
        if(0 == net)
        {
            if(',' == optionStr[pos])
            {
                return pos;
            }
        }
        ++pos;
    }
    return string::npos;
}

void TestDialog::UpdateRowCol(int numCols, int& row, int &col)
{
    const int maxCol(numCols - 1);
    row = (maxCol == col ? row + 1 : row);
    col = (maxCol == col ? 0 : col + 1);
}

void TestDialog::HandleOptionLine(const string& nextLineStr, string& optionStr,
                                  string& tooltipStr,
                                  CheckBoxItemVect& cbItemVect)
{
    if((0 < tooltipStr.length()) && (0 < optionStr.length()))
    {
        stripTrailingLF(tooltipStr);

        size_t pos0(0);
        size_t pos1(FindNextOptionDelim(0, optionStr));
        CheckBoxItemVect tmpCBItemVect;
        CheckBoxItem tmpCBItem;
        if(string::npos == pos1 )
        {
            tmpCBItem.text = optionStr;
            tmpCBItemVect.push_back(tmpCBItem);
        }
        else
        {
            string parsedOptionStr;
            do
            {
                string parsedOptionStr = optionStr.substr(pos0, pos1 - pos0);
                stripLeadingWS(parsedOptionStr);
                tmpCBItem.text = parsedOptionStr;
                tmpCBItemVect.push_back(tmpCBItem);
                pos0 = pos1;
                pos1 = FindNextOptionDelim(++pos0, optionStr);
            }
            while(string::npos != pos1);
            parsedOptionStr = optionStr.substr(pos0);
            stripLeadingWS(parsedOptionStr);
            tmpCBItem.text = parsedOptionStr;
            tmpCBItemVect.push_back(tmpCBItem);
        }
        for(CheckBoxItemVectCItr itr = tmpCBItemVect.begin();
            tmpCBItemVect.end() != itr; ++itr)
        {
            CheckBoxItem itrCBItem = (*itr);
            if(!HandleSpecialOptionLine(itrCBItem.text, tooltipStr, cbItemVect))
            {
                tmpCBItem.text = (*itr).text;
                tmpCBItem.tooltip = (*itr).tooltip;
                cbItemVect.push_back(tmpCBItem);
            }
        }

        optionStr.clear();
        tooltipStr.clear();
    }
    optionStr.append(0 < optionStr.length() ? " " : "");
    optionStr.append(nextLineStr);
}

void TestDialog::SetCommand(const QString& cmd, const QString& arg0)
{
    m_gridLayout.invalidate();
    m_cmd = cmd;
    m_arg0 = arg0;

    const string gitCmd(cmd.toStdString() + "-" + arg0.toStdString());
    TStrVect resultVect = GitJsonText::instance().GetGitCmdVect(gitCmd);
    if(0 == resultVect.size())
    {
        // Get the help text for this git command
        const string cmdStr("git");
        const string execDir(".");
        TStrVect args;
        args.push_back(arg0.toStdString());
        args.push_back("--help");
        MainWindow::GetProcessResults(cmdStr, execDir, args, resultVect);
    }

    string title("git-");
    title.append(arg0.toStdString());
    SetTitle(title.c_str());
    cout << endl << "***** " << title << endl;
    QJsonArray jsonArry;
    CheckBoxItemVect cbItemVect;

    // Parse all the OPTIONs
    for(TStrVectCItr itr = resultVect.begin(); resultVect.end() != itr; ++itr)
    {
       string lineStr(*itr);
       jsonArry.append(QString(lineStr.c_str()));
       QRegularExpressionMatch match = reIsSection.match(lineStr.c_str());
       if(match.hasMatch())
       {
           bool firstOpt(true);
           if(0 == lineStr.compare("OPTIONS"))
           {
               string optionStr;
               string tooltipStr;
               do
               {
                   ++itr;
                   try
                   {
                       lineStr = *itr;
                       jsonArry.append(QString(lineStr.c_str()));
                       switch(GetToolTipLineType(lineStr))
                       {
                           // Append the help text for each option in its tool
                           // tip
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
                               if(firstOpt && (tooltipStr.empty()))
                               {
                                   optionStr.clear();
                                   firstOpt = false;
                               }
                               else
                               {
                                   tooltipStr.append("\n\n");
                               }
                               break;
                           // Parse the command line option
                           case OptionLine:
                               HandleOptionLine(lineStr, optionStr, tooltipStr,
                                                cbItemVect);
                               break;
                           case NextSectionLine:
                               break;
                           default:
                               throw runtime_error("Unknown "
                                                   "GetToolTipLineType()");
                               break;
                       }
                   }
                   catch(exception& e)
                   {
                       QMessageBox msgBox;
                       QString msg("Exception throw: ");
                       msg.append(e.what());
                       msgBox.setText(msg);
                       msgBox.exec();
                   }
                   catch(...)
                   {
                       QMessageBox msgBox;
                       QString msg("Exception throw: UNKNOWN");
                       msg.append(__FILE__);
                       msg.append(": ");
                       msg.append(__LINE__);
                       msgBox.setText(msg);
                       msgBox.exec();
                   }
               } while(NextSectionLine != GetToolTipLineType(lineStr));
               HandleOptionLine(lineStr, optionStr, tooltipStr, cbItemVect);
           }
       }
    }
    int row(0), col(0);
    m_lastRow = 0;

    const int quot(div(cbItemVect.size(), addColumnThreshold).quot);
    const int maxCols(minCols >= quot ? minCols : quot);
    for(CheckBoxItemVectCItr itr = cbItemVect.begin(); cbItemVect.end() != itr;
        ++itr)
    {
        // Populate tmpCBItemVect with all item1s having the same tooltip
        CheckBoxItemVect tmpCBItemVect;
        const string tooltip((*itr).tooltip);
        do
        {
            const string txt((*itr).text);
            tmpCBItemVect.push_back(*itr);
            CheckBoxItemVectCItr tmpItr = itr + 1;
            if((cbItemVect.end() == tmpItr)
                    || (0 != (*tmpItr).tooltip.compare(tooltip)))
            {
                break;
            }
            ++itr;
        } while(cbItemVect.end() != itr);

        // Adjust row/column so that same tooltip items start at column 0 if
        // they will wrap around
        bool tooManyCols(maxCols < col + (int)tmpCBItemVect.size());
        if((1 < tmpCBItemVect.size()) && (tooManyCols))
        {
            ++row;
            col = 0;
        }

        // Populate same tooltip check boxes in dialog box
        const QString cbTooltip(tooltip.c_str());
        for(CheckBoxItemVectCItr itr2 = tmpCBItemVect.begin();
            tmpCBItemVect.end() != itr2; ++itr2)
        {
            const CheckBoxItem& cbItem = (*itr2);
            const QString cbTitle(cbItem.text.c_str());
            AddCheckbox(cbTitle, cbTooltip, row, col);
            UpdateRowCol(maxCols, row, col);
        }
    }
}

void TestDialog::SetTitle(const QString& title)
{
    setWindowTitle(title);
}

void TestDialog::AddCheckbox(const QString& cbTitle, const QString& cbTooltip,
                             int& row, int& col, bool disabled)
{
    static QString prevTooltip;
    bool changeColor = (cbTooltip != prevTooltip);
    static Qt::GlobalColor cbColor;
    if((0 == row) && (0 == col))
    {
        cbColor = Qt::blue;
    }
    else if(changeColor)
    {
        cbColor = (Qt::blue == cbColor ? Qt::darkMagenta : Qt::blue);
    }
    prevTooltip = cbTooltip;

    GitCheckBox* cb = new GitCheckBox(cbTitle, this);
    cb->SetHashText(cbTooltip);

    QPalette p = cb->palette();
    p.setColor(QPalette::Active, QPalette::WindowText, cbColor);
    cb->setPalette(p);

    m_strCBMap.insert(TStrCBPair(cbTitle.toStdString(), cb));
    m_strBoolMap.insert(TStrBoolPair(cbTitle.toStdString(), false));
    cb->setDisabled(disabled);
    MainWindow::SetButtonFormattedToolTip(cb, cbTooltip);
    m_gridLayout.addWidget(cb, row, col);

    cb->Connect(this);

    m_lastRow = (row > m_lastRow ? row : m_lastRow);
//    row = (1 == col ? row + 1  : row);
//    col = (0 == col ? 1 : 0);
}

void TestDialog::ExecuteLayout()
{
    m_gridLayout.addWidget(&m_okCancelBtns, m_lastRow + 1, 0, m_lastRow + 1, 1);
    setLayout(&m_gridLayout);
    show();
}

bool TestDialog::HasPrefix(char prefix, const string& text) const
{
    size_t pos(0);
    while(text.length() > pos + 1)
    {
        const char c = text[pos];
        if((' ' != c) && (prefix != c))
        {
            return false;
        }
        if(prefix == c)
        {
            return true;
        }
        ++pos;
    }
    return false;
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

//                QMessageBox msgBox;
//                QString msg("Long Command: ");
//                msg.append(match.captured(1));
//                msgBox.setText(msg);
//                msgBox.exec();
            }
            else
            {
                match = QRegularExpression(
                            shortCmdSpec).match(QString(cmdSpec.c_str()));
                if(match.hasMatch())
                {
                    args.push_back(match.captured(1).toStdString());

//                    QMessageBox msgBox;
//                    QString msg("Short Command: ");
//                    msg.append(match.captured(1));
//                    msgBox.setText(msg);
//                    msgBox.exec();
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
        msg.append(QString(str.c_str())).append("\n");
    }
    msgBox.setText(msg);
    msgBox.exec();

    QDialog::accept();
}

void TestDialog::CBStateChanged(int i)
{
    ++i;    // Just to get rid of compiler warning
    for(TStrCBMapCItr itr = m_strCBMap.begin(); m_strCBMap.end() != itr; ++itr)
    {
        GitCheckBox* cb = (*itr).second;
        auto boolItr = m_strBoolMap.find((*itr).first);
        const bool isChecked = (*boolItr).second;
        const bool match((cb->isChecked() && !isChecked)
                         || (!cb->isChecked() && isChecked));
        (*boolItr).second = cb->isChecked();

        const char* title = (*itr).first.c_str();

        if(match)
        {
            ChangeEnabledState(cb);
            if(cb->isChecked())
            {
                for(auto itr: paramSpecs)
                {
                    const QRegularExpressionMatch match =
                            QRegularExpression(itr).match(QString(title));
                    if(match.hasMatch())
                    {
                        QString newText(match.captured(1));
                        bool eqPrmSpc(optionalEqualsParamSpec == itr);
                        const QString userValue(eqPrmSpc ? "=[USER VALUES]"
                                                         : "[USER VALUES]");
                        bool ok;
                        QString text =
                                QInputDialog::getText(this,
                                                      tr("Input:"),
                                                      newText,
                                                      QLineEdit::Normal,
                                                      userValue, &ok);
                        if ((onlyparamSpec == itr)
                                || (multipleParamSpec == itr))
                        {
                            newText.clear();
                        }

                        if(ok && (0 != text.compare(userValue)))
                        {
                            if (!text.isEmpty())
                            {
                                if(eqPrmSpc)
                                {
                                    if(!HasPrefix('=', text.toStdString()))
                                    {
                                        newText.append("=");
                                    }
                                }
                                newText.append(text);
                            }
                            else
                            {
                                if((optionalParamSpec != itr)
                                        && (optionalSlashParamSpec != itr)
                                        && (optionalEqualsParamSpec != itr))
                                {
                                    cb->setCheckState(Qt::Unchecked);
                                    (*boolItr).second = false;
                                    cb->clearParamText();
                                    cb->setText(cb->getOrigText());
                                    break;
                                }
                            }
                            cb->setText(newText);
                        }
                        else
                        {
                            cb->setCheckState(Qt::Unchecked);
                            (*boolItr).second = false;
                            cb->clearParamText();
                            cb->setText(cb->getOrigText());
                        }
                        break;
                    }
                    else
                    {
                        if(QRegularExpression(choiceSpec).match(
                                    QString(title)).hasMatch())
                        {
                            bool ok;
                            string option;
                            QString result =
                                    GetXORChoiceDialog(title, ok, option);
                            if(0 < result.length())
                            {
                                cb->clearParamText();
                                QString newText(option.c_str());
                                if(0 != result.compare(" "))
                                {
                                    newText.append("=").append(result);
                                }
                                cb->setText(newText);
                            }
                            else
                            {
                                cb->setCheckState(Qt::Unchecked);
                                (*boolItr).second = false;
                                cb->clearParamText();
                                cb->setText(cb->getOrigText());
                            }
                            break;
                        }

                    }
                }
            }
            else
            {
                cb->clearParamText();
                cb->setText(cb->getOrigText());
            }
        }   // if(match)
    }   //  for(TStrCBMapCItr itr ...
}

void TestDialog::StripChar(char c, string& text)
{
    size_t pos = text.find_first_of(c);
    while(string::npos != pos)
    {
        text.erase(pos, 1);
        pos = text.find_first_of(c);
    }
}

QString TestDialog::GetXORChoiceDialog(string cmdStr, bool& ok, string& option)
{
    //--recurse-submodules[=yes|on-demand|no]
    //--recurse-submodules-default=[yes|on-demand]
    const size_t eqPos(cmdStr.find_first_of('='));
    const size_t lsbPos(cmdStr.find_first_of('['));
    option = cmdStr.substr(0, eqPos > lsbPos ? lsbPos : eqPos);
    bool optParam(eqPos > lsbPos);  // ...[=...
    TStrVect optionsVect;
    size_t pos0(eqPos > lsbPos ? eqPos : lsbPos);
    size_t pos1(cmdStr.find_first_of('|'));
    const size_t end(cmdStr.find(']'));

    ++pos0;
    string opt;
    while(string::npos != (pos1 = cmdStr.find('|', pos0)))
    {
        opt = cmdStr.substr(pos0, pos1 - pos0);
        StripChar('(', opt);
        StripChar(')', opt);
        optionsVect.push_back(opt);
        pos0 = ++pos1;
    }
    opt = cmdStr.substr(pos0, end - pos0);
    StripChar('(', opt);
    StripChar(')', opt);
    optionsVect.push_back(opt);

    int row(0), col(0);
    QDialog* optDlg = new QDialog(this);
    optDlg->setWindowTitle(option.c_str());
    QGridLayout* gridLayout = new QGridLayout(optDlg);

    vector<QRadioButton*> btnVect;
    const QString NoOptionStr("(No Option)");
    if(optParam)
    {
        QRadioButton* rb = new QRadioButton(NoOptionStr, optDlg);
        btnVect.push_back(rb);
        gridLayout->addWidget(rb, row++, col);
    }
    for(TStrVectCItr itr = optionsVect.begin(); optionsVect.end() != itr; ++itr)
    {
        QRadioButton* rb = new QRadioButton((*itr).c_str(), optDlg);
        btnVect.push_back(rb);
        gridLayout->addWidget(rb, row++, col);
    }

    QDialogButtonBox* okCancelBtns = new QDialogButtonBox(QDialogButtonBox::Ok
                                                          | QDialogButtonBox::Cancel);
    connect(okCancelBtns, &QDialogButtonBox::accepted, optDlg, &QDialog::accept);
    connect(okCancelBtns, &QDialogButtonBox::rejected, optDlg, &QDialog::reject);
    gridLayout->addWidget(okCancelBtns, row, col);
    optDlg->setLayout(gridLayout);
    ok = optDlg->exec();
    if(ok)
    {
        size_t cnt(0);
        while(btnVect.size() > cnt)
        {
            const QRadioButton* rb = btnVect[cnt];
            if(rb->isChecked())
            {
                bool b = (NoOptionStr == rb->text());
                return (b ? " " : rb->text());
            }
            ++cnt;
        }
    }
    return QString("");
}

void TestDialog::ChangeEnabledState(const GitCheckBox* cb)
{
    if('-' == cb->text().toStdString()[0])
    {
        const QByteArray hash = cb->getHash();
        const QString text = cb->text();
        for(TStrCBMapCItr itr = m_strCBMap.begin(); m_strCBMap.end() != itr;
            ++itr)
        {
            GitCheckBox* currCb = (*itr).second;
            if((text != currCb->text()) && (hash == currCb->getHash()))
            {
                currCb->setEnabled(!cb->isChecked());
            }
        }
    }
}
