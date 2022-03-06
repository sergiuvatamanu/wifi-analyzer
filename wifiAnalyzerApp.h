#pragma once

#include <QtWidgets/QWidget>
#include <QtCharts>

#include "ui_wifiAnalyzerApp.h"
#include "mymodel.h"


class wifiAnalyzerApp : public QWidget
{
    Q_OBJECT

public:
    wifiAnalyzerApp(QWidget *parent = Q_NULLPTR);

private:
    // singletonish treatment
    HANDLE hClient = NULL; // the interface handler
    DWORD dwMaxClient = 2; // 1 for windows xp apparently
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    Ui::wifiAnalyzerAppClass ui;
    NetworkListModel model;
    
    // Programmatic UI elements
    QChartView* chartView;
    QChart* chart;

    void initializeWlanDetails();
    void initializeChart();
    void performScan();
    void plotData();

    void cleanup() {
        if (hClient != NULL) {
            WlanCloseHandle(hClient, NULL);
            hClient = NULL;
        }
        if (pIfList != NULL) {
            WlanFreeMemory(pIfList);
            pIfList = NULL;
        }
    };

private slots:
    void on_scanButton_clicked();
};
