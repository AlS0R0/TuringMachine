#include "conditiontable.h"
#include <QAbstractTableModel>

ConditionTable::ConditionTable(int rows, const QString &str1, const QString &str2, QObject* parent)
    : QAbstractTableModel(parent), rowCount_(rows), colCount_(str1.size() + str2.size() + 1)
{
    QString all = str1 + str2;
    for (QChar ch : all) columnHeaders.append(ch);
    columnHeaders.append('^');  // пустой символ
    colCount_ = columnHeaders.size();
}

int ConditionTable::rowCount(const QModelIndex &parent) const
{
    return rowCount_;
}

int ConditionTable::columnCount(const QModelIndex &parent) const
{
    return colCount_;
}

QVariant ConditionTable::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return cellData_.value(index, QVariant());
    }

    return QVariant();
}

bool ConditionTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        cellData_.insert(index, value);
        emit dataChanged(index, index);
    }

    return false;
}

Qt::ItemFlags ConditionTable::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

/*
bool ConditionTable::addRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount_) return false;
    beginInsertRows(parent, row, row + count - 1);
    rowCount_ += count;
    // Здесь можно сдвинуть данные, но для пустых строк необязательно
    endInsertRows();
    return true;
}
*/

bool ConditionTable::addEmptyRow()
{
    beginInsertRows(QModelIndex(), rowCount_, rowCount_);
    rowCount_ += 1;
    endInsertRows();

    return true;
}

bool ConditionTable::removeRow()
{
    if (rowCount_ == 1) return false;

    int lastRow = rowCount_ - 1;
    beginRemoveRows(QModelIndex(), lastRow, lastRow);

    for (int col = 0; col < colCount_; ++col) {
        cellData_.remove(index(lastRow, col));
    }

    rowCount_--;

    endRemoveRows();

    return true;
}

QVariant ConditionTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return QString("q%1").arg(section);
    } else {
        if (section >= 0 && section < columnHeaders.size())
            return QString(columnHeaders[section]);
    }
    return QVariant();
}