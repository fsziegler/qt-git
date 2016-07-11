#ifndef GITCHECKBOX_H
#define GITCHECKBOX_H

#include <QObject>
#include <QWidget>
#include "testdialog.h"

class GitCheckBox : public QCheckBox
{
public:
    explicit GitCheckBox(const QString &text, TestDialog* pTestDialog,
                         QWidget *parent = Q_NULLPTR);
    GitCheckBox();
    void Connect(TestDialog* pTestDialog);

    const QString& getOrigText() const;
    void clearParamText();

    void SetHashText(const QString& hashText);
    QByteArray getHash() const;

private:
    TestDialog* mp_TestDialog;
    bool m_stateRead;
    const QString m_origText;
    QString m_paramText;
    QString m_toolTip;
    QByteArray m_hash;
};

#endif // GITCHECKBOX_H
