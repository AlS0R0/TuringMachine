#include "conditiontable.h"
#include <QAbstractTableModel>

// ConditionTable::ConditionTable(int rows, const QString &str1, const QString &str2, QObject* parent)
//     : QAbstractTableModel(parent), rowCount_(rows), colCount_(str1.size() + str2.size() + 1)
// {
//     QString all = str1 + str2;
//     for (QChar ch : all) columnHeaders.append(ch);
//     columnHeaders.append('^');  // пустой символ
//     colCount_ = columnHeaders.size();
// }

ConditionTable::ConditionTable(int rows, const QVector<QChar> &vec, QObject* parent)
    : QAbstractTableModel(parent), rowCount_(rows), colCount_(vec.size() + 1), columnHeaders(vec)
{
    columnHeaders.append('^');  // пустой символ
}

int ConditionTable::rowCount(const QModelIndex &parent) const
{
    return rowCount_;
}

int ConditionTable::columnCount(const QModelIndex &parent) const
{
    return colCount_;
}

QVector<QChar> ConditionTable::getColumnHeaders() const
{
    return columnHeaders;
}

QVector<QChar> ConditionTable::getRowHeaders() const {
    QVector<QChar> headers;

    int rowCount = this->rowCount_;

    for (int i = 0; i < rowCount; ++i) {
        QVariant data = this->headerData(i, Qt::Vertical, Qt::DisplayRole);
        headers.push_back(data.toChar());
    }

    return headers;
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

void ConditionTable::updateAlphabet(QVector<QChar> newHeaders)
{
    newHeaders.append('^');
    bool allOldExist = true;
    //qDebug() << newHeaders << '\n';

    for (QChar oldCh : columnHeaders) {
        if (!newHeaders.contains(oldCh)) {
            allOldExist = false;
            break;
        }
    }

    if (!allOldExist) {
        //qDebug() << "Out" << '\n';

        beginResetModel();
        cellData_.clear();
        columnHeaders = newHeaders;
        colCount_ = columnHeaders.size();
        endResetModel();
        return;
    }

    beginResetModel();
    int flag = 1;

    for (int i = 0; i < newHeaders.size(); ++i) {
        for (int j = 0; j < columnHeaders.size(); ++j) {
            if(newHeaders[i] == columnHeaders[j]) {
                flag = 0;
                break;
            }
        }

        if (flag == 1) {
            columnHeaders.append(newHeaders[i]);
            //qDebug() << newHeaders[i];
        }

        flag = 1;
    }

    //qDebug() << '\n';

    colCount_ = columnHeaders.size();
    endResetModel();
}