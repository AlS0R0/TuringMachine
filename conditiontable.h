#ifndef CONDITIONTABLE_H
#define CONDITIONTABLE_H

#include <QAbstractTableModel>
#include <QObject>

class ConditionTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    // ConditionTable(QObject* parent = nullptr);
    // ConditionTable(int rows, int cols, QObject* parent = nullptr);
    ConditionTable(int rows, const QString &str1, const QString &str2, QObject* parent = nullptr);
private:
    int rowCount_ = 0;
    int colCount_ = 0;
protected:
    QHash<QModelIndex, QVariant> cellData_;
    QVector<QChar> columnHeaders;
    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVector<QChar> getColumnHeaders() const;
    QVector<QChar> getRowHeaders() const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    // bool addRows(int row, int count, const QModelIndex &parent);
    bool addEmptyRow();
    bool removeRow();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void updateAlphabet(const QString &str1, const QString &str2, bool forceClear);
};

#endif // CONDITIONTABLE_H
