//
// Created by changyuli on 6/11/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  this->setWindowTitle("RyzaModManager");
  this->resize(1536, 864);
  connect(ui->openPushButton, SIGNAL(clicked()), this, SLOT(ClickOpenButton()));
  InitialWD();
  InitialModList();
  connect(ui->modLists, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));
  connect(ui->playPushButton, SIGNAL(clicked(bool)), this, SLOT(Play()));
}

void MainWindow::InitialModList() {// 初始化mod列表
  // 初始化记录器
  recorder_ = std::make_unique<Recorder>(QDir::currentPath());
  /* 创建数据模型 */
  q_standard_item_model_ = new QStandardItemModel(this);
  q_item_selection_model_ = new QItemSelectionModel(q_standard_item_model_);
  /* 设置表格标题行(输入数据为QStringList类型) */
  q_standard_item_model_->setHorizontalHeaderLabels({"项目", "状态"});
  /* 读取mod文件夹内容,并更新数据模型 */
  InitModItem(q_standard_item_model_);
  /* 设置表格视图数据 */
  ui->modLists->setModel(q_standard_item_model_);
  ui->modLists->setSelectionModel(q_item_selection_model_);
  ui->modLists->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->modLists->setSelectionBehavior(QAbstractItemView::SelectItems);
  // 设置选中为整行选中
  ui->modLists->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->modLists->setColumnWidth(0, 600);
  ui->modLists->setContextMenuPolicy(Qt::CustomContextMenu); // 可弹出菜单
  /* 显示 */
  ui->modLists->show();

  pop_menu_ = new QMenu(ui->modLists); // 右键弹出菜单
  pop_menu_->addAction("安装", this, SLOT(InstallMod()));
  pop_menu_->addAction("卸载", this, SLOT(DeleteMod()));
  pop_menu_->addAction("显示", this, SLOT(ShowImage()));
}

void MainWindow::slotContextMenu(QPoint pos) {
  auto index = ui->modLists->indexAt(pos);
  if (index.isValid())
  {
	pop_menu_->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
  }
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::ClickOpenButton() {
  QStringList fileNameList;
  // 创建一个打开文件对话框
  auto directory_name = QFileDialog::getExistingDirectory(this);
  if (!directory_name.isEmpty()) {
    // std::cout << "Open directory: \"" << directory_name.toStdString() << "\" successful." << std::endl;
    working_directory_ = directory_name;
    // 更新显示框内容为路径
    ui->directoryRoot->setText(working_directory_);
  } else {
	std::cerr << "\"" <<  directory_name.toStdString() << "\" open failed." << std::endl;
  }
}

void MainWindow::InitModItem(QStandardItemModel *p_model) {
  // 1. 获取当前可执行文件路径
  auto path = QDir::currentPath();
  // 2. 检查当前路径有无mods文件夹
  auto root_path = QDir(path);
  root_dir_ = std::make_unique<QDir>(root_path);
  if (!root_path.exists("mods")) {
    // 创建一个文件夹
    root_path.mkdir("mods");
  }
  // 获取mods项目
  auto mods_directory{ root_path };
  mods_directory.cd("mods");
  mod_dir_ = std::make_unique<QDir>(mods_directory);
  auto mods_name = mods_directory.entryList(QDir::Filters {QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries});
  for (int i = 0; i < mods_name.size(); ++i) {
	// 构建数据模型
	p_model->setItem(i, 0, new QStandardItem(mods_name[i]));
	if (recorder_->contain(mods_name[i].toStdString())) {
	  p_model->setItem(i, 1, new QStandardItem("已安装"));
	} else {
	  p_model->setItem(i, 1, new QStandardItem(""));
	}
  }
}

void MainWindow::ShowImage() {
  // 获取当前选中的mod
  auto curIndexList = q_item_selection_model_->selectedRows();
  if (!curIndexList.empty()) {
	auto mod_item_index = curIndexList[0];
	auto mod_name = q_standard_item_model_->item(mod_item_index.row(), mod_item_index.column());
	// 查找图片
	auto temp_path = *mod_dir_;
	if(temp_path.cd(mod_name->text())) {
	  // 成功打开
	  auto list = temp_path.entryList();
      auto filtered = list.filter(QRegularExpression("\\.(jpg|gif|png)$"));
	  if (!filtered.empty()) {
	    auto img_name = filtered[0];
	    // 显示图片
	    auto img_full_path = temp_path.absoluteFilePath(img_name);
	    QPixmap pic_temp(img_full_path);
	    picture_ = std::make_unique<QPixmap>(pic_temp.scaled(ui->showImageLabel->size(), Qt::KeepAspectRatio));
	    ui->showImageLabel->setPixmap(*picture_);
	  }
	}
  }
}

void MainWindow::InstallMod() {
  if (working_directory_.isEmpty()) {
    qDebug() << "没有选择文件夹";
    auto message_box = QMessageBox::critical(this, "错误", "请先选择游戏目录");
	return;
  }
  // 获取当前选中的mod
  auto curIndexList = q_item_selection_model_->selectedRows();
  if (!curIndexList.empty()) {
	auto mod_item_index = curIndexList[0];
	auto mod_name = q_standard_item_model_->item(mod_item_index.row(), mod_item_index.column());
	// 查找图片
	auto temp_path = *mod_dir_;
	if(temp_path.cd(mod_name->text())) {
	  // 成功打开
	  QString base_path = temp_path.absolutePath();
	  auto file_path_vec = GetAllFilePath(base_path);
	  // 检查冲突
      if (CheckConflict(mod_name->text(), file_path_vec)) {
	    auto mes = QMessageBox::critical(this, "错误", "目标文件已经被替换过");
		return;
	  }
	  // 替换并备份文件
	  bool success;
	  auto file_names = ReplaceFiles(file_path_vec, mod_name, 0, success);
	  // 记录替换信息
	  if (success) {
		recorder_->Save(mod_name->text().toStdString(), file_names);
		// 更新ui
		auto mod_status = q_standard_item_model_->item(mod_item_index.row(), 1);
		mod_status->setText("已安装");
	  }
	}
  }
}

void MainWindow::DeleteMod() {
  if (working_directory_.isEmpty()) {
	qDebug() << "没有选择文件夹";
	auto message_box = QMessageBox::critical(this, "错误", "请先选择游戏目录");
	return;
  }
  // 获取当前选中的mod
  auto curIndexList = q_item_selection_model_->selectedRows();
  if (!curIndexList.empty()) {
	auto mod_item_index = curIndexList[0];
	auto mod_name = q_standard_item_model_->item(mod_item_index.row(), mod_item_index.column());
	auto temp_path = *mod_dir_;
	if(temp_path.cd(mod_name->text())) {
	  // 成功打开
	  QString base_path = temp_path.absolutePath();
	  auto file_path_vec = GetAllFilePath(base_path);
	  // 恢复原版文件
	  bool success;
	  ReplaceFiles(file_path_vec, mod_name, 1, success);
	  if (success) {
		// 删除替换信息
		recorder_->Delete(mod_name->text().toStdString());
		// 更新ui
		auto mod_status = q_standard_item_model_->item(mod_item_index.row(), 1);
		mod_status->setText("");
	  }
	}
  }
}

QStringList MainWindow::GetAllFilePath(const QString &q_string) {
  QStringList result {};
  GetAllFilePath_(QDir(q_string), result);
  return result;
}

void MainWindow::GetAllFilePath_(const QDir &q_dir, QStringList& list) {
  if (q_dir.isEmpty())
	return;
  auto entry_lists = q_dir.entryInfoList(QDir::Filters {QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot});
  for (const auto& entry : entry_lists) {
    if (entry.isDir()) {
      // directory
      auto temp_path {q_dir.absoluteFilePath(entry.fileName())};
	  GetAllFilePath_(QDir(temp_path), list);
    } else if (entry.isFile() &&
		(entry.suffix() != "jpg" && entry.suffix() != "png")) {
      // file
      list.append(entry.absoluteFilePath());
    }
  }
}

// mode 0: install
// mode 1: uninstall
std::vector<std::string> MainWindow::ReplaceFiles(const QStringList &list, QStandardItem *p_item, int mode, bool& result) {
  result = true;
  std::vector<std::string> file_names;
  if (working_directory_.isEmpty()) {
    qDebug() << "没有选择文件夹";
    result = false;
	return file_names;
  }
  for (const auto& file : list) {
    // 获得相对路径
    auto relative_path = file.split(p_item->text())[1];
    relative_path.remove(0, 1);
    // std::cout << relative_path.toStdString() << std::endl;
    file_names.push_back(relative_path.toStdString());
    // qDebug() << relative_path;
    auto target_file = QDir(working_directory_).filePath(relative_path);
    QString to {target_file + "_origin"};
    // std::cout << "from: " << target_file.toStdString() << "\nto: " << to.toStdString() << std::endl;
    if (mode == 0) {
      if (!QFile::exists(target_file) && QFile::exists(file)) {
        // 新添加的文件,把mod文件复制过来
        QFileInfo file_info {target_file};
        // std::cout << file_info.absolutePath().toStdString() << std::endl;
        if (!QDir(working_directory_).mkpath(file_info.absolutePath())) {
          QMessageBox::critical(this, "错误", "无法创建目录" + file_info.absolutePath());
          result = false;
          break;
        }
        // 复制文件过来
        if (!QFile::copy(file, target_file)) {
          QMessageBox::critical(this, "错误", "无法把文件: " + target_file + "重命名成: " + to);
          result = false;
          break;
        }
      }
      else if (QFile::exists(target_file) && QFile::exists(file)) {
        // 旧文件改名
        if (!QFile::rename(target_file, to)) {
          QMessageBox::critical(this, "错误", "无法把文件: " + target_file + "重命名成: " + to);
          result = false;
          break;
        }
        // 把mod文件复制过来
        if (!QFile::copy(file, target_file)) {
          QMessageBox::critical(this, "错误", "无法把文件: " + target_file + "重命名成: " + to);
          result = false;
          break;
        }
      } else if (!QFile::exists(target_file) || !QFile::exists(file)) {
        QMessageBox::critical(this, "错误", "文件不存在");
        result = false;
        break;
      }
    } else if (mode == 1) {
      if (QFile::exists(target_file) && !QFile::exists(to)) {
        // 删除现在的文件
        if (!QFile::remove(target_file)) {
          QMessageBox::critical(this, "错误", "无法删除文件: " + target_file);
          result = false;
          break;
        }
      } else if (QFile::exists(target_file) && QFile::exists(to)) {
        // 删除现在的文件
        if (!QFile::remove(target_file)) {
          QMessageBox::critical(this, "错误", "无法删除文件: " + target_file);
          result = false;
          break;
        }
        // 把备份文件替换成源文件
        if (!QFile::rename(to, target_file)) {
          QMessageBox::critical(this, "错误", "无法把文件: " + to + "重命名成: " + target_file);
          result = false;
          break;
        }
      } else {
        QMessageBox::critical(this, "错误", "文件不存在");
        result = false;
        break;
      }
    }
  }
  return file_names;
}

void MainWindow::InitialWD() {
  auto path = QDir::currentPath();
  QDir path_dir = QDir(path);
  if (path_dir.isEmpty()) {
    qDebug() << "当前目录不是游戏根目录, 请手动选择";
    auto message_box = QMessageBox::warning(this, "警告", "当前目录不是游戏根目录, 请手动选择");
	return;
  }
  auto lists = path_dir.entryInfoList(QDir::Filters {QDir::Filter::Files | QDir::Filter::Executable});
  if (lists.isEmpty()) {
	qDebug() << "当前目录不是游戏根目录, 请手动选择";
	auto message_box = QMessageBox::warning(this, "警告", "当前目录不是游戏根目录, 请手动选择");
	return;
  }
  executable_path_ = lists[0].absoluteFilePath();
  ui->directoryRoot->setText(executable_path_);
  qDebug() << "检测到游戏运行文件： " << executable_path_;
}

QString MainWindow::GetModRelativePath(const QString& mod_name, const QString& path) {
    QString current_mod_path(this->mod_dir_->filePath(mod_name));
    return QDir(current_mod_path).relativeFilePath(path);
}

bool MainWindow::CheckConflict(const QString& mod_name, const QStringList &list) {
  for (const auto& file : list) {
	// 获得相对路径
    auto relative_path = this->GetModRelativePath(mod_name, file);
	qDebug() << relative_path;
	if (recorder_->ExistModFile(relative_path.toStdString()))
	  return true;
  }
  return false;
}

void MainWindow::Play() {
  if (!executable_path_.isEmpty()) {
    qDebug() << executable_path_;
    auto code = QProcess::execute(executable_path_);
    qDebug() << code;
  }
}




