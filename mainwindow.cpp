#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ReLed)
{
    ui->setupUi(this);

    //Deberiamos tener una funcion que se llame incializarReloj para hacer todas la configuracion basicas
    //Recibe el tiempo actual
    connect(ui->timeEdit, &QTimeEdit::timeChanged, this, &MainWindow::onTimeChanged);
    //Pone de default el La franja horaria automatica
    //ui->CheckBox_TimeZoneEnable->setCheckState(Qt::Checked);


}


void MainWindow::onTimeChanged(const QTime &time)
{
    if (!ui->timeEdit->isEnabled()) {
        return; // No actualizamos si el QTimeEdit está deshabilitado
    }

    int timeAsInt = time.hour() * 100 + time.minute(); // Convertir a int
    ui->lcdNumber->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Btn_Home_clicked()
{
    ui->Windows_Widget->setCurrentIndex(HOME);
}


void MainWindow::on_Btn_Menu_1_clicked()
{
    ui->Windows_Widget->setCurrentIndex(CLOCK);

    // Obtener el estado actual del checkbox
    Qt::CheckState currentState = ui->CheckBox_TimeZoneEnable->checkState();

    // Llamar a la función con el estado actual
    on_CheckBox_TimeZoneEnable_checkStateChanged(currentState);
}


void MainWindow::on_Btn_Menu_2_clicked()
{
    ui->Windows_Widget->setCurrentIndex(ALARM);
}


void MainWindow::on_Btn_Menu_3_clicked()
{
    ui->Windows_Widget->setCurrentIndex(TIMER);
}


void MainWindow::on_Btn_Menu_4_clicked()
{
    ui->Windows_Widget->setCurrentIndex(CRONO);
}


void MainWindow::on_Btn_Menu_5_clicked()
{
    ui->Windows_Widget->setCurrentIndex(CLIMA);

    MainWindow::fetchWeatherData();

}


void MainWindow::on_Btn_Menu_6_clicked()
{
    ui->Windows_Widget->setCurrentIndex(DRAW);
}


void MainWindow::on_Btn_Menu_7_clicked()
{
    ui->Windows_Widget->setCurrentIndex(SPOTIFY);
}

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



void MainWindow::fetchWeatherData() {
    QNetworkAccessManager Manager;
    //En el URL de abajo la latitud y longitud son los numero que aparecen. habria que convertirlos automaticamente al lugar de donde este la
    //computadora. Ademas hacer la Key como variable global.
    QUrl APIUrl("https://api.openweathermap.org/data/2.5/weather?lat=-34.6037&lon=-58.3816&appid=87492aebac99f1806c24cf14c9274227&units=metric");
    QNetworkRequest request(APIUrl);

    QNetworkReply *reply = Manager.get(request);

    // Esperar la respuesta
    QEventLoop loop;
    MainWindow::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        // Obtener datos de la respuesta
        QByteArray responseData = reply->readAll();
        //qDebug() << "API Response :" << responseData;

        // Parsear el JSON
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();

        // Extraer variables
        QString cityName = jsonObj["name"].toString();
        double temperature = jsonObj["main"].toObject()["temp"].toDouble();
        double feelsLike = jsonObj["main"].toObject()["feels_like"].toDouble();
        int humidity = jsonObj["main"].toObject()["humidity"].toInt();
        double windSpeed = jsonObj["wind"].toObject()["speed"].toDouble();
        QString weatherDescription = jsonObj["weather"].toArray()[0].toObject()["description"].toString();

        // Mostrar variables
        ui->lineEdit_Ciudad->setText(cityName);
        qDebug() << "City Name:" << cityName;
        qDebug() << "Temperature:" << temperature;
        qDebug() << "Feels Like:" << feelsLike;
        qDebug() << "Humidity:" << humidity;
        qDebug() << "Wind Speed:" << windSpeed;
        qDebug() << "Weather Description:" << weatherDescription;
    } else {
        qDebug() << "Error :" << reply->errorString();
    }

    reply->deleteLater(); // Liberar memoria
}
