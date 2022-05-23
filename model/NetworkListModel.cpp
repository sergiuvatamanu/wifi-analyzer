#pragma once
#include "NetworkListModel.h"
#include <QDebug>

NetworkListModel::NetworkListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int NetworkListModel::rowCount(const QModelIndex& /*parent*/) const
{
    return this->netList.size();
}

QVariant NetworkListModel::data(const QModelIndex& index, int role) const
{
    NetworkListItem netItem = this->netList[index.row()];

    if (role == Qt::DisplayRole)
        return QString::fromStdString(netItem.ssid)
        + "(" + QString::fromStdString(netItem.mac) + ")\n"
        + QString::number(netItem.rssi) + "dBm\nCH "
        + QString::number(netItem.channel) + "("+ QString::number(netItem.centerFrequency/1000)+ "MHz)\n"
        + QString::fromStdString(netItem.auth + " | " + netItem.cipher);
    
    if (role == Qt::DecorationRole) {
        if (netItem.rssi >= -60) {
            return highIcon;
        } else if(netItem.rssi >= -70) {
            return medIcon;
        } else if (netItem.rssi >= -80) {
            return lowIcon;
        } else {
            return noIcon;
        }
    }
    return QVariant();
}

void NetworkListModel::updateModel(std::vector<NetworkListItem> netlist) {
    beginResetModel();
    this->netList.clear();
    this->netList = netlist;
    endResetModel();
}
