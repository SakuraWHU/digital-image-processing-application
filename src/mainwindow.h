#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<opencv2/opencv.hpp>

#include"DIPfunctions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_6_triggered();

    void on_action_7_triggered();

    void on_action_10_triggered();

    void on_action_5_triggered();

    void on_action_11_triggered();

    void on_actionsobel_triggered();

    void on_action_14_triggered();

    void on_action_15_triggered();

    void on_action_9_triggered();

    void on_action_3_triggered();

    void on_action_4_triggered();

    void on_action_12_triggered();

    void on_action_13_triggered();

    void on_action_8_triggered();

    void on_action_16_triggered();

private:
    Ui::MainWindow *ui;

    //下面是自己添加的私有成员变量
    cv::Mat img;
    cv::Mat orimg;
    int width;
    int height;
    //显示函数
    void SHOW(cv::Mat img);
};
#endif // MAINWINDOW_H
