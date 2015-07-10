#include "hashsortfilterproxymodel.h"

HashSortFilterProxyModel::HashSortFilterProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{

}

bool HashSortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    // TODO : use the m_filteredColumns attribute instead of hardcoding column 0 1 2
    bool isAccepted = false;
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);

    return (sourceModel()->data(index0).toString().contains(filterRegExp())
            || sourceModel()->data(index1).toString().contains(filterRegExp()))
            || sourceModel()->data(index2).toString().contains(filterRegExp());
}
