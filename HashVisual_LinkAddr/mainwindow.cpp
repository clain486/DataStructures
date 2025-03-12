#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidgetOpen->setRowCount(1); //创建初始行
    ui->tableWidgetOpen->setVerticalHeaderLabels(QStringList()<<QStringLiteral("初始行"));
    ui->tableWidgetOpen->setColumnCount(0);

    ui->lEdtMod->setValidator(new QIntValidator(1,99,this));//设置模数1-99的整数验证器
    ui->lEdtTableSize->setValidator(new QIntValidator(1,99,this));//设置长度1-99的整数验证器

    ui->lEdtMod->clear();//清空模数
    ui->lEdtTableSize->clear();//清空长度

    ui->lEdtMod->setEnabled(true);
    ui->lEdtTableSize->setEnabled(true);

    ui->Apply->setEnabled(true);

    ui->Clear->setEnabled(false);
    ui->Delete->setEnabled(false);
    ui->Insert->setEnabled(false);
    ui->Search->setEnabled(false);
}

MainWindow::~MainWindow()//析构函数
{
    delete ui;
}

void MainWindow::on_Apply_clicked()
{
    qDebug() << "创建";

    QMessageBox msgBox(QMessageBox::Warning,"确认创建哈希表吗？","如需再次更改，需要先清空哈希表",nullptr,this);
    msgBox.addButton(tr("确认"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
    if (msgBox.exec() != QMessageBox::AcceptRole){
        return;
    }

    qDebug() <<"创建中";

    this->mod = ui->lEdtMod->text().toInt();
    this->tableSize= ui->lEdtTableSize->text().toInt();


    qDebug() << "模数 " << this->mod;
    qDebug() << "哈希表长度 " << this->tableSize;

    if(this->tableSize <= 0 || this->mod <= 0){
        QMessageBox msgBox(QMessageBox::Warning,"错误提示","设置值不合法",nullptr,this);
        msgBox.addButton(tr("确认"), QMessageBox::AcceptRole);
        msgBox.exec();

        ui->lEdtMod->clear();
        ui->lEdtTableSize->clear();

        ui->lEdtMod->setEnabled(true);
        ui->lEdtTableSize->setEnabled(true);

        ui->Apply->setEnabled(true);

        return;
    }

    if(this->tableSize < this->mod){
        QMessageBox msgBox(QMessageBox::Warning,"错误提示","哈希表长度必须大于等于模数",nullptr,this);
        msgBox.addButton(tr("确认"), QMessageBox::AcceptRole);
        msgBox.exec();

        ui->lEdtMod->clear();
        ui->lEdtTableSize->clear();

        ui->lEdtMod->setEnabled(true);
        ui->lEdtTableSize->setEnabled(true);

        ui->Apply->setEnabled(true);
        return;
    }

    ui->tableWidgetOpen->clear();

    // 设置单元格的边框样式和宽度
    ui->tableWidgetOpen->setStyleSheet("QTableWidget::item { border: 1px solid black; }");
    ui->tableWidgetOpen->horizontalHeader()->setDefaultSectionSize(75);


    ui->tableWidgetOpen->setRowCount(1); //创建初始行
    ui->tableWidgetOpen->setVerticalHeaderLabels(QStringList()<<QStringLiteral("1"));
    ui->tableWidgetOpen->setColumnCount(this->tableSize);
    ui->tableWidgetOpen->setColumnWidth(0, 75); // 设置宽度

    ui->lEdtMod->setEnabled(false);
    ui->lEdtTableSize->setEnabled(false);

    ui->Apply->setEnabled(true);

    ui->Clear->setEnabled(true);
    ui->Delete->setEnabled(true);
    ui->Insert->setEnabled(true);
    ui->Search->setEnabled(true);

    return;
}

void MainWindow::on_Insert_clicked()
{
    qDebug() << "Search";
    QInputDialog dia(this);
    dia.setFixedHeight(107);
    dia.setFixedWidth(800);
    dia.setWindowTitle("插入");
    dia.setLabelText("请输入关键字：");
    dia.setInputMode(QInputDialog::TextInput);//可选参数：DoubleInput  TextInput
    if(dia.exec() != QInputDialog::Accepted){
       return;
    }
    qDebug() << dia.textValue();
    QString key = dia.textValue().trimmed();
    int addr = key.toInt() % (this->mod);
    QTableWidgetItem *curItem;
    int cnt = 0;
    bool isFailed = 1;
    while(1) {
        curItem = ui->tableWidgetOpen->item(cnt, addr);
        if(curItem == nullptr || (curItem && curItem->text()==tr(""))){//如果目标位置为空
            if(cnt + 1 > ui->tableWidgetOpen->rowCount()){
                ui->tableWidgetOpen->setRowCount(cnt + 1);
            }
            ui->tableWidgetOpen->setItem(cnt, addr, new QTableWidgetItem(key));
            isFailed = 0;
            break;
        }
        else {
            if(curItem->text()==key){
                qDebug() << "insert failed(exist)";
                isFailed = 1;
                break;
            }
            else {
                //addr = (addr + 1) % mod;
                cnt++;
                qDebug() << "Link addressing";
            }
        }
    }
    if(isFailed){
        // 错误提示
        QMessageBox msgBox(QMessageBox::Warning,"错误提示","关键字已存在",nullptr,this);
        msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);
        if (msgBox.exec() == QMessageBox::AcceptRole){
            qDebug() <<"insert failed";
        }
    }
    else {
        qDebug() <<"insert success";
    }
    return;
}

void MainWindow::on_Search_clicked()
{
    qDebug() << "Search";
    QInputDialog dia(this);
    dia.setFixedHeight(107);
    dia.setFixedWidth(800);
    dia.setWindowTitle("搜索");
    dia.setLabelText("请输入关键字：");
    dia.setInputMode(QInputDialog::TextInput);//可选参数：DoubleInput  TextInput
    if(dia.exec() != QInputDialog::Accepted){
       return;
    }
    qDebug() << dia.textValue();

    QString key = dia.textValue().trimmed();//去除字符串两侧的空格
    int addr = key.toInt() % this->mod;
    int cnt = 0;
    bool isFailed = 0;

    QTableWidgetItem *curItem;

    while(1) {
        curItem = ui->tableWidgetOpen->item(cnt, addr);
        if(curItem == nullptr || (curItem && ((curItem->text()) == tr("")))){
            isFailed = 1;
            break;
        }
        else {
            if(curItem->text() == key){
                qDebug() << "search success";
                isFailed = 0;
                break;
            }
            else {
                cnt++;
                qDebug() << "Link addressing";
                if(cnt >= this->tableSize){
                    break;
                }
            }
        }
    }// end of while

    // 满了
    if(isFailed){
        // 错误提示
        QMessageBox msgBox(QMessageBox::Warning,"错误提示","查找失败",nullptr,this);
        msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);
        //msgBox.addButton(tr("&Continue"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole){
            qDebug() <<"search failed";
        }
        return;
    }

    QString resultText = "查找成功,目标位于第" + QString::number(addr + 1) + "列，第"+QString::number(cnt + 1) + "行"; // 使用 QString::number() 将变量值转换为字符串
    QMessageBox msgBox(QMessageBox::Information, "查找结果提示", resultText, nullptr, this);
    msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);

    if (msgBox.exec() == QMessageBox::AcceptRole){
        qDebug() <<"search success";
    }
    return;
}

void MainWindow::on_Delete_clicked()
{
    qDebug() << "Delete";
    QInputDialog dia(this);
    dia.setFixedHeight(107);
    dia.setFixedWidth(800);
    dia.setWindowTitle("删除");
    dia.setLabelText("请输入关键字：");
    dia.setInputMode(QInputDialog::TextInput);//可选参数：DoubleInput  TextInput
    int cnt = 0;
    bool isFailed = 0;
    if(dia.exec() != QInputDialog::Accepted){
       return;
    }
    qDebug() << dia.textValue();
    QString key = dia.textValue().trimmed();
    int addr = key.toInt() % this->mod;
    QTableWidgetItem *curItem;

    while(1) {
        curItem = ui->tableWidgetOpen->item(cnt, addr);
        if(curItem == nullptr || (curItem && ((curItem->text())==tr("")))){
            isFailed = 1;
            break;
        }
        else {
            if(curItem->text()==key){
                curItem->setText("");
                qDebug() << "delete success";
                int numRow = ui->tableWidgetOpen->rowCount();
                // 链表后面的往前移动
                int i;
                for(i = cnt; i < numRow - 1; i++){
                    if(ui->tableWidgetOpen->item(i + 1, addr)){
                        ui->tableWidgetOpen->setItem(i, addr, ui->tableWidgetOpen->item(i + 1, addr)->clone());
                    }
                    else {
                       break;
                    }
                }
                ui->tableWidgetOpen->takeItem(i, addr);
                isFailed = 0;
                break;
            }
            else {
                cnt++;
                qDebug() << "Link addressing";
            }
        }
    }

    // 满了
    if(cnt >= this->tableSize || isFailed){
        // 错误提示
        QMessageBox msgBox(QMessageBox::Warning,"错误提示","删除失败",nullptr,this);
        msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);
        if (msgBox.exec() == QMessageBox::AcceptRole){
            qDebug() <<"delete failed";
        }
        return;
    }
    QString resultText = "目标位于第" + QString::number(addr + 1) + "列，第"+QString::number(cnt + 1) + "行，删除成功"; // 使用 QString::number() 将变量值转换为字符串
    QMessageBox msgBox(QMessageBox::Information, "查找结果提示", resultText, nullptr, this);
    msgBox.addButton(tr("我知道了"), QMessageBox::AcceptRole);
    if (msgBox.exec() == QMessageBox::AcceptRole){
        qDebug() <<"delete success";
    }
    return;
}

void MainWindow::on_Clear_clicked()
{
    qDebug() << "Clear";
    QMessageBox msgBox(QMessageBox::Warning,"提示","请确认是否要清除哈希表？",nullptr,this);
    msgBox.addButton(tr("是"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("否"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole){
        qDebug() <<"clear";
        ui->tableWidgetOpen->clear();
        this->mod = -1;
        this->tableSize = -1;

        ui->tableWidgetOpen->setRowCount(1); //创建初始行
        ui->tableWidgetOpen->setVerticalHeaderLabels(QStringList()<<QStringLiteral("初始行"));
        ui->tableWidgetOpen->setColumnCount(0);
        ui->lEdtMod->clear();
        ui->lEdtTableSize->clear();
        ui->lEdtMod->setEnabled(true);
        ui->lEdtTableSize->setEnabled(true);

        ui->Clear->setEnabled(false);
        ui->Delete->setEnabled(false);
        ui->Insert->setEnabled(false);
        ui->Search->setEnabled(false);
    }
    return;
}
