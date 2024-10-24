#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QTime>
#include <QtNetwork/QtNetwork>
#include <QtGui>
#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui {
class ReLed;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum Page_Name{HOME,CLOCK,ALARM,TIMER,CRONO,CLIMA,DRAW,SPOTIFY };

private slots:


    void on_Btn_Home_clicked();

    void on_Btn_Menu_1_clicked();

    void on_Btn_Menu_2_clicked();

    void on_Btn_Menu_3_clicked();

    void on_Btn_Menu_4_clicked();

    void on_Btn_Menu_5_clicked();

    void on_Btn_Menu_6_clicked();

    void on_Btn_Menu_7_clicked();

    void on_CheckBox_TimeZoneEnable_checkStateChanged(const Qt::CheckState &arg1);

    void onTimeChanged(const QTime &time);

    void updateLCDWithTime(const QTime &time);

private:
    Ui::ReLed *ui;

private:
    QTime storedTime;
    void fetchWeatherData();


};

#endif // MAINWINDOW_H
