#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QQueue>
#include <QSet>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QtAlgorithms>
#include <QDir>
#include <QFileDialog>
#include <QLocale>
#include <QDateTime>
#include <mk3010p.h>
#include <powerswitchingmodule.h>
#include <mag612.h>
#include <qcustomplot.h>
#include "windows.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void dataCameIn();
    void startSavingData();
    void stopSavingData();

private slots:
    void goMAIN();
    void goCONFIG();
    void goPSCONTROL();
    void goMAGCAL();
    void bufferTx_1();
    void bufferTx_2();
    void bufferTx_3();
    void bufferTx_4();
    void readStatus_1();
    void readStatus_2();
    void readStatus_3();
    void readStatus_4();
    void realTimePlot();
    void enqueueStatusCMD_1();
    void enqueueStatusCMD_2();
    void enqueueStatusCMD_3();
    void enqueueStatusCMD_4();
    void initializePlot();
    void makeSaveDirectory();
    void saveAs();
    void makeAutoSaveFile();
    void closeAutoSaveFile();
    void autoSave();
    void setXAxisMagFieldStrength();
    void setYAxisMagFieldStrength();
    void setZAxisMagFieldStrength();
    void setXAxisMagFieldStrength(double value);
    void setYAxisMagFieldStrength(double value);
    void setZAxisMagFieldStrength(double value);
    void magCal();
    void createCalDataFile();
    void closeCalDataFile();
    void saveCalData();
    QString TCHARToQString(const TCHAR *ptsz);

    ///////////////////////////////////////////////////////

    void on_btnConnect_1_toggled(bool checked);
    void on_btnConnect_2_toggled(bool checked);
    void on_btnConnect_3_toggled(bool checked);
    void on_btnConnect_4_toggled(bool checked);
    void on_btnConnect_5_toggled(bool checked);
    void on_btnPortScan_1_clicked();
    void on_btnPortScan_2_clicked();
    void on_btnPortScan_3_clicked();
    void on_btnPortScan_4_clicked();
    void on_btnSetV_1_clicked();
    void on_btnSetA_1_clicked();
    void on_btnOutputOn_1_clicked();
    void on_btnOutputOff_1_clicked();
    void on_btnPowerSwitching_1_clicked();
    void on_btnSetV_2_clicked();
    void on_btnSetA_2_clicked();
    void on_btnOutputOn_2_clicked();
    void on_btnOutputOff_2_clicked();
    void on_btnPowerSwitching_2_clicked();
    void on_btnSetV_3_clicked();
    void on_btnSetA_3_clicked();
    void on_btnOutputOn_3_clicked();
    void on_btnOutputOff_3_clicked();
    void on_btnPowerSwitching_3_clicked();
    void on_btnStartPlot_toggled(bool checked);
    void on_btnSaveAs_toggled(bool checked);
    void on_btnAutoSave_toggled(bool checked);
    void on_btnSet_uT_clicked();
    void on_btnMAGCALStart_toggled(bool checked);
    void on_btnFindNSet_clicked();

private:
    Ui::MainWindow *ui;

    MK3010P *xAxisPS, *yAxisPS, *zAxisPS;
    PowerSwitchingModule *powerSwitchingModule;
    MAG612 *mag612;
    QCustomPlot *mag612Plot;

    QQueue<QString> *txBuffer_1, *txBuffer_2, *txBuffer_3;
    QQueue<char> *txBuffer_4;
    QQueue<char> *rxBuffer_4;
    QTimer *txTimer_1, *txTimer_2, *txTimer_3, *txTimer_4;
    QTimer *rxTimer_1, *rxTimer_2, *rxTimer_3;
    QTimer *plotTimer;
    QTimer *enqueueStatusCMDTimer_1, *enqueueStatusCMDTimer_2, *enqueueStatusCMDTimer_3; // Enqueue VOUT? & IOUT? CMD to txBuffer
    QTimer *enqueueStatusCMDTimer_4; // Enqueue 0x40 CMD to txBuffer
    QTimer *magCalTimer;

    QFile *saveFile, *autoSaveFile;
    QFile *calDataFile;

public:
    QQueue<int> minQueue, maxQueue;
    QString hourCheck = "";
    int endTime = 0;
    QVector<int> testList_uT;
    int totalStep;
    int currentStep;
    int durationPerStep;
};

#endif // MAINWINDOW_H
