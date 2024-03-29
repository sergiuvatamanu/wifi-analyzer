#pragma once
#include <QtWidgets/QWidget>
#include <QtCharts>

#include <wlanapi.h> // Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")
#include "ui_wifiAnalyzer.h"
#include "model/NetworkListModel.h"

class WifiAnalyzer : public QWidget
{
    Q_OBJECT

public:
    WifiAnalyzer(QWidget *parent = Q_NULLPTR);

private:
    // singletonish treatment
    HANDLE hClient = NULL; // the interface handler
    DWORD dwMaxClient = 2; // 2 for NT
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
