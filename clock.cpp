#include "clock.h"
#include "mainwindow.h"

Clock::Clock() {}

void MainWindow::updateLCDWithTime(const QTime &time)
{
    int timeAsInt = time.hour() * 100 + time.minute(); // Cambiar a int
    ui->lcdNumber->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
}

void MainWindow::on_CheckBox_TimeZoneEnable_checkStateChanged(const Qt::CheckState &arg1)
{
    if (arg1 == Qt::Checked) {
        // El checkbox está marcado
        storedTime = QTime::currentTime();
        MainWindow::updateLCDWithTime(storedTime);

        ui->timeEdit->setDisabled(true);



        qDebug() << "Timezone enabled.";
    } else if (arg1 == Qt::Unchecked) {


        // El checkbox no está marcado
        ui->timeEdit->setDisabled(false);
        storedTime = ui->timeEdit->time();
        int timeAsInt = storedTime.hour() * 100 + storedTime.minute(); // Cambiar a int
        ui->lcdNumber->display(static_cast<double>(timeAsInt));

        qDebug() << "Timezone disabled.";
    }

}
