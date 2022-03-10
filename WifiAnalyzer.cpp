#include "WifiAnalyzer.h"
#include <QListView>
#include <QMessageBox>
#include <QDebug>
#include <QtCharts>
#include "NetworkListItem.h"

using namespace QtCharts;

WifiAnalyzer::WifiAnalyzer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initializeWlanDetails();
    initializeChart();
    ui.listView->setModel(&model);
}

std::string getMacStdString(unsigned char* mac) {
    char mac_cstr[64];
    snprintf(mac_cstr, 64, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(mac_cstr);
}

std::string getAuthoAlgorithmString(DOT11_AUTH_ALGORITHM authAlgorithm) {
    std::string authString;
    switch (authAlgorithm) {
    case DOT11_AUTH_ALGO_80211_OPEN:
        authString.assign("802.11 Open");
        break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
        authString.assign("802.11 Shared");
        break;
    case DOT11_AUTH_ALGO_WPA:
        authString.assign("WPA");
        break;
    case DOT11_AUTH_ALGO_WPA_PSK:
        authString.assign("WPA-PSK");
        break;
    case DOT11_AUTH_ALGO_WPA_NONE:
        authString.assign("WPA-None");
        break;
    case DOT11_AUTH_ALGO_RSNA:
        authString.assign("RSNA");
        break;
    case DOT11_AUTH_ALGO_RSNA_PSK:
        authString.assign("RSNA with PSK");
        break;
    default:
        authString.assign("Other");
        break;
    }
    return authString;
}

unsigned int getChForFrequency(unsigned long kHz) {
    unsigned int chFreq = 2412; //first channel centered here
    int ch = 1;
    for (ch = 1; ch <= 13 && chFreq != kHz / 1000; ch++) {
        chFreq += 5; // center step 5 mhz
    }
    return ch;
}

std::string getCipherAlgorithmString(DOT11_CIPHER_ALGORITHM cipherAlgorithm) {
    std::string ciphString;
    switch (cipherAlgorithm) {
    case DOT11_CIPHER_ALGO_NONE:
        ciphString.assign("None");
        break;
    case DOT11_CIPHER_ALGO_WEP40:
        ciphString.assign("WEP-40");
        break;
    case DOT11_CIPHER_ALGO_TKIP:
        ciphString.assign("TKIP");
        break;
    case DOT11_CIPHER_ALGO_CCMP:
        ciphString.assign("CCMP");
        break;
    case DOT11_CIPHER_ALGO_WEP104:
        ciphString.assign("WEP-104");
        break;
    case DOT11_CIPHER_ALGO_WEP:
        ciphString.assign("WEP");
        break;
    default:
        ciphString.assign("Other");
        break;
    }
    return ciphString;
}

void WifiAnalyzer::initializeWlanDetails() {
    int iRet = 0;

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

    std::vector<NetworkListItem> netList;

    PWLAN_AVAILABLE_NETWORK_LIST pNetList = NULL;
    PWLAN_AVAILABLE_NETWORK pNetEntry = NULL;

    PWLAN_BSS_LIST pBssList = NULL;
    PWLAN_BSS_ENTRY pBssEntry = NULL;

    WlanScan(hClient, &pIfInfo->InterfaceGuid, NULL, NULL, NULL);

    dwResult = WlanGetAvailableNetworkList(hClient,
        &pIfInfo->InterfaceGuid,
        0,
        NULL,
        &pNetList);

    if (dwResult != ERROR_SUCCESS) {
        qDebug() << "WlanGetAvailableNetworkList failed with error: %u\n",dwResult;
    }

    for (int i = 1; i < pNetList->dwNumberOfItems; i++) {
        auto pNetEntry = pNetList->Network[i];

        dwResult = WlanGetNetworkBssList(hClient,
            &pIfInfo->InterfaceGuid,
            &pNetEntry.dot11Ssid,
            dot11_BSS_type_infrastructure,
            pNetEntry.bSecurityEnabled,
            NULL,
            &pBssList);

        if (dwResult != ERROR_SUCCESS) {
            qDebug() << "WlanGetNetworkBssList failed with error: %u\n", dwResult;
        }
        qDebug() << pBssList->dwNumberOfItems;

        for (int j = 0; j < pBssList->dwNumberOfItems; j++) {
            auto pBssEntry = pBssList->wlanBssEntries[j];

            auto netItem = NetworkListItem();

            netItem.ssid = std::string((char*) pBssEntry.dot11Ssid.ucSSID, pBssEntry.dot11Ssid.uSSIDLength);
            netItem.mac = getMacStdString(pBssEntry.dot11Bssid);
            netItem.rssi = pBssEntry.lRssi;
            netItem.centerFrequency = pBssEntry.ulChCenterFrequency;
            netItem.channel = getChForFrequency(netItem.centerFrequency);
            netItem.auth = getAuthoAlgorithmString(pNetEntry.dot11DefaultAuthAlgorithm);
            netItem.cipher = getCipherAlgorithmString(pNetEntry.dot11DefaultCipherAlgorithm);
            netList.push_back(netItem);
        }
    }
    qDebug() << "works";
    std::sort(netList.begin(), netList.end(), [](auto a, auto b) {return a.rssi > b.rssi; });
    model.updateModel(netList);
}

void WifiAnalyzer::plotData()
{
    this->chart->removeAllSeries();
    
    auto netList = model.getNetList();

    for (auto netItem : netList) {
        QLineSeries* series = new QLineSeries();
        QLineSeries* series_name = new QLineSeries(); // ui hack

        auto ch = netItem.channel;
        auto rssi = netItem.rssi;
        auto ssid = netItem.ssid;

        series->append(ch-2, -100);
        series->append(ch-1, rssi);
        series_name->append(ch, rssi + 2);
        series->append(ch+1, rssi);
        series->append(ch+2, -100);
        this->chart->addSeries(series);
            
        series_name->setPointLabelsVisible(true);
        series_name->setPointLabelsClipping(false);
        series_name->setPointLabelsColor(series->color());

        series_name->setPointLabelsFormat(netItem.ssid.c_str());

        this->chart->addSeries(series_name);
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
