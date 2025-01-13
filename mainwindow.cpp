#include "mainwindow.h"
#include "ui_mainwindow.h"
QSerialPort serialPort;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ReLed)
    ,settings("MyApp", "MyAppSettings") // Aquí se inicializa QSettings
{
    ui->setupUi(this);



    connect(ui->timeEdit, &QTimeEdit::timeChanged, this, &MainWindow::onTimeChanged);

    serialPort.setPortName("COM7");  // Reemplazar con el nombre del puerto correcto, por ejemplo "COM1" en Windows o "/dev/ttyS0" en Linux
    serialPort.setBaudRate(QSerialPort::Baud9600);  // Establecer la velocidad (Baud Rate)
    serialPort.setDataBits(QSerialPort::Data8);     // Establecer los bits de datos
    serialPort.setParity(QSerialPort::NoParity);   // Establecer paridad
    serialPort.setStopBits(QSerialPort::OneStop);  // Establecer los bits de parada
    serialPort.setFlowControl(QSerialPort::NoFlowControl);  // Establecer control de flujo
    if (!serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "Error al abrir el puerto serial.";
    }
}

void MainWindow::enviarTrama()
{
    // Convertir 'send' (suponiendo que es un char[]) a QByteArray
    QByteArray data = QByteArray::fromRawData(send, strlen(send)); // Convierte send en QByteArray

    // Verificar si hay algo para enviar
    if (!data.isEmpty()) {
        // Enviar los datos
        qint64 bytesWritten = serialPort.write(data);

        // Verificar si la transmisión fue exitosa
        if (bytesWritten == -1) {
            qDebug() << "Error al escribir en el puerto serial.";
        } else {
            qDebug() << "Datos enviados exitosamente.";
        }
    } else {
        qDebug() << "No hay datos para enviar.";
    }
}

void MainWindow::onTimeChanged(const QTime &time)
{
    if (!ui->timeEdit->isEnabled()) {
        return; // No actualizamos si el QTimeEdit está deshabilitado
    }

    int timeAsInt = time.hour() * 100 + time.minute(); // Convertir a int
    ui->lcdNumber->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
}

MainWindow::~MainWindow() // Destructor de mainWindow
{
    delete ui;
}

void MainWindow::on_Btn_Home_clicked()
{
    ui->Windows_Widget->setCurrentIndex(HOME); // Boton para volver a Home

}

void MainWindow::on_Btn_Menu_1_clicked() // Boton para volver a Clock
{


    ui->Windows_Widget->setCurrentIndex(CLOCK);

    // Obtener el estado actual del checkbox
    Qt::CheckState currentState = ui->CheckBox_TimeZoneEnable->checkState();

    // Llamar a la función con el estado actual
    on_CheckBox_TimeZoneEnable_checkStateChanged(currentState);



}

void MainWindow::on_Btn_Menu_2_clicked() // Boton para volver a Alarm
{
    ui->Windows_Widget->setCurrentIndex(ALARM);


    loadSettings();
}

void MainWindow::on_Btn_Menu_3_clicked() // Boton para volver a Timer
{
    ui->Windows_Widget->setCurrentIndex(TIMER);
    loadSettingsTimer();
}

void MainWindow::on_Btn_Menu_5_clicked() // Bootn para volver a Clima
{
    ui->Windows_Widget->setCurrentIndex(CLIMA);

    MainWindow::fetchWeatherData();

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
        ui->label_DatosGuardados_2->setText("Cambios no guardados");



        qDebug() << "Timezone enabled.";
    } else if (arg1 == Qt::Unchecked) {


        // El checkbox no está marcado
        ui->timeEdit->setDisabled(false);
        storedTime = ui->timeEdit->time();
        int timeAsInt = storedTime.hour() * 100 + storedTime.minute(); // Cambiar a int
        ui->lcdNumber->display(static_cast<double>(timeAsInt));
        ui->label_DatosGuardados_2->setText("Cambios no guardados");

        qDebug() << "Timezone disabled.";
    }

}

void MainWindow::fetchWeatherData() { // Cargo datos de la API para clima
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

        ui->lineEdit_Tempe->setText(QString::number(temperature));

        ui->lineEdit_Sensacion->setText(QString::number(feelsLike));

        ui->lineEdit_Humedad->setText(QString::number(humidity));

        ui->lineEdit_Viento->setText(QString::number(windSpeed));

        ui->lineEdit_Descripcion->setText(weatherDescription);

        /*
        Opcional si queres le podemos agregar el resto de datos del Clima para transmitir, quedara una trama mas larga
        pero si son valores de tamaño fijo podemos ver, por ahora si queres dejalo comentado y vemos mañana
        // Saco el valor de Temperatura
        QString valorTemperaturaStr = QString::number(temperature);

        // Saco la Ciudad
        QString valorCiudadStr = cityName;

        // Saco el valor de Sensacion Termica
        QString valorSensacionStr = QString::number(feelsLike);

        // Saco el valor de Humedad
        QString valorHumedadStr = QString::number(humidity);

        // Saco el valor de Viento
        QString valorVientoStr = QString::number(windSpeed);

        // Saco el Descripcion de Clima
        QString valorDescripcionStr = weatherDescription;

        // Creo un nuevo QString para agregar la 'C' al principio y armo la trama con los strings correspondientes
        QString tramaClima = "T" + valorTemperaturaStr + QString valorCiudadStr + QString valorHumedadStr + valorSensacionStr + valorVientoStr + valorDescripcionStr;
         */

        // Crear un nuevo String para armar la trama de Hora
        char tramaClima[40] = {0};

        if(temperature >= 10.0)
        {
            // Construir la trama con 'T' al principio
            sprintf(tramaClima, "T%d00000000", (int)(temperature * 100));
        }
        else
        {
            // Construir la trama con 'C' al principio
            sprintf(tramaClima, "T0%d00000000", (int)(temperature * 100));
        }

        // Imprimir el resultado
        qDebug() << "LCD Clima:" << tramaClima;

        // Copiar la trama a la variable global 'send'
        strcpy(send, tramaClima);

        enviarTrama();  // Llamar a la función para enviar la trama

    } else {
        qDebug() << "Error :" << reply->errorString();
    }

    reply->deleteLater(); // Liberar memoria
}

void MainWindow::on_checkBox_amanecer_checkStateChanged(const Qt::CheckState &arg1) // CheckBox para LED AMANECER
{
    settings.setValue("checkBox_amanecer", arg1); // Guardar el estado
    qDebug() << "Guardado checkBox_amanecer:" << arg1;

    ui->label_DatosGuardados->setText("Cambios no guardados");

}

void MainWindow::on_checkBox_buzzer_checkStateChanged(const Qt::CheckState &arg1) // CheckBox para BUZZER
{
    settings.setValue("checkBox_buzzer", arg1); // Guardar el estado
    qDebug() << "Guardado checkBox_amanecer:" << arg1;

    ui->label_DatosGuardados->setText("Cambios no guardados");
}

void MainWindow::on_timeEdit_configAlarma_userTimeChanged(const QTime &time) // LCD configurable para ALARMA
{
    int timeAsInt = time.hour() * 100 + time.minute(); // Cambiar a int
    settings.setValue("timeEdit_configAlarma", time); //guardar ene settings la alarma
    ui->lcdnumber_alarma->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
}

void MainWindow::on_pushButton_confirmarDatos_clicked() // Boton confirmar y enviar datos de ALARMA
{
    ui->label_DatosGuardados->setText("Cambios Guarados");

    // Obtener el estado del checkbox amanecer
    int isAlarmaChecked = ui->checkBox_alarma->isChecked();
    int isAmanecerChecked = ui->checkBox_amanecer->isChecked();
    int isBuzzerChecked = ui->checkBox_buzzer->isChecked();

    // Leo el LCD en la seccion de Alarma
    int valorLCDAlarma = ui->lcdnumber_alarma->intValue();

    // Crear un nuevo String para armar la trama de Hora
    char tramaAlarma[40] = {0};

    if(valorLCDAlarma < 10)
    {
        // Construir la trama con 'A' al principio y los strings
        sprintf(tramaAlarma, "A%d%d%d000%d00000", isAlarmaChecked, isAmanecerChecked, isBuzzerChecked, valorLCDAlarma);
    }
    else if(valorLCDAlarma < 100 && valorLCDAlarma >= 10)
    {
        // Construir la trama con 'A' al principio y los strings
        sprintf(tramaAlarma, "A%d%d%d00%d00000", isAlarmaChecked, isAmanecerChecked, isBuzzerChecked, valorLCDAlarma);
    }
    else if(valorLCDAlarma < 1000 && valorLCDAlarma >= 100)
    {
        // Construir la trama con 'A' al principio y los strings
        sprintf(tramaAlarma, "A%d%d%d0%d00000", isAlarmaChecked, isAmanecerChecked, isBuzzerChecked, valorLCDAlarma);
    }
    else if(valorLCDAlarma >= 1000)
    {
        // Construir la trama con 'A' al principio y los strings
        sprintf(tramaAlarma, "A%d%d%d%d00000", isAlarmaChecked, isAmanecerChecked, isBuzzerChecked, valorLCDAlarma);
    }
    else
    {
        // Construir la trama con 'A' al principio y los strings y agrego el 0 adelante
        sprintf(tramaAlarma, "A%d%d%d0%d00000", isAlarmaChecked, isAmanecerChecked, isBuzzerChecked, valorLCDAlarma);
    }
    // Imprimir el resultado
    qDebug() << "LCD Alarma:" << tramaAlarma;

    // Copiar la trama a la variable global 'send'
    strcpy(send, tramaAlarma);

    enviarTrama();  // Llamar a la función para enviar la trama

}

void MainWindow::on_checkBox_alarma_checkStateChanged(const Qt::CheckState &arg1) // CheckBox para ALARMA
{
    settings.setValue("checkBox_alarma", arg1); // Guardar el estado
    qDebug() << "Guardado checkBox_alarma:" << arg1;


    ui->label_DatosGuardados->setText("Cambios no guardados");
}

void MainWindow::loadSettings() // Funcion para cargar configuraciones de Alarma
{
    // Restaurar el estado de los QTimeEdits
    if (settings.contains("timeEdit_configAlarma")) {
        ui->timeEdit_configAlarma->setTime(settings.value("timeEdit_configAlarma").toTime());
    }


    // Restaurar el estado de los QCheckBoxes
    if (settings.contains("checkBox_alarma")) {
        ui->checkBox_alarma->setChecked(settings.value("checkBox_alarma").toBool());
    }

    if (settings.contains("checkBox_amanecer")) {
        ui->checkBox_amanecer->setChecked(settings.value("checkBox_amanecer").toBool());
    }

    if (settings.contains("checkBox_buzzer")) {
        ui->checkBox_buzzer->setChecked(settings.value("checkBox_buzzer").toBool());
    }

    qDebug() << "Configuración cargada.";
}

void MainWindow::on_timeEdit_Temporizador_userTimeChanged(const QTime &time)
{
    int timeAsInt = time.minute() * 100 + time.second(); // Cambiar a int
    settings.setValue("timeEdit_Temporizador", time); //guardar ene settings la alarma
    ui->lcdNumber_Temporizador->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
    ui->label_datosTimer->setText("Cambios no Guarados");
}


void MainWindow::on_timeEdit_PomodoroWork_userTimeChanged(const QTime &time)
{
    int timeAsInt = time.minute() * 100 + time.second(); // Cambiar a int
    settings.setValue("timeEdit_PomodoroWork", time); //guardar ene settings la alarma
    ui->lcdnumber_PomodoroWork->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
    ui->label_datosTimer->setText("Cambios no Guarados");
}

void MainWindow::on_timeEdit_PomodoroBreak_userTimeChanged(const QTime &time)
{
    int timeAsInt = time.minute() * 100 + time.second(); // Cambiar a int
    settings.setValue("timeEdit_PomodoroBreak", time); //guardar ene settings la alarma
    ui->lcdNumber_PomodoroBreak->display(static_cast<double>(timeAsInt)); // Actualizar el QLCDNumber
    ui->label_datosTimer->setText("Cambios no Guarados");
}

void MainWindow::on_pushButton_datosTimer_clicked() // Boton para confirmar y enviar datos de TIMER
{
    ui->label_datosTimer->setText("Cambios Guardados");
    // Leo los 3 LCD en la seccion de Pomodoro
    int valorLCDPomodoroTemporizador = ui->lcdNumber_Temporizador->intValue();
    int valorLCDPomodoroWork = ui->lcdnumber_PomodoroWork->intValue();
    int valorLCDPomodoroBreak = ui->lcdNumber_PomodoroBreak->intValue();

    // Crear un nuevo String para armar la trama de Pomodoro
    char tramaTemporizador[40] = {0};
    char tramaPomodoroWork[40] = {0};
    char tramaPomodoroBreak[40] = {0};
    char tramaPomodoro[40] = {0};

    if(valorLCDPomodoroTemporizador < 10)
    {
        sprintf(tramaTemporizador, "R000%d", valorLCDPomodoroTemporizador);
    }
    else if(valorLCDPomodoroTemporizador < 100 && valorLCDPomodoroTemporizador >= 10)
    {
        sprintf(tramaTemporizador, "R00%d", valorLCDPomodoroTemporizador);
    }
    else if(valorLCDPomodoroTemporizador < 1000 && valorLCDPomodoroTemporizador >= 100)
    {
        sprintf(tramaTemporizador, "R0%d", valorLCDPomodoroTemporizador);
    }
    else
    {
        sprintf(tramaTemporizador, "R%d", valorLCDPomodoroTemporizador);
    }


    if(valorLCDPomodoroWork < 10)
    {
        sprintf(tramaPomodoroWork, "000%d", valorLCDPomodoroWork);
    }
    else if(valorLCDPomodoroWork < 100 && valorLCDPomodoroWork >= 10)
    {
        sprintf(tramaPomodoroWork, "00%d", valorLCDPomodoroWork);
    }
    else if(valorLCDPomodoroWork < 1000 && valorLCDPomodoroWork >= 100)
    {
        sprintf(tramaPomodoroWork, "0%d", valorLCDPomodoroWork);
    }
    else
    {
        sprintf(tramaPomodoroWork, "%d", valorLCDPomodoroWork);
    }


    if(valorLCDPomodoroBreak < 10)
    {
        sprintf(tramaPomodoroBreak, "000%d", valorLCDPomodoroBreak);
    }
    else if(valorLCDPomodoroBreak < 100 && valorLCDPomodoroBreak >= 10)
    {
        sprintf(tramaPomodoroBreak, "00%d", valorLCDPomodoroBreak);
    }
    else if(valorLCDPomodoroBreak < 1000 && valorLCDPomodoroBreak >= 100)
    {
        sprintf(tramaPomodoroBreak, "0%d", valorLCDPomodoroBreak);
    }
    else
    {
        sprintf(tramaPomodoroBreak, "%d", valorLCDPomodoroBreak);
    }

    // Construir la trama con 'R' al principio
    sprintf(tramaPomodoro, "%s%s%s", tramaTemporizador, tramaPomodoroWork, tramaPomodoroBreak);

    // Imprimir el resultado
    qDebug() << "LCD Pomodoro:" << tramaPomodoro;

    // Copiar la trama a la variable global 'send'
    strcpy(send, tramaPomodoro);

    enviarTrama();  // Llamar a la función para enviar la trama
}

void MainWindow::loadSettingsTimer() // Funcion para cargar configuraciones de timers
{
    // Restaurar el estado de los QTimeEdits
    if (settings.contains("timeEdit_PomodoroWork")) {
        ui->timeEdit_PomodoroWork->setTime(settings.value("timeEdit_PomodoroWork").toTime());
    }
    if (settings.contains("timeEdit_PomodoroBreak")) {
        ui->timeEdit_PomodoroBreak->setTime(settings.value("timeEdit_PomodoroBreak").toTime());
    }
    if (settings.contains("timeEdit_Temporizador")) {
        ui->timeEdit_Temporizador->setTime(settings.value("timeEdit_Temporizador").toTime());
    }


    qDebug() << "Configuración cargada.";

}


void MainWindow::on_pushButton_confirmarDatos_2_clicked() // Boton para confirmar y enviar datos de HORA ACTUAL o CLOCK
{
    ui->label_DatosGuardados_2->setText("Cambios guardados");
    int valorLCDClock = ui->lcdNumber->intValue();

    // Crear un nuevo String para armar la trama de Hora
    char tramaHora[40] = {0};

    if(valorLCDClock < 10)
    {
        // Construir la trama con 'H' al principio
        sprintf(tramaHora, "H000%d00000000", valorLCDClock);
    }
    else if(valorLCDClock < 100 && valorLCDClock >= 10)
    {
        // Construir la trama con 'H' al principio
        sprintf(tramaHora, "H00%d00000000", valorLCDClock);
    }
    else if(valorLCDClock >= 1000)
    {
        // Construir la trama con 'H' al principio
        sprintf(tramaHora, "H%d00000000", valorLCDClock);
    }
    else
    {
        // Construir la trama con 'H' al principio y agrego el 0 adelante
        sprintf(tramaHora, "H0%d00000000", valorLCDClock);
    }

    // Imprimir el resultado
    qDebug() << "LCD Clock:" << tramaHora;

    // Copiar la trama a la variable global 'send'
    strcpy(send, tramaHora);

    enviarTrama();  // Llamar a la función para enviar la trama
}
