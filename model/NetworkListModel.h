#pragma once
#include <QAbstractListModel>
#include <QIcon>

#include "NetworkListItem.h"

class NetworkListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    NetworkListModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void updateModel(std::vector<NetworkListItem> netList);

    const std::vector<NetworkListItem> getNetList() {
        return netList;
    }

private:
    QIcon noIcon = QIcon("resources/no_signal.png");
    QIcon lowIcon = QIcon("resources/low_signal.png");
    QIcon medIcon = QIcon("resources/med_signal.png");
    QIcon highIcon = QIcon("resources/high_signal.png");

    std::vector<NetworkListItem> netList;
};
