#pragma once
#include "NetworkListModel.h"
#include<stdlib.h>
#include <QDebug>
#include <regex>

NetworkListModel::NetworkListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int NetworkListModel::rowCount(const QModelIndex& /*parent*/) const
{
    if(pBssList != NULL)
        return pBssList->dwNumberOfItems;
    return 0;
}

unsigned int getChForFrequency(unsigned long kHz) {
    unsigned int chFreq = 2412; //first channel centered here
    int ch = 1;
    for (ch = 1; ch <= 13 && chFreq != kHz/1000; ch++) {
        chFreq += 5; // center step 5 mhz
    }
    return ch;
}

std::string getSecurityString(WLAN_AVAILABLE_NETWORK* pBssEntry) {
    std::string auth;
    switch (pBssEntry->dot11DefaultAuthAlgorithm) {
    case DOT11_AUTH_ALGO_80211_OPEN:
        auth.assign("802.11 Open");
        break;
    case DOT11_AUTH_ALGO_80211_SHARED_KEY:
        auth.assign("802.11 Shared");
        break;
    case DOT11_AUTH_ALGO_WPA:
        auth.assign("WPA");
        break;
    case DOT11_AUTH_ALGO_WPA_PSK:
        auth.assign("WPA-PSK");
        break;
    case DOT11_AUTH_ALGO_WPA_NONE:
        auth.assign("WPA-None");
        break;
    case DOT11_AUTH_ALGO_RSNA:
        auth.assign("RSNA");
        break;
    case DOT11_AUTH_ALGO_RSNA_PSK:
        auth.assign("RSNA with PSK");
        break;
    default:
        auth.assign("Other");
        break;
    }
    std::string ciph;
    switch (pBssEntry->dot11DefaultCipherAlgorithm) {
    case DOT11_CIPHER_ALGO_NONE:
        ciph.assign("None");
        break;
    case DOT11_CIPHER_ALGO_WEP40:
        ciph.assign("WEP-40");
        break;
    case DOT11_CIPHER_ALGO_TKIP:
        ciph.assign("TKIP");
        break;
    case DOT11_CIPHER_ALGO_CCMP:
        ciph.assign("CCMP");
        break;
    case DOT11_CIPHER_ALGO_WEP104:
        ciph.assign("WEP-104");
        break;
    case DOT11_CIPHER_ALGO_WEP:
        ciph.assign("WEP");
        break;
    default:
        ciph.assign("Other");
        break;
    }
    return "Security: " + auth + " | " + ciph;
}
QVariant NetworkListModel::data(const QModelIndex& index, int role) const
{
    WLAN_BSS_ENTRY pBssEntry = pBssList->wlanBssEntries[index.row()];
    WLAN_AVAILABLE_NETWORK pAvNet = pNetList->Network[index.row()];
    pAvNet.dot11DefaultAuthAlgorithm;
    pAvNet.dot11DefaultCipherAlgorithm;

    QString ssid = QString::fromStdString(std::string((char*)pBssEntry.dot11Ssid.ucSSID, pBssEntry.dot11Ssid.uSSIDLength));
    
    UCHAR *mac;
    mac = pBssEntry.dot11Bssid;

    if (role == Qt::DisplayRole)
        return ssid
        + QString().asprintf("(%x:%x:%x:%x:%x:%x)\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5])
        + QString().asprintf(" %lddBm\n", pBssEntry.lRssi)
        + QString().asprintf("CH %u (%uMHz)\n", getChForFrequency(pBssEntry.ulChCenterFrequency), pBssEntry.ulChCenterFrequency / 1000)
        + QString::fromStdString(getSecurityString(&pAvNet));
    
    if (role == Qt::DecorationRole) {
        if (pBssEntry.lRssi >= -60) {
            return highIcon;
        } else if(pBssEntry.lRssi >= -70) {
            return medIcon;
        } else if (pBssEntry.lRssi >= -80) {
            return lowIcon;
        } else {
            return noIcon;
        }

    }
    return QVariant();
}

void NetworkListModel::updateModel(PWLAN_AVAILABLE_NETWORK_LIST pNetList, PWLAN_BSS_LIST pBssList) {
    beginResetModel();
    this->pBssList = pBssList;
    this->pNetList = pNetList;
    mapSsidsToChannels();
    endResetModel();
}

void NetworkListModel::mapSsidsToChannels()
{
    this->mymap.clear();
    for (int j = 0; j < pBssList->dwNumberOfItems; j++) {
        
        auto pBssEntry = pBssList->wlanBssEntries[j];
        std::string ssid = std::string((char*)pBssEntry.dot11Ssid.ucSSID, pBssEntry.dot11Ssid.uSSIDLength);
        auto fq = pBssEntry.lRssi;
        this->mymap[getChForFrequency(pBssEntry.ulChCenterFrequency)].push_back(std::make_pair(ssid, fq));
    }
    
}

std::map<int, std::vector<std::pair<std::string, long>>> NetworkListModel::getSsidChannelMap()
{
    return this->mymap;
}
