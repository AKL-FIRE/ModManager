#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow mywindow(nullptr);
  mywindow.show();
  return QApplication::exec();
}
