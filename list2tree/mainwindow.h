#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileDevice>
#include <QTreeWidget>
//#include "list2tree.cpp"
#include <QStack>

typedef int ElemType;

typedef struct
{
    int id;			// 该线性表中所有节点的ID唯一
    ElemType data;	// 节点的值，自定义数据类型
    int pid;		// 表示该节点的父节点，值为0表示根，对应线性表中其他节点的id值
}ListNode; //线性表

Q_DECLARE_METATYPE(ListNode)

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);//显式构造，避免隐式转换
    ~MainWindow();

private slots:
    void on_actionImportTxt_triggered();
    void on_actionClear_triggered();
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    bool addChildNode();
    bool deleteNode();

private:
    Ui::MainWindow *ui;
    ListNode node;
    QTreeWidgetItem *curItem;
    QAction *addChildNodeAction;
    QAction *deleteNodeAction;
};
#endif // MAINWINDOW_H
