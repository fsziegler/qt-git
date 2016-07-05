#include "gitcheckbox.h"
#include <QMessageBox>

GitCheckBox::GitCheckBox(const QString &text, TestDialog* pTestDialog,
                         QWidget *parent)
: QCheckBox(text, parent),
  mp_TestDialog(pTestDialog),
  m_stateRead(false),
  m_origText(text)
{
}

void GitCheckBox::Connect(TestDialog* pTestDialog)
{
    QObject::connect(this, SIGNAL(stateChanged(int)),
                     pTestDialog, SLOT(CBStateChanged(int)));
}

const QString& GitCheckBox::getOrigText() const
{
    return m_origText;
}

void GitCheckBox::clearParamText()
{
    m_paramText.clear();
}
