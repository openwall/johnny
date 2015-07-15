#include "hashsortfilterproxymodel.h"

HashSortFilterProxyModel::HashSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

void HashSortFilterProxyModel::setFilteredColumns(const QList<int> &index)
{
    m_filteredColumns = index;
}

bool HashSortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    bool isAccepted = false;
    QModelIndex index;
    int currentColumn = 0;
    if (filterRegExp().isEmpty()) {
        return true;
    }
    while ((isAccepted == false) && (currentColumn < m_filteredColumns.count())) {
        index = sourceModel()->index(sourceRow, currentColumn, sourceParent);
        isAccepted |= sourceModel()->data(index).toString().contains(filterRegExp());
        currentColumn++;
    }
    return isAccepted;
}
