#ifndef GITCLONEDIALOG_H
#define GITCLONEDIALOG_H

#include <QDialog>

namespace Ui {
class GitCloneDialog;
}

class GitCloneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GitCloneDialog(QWidget *parent = 0);
    ~GitCloneDialog();

private slots:
    void on_btn_directory_clicked();

private:
    Ui::GitCloneDialog *ui;
};

#endif // GITCLONEDIALOG_H
