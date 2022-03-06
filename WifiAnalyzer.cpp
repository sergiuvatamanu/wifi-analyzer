#include "WifiAnalyzer.h"
#include <QListView>
#include <QMessageBox>
#include <QDebug>
#include <QtCharts>

using namespace QtCharts;

WifiAnalyzer::WifiAnalyzer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initializeWlanDetails();
    initializeChart();
    ui.listView->setModel(&model);
}

void WifiAnalyzer::initializeWlanDetails() {
    int iRet = 0;

    WCHAR GuidString[39] = { 0 }; // interface GUID string

    unsigned int i, j, k;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        QMessageBox::critical(this, "Error", "WlanOpenHandle failed\n" + dwResult);
        return;
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        QMessageBox::critical(this, "Error", "WlanEnumInterfacesFailedWith\n" + dwResult);
        return;
    }

    int idx = 0; // Assume first available wlan interface
    pIfInfo = (WLAN_INTERFACE_INFO*) &pIfList->InterfaceInfo[idx];

    iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
        sizeof(GuidString) / sizeof(*GuidString));

    if (iRet == 0) {
        QMessageBox::critical(this, "Error", "StringFromGUID2 failed\n");
    }
    else {
        ui.labelGUID->setText(QString().asprintf("GUID: %ls", GuidString));
    }

    ui.labelDesc->setText(QString().sprintf("Description : %ls", pIfInfo->strInterfaceDescription));

    if (pIfInfo->isState == wlan_interface_state_connected) {
        ui.labelState->setText("State: Connected");
    }
    else {
        ui.labelState->setText("Unknown");
    }

}

void WifiAnalyzer::initializeChart() {
    QSplineSeries* series = new QSplineSeries();
    this->chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();

    QValueAxis* axisX = static_cast<QValueAxis*>(chart->axisX());
    QValueAxis* axisY = static_cast<QValueAxis*>(chart->axisY());
    axisX->setTitleText("Channel");
    axisY->setTitleText("RSSI(dbm)");

    axisX->setRange(-1, 15);
    axisX->setTickCount(17);
    axisX->setLabelFormat("%d");

    axisY->setRange(-100, -10);
    axisY->setTickCount(10);
    chart->setTitle("Channel distribution");

    this->chartView = new QChartView(chart);
    ui.horizontalLayout_2->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void WifiAnalyzer::performScan() {
    /* used for available networks*/
    PWLAN_AVAILABLE_NETWORK_LIST pNetList = NULL;
    PWLAN_AVAILABLE_NETWORK pNetEntry = NULL;

    /* actual bss*/
    PWLAN_BSS_LIST pBssList = NULL;
    PWLAN_BSS_ENTRY pBssEntry = NULL;

    WlanScan(hClient, &pIfInfo->InterfaceGuid, NULL, NULL, NULL);

    dwResult = WlanGetAvailableNetworkList(hClient,
        &pIfInfo->InterfaceGuid,
        0,
        NULL,
        &pNetList);

    if (dwResult != ERROR_SUCCESS) {
        qDebug() << "broken";
        wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n",dwResult);
        dwRetVal = 1;
    }

    dwResult = WlanGetNetworkBssList(hClient,
        &pIfInfo->InterfaceGuid,
        NULL,
        dot11_BSS_type_any,
        NULL,
        NULL,
        &pBssList);

    if (dwResult != ERROR_SUCCESS) {
        qDebug() << "broken\n";
        wprintf(L"WlanGetNetworkBssList failed with error: %u\n", dwResult);
        dwRetVal = 1;
    }

    qDebug() << "reached this well";
    model.updateModel(pNetList, pBssList);
}

void WifiAnalyzer::plotData()
{
    this->chart->removeAllSeries();

    const auto map = model.getSsidChannelMap();

    for (auto pair : map) {
        for (int i = 0 ; i < pair.second.size(); i++) {

            auto ssid = QString::fromStdString(pair.second[i].first);
            auto fq = pair.second[i].second;

            QLineSeries* series = new QLineSeries();
            QLineSeries* series_name = new QLineSeries(); // ui hack

            series->append(pair.first-2, -100);
            series->append(pair.first-1, fq);
            series_name->append(pair.first, fq + 2);
            series->append(pair.first+1, fq);
            series->append(pair.first+2, -100);
            this->chart->addSeries(series);
            
            series_name->setPointLabelsVisible(true);
            series_name->setPointLabelsClipping(false);
            series_name->setPointLabelsColor(series->color());
            series_name->setPointLabelsFormat(ssid);

            this->chart->addSeries(series_name);
        }
    }

    chart->createDefaultAxes();
    QValueAxis* axisX = static_cast<QValueAxis*>(chart->axisX());
    QValueAxis* axisY = static_cast<QValueAxis*>(chart->axisY());
    axisX->setTitleText("Channel");
    axisY->setTitleText("RSSI(dbm)");

    axisX->setRange(-1, 15);
    axisX->setTickCount(17);
    axisX->setLabelFormat("%d");

    axisY->setRange(-100, -10);
    axisY->setTickCount(10);
}

void WifiAnalyzer::on_scanButton_clicked() {
    performScan();
    plotData();
}
