#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QFileInfo>

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    cout << "argv[0] = " << argv[0] << endl;
    QString cmdStr(argv[0]);
    QFileInfo fi(cmdStr);
    cout << "QFileInfo = " << fi.path().toStdString() << endl;

    MainWindow w(cmdStr);
    w.show();

    return a.exec();
}
