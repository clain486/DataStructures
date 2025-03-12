#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化树
    ui->treeWidget->setColumnCount(2);//创建初始两列 id 和 data
    ui->treeWidget->setHeaderLabels(QStringList()<<QStringLiteral("id")<<QStringLiteral("data"));

    //被创建为用于添加子节点和删除节点的QAction对象。
    addChildNodeAction = new QAction("&添加子节点", this);
    deleteNodeAction = new QAction("&删除节点", this);

    //将信号在顶层MainWindow中连接
    //将 addChildNodeAction 的 triggered 信号连接到当前 MainWindow 实例的 addChildNode() 槽
    connect(addChildNodeAction, SIGNAL(triggered()), this, SLOT(addChildNode()));
    //将 deleteNodeAction 的 triggered 信号连接到当前 MainWindow 实例的 deleteNode() 槽
    connect(deleteNodeAction, SIGNAL(triggered()), this,SLOT(deleteNode()));
    //将主窗口的 customContextMenuRequested 信号连接到 on_treeWidget_customContextMenuRequested(QPoint) 槽
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_treeWidget_customContextMenuRequested(QPoint)));

    // 初始化线性表
    // 设置单元格的边框样式和宽度
    ui->tableWidget->setStyleSheet("QTableWidget::item { border: 1px solid black; }");
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(75);

    ui->tableWidget->setColumnCount(3); // 创建初始三列,分别为id, data 和 pid
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << QStringLiteral("id") << QStringLiteral("data") << QStringLiteral("pid"));
    ui->tableWidget->setRowCount(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 导入文件生成
void MainWindow::on_actionImportTxt_triggered()
{
    QString curPath = QDir::currentPath();
    QString dlgTitle = "选择打开txt文件";
    QString filter = "(*.txt)";
    QString aFileName = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
    if (aFileName.isEmpty()){
        return;
    }
    QFile aFile(aFileName);
    if (!aFile.exists()){ //文件不存在
        return;
    }
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text)){ //打开失败
        return;
    }

    //QByteArray line = aFile.readLine();
    //QString str(line);

    int flag_root = 0; //用于标记是否遇到根节点
    int rowCnt = 0; //用于跟踪表格行数

    // 设置单元格的边框样式和宽度
    ui->tableWidget->setStyleSheet("QTableWidget::item { border: 1px solid black; }");
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(75);

    while (!aFile.atEnd()){ //文件没有读完
        QByteArray line = aFile.readLine(); //接着读下一行
        QString str(line);
        str = str.trimmed(); //移除字符串两端的空格
        QStringList sections = str.split(" "); //使用空格分割字符串，得到包含三个部分的 sections 列表

        // 找有没有id重复
        if(ui->treeWidget->findItems(sections[0], Qt::MatchRecursive,0).size()!=0){ //如果重复
            //错误提示
            QMessageBox msgBox(QMessageBox::Warning, "错误提示", "id出现重复，将清空所有内容，请在确认源文件后重新导入！", 0, this);
            msgBox.addButton(tr("确认"), QMessageBox::AcceptRole);

            if(msgBox.exec() == QMessageBox::AcceptRole){
                qDebug() << "init failed";
                ui->treeWidget->clear();
                ui->tableWidget->clear();
                ui->tableWidget->setRowCount(0); //否则之后再导入数据会有问题
                return;
            }
        }

        // 要求第一个是根节点
        if(flag_root == 0){ //表示这是第一个节点（根节点）
            rowCnt = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCnt);
            ui->tableWidget->setItem(rowCnt, 0, new QTableWidgetItem(sections[0]));
            ui->tableWidget->setItem(rowCnt, 1, new QTableWidgetItem(sections[1]));
            ui->tableWidget->setItem(rowCnt, 2, new QTableWidgetItem(sections[2]));

            //创建一个新的树节点（QTreeWidgetItem）和一行数据到表格部件，并设置 flag_root 为1
            QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget, QStringList() << sections[0] << sections[1]);
            ui->treeWidget->insertTopLevelItem(0, root);
            flag_root = 1;
        }
        else {
            // 非根节点需要找到父节点
            rowCnt = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(rowCnt);
            ui->tableWidget->setItem(rowCnt, 0, new QTableWidgetItem(sections[0]));
            ui->tableWidget->setItem(rowCnt, 1, new QTableWidgetItem(sections[1]));
            ui->tableWidget->setItem(rowCnt, 2, new QTableWidgetItem(sections[2]));

            //使用 ui->treeWidget->findItems 方法找到具有匹配pid的父节点
            QList<QTreeWidgetItem *> QList = ui->treeWidget->findItems(sections[2], Qt::MatchRecursive, 0);
            if(QList.length() == 1){ //父节点存在且唯一
                //将当前节点添加为其子节点
                new QTreeWidgetItem(QList[0], QStringList() << sections[0] << sections[1]);
            }
            else {
                qDebug() << "Err";
            }
        }
        qDebug() << str;
    }
    aFile.close();
    ui->treeWidget->expandAll(); //展开树部件的所有节点
}

// 清空
void MainWindow::on_actionClear_triggered(){
    ui->treeWidget->clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    return;
}

// 添加节点
bool MainWindow::addChildNode()
{
    qDebug() << "addChildNode";
    QInputDialog dia(this);
    dia.setFixedHeight(107);
    dia.setFixedWidth(800);
    dia.setWindowTitle("请输入节点信息");
    dia.setLabelText("id和data中间用空格分割开");
    dia.setInputMode(QInputDialog::TextInput);//可选参数：DoubleInput  TextInput
    if(dia.exec() == QInputDialog::Accepted){
       qDebug() << dia.textValue();
       QStringList sections = dia.textValue().trimmed().split(" ");
       if(sections.size()!=2){
           QMessageBox msgBox(QMessageBox::Warning, "错误提示", "输入数据有误！", 0, this);
           msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);
           if (msgBox.exec() == QMessageBox::AcceptRole){
               qDebug() <<"addNode failed";
               return false;
           }
       }
       // 找有没有id重复
       if(ui->treeWidget->findItems(sections[0], Qt::MatchRecursive,0).size()!=0){
           // 错误提示
           QMessageBox msgBox(QMessageBox::Warning, "错误提示", "id出现重复，请确认id！", 0, this);
           msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);
           if (msgBox.exec() == QMessageBox::AcceptRole){
               qDebug() << "addNode failed";
               return false;
           }
       }

       // 输入数据有效，创建一个新的 QTreeWidgetItem，并将它作为当前项 curItem 的子项，其中包含两列数据，分别是 sections[0] 和 sections[1]
       new QTreeWidgetItem(curItem, QStringList() << sections[0] << sections[1]);
       int rowCnt = ui->tableWidget->rowCount();
       qDebug() << "个数是" << rowCnt;

       // 然后插入一行新的数据，并将 sections[0] 和 sections[1] 分别放置在表格的第一列和第二列，将 curItem 的文本值放置在表格的第三列
       ui->tableWidget->insertRow(rowCnt);
       ui->tableWidget->setItem(rowCnt, 0, new QTableWidgetItem(sections[0]));
       ui->tableWidget->setItem(rowCnt, 1, new QTableWidgetItem(sections[1]));
       ui->tableWidget->setItem(rowCnt, 2, new QTableWidgetItem(curItem->text(0)));
       return true;
    }
    else {
        return false;
    }
}

// 删除节点
bool MainWindow::deleteNode()
{
   qDebug() << "deleteNode";
   QTreeWidgetItem* parent = curItem->parent();
   if(parent == nullptr){
        qDebug() << "rootNode";
       ui->tableWidget->clear();
       ui->tableWidget->setRowCount(0);
       // 得到索引
       ui->treeWidget->clear();
   }
   else{ // parent 不为 nullptr，表示当前项不是根节点
         // 创建一个堆栈 childStack，并将当前项 curItem 推入堆栈中
       QStack <QTreeWidgetItem *> childStack;
       int childNum;
       childStack.push(curItem);

       QTreeWidgetItem * id;
       QString index;
       // 获取所有将要删除的table控件的id
       while(childStack.isEmpty() == false){
           id = childStack.pop();
           index = id->text(0);
           // 在tableWidget对应删除
           int i = 0;
           while(i < ui->tableWidget->rowCount()){
               if(ui->tableWidget->item(i, 0)->text() == index){ //在表格控件 ui->tableWidget 中查找具有相同 index 的行，并将这些行从表格中移除
                   ui->tableWidget->removeRow(i);
               }
               else{
                   i++;
               }
           }
           childNum = id->childCount();
           for (int j = 0; j < childNum; j++) { //将子项的所有子项推入堆栈中，以便继续处理它们
               childStack.push(id->child(j));
           }
       }
       parent->removeChild(curItem); //从父项 parent 中移除当前项 curItem，从树形控件中删除
       // list的都要删掉
   }
   return true;
   //删除数据库一行
}

// 右键弹出菜单
void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    curItem = ui->treeWidget->itemAt(pos);  //获取当前被点击的节点

    //在空白位置点击，弹出菜单：添加根节点\删除所有模板
    if(curItem == nullptr){
        QMenu *popMenu = new QMenu(this);//定义一个右键弹出菜单
        popMenu->exec(QCursor::pos());
    }
    else{
        QVariant var = curItem->data(0, Qt::UserRole);
        qDebug() << var.toString() << endl;
        qDebug() << "category";
        QMenu *popMenu = new QMenu(this);
        popMenu->addAction(addChildNodeAction);
        popMenu->addAction(deleteNodeAction);
        popMenu->exec(QCursor::pos());
    }
}


