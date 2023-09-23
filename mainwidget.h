#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include <QtSerialPort>
#include <QSerialPortInfo>

#include <QtDebug>

#include <QString>

#include <QScrollBar>

#include <QFileDialog>
#include <QDir>
//#include <QDirIterator>

#include "camera_preview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    void log(QString text);
    void logln(QString text);
    //void logln();
    ~MainWidget();

private slots:
    void on_refresh_buttons_clicked();

    void on_connect_button_clicked();

    void serialEvent();

    void on_poudre_up_clicked();

    void on_poudre_down_clicked();

    void on_ping_button_clicked();

    void on_racleur_up_clicked();

    void on_racleur_down_clicked();

    void on_racleur_back_clicked();

    void on_racleur_forward_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_3_clicked();

    void on_unlock_button_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_14_clicked();

    void on_deux_clicked();

    void on_zero_clicked();

    void on_un_clicked();

    void on_trois_clicked();

    void on_quatre_clicked();

    void on_cinq_clicked();

    void on_six_clicked();

    void on_sept_clicked();

    void on_huit_clicked();

    void on_neuf_clicked();

    void on_back_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_tabWidget_3_tabBarClicked(int index);

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

private:
    Ui::MainWidget *ui;
    QSerialPort serialport;
    bool unlocked;
    void appendline(QString);
    void deleteline();
    Camera_preview* mm_cam;
};
#endif // MAINWIDGET_H
