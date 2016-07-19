#ifndef OCTALDIALOG_H
#define OCTALDIALOG_H

#include <QDialog>
#include <string>

using namespace std;

namespace Ui
{
class OctalDialog;
}

class OctalDialog : public QDialog
{
   Q_OBJECT

public:
   explicit OctalDialog(QWidget* parent = 0);
   ~OctalDialog();

   const string& getOctalValStr() const;

private slots:
   void on_radio_btn_clicked();

private:
   Ui::OctalDialog* ui;
   string m_octalValStr;
};

#endif // OCTALDIALOG_H
