#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#define HASH_TABLE_SIZE  10

namespace Ui {class MainWindow;}//命名空间

class MainWindow : public QMainWindow//父类
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);//构造函数

    ~MainWindow();//析构函数

private slots://槽函数
    void on_Search_clicked();
    void on_Delete_clicked();
    void on_Insert_clicked();
    void on_Clear_clicked();
    void on_Apply_clicked();

private:
    Ui::MainWindow *ui;
    int tableSize;
    int mod;
};
#endif // MAINWINDOW_H

