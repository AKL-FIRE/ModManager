//
// Created by changyuli on 6/11/21.
//

#ifndef RYZAMODMANAGER__MAINWINDOW_H_
#define RYZAMODMANAGER__MAINWINDOW_H_

#include "Recorder.h"

#include <iostream>
#include <memory>

#include <QWidget>
#include <QStandardItemModel>
#include <QMenu>
#include <QFileDialog>
#include <QPushButton>
#include <QDebug>
#include <QItemSelectionModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
 Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 public slots:
  // 选择目录
  void ClickOpenButton();
  // 右键菜单响应函数
  void slotContextMenu(QPoint pos);
  // 选中一项
  void ShowImage();
  // 安装mod
  void InstallMod();
  // 卸载mod
  void DeleteMod();

 private:
  Ui::MainWindow *ui;
  QString working_directory_;
  QMenu* pop_menu_;
  QStandardItemModel* q_standard_item_model_;
  QItemSelectionModel* q_item_selection_model_;
  std::unique_ptr<QDir> root_dir_;
  std::unique_ptr<QDir> mod_dir_;
  std::unique_ptr<QPixmap> picture_;
  std::unique_ptr<Recorder> recorder_;
  QString executable_path_;


  void InitModItem(QStandardItemModel *p_model);
  void InitialModList();
  QStringList GetAllFilePath(const QString &q_string);
  void GetAllFilePath_(const QDir &q_string, QStringList& list);
  void ReplaceFiles(const QStringList &list, QStandardItem *p_item, int mode);
  void InitialWD();
};

#endif //RYZAMODMANAGER__MAINWINDOW_H_
