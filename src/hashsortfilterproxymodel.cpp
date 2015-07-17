#include "hashsortfilterproxymodel.h"

HashSortFilterProxyModel::HashSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

void HashSortFilterProxyModel::setFilteredColumns(const QList<int> &index)
{
    m_filteredColumns = index;
    invalidateFilter();
}

void HashSortFilterProxyModel::setShowCheckedRowsOnly(bool showCheckedOnly)
{
    m_showCheckedRowsOnly = showCheckedOnly;
    invalidateFilter();
}

bool HashSortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    bool isAccepted = false;
    QModelIndex index;
    int currentColumn = 0;

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    if (m_showCheckedRowsOnly && sourceModel()->data(index0, Qt::CheckStateRole) == Qt::Unchecked) {
        return false;
    }

    if (filterRegExp().isEmpty()) {
        return true;
    }

    while ((isAccepted == false) && (currentColumn < m_filteredColumns.count())) {
        index = sourceModel()->index(sourceRow, m_filteredColumns[currentColumn], sourceParent);
        isAccepted |= sourceModel()->data(index).toString().contains(filterRegExp());
        currentColumn++;
    }
    return isAccepted;
}

bool HashSortFilterProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
    QString leftData = sourceModel()->data(left).toString();
    QString rightData = sourceModel()->data(right).toString();
    int result = leftData.compare(rightData, Qt::CaseInsensitive);

    if (result == 0) {
        return left.row() < right.row();
    } else if (result < 0) {
        return true;
    } else {
        return false;
    }
}
