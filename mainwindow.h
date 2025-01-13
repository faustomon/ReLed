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
#include <qsettings.h>
#include <QTcpSocket>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui {
class ReLed;
}
QT_END_NAMESPACE

extern QSerialPort serialPort;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum Page_Name{HOME,CLOCK,ALARM,TIMER,CRONO,CLIMA };

private slots:


    void on_Btn_Home_clicked();

    void on_Btn_Menu_1_clicked();

    void on_Btn_Menu_2_clicked();

    void on_Btn_Menu_3_clicked();

    void on_Btn_Menu_5_clicked();

    void on_CheckBox_TimeZoneEnable_checkStateChanged(const Qt::CheckState &arg1);

    void onTimeChanged(const QTime &time);

    void updateLCDWithTime(const QTime &time);



    void on_checkBox_amanecer_checkStateChanged(const Qt::CheckState &arg1);

    void on_checkBox_buzzer_checkStateChanged(const Qt::CheckState &arg1);

    void on_timeEdit_configAlarma_userTimeChanged(const QTime &time);

    void on_pushButton_confirmarDatos_clicked();

    void on_checkBox_alarma_checkStateChanged(const Qt::CheckState &arg1);

    void on_timeEdit_Temporizador_userTimeChanged(const QTime &time);

    void on_timeEdit_PomodoroWork_userTimeChanged(const QTime &time);

    void on_timeEdit_PomodoroBreak_userTimeChanged(const QTime &time);



    void on_pushButton_datosTimer_clicked();

    void on_pushButton_confirmarDatos_2_clicked();

private:
    Ui::ReLed *ui;
    QSettings settings;
    QTimer m_timer_wifi;
    QTcpSocket *m_socket;
    char send[40]; // Buffer para enviar datos

private:
    QTime storedTime;
    void fetchWeatherData();
    void fetchDailyData();
    void loadSettings( void );
    void loadSettingsTimer( void );
    void enviarTrama();
private:
    void searchSong(const QString &songQuery);
    void parseSearchResponse(const QByteArray &response);


};

#endif // MAINWINDOW_H
