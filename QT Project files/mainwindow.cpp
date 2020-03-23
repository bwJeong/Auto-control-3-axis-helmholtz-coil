#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickconfig.h"

static QTime plotTime(QTime::currentTime()); // [Need update]

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    xAxisPS = new MK3010P();
    yAxisPS = new MK3010P();
    zAxisPS = new MK3010P();
    powerSwitchingModule = new PowerSwitchingModule();
    mag612 = new MAG612();
    mag612Plot = new QCustomPlot();

    txBuffer_1 = new QQueue<QString>();
    txBuffer_2 = new QQueue<QString>();
    txBuffer_3 = new QQueue<QString>();
    txBuffer_4 = new QQueue<char>();

    rxBuffer_4 = new QQueue<char>();

    txTimer_1 = new QTimer();
    txTimer_2 = new QTimer();
    txTimer_3 = new QTimer();
    txTimer_4 = new QTimer();

    rxTimer_1 = new QTimer();
    rxTimer_2 = new QTimer();
    rxTimer_3 = new QTimer();

    plotTimer = new QTimer();

    magCalTimer = new QTimer();

    enqueueStatusCMDTimer_1 = new QTimer();
    enqueueStatusCMDTimer_2 = new QTimer();
    enqueueStatusCMDTimer_3 = new QTimer();
    enqueueStatusCMDTimer_4 = new QTimer();

    connect(txTimer_1, SIGNAL(timeout()), this, SLOT(bufferTx_1()));
    connect(txTimer_2, SIGNAL(timeout()), this, SLOT(bufferTx_2()));
    connect(txTimer_3, SIGNAL(timeout()), this, SLOT(bufferTx_3()));
    connect(txTimer_4, SIGNAL(timeout()), this, SLOT(bufferTx_4()));

    connect(rxTimer_1, SIGNAL(timeout()), this, SLOT(readStatus_1()));
    connect(rxTimer_2, SIGNAL(timeout()), this, SLOT(readStatus_2()));
    connect(rxTimer_3, SIGNAL(timeout()), this, SLOT(readStatus_3()));

    connect(enqueueStatusCMDTimer_1, SIGNAL(timeout()), this, SLOT(enqueueStatusCMD_1()));
    connect(enqueueStatusCMDTimer_2, SIGNAL(timeout()), this, SLOT(enqueueStatusCMD_2()));
    connect(enqueueStatusCMDTimer_3, SIGNAL(timeout()), this, SLOT(enqueueStatusCMD_3()));
    connect(enqueueStatusCMDTimer_4, SIGNAL(timeout()), this, SLOT(enqueueStatusCMD_4()));

    connect(magCalTimer, SIGNAL(timeout()), this, SLOT(magCal()));

    connect(this, SIGNAL(startSavingData()), this, SLOT(createCalDataFile()));
    connect(this, SIGNAL(stopSavingData()), this, SLOT(closeCalDataFile()));

    // Banner
    QPixmap banner(":/resources/img/Banner.png");
    ui->lbBanner->setPixmap(banner);
    // KHU Logo
    QPixmap khuLogo(":/resources/img/khu.png");
    ui->lbKhuLogo->setPixmap(khuLogo);

    connect(ui->lbMAIN, SIGNAL(Mouse_Pressed()), this, SLOT(goMAIN()));
    connect(ui->lbCONFIG, SIGNAL(Mouse_Pressed()), this, SLOT(goCONFIG()));
    connect(ui->lbPSCONTROL, SIGNAL(Mouse_Pressed()), this, SLOT(goPSCONTROL()));
    connect(ui->lbMAGCAL, SIGNAL(Mouse_Pressed()), this, SLOT(goMAGCAL()));

    //

    initializePlot();
    makeSaveDirectory();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::goMAIN()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::goCONFIG()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::goPSCONTROL()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::goMAGCAL()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::bufferTx_1()
{
    if (!txBuffer_1->isEmpty()) {
        xAxisPS->port->write(txBuffer_1->dequeue().toStdString().c_str());
    }
}

void MainWindow::bufferTx_2()
{
    if (!txBuffer_2->isEmpty()) {
        yAxisPS->port->write(txBuffer_2->dequeue().toStdString().c_str());
    }
}

void MainWindow::bufferTx_3()
{
    if (!txBuffer_3->isEmpty()) {
        zAxisPS->port->write(txBuffer_3->dequeue().toStdString().c_str());
    }
}

void MainWindow::bufferTx_4()
{
    if (!txBuffer_4->isEmpty()) {
        char tmp = txBuffer_4->dequeue();
        const char* cmd = &tmp;
        powerSwitchingModule->port->write(cmd);

    }
}

void MainWindow::readStatus_1()
{
    QByteArray rxBuffer = xAxisPS->port->read(10);

    if (rxBuffer.length() == 10) {
        QString voltageOutput = "";
        QString currentOutput = "";

        for (int i = 0; i < 10; i++) {
            if (i < 5) {
                voltageOutput += rxBuffer[i];
            }
            else {
                currentOutput += rxBuffer[i];
            }
        }

        ui->lcdNumV_1->display(voltageOutput.toDouble());
        ui->lcdNumA_1->display(currentOutput.toDouble());
    }
}

void MainWindow::readStatus_2()
{
    QByteArray rxBuffer = yAxisPS->port->read(10);

    if (rxBuffer.length() == 10) {
        QString voltageOutput = "";
        QString currentOutput = "";

        for (int i = 0; i < 10; i++) {
            if (i < 5) {
                voltageOutput += rxBuffer[i];
            }
            else {
                currentOutput += rxBuffer[i];
            }
        }

        ui->lcdNumV_2->display(voltageOutput.toDouble());
        ui->lcdNumA_2->display(currentOutput.toDouble());
    }
}

void MainWindow::readStatus_3()
{
    QByteArray rxBuffer = zAxisPS->port->read(10);

    if (rxBuffer.length() == 10) {
        QString voltageOutput = "";
        QString currentOutput = "";

        for (int i = 0; i < 10; i++) {
            if (i < 5) {
                voltageOutput += rxBuffer[i];
            }
            else {
                currentOutput += rxBuffer[i];
            }
        }

        ui->lcdNumV_3->display(voltageOutput.toDouble());
        ui->lcdNumA_3->display(currentOutput.toDouble());
    }
}

void MainWindow::readStatus_4()
{
    QByteArray tmpBuffer = powerSwitchingModule->port->readAll();

    for (auto elem : tmpBuffer) {
        rxBuffer_4->enqueue(elem);
    }

    if (rxBuffer_4->length() >= 3) {
        QString powerSwitchingModuleStatus = "";

        for (int i = 0; i < 3; i ++) {
            powerSwitchingModuleStatus += rxBuffer_4->dequeue();
        }

        if(powerSwitchingModuleStatus[0] == '1') {
            ui->lbPowerDirection_1->setText("Positive");
            ui->lbPowerDirection_1->setStyleSheet("QLabel {color : Lime;}");
        }
        else if(powerSwitchingModuleStatus[0] == '0') {
            ui->lbPowerDirection_1->setText("Negative");
            ui->lbPowerDirection_1->setStyleSheet("QLabel {color : Red;}");
        }

        if(powerSwitchingModuleStatus[1] == '1') {
            ui->lbPowerDirection_2->setText("Positive");
            ui->lbPowerDirection_2->setStyleSheet("QLabel {color : Lime;}");
        }
        else if(powerSwitchingModuleStatus[1] == '0') {
            ui->lbPowerDirection_2->setText("Negative");
            ui->lbPowerDirection_2->setStyleSheet("QLabel {color : Red;}");
        }

        if(powerSwitchingModuleStatus[2] == '1') {
            ui->lbPowerDirection_3->setText("Positive");
            ui->lbPowerDirection_3->setStyleSheet("QLabel {color : Lime;}");
        }
        else if(powerSwitchingModuleStatus[2] == '0') {
            ui->lbPowerDirection_3->setText("Negative");
            ui->lbPowerDirection_3->setStyleSheet("QLabel {color : Red;}");
        }
    }
}

void MainWindow::realTimePlot() // [Need Update]
{
    double time_10Hz = plotTime.elapsed() / 100.; // Sampling rate: 10 Hz
    int startTime = int(time_10Hz);

    if (startTime - endTime < 0) { // If startTime is initialized, endTime is also initialized.
        endTime = 0;
    }

    if (startTime - endTime == 1) { // 1 == 100 ms
        mag612->collectData();
        mag612->calibrateData();
        emit dataCameIn();

        int x = mag612->getXAxisMagFieldStrength();
        int y = mag612->getYAxisMagFieldStrength();
        int z = mag612->getZAxisMagFieldStrength();

        ui->widgetPlot->graph(0)->addData(startTime / 10., x);
        ui->widgetPlot->graph(1)->addData(startTime / 10., y);
        ui->widgetPlot->graph(2)->addData(startTime / 10., z);
        ui->leRefX->setText(QString::number(x));
        ui->leRefY->setText(QString::number(y));
        ui->leRefZ->setText(QString::number(z));

        // Find min & max [Need update]
        if (maxQueue.length() == 80) {
            maxQueue.dequeue();
            minQueue.dequeue();
        }

        maxQueue.enqueue(qMax(x, qMax(y, z)));
        minQueue.enqueue(qMin(x, qMin(y, z)));

        int min = maxQueue[0];
        int max = minQueue[0];

        for (auto elem : maxQueue) {
            if (elem > max) {
                max = elem;
            }
        }

        for (auto elem : minQueue) {
            if (elem < min) {
                min = elem;
            }
        }
        //

        ui->widgetPlot->yAxis->setRange(min - 50, max + 50);

        endTime = startTime;
    }

    ui->widgetPlot->xAxis->setRange(time_10Hz / 10, 8, Qt::AlignRight);
    ui->widgetPlot->replot();
}

void MainWindow::enqueueStatusCMD_1()
{
    txBuffer_1->enqueue(xAxisPS->VOUT());
    txBuffer_1->enqueue(xAxisPS->IOUT());
}

void MainWindow::enqueueStatusCMD_2()
{
    txBuffer_2->enqueue(yAxisPS->VOUT());
    txBuffer_2->enqueue(yAxisPS->IOUT());
}

void MainWindow::enqueueStatusCMD_3()
{
    txBuffer_3->enqueue(zAxisPS->VOUT());
    txBuffer_3->enqueue(zAxisPS->IOUT());
}

void MainWindow::enqueueStatusCMD_4()
{
    txBuffer_4->enqueue(powerSwitchingModule->allAxisStatusCheck());
}

void MainWindow::initializePlot()
{
    ui->widgetPlot->addGraph();
    ui->widgetPlot->addGraph();
    ui->widgetPlot->addGraph();
    ui->widgetPlot->graph(0)->setPen(QPen(QColor(255, 0, 0)));
    ui->widgetPlot->graph(0)->setName("X-axis");
    ui->widgetPlot->graph(1)->setPen(QPen(QColor(0, 255, 0)));
    ui->widgetPlot->graph(1)->setName("Y-axis");
    ui->widgetPlot->graph(2)->setPen(QPen(QColor(0, 0, 255)));
    ui->widgetPlot->graph(2)->setName("Z-axis");
    ui->widgetPlot->legend->setVisible(true);
    ui->widgetPlot->legend->setFont(QFont("Arial", 8));
    ui->widgetPlot->legend->setRowSpacing(-3);
    ui->widgetPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widgetPlot->xAxis->setTicker(timeTicker);
    //ui->widgetPlot->xAxis->setTickLabelRotation(25);
    ui->widgetPlot->axisRect()->setupFullAxesBox();
    ui->widgetPlot->yAxis->setRange(-110000, 110000);
}

void MainWindow::makeSaveDirectory()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir saveDirectory(documentsPath + "/AUTOCONTROL_SAVEFILES");
    saveDirectory.mkpath(documentsPath + "/AUTOCONTROL_SAVEFILES");
    ui->leSaveDirPath->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/AUTOCONTROL_SAVEFILES");
}

void MainWindow::saveAs()
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString time = QDateTime::currentDateTime().toString(" HH:mm:ss.zzz");
    saveFile->write(date.toStdString().c_str());
    saveFile->write(",");
    saveFile->write(time.toStdString().c_str());
    saveFile->write(",");
    saveFile->write(QString::number(mag612->getXAxisMagFieldStrength()).toStdString().c_str());
    saveFile->write(",");
    saveFile->write(QString::number(mag612->getYAxisMagFieldStrength()).toStdString().c_str());
    saveFile->write(",");
    saveFile->write(QString::number(mag612->getZAxisMagFieldStrength()).toStdString().c_str());
    saveFile->write("\n");
}

void MainWindow::makeAutoSaveFile()
{
    QString saveDirectoryPath = ui->leSaveDirPath->text();
    QString fileName = QDateTime::currentDateTime().toString("/yyMMdd_HHmmss");
    autoSaveFile = new QFile(saveDirectoryPath + fileName + "_AutoSave.csv");
    autoSaveFile->open(QIODevice::WriteOnly);
}

void MainWindow::closeAutoSaveFile()
{
    autoSaveFile->close();
    delete autoSaveFile;
}

void MainWindow::autoSave()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    if ((hourCheck != dateTime.toString(" HH")) && (hourCheck != "")) { // [Need update]
        closeAutoSaveFile();
        makeAutoSaveFile();
    }
    hourCheck = dateTime.toString(" HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString(" HH:mm:ss.zzz");

    autoSaveFile->write(date.toStdString().c_str());
    autoSaveFile->write(",");
    autoSaveFile->write(time.toStdString().c_str());
    autoSaveFile->write(",");
    autoSaveFile->write(QString::number(mag612->getXAxisMagFieldStrength()).toStdString().c_str());
    autoSaveFile->write(",");
    autoSaveFile->write(QString::number(mag612->getYAxisMagFieldStrength()).toStdString().c_str());
    autoSaveFile->write(",");
    autoSaveFile->write(QString::number(mag612->getZAxisMagFieldStrength()).toStdString().c_str());
    autoSaveFile->write("\n");
}

void MainWindow::setXAxisMagFieldStrength()
{
    double coefficientZ = 0.0231; // Helm Z: dV per 1 uT
    double xAxis_duT = ui->sbSetting_uT_1->value() - mag612->getXAxisMagFieldStrength() / 1000.; // Ref. sensor axis
    double zAxis_dV = int(xAxis_duT * coefficientZ * 100) / 100.; // Helm axis
    double appliedZAxisV;

    if (ui->lbPowerDirection_3->text() == "Positive") {
        appliedZAxisV = ui->lcdNumV_3->value() + zAxis_dV;
    }
    else {
        appliedZAxisV = -(ui->lcdNumV_3->value()) + zAxis_dV;
    }

    if (appliedZAxisV < 0) {
        appliedZAxisV = fabs(appliedZAxisV);
        if (ui->lbPowerDirection_3->text() == "Positive") {
            txBuffer_4->enqueue(powerSwitchingModule->zAxisPowerSwitching());
        }

        txBuffer_3->enqueue(zAxisPS->ISET("1.000"));
        txBuffer_3->enqueue(zAxisPS->VSET(QString::number(appliedZAxisV)));
    }
    else {
        if (ui->lbPowerDirection_3->text() == "Negative") {
            txBuffer_4->enqueue(powerSwitchingModule->zAxisPowerSwitching());
        }

        txBuffer_3->enqueue(zAxisPS->ISET("1.000"));
        txBuffer_3->enqueue(zAxisPS->VSET(QString::number(appliedZAxisV)));
    }
}

void MainWindow::setYAxisMagFieldStrength()
{
    double coefficientX = 0.028; // Helm X: dV per 1 uT (before: 0.0301)
    double yAxis_duT = ui->sbSetting_uT_2->value() - mag612->getYAxisMagFieldStrength() / 1000.; // Ref. sensor axis
    double xAxis_dV = int(yAxis_duT * coefficientX * 100) / 100.; // Helm axis
    double appliedXAxisV;

    if (ui->lbPowerDirection_1->text() == "Positive") {
        appliedXAxisV = ui->lcdNumV_1->value() + xAxis_dV;
    }
    else {
        appliedXAxisV = -(ui->lcdNumV_1->value()) + xAxis_dV;
    }

    if (appliedXAxisV < 0) {
        appliedXAxisV = fabs(appliedXAxisV);
        if (ui->lbPowerDirection_1->text() == "Positive") {
            txBuffer_4->enqueue(powerSwitchingModule->xAxisPowerSwitching());
        }

        txBuffer_1->enqueue(xAxisPS->ISET("1.000"));
        txBuffer_1->enqueue(xAxisPS->VSET(QString::number(appliedXAxisV)));
    }
    else {
        if (ui->lbPowerDirection_1->text() == "Negative") {
            txBuffer_4->enqueue(powerSwitchingModule->xAxisPowerSwitching());
        }

        txBuffer_1->enqueue(xAxisPS->ISET("1.000"));
        txBuffer_1->enqueue(xAxisPS->VSET(QString::number(appliedXAxisV)));
    }
}

void MainWindow::setZAxisMagFieldStrength()
{
    double coefficientY = 0.02615; // Helm Y: dV per 1 uT (before: 0.02635)
    double zAxis_duT = ui->sbSetting_uT_3->value() - mag612->getZAxisMagFieldStrength() / 1000.; // Ref. sensor axis
    double yAxis_dV = int(zAxis_duT * coefficientY * 100) / 100.; // Helm axis
    double appliedYAxisV;

    if (ui->lbPowerDirection_2->text() == "Positive") {
        appliedYAxisV = ui->lcdNumV_2->value() + yAxis_dV;
    }
    else {
        appliedYAxisV = -(ui->lcdNumV_2->value()) + yAxis_dV;
    }

    if (appliedYAxisV < 0) {
        appliedYAxisV = fabs(appliedYAxisV);
        if (ui->lbPowerDirection_2->text() == "Positive") {
            txBuffer_4->enqueue(powerSwitchingModule->yAxisPowerSwitching());
        }

        txBuffer_2->enqueue(yAxisPS->ISET("1.000"));
        txBuffer_2->enqueue(yAxisPS->VSET(QString::number(appliedYAxisV)));
    }
    else {
        if (ui->lbPowerDirection_2->text() == "Negative") {
            txBuffer_4->enqueue(powerSwitchingModule->yAxisPowerSwitching());
        }

        txBuffer_2->enqueue(yAxisPS->ISET("1.000"));
        txBuffer_2->enqueue(yAxisPS->VSET(QString::number(appliedYAxisV)));
    }
}

void MainWindow::setXAxisMagFieldStrength(double value)
{
    double coefficientZ = 0.0231; // Helm Z: dV per 1 uT
    double xAxis_duT = value - mag612->getXAxisMagFieldStrength() / 1000.; // Ref. sensor axis
    double zAxis_dV = int(xAxis_duT * coefficientZ * 100) / 100.; // Helm axis
    double appliedZAxisV;

    if (ui->lbPowerDirection_3->text() == "Positive") {
        appliedZAxisV = ui->lcdNumV_3->value() + zAxis_dV;
    }
    else {
        appliedZAxisV = -(ui->lcdNumV_3->value()) + zAxis_dV;
    }

    if (appliedZAxisV < 0) {
        appliedZAxisV = fabs(appliedZAxisV);
        if (ui->lbPowerDirection_3->text() == "Positive") {
            txBuffer_4->enqueue(powerSwitchingModule->zAxisPowerSwitching());
        }

        txBuffer_3->enqueue(zAxisPS->ISET("1.000"));
        txBuffer_3->enqueue(zAxisPS->VSET(QString::number(appliedZAxisV)));
    }
    else {
        if (ui->lbPowerDirection_3->text() == "Negative") {
            txBuffer_4->enqueue(powerSwitchingModule->zAxisPowerSwitching());
        }

        txBuffer_3->enqueue(zAxisPS->ISET("1.000"));
        txBuffer_3->enqueue(zAxisPS->VSET(QString::number(appliedZAxisV)));
    }
}

void MainWindow::setYAxisMagFieldStrength(double value)
{
    double coefficientX = 0.028; // Helm X: dV per 1 uT (before: 0.0301)
    double yAxis_duT = value - mag612->getYAxisMagFieldStrength() / 1000.; // Ref. sensor axis
    double xAxis_dV = int(yAxis_duT * coefficientX * 100) / 100.; // Helm axis
    double appliedXAxisV;

    if (ui->lbPowerDirection_1->text() == "Positive") {
        appliedXAxisV = ui->lcdNumV_1->value() + xAxis_dV;
    }
    else {
        appliedXAxisV = -(ui->lcdNumV_1->value()) + xAxis_dV;
    }

    if (appliedXAxisV < 0) {
        appliedXAxisV = fabs(appliedXAxisV);
        if (ui->lbPowerDirection_1->text() == "Positive") {
            txBuffer_4->enqueue(powerSwitchingModule->xAxisPowerSwitching());
        }

        txBuffer_1->enqueue(xAxisPS->ISET("1.000"));
        txBuffer_1->enqueue(xAxisPS->VSET(QString::number(appliedXAxisV)));
    }
    else {
        if (ui->lbPowerDirection_1->text() == "Negative") {
            txBuffer_4->enqueue(powerSwitchingModule->xAxisPowerSwitching());
        }

        txBuffer_1->enqueue(xAxisPS->ISET("1.000"));
        txBuffer_1->enqueue(xAxisPS->VSET(QString::number(appliedXAxisV)));
    }
}

void MainWindow::setZAxisMagFieldStrength(double value)
{
    double coefficientY = 0.02615; // Helm Y: dV per 1 uT (before: 0.02635)
    double zAxis_duT = value - mag612->getZAxisMagFieldStrength() / 1000.; // Ref. sensor axis
    double yAxis_dV = int(zAxis_duT * coefficientY * 100) / 100.; // Helm axis
    double appliedYAxisV;

    if (ui->lbPowerDirection_2->text() == "Positive") {
        appliedYAxisV = ui->lcdNumV_2->value() + yAxis_dV;
    }
    else {
        appliedYAxisV = -(ui->lcdNumV_2->value()) + yAxis_dV;
    }

    if (appliedYAxisV < 0) {
        appliedYAxisV = fabs(appliedYAxisV);
        if (ui->lbPowerDirection_2->text() == "Positive") {
            txBuffer_4->enqueue(powerSwitchingModule->yAxisPowerSwitching());
        }

        txBuffer_2->enqueue(yAxisPS->ISET("1.000"));
        txBuffer_2->enqueue(yAxisPS->VSET(QString::number(appliedYAxisV)));
    }
    else {
        if (ui->lbPowerDirection_2->text() == "Negative") {
            txBuffer_4->enqueue(powerSwitchingModule->yAxisPowerSwitching());
        }

        txBuffer_2->enqueue(yAxisPS->ISET("1.000"));
        txBuffer_2->enqueue(yAxisPS->VSET(QString::number(appliedYAxisV)));
    }
}

void MainWindow::magCal()
{
    if (currentStep != 0) {
        emit stopSavingData();
    }

    totalStep = testList_uT.size() * 3;

    ui->progressBar->setValue((currentStep + 1) / double(totalStep) * 100);

    if (currentStep / testList_uT.size() == 0) {
        setXAxisMagFieldStrength(testList_uT[currentStep % testList_uT.size()]);
        setYAxisMagFieldStrength(0);
        setZAxisMagFieldStrength(0);
        QTimer::singleShot(3000, this, [&]() {
            setXAxisMagFieldStrength(testList_uT[currentStep % testList_uT.size()]);
            setYAxisMagFieldStrength(0);
            setZAxisMagFieldStrength(0);
            currentStep++;
            QTimer::singleShot(300, this, [&]() {
                emit startSavingData();
            });
        });
    }
    else if (currentStep / testList_uT.size() == 1) {
        setXAxisMagFieldStrength(0);
        setYAxisMagFieldStrength(testList_uT[currentStep % testList_uT.size()]);
        setZAxisMagFieldStrength(0);
        QTimer::singleShot(3000, this, [&]() {
            setXAxisMagFieldStrength(0);
            setYAxisMagFieldStrength(testList_uT[currentStep % testList_uT.size()]);
            setZAxisMagFieldStrength(0);
            currentStep++;
            QTimer::singleShot(300, this, [&]() {
                emit startSavingData();
            });
        });
    }
    else if (currentStep / testList_uT.size() == 2) {
        setXAxisMagFieldStrength(0);
        setYAxisMagFieldStrength(0);
        setZAxisMagFieldStrength(testList_uT[currentStep % testList_uT.size()]);
        QTimer::singleShot(3000, this, [&]() {
            setXAxisMagFieldStrength(0);
            setYAxisMagFieldStrength(0);
            setZAxisMagFieldStrength(testList_uT[currentStep % testList_uT.size()]);
            currentStep++;
            QTimer::singleShot(300, this, [&]() {
                emit startSavingData();
            });

            if (currentStep == totalStep) {
                QTimer::singleShot(durationPerStep - 3000, this, [&]() {
                    emit stopSavingData();

                    txBuffer_1->enqueue(xAxisPS->OUT0());
                    txBuffer_2->enqueue(yAxisPS->OUT0());
                    txBuffer_3->enqueue(zAxisPS->OUT0());

                    ui->btnMAGCALStart->setText("Start");
                    ui->btnMAGCALStart->setChecked(false);
                    ui->cbDurationPerStep->setEnabled(true);
                    ui->cbFullRange->setEnabled(true);
                    ui->cbInterval->setEnabled(true);

                    magCalTimer->stop();
                });
            }
        });
    }
}

void MainWindow::createCalDataFile()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    static QString saveDirectoryPath;

    if (currentStep - 1 == 0) {
        QDir saveDirectory(documentsPath + "/AUTOCONTROL_SAVEFILES" + QDateTime::currentDateTime().toString("/yyMMdd_HHmmss") + "_CAL");
        saveDirectory.mkpath(documentsPath + "/AUTOCONTROL_SAVEFILES" + QDateTime::currentDateTime().toString("/yyMMdd_HHmmss") + "_CAL");
        saveDirectoryPath = saveDirectory.path();
    }

    QString fileName;

    if ((currentStep - 1) / testList_uT.size() == 0) {
        fileName = saveDirectoryPath + "/xAxis_" + QString::number(testList_uT[(currentStep - 1) % testList_uT.size()]) + "uT.csv";
        qDebug() << fileName;
    }
    else if ((currentStep - 1) / testList_uT.size() == 1) {
        fileName = saveDirectoryPath+ "/yAxis_" + QString::number(testList_uT[(currentStep - 1) % testList_uT.size()]) + "uT.csv";
        qDebug() << fileName;
    }
    else if ((currentStep - 1) / testList_uT.size() == 2) {
        fileName = saveDirectoryPath + "/zAxis_" + QString::number(testList_uT[(currentStep - 1) % testList_uT.size()]) + "uT.csv";
        qDebug() << fileName;
    }

    calDataFile = new QFile(fileName);
    if (calDataFile->open(QIODevice::WriteOnly)) {
        connect(this, SIGNAL(dataCameIn()), this, SLOT(saveCalData()));
    }
}

void MainWindow::closeCalDataFile()
{
    disconnect(this, SIGNAL(dataCameIn()), this, SLOT(saveCalData()));

    calDataFile->close();
    delete calDataFile;
}

void MainWindow::saveCalData()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString(" HH:mm:ss.zzz");

    calDataFile->write(date.toStdString().c_str());
    calDataFile->write(",");
    calDataFile->write(time.toStdString().c_str());
    calDataFile->write(",");
    calDataFile->write(QString::number(mag612->getXAxisMagFieldStrength()).toStdString().c_str());
    calDataFile->write(",");
    calDataFile->write(QString::number(mag612->getYAxisMagFieldStrength()).toStdString().c_str());
    calDataFile->write(",");
    calDataFile->write(QString::number(mag612->getZAxisMagFieldStrength()).toStdString().c_str());
    calDataFile->write("\n");
}

QString MainWindow::TCHARToQString(const TCHAR *ptsz)
{
    int len = wcslen((wchar_t*)ptsz);
    char* psz = new char[2 * len + 1];
    wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
    QString s = psz;

    delete[] psz;

    return s;
}

void MainWindow::on_btnConnect_1_toggled(bool checked)
{
    if(checked) {
        xAxisPS->setPort(ui->cbPortName_1->currentText());
        if(!xAxisPS->port->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, "Error", "Cannot connect!", "OK");
            ui->btnConnect_1->setChecked(false);
            ui->btnConnect_1->setText("Connect");
        }
        else {
            ui->btnConnect_1->setText("Clear");
            ui->cbPortName_1->setEnabled(false);
            ui->btnPortScan_1->setEnabled(false);
            txTimer_1->start(70);
        }
    }
    else {
        txTimer_1->stop();
        xAxisPS->port->close();
        ui->btnConnect_1->setText("Connect");
        ui->cbPortName_1->setEnabled(true);
        ui->btnPortScan_1->setEnabled(true);
    }
}

void MainWindow::on_btnConnect_2_toggled(bool checked)
{
    if(checked) {
        yAxisPS->setPort(ui->cbPortName_2->currentText());
        if(!yAxisPS->port->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, "Error", "Cannot connect!", "OK");
            ui->btnConnect_2->setChecked(false);
            ui->btnConnect_2->setText("Connect");
        }
        else {
            ui->btnConnect_2->setText("Clear");
            ui->cbPortName_2->setEnabled(false);
            ui->btnPortScan_2->setEnabled(false);
            txTimer_2->start(70);
        }
    }
    else {
        txTimer_2->stop();
        yAxisPS->port->close();
        ui->btnConnect_2->setText("Connect");
        ui->cbPortName_2->setEnabled(true);
        ui->btnPortScan_2->setEnabled(true);
    }
}

void MainWindow::on_btnConnect_3_toggled(bool checked)
{
    if(checked) {
        zAxisPS->setPort(ui->cbPortName_3->currentText());
        if(!zAxisPS->port->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, "Error", "Cannot connect!", "OK");
            ui->btnConnect_3->setChecked(false);
            ui->btnConnect_3->setText("Connect");
        }
        else {
            ui->btnConnect_3->setText("Clear");
            ui->cbPortName_3->setEnabled(false);
            ui->btnPortScan_3->setEnabled(false);
            txTimer_3->start(70);
        }
    }
    else {
        txTimer_3->stop();
        zAxisPS->port->close();
        ui->btnConnect_3->setText("Connect");
        ui->cbPortName_3->setEnabled(true);
        ui->btnPortScan_3->setEnabled(true);
    }
}

void MainWindow::on_btnConnect_4_toggled(bool checked)
{
    if(checked) {
        powerSwitchingModule->setPort(ui->cbPortName_4->currentText());
        if(!powerSwitchingModule->port->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, "Error", "Cannot connect!", "OK");
            ui->btnConnect_4->setChecked(false);
            ui->btnConnect_4->setText("Connect");
        }
        else {
            ui->btnConnect_4->setText("Clear");
            ui->cbPortName_4->setEnabled(false);
            ui->btnPortScan_4->setEnabled(false);
            txTimer_4->start(70);
            enqueueStatusCMDTimer_4->start(500);
            connect(powerSwitchingModule->port, SIGNAL(readyRead()),this, SLOT(readStatus_4()));
        }
    }
    else {
        powerSwitchingModule->port->close();
        ui->btnConnect_4->setText("Connect");
        ui->cbPortName_4->setEnabled(true);
        ui->btnPortScan_4->setEnabled(true);
        ui->lbPowerDirection_1->setText("");
        ui->lbPowerDirection_2->setText("");
        ui->lbPowerDirection_3->setText("");
        txTimer_4->stop();
        enqueueStatusCMDTimer_4->stop();
        disconnect(powerSwitchingModule->port, SIGNAL(readyRead()),this, SLOT(readStatus_4()));
    }
}

void MainWindow::on_btnConnect_5_toggled(bool checked)
{
    if(checked) {
        if(!mag612->connectUSB()) {
            QMessageBox::information(this, "Error", "Cannot connect!", "OK");
            ui->btnConnect_5->setChecked(false);
            ui->btnConnect_5->setText("Connect");
        }
        else {
            ui->btnConnect_5->setText("Clear");
        }
    }
    else {
        mag612->disconnectUSB();
        ui->btnConnect_5->setText("Connect");
    }
}

void MainWindow::on_btnPortScan_1_clicked()
{
    ui->cbPortName_1->clear();

    HKEY hKey;
    RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

    TCHAR szData[20], szName[100];
    DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
    memset(szData, 0x00, sizeof(szData));
    memset(szName, 0x00, sizeof(szName));

    while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL)) {
        index++;

        RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
        QString tem = TCHARToQString(szData);
        ui->cbPortName_1->addItem(tem);

        memset(szData, 0x00, sizeof(szData));
        memset(szName, 0x00, sizeof(szName));
        dwSize = 100;
        dwSize2 = 20;
    }

    RegCloseKey(hKey);
}

void MainWindow::on_btnPortScan_2_clicked()
{
    ui->cbPortName_2->clear();

    HKEY hKey;
    RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

    TCHAR szData[20], szName[100];
    DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
    memset(szData, 0x00, sizeof(szData));
    memset(szName, 0x00, sizeof(szName));

    while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL)) {
        index++;

        RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
        QString tem = TCHARToQString(szData);
        ui->cbPortName_2->addItem(tem);

        memset(szData, 0x00, sizeof(szData));
        memset(szName, 0x00, sizeof(szName));
        dwSize = 100;
        dwSize2 = 20;
    }

    RegCloseKey(hKey);
}

void MainWindow::on_btnPortScan_3_clicked()
{
    ui->cbPortName_3->clear();

    HKEY hKey;
    RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

    TCHAR szData[20], szName[100];
    DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
    memset(szData, 0x00, sizeof(szData));
    memset(szName, 0x00, sizeof(szName));

    while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL)) {
        index++;

        RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
        QString tem = TCHARToQString(szData);
        ui->cbPortName_3->addItem(tem);

        memset(szData, 0x00, sizeof(szData));
        memset(szName, 0x00, sizeof(szName));
        dwSize = 100;
        dwSize2 = 20;
    }

    RegCloseKey(hKey);
}

void MainWindow::on_btnPortScan_4_clicked()
{
    ui->cbPortName_4->clear();

    HKEY hKey;
    RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

    TCHAR szData[20], szName[100];
    DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
    memset(szData, 0x00, sizeof(szData));
    memset(szName, 0x00, sizeof(szName));

    while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL)) {
        index++;

        RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
        QString tem = TCHARToQString(szData);
        ui->cbPortName_4->addItem(tem);

        memset(szData, 0x00, sizeof(szData));
        memset(szName, 0x00, sizeof(szName));
        dwSize = 100;
        dwSize2 = 20;
    }

    RegCloseKey(hKey);
}

void MainWindow::on_btnSetV_1_clicked()
{
    txBuffer_1->enqueue(xAxisPS->VSET(ui->sbV_1->text()));
    ui->lcdNumV_1->display(ui->sbV_1->value());
}

void MainWindow::on_btnSetA_1_clicked()
{
    txBuffer_1->enqueue(xAxisPS->ISET(ui->sbA_1->text()));
    ui->lcdNumA_1->display(ui->sbA_1->value());
}

void MainWindow::on_btnOutputOn_1_clicked()
{
    txBuffer_1->enqueue(xAxisPS->OUT1());
    enqueueStatusCMDTimer_1->start(500);
    QTimer::singleShot(100, this, [&]() {
        rxTimer_1->start(500);
    });
    ui->lbOn_1->setStyleSheet("QLabel {color : Lime;}");
    ui->lbOff_1->setStyleSheet("QLabel {color: rgb(107, 107, 107);}");

}

void MainWindow::on_btnOutputOff_1_clicked()
{
    txBuffer_1->enqueue(xAxisPS->OUT0());
    enqueueStatusCMDTimer_1->stop();
    rxTimer_1->stop();
    ui->lcdNumV_1->display(0);
    ui->lcdNumA_1->display(0);
    ui->lbOn_1->setStyleSheet("QLabel {color: rgb(107, 107, 107);}");
    ui->lbOff_1->setStyleSheet("QLabel {color : Red;}");
}

void MainWindow::on_btnPowerSwitching_1_clicked()
{
    txBuffer_4->enqueue(powerSwitchingModule->xAxisPowerSwitching());
}

void MainWindow::on_btnSetV_2_clicked()
{
    txBuffer_2->enqueue(yAxisPS->VSET(ui->sbV_2->text()));
    ui->lcdNumV_2->display(ui->sbV_2->value());
}

void MainWindow::on_btnSetA_2_clicked()
{
    txBuffer_2->enqueue(yAxisPS->ISET(ui->sbA_2->text()));
    ui->lcdNumA_2->display(ui->sbA_2->value());
}

void MainWindow::on_btnOutputOn_2_clicked()
{
    txBuffer_2->enqueue(yAxisPS->OUT1());
    enqueueStatusCMDTimer_2->start(500);
    QTimer::singleShot(100, this, [&]() {
        rxTimer_2->start(500);
    });
    ui->lbOn_2->setStyleSheet("QLabel {color : Lime;}");
    ui->lbOff_2->setStyleSheet("QLabel {color: rgb(107, 107, 107);}");
}

void MainWindow::on_btnOutputOff_2_clicked()
{
    txBuffer_2->enqueue(yAxisPS->OUT0());
    enqueueStatusCMDTimer_2->stop();
    rxTimer_2->stop();
    ui->lcdNumV_2->display(0);
    ui->lcdNumA_2->display(0);
    ui->lbOn_2->setStyleSheet("QLabel {color: rgb(107, 107, 107);}");
    ui->lbOff_2->setStyleSheet("QLabel {color : Red;}");
}

void MainWindow::on_btnPowerSwitching_2_clicked()
{
    txBuffer_4->enqueue(powerSwitchingModule->yAxisPowerSwitching());
}

void MainWindow::on_btnSetV_3_clicked()
{
    txBuffer_3->enqueue(zAxisPS->VSET(ui->sbV_3->text()));
    ui->lcdNumV_3->display(ui->sbV_3->value());
}

void MainWindow::on_btnSetA_3_clicked()
{
    txBuffer_3->enqueue(zAxisPS->ISET(ui->sbA_3->text()));
    ui->lcdNumA_3->display(ui->sbA_3->value());
}

void MainWindow::on_btnOutputOn_3_clicked()
{
    txBuffer_3->enqueue(zAxisPS->OUT1());
    enqueueStatusCMDTimer_3->start(500);
    QTimer::singleShot(100, this, [&]() {
        rxTimer_3->start(500);
    });
    ui->lbOn_3->setStyleSheet("QLabel {color : Lime;}");
    ui->lbOff_3->setStyleSheet("QLabel {color: rgb(107, 107, 107);}");
}

void MainWindow::on_btnOutputOff_3_clicked()
{
    txBuffer_3->enqueue(zAxisPS->OUT0());
    enqueueStatusCMDTimer_3->stop();
    rxTimer_3->stop();
    ui->lcdNumV_3->display(0);
    ui->lcdNumA_3->display(0);
    ui->lbOn_3->setStyleSheet("QLabel {color: rgb(107, 107, 107);}");
    ui->lbOff_3->setStyleSheet("QLabel {color : Red;}");
}

void MainWindow::on_btnPowerSwitching_3_clicked()
{
    txBuffer_4->enqueue(powerSwitchingModule->zAxisPowerSwitching());
}

void MainWindow::on_btnStartPlot_toggled(bool checked)
{
    if (checked) {
        plotTime.restart();
        ui->btnStartPlot->setText("Stop");
        connect(ui->widgetPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(ui->widgetPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetPlot->yAxis2, SLOT(setRange(QCPRange)));
        connect(plotTimer, SIGNAL(timeout()), this, SLOT(realTimePlot()));
        plotTimer->start(0);
    }
    else {
        ui->btnStartPlot->setText("Start measurement");
        for (int i = 0; i < 2; i++) {
            ui->widgetPlot->graph(i)->data()->clear();
        }
        disconnect(ui->widgetPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetPlot->xAxis2, SLOT(setRange(QCPRange)));
        disconnect(ui->widgetPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetPlot->yAxis2, SLOT(setRange(QCPRange)));
        disconnect(plotTimer, SIGNAL(timeout()), this, SLOT(realTimePlot()));
        plotTimer->stop();
    }
}

void MainWindow::on_btnSaveAs_toggled(bool checked) // [Need update]
{
    if (checked) {
        QString saveDirectoryPath = ui->leSaveDirPath->text();
        QString fileName = QFileDialog::getSaveFileName(
                    this,
                    tr("Save as..."),
                    saveDirectoryPath,
                    "csv files (*.csv)"
                    );

        if (fileName != "") {
            ui->btnSaveAs->setText("Saving...");
            ui->btnAutoSave->setEnabled(false);
            saveFile = new QFile(fileName);
            if (saveFile->open(QIODevice::WriteOnly)) {
                connect(this, SIGNAL(dataCameIn()), this, SLOT(saveAs()));
            }
        }
        else {
            ui->btnSaveAs->setCheckable(false);
            ui->btnSaveAs->setCheckable(true);
        }
    }
    else {
        if (saveFile->isOpen()) {
            disconnect(this, SIGNAL(dataCameIn()), this, SLOT(saveAs()));
            saveFile->close();
            delete saveFile;
            ui->btnSaveAs->setText("Save as...");
            ui->btnAutoSave->setEnabled(true);
        }
    }
}

void MainWindow::on_btnAutoSave_toggled(bool checked)
{
    if (checked) {
        makeAutoSaveFile();
        connect(this, SIGNAL(dataCameIn()), this, SLOT(autoSave()));
        ui->btnAutoSave->setText("Auto saving...");
        ui->btnSaveAs->setEnabled(false);
    }
    else {
        disconnect(this, SIGNAL(dataCameIn()), this, SLOT(autoSave()));
        closeAutoSaveFile();
        ui->btnAutoSave->setText("Auto save (Every hour)");
        ui->btnSaveAs->setEnabled(true);
    }
}

void MainWindow::on_btnSet_uT_clicked()
{
    setXAxisMagFieldStrength();
    setYAxisMagFieldStrength();
    setZAxisMagFieldStrength();
}

void MainWindow::on_btnMAGCALStart_toggled(bool checked)
{
    if (checked) {
        currentStep = 0;

        if (ui->cbDurationPerStep->currentIndex() == 0) {
            durationPerStep = 303300;
        }
        else if (ui->cbDurationPerStep->currentIndex() == 1) {
            durationPerStep = 63300; // 63300
        }
        else if (ui->cbDurationPerStep->currentIndex() == 2) {
            durationPerStep = 33300;
        }

        if (ui->cbFullRange->currentIndex() == 0 && ui->cbInterval->currentIndex() == 0) {
            testList_uT = {100, 50, 0, -50, -100};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 0 && ui->cbInterval->currentIndex() == 1) {
            testList_uT = {100, 75, 50, 25, 0, -25, -50, -75, -100};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 0 && ui->cbInterval->currentIndex() == 2) {
            testList_uT = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0, -10, -20, -30, -40, -50, -60, -70, -80, -90, -100};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 0 && ui->cbInterval->currentIndex() == 3) {
            testList_uT = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0,
                           -5, -10, -15, -20, -25, -30, -35, -40, -45, -50, -55, -60, -65, -70, -75, -80, -85, -90, -95, -100};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 1 && ui->cbInterval->currentIndex() == 0) {
            testList_uT = {50, 0, -50};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 1 && ui->cbInterval->currentIndex() == 1) {
            testList_uT = {50, 25, 0, -25, -50};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 1 && ui->cbInterval->currentIndex() == 2) {
            testList_uT = {50, 40, 30, 20, 10, 0, -10, -20, -30, -40, -50};
            magCalTimer->start(durationPerStep);
        }
        else if (ui->cbFullRange->currentIndex() == 1 && ui->cbInterval->currentIndex() == 3) {
            testList_uT = {50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0, -5, -10, -15, -20, -25, -30, -35, -40, -45, -50};
            magCalTimer->start(durationPerStep);
        }

        ui->btnMAGCALStart->setText("Stop");
        ui->cbDurationPerStep->setEnabled(false);
        ui->cbFullRange->setEnabled(false);
        ui->cbInterval->setEnabled(false);
    }
    else {
        magCalTimer->stop();
        ui->btnMAGCALStart->setText("Start");
        ui->cbDurationPerStep->setEnabled(true);
        ui->cbFullRange->setEnabled(true);
        ui->cbInterval->setEnabled(true);
        ui->progressBar->setValue(0);
    }
}

void MainWindow::on_btnFindNSet_clicked()
{
    QString saveDirName = QFileDialog::getExistingDirectory(
                this,
                tr("Open directory"),
                ui->leSaveDirPath->text(),
                QFileDialog::ShowDirsOnly
                | QFileDialog::DontResolveSymlinks);

    if (saveDirName != "") {
        ui->leSaveDirPath->setText(saveDirName);
    }
}
