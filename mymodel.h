#pragma once
#include <QAbstractListModel>
#include <QIcon>

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <iphlpapi.h>

#include <stdio.h>
#include <stdlib.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

class NetworkListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    NetworkListModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void updateModel(PWLAN_AVAILABLE_NETWORK_LIST pNetList, PWLAN_BSS_LIST pBssList);

    std::map <int, std::vector<std::pair<std::string, long>>> mymap;
    
    void mapSsidsToChannels();
    std::map <int, std::vector<std::pair<std::string, long>>> getSsidChannelMap();

private:
    QIcon noIcon = QIcon("no_signal.png"); // initialize them here so you don't create them every time
    QIcon lowIcon = QIcon("low_signal.png");
    QIcon medIcon = QIcon("med_signal.png");
    QIcon highIcon = QIcon("high_signal.png");

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    PWLAN_AVAILABLE_NETWORK_LIST pNetList = NULL;
    PWLAN_BSS_LIST pBssList = NULL;

    WLAN_BSS_ENTRY getBssEntry(int idx) {

    }

    WLAN_AVAILABLE_NETWORK getAvailableNetworkEntry(int idx) {

    }


};