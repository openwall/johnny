/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "hashsortfilterproxymodel.h"
#include "passwordfilemodel.h"

#include <QRegExp>

HashSortFilterProxyModel::HashSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void HashSortFilterProxyModel::setFilteredColumns(const QList<int> &index,
                                                  bool shouldInvalidateFilter)
{
    m_filteredColumns = index;
    if(shouldInvalidateFilter)
    {
        invalidateFilter();
    }
}

void HashSortFilterProxyModel::setShowCheckedRowsOnly(bool showCheckedOnly,
                                                      bool shouldInvalidateFilter)
{
    m_showCheckedRowsOnly = showCheckedOnly;
    if(shouldInvalidateFilter)
    {
        invalidateFilter();
    }
}

void HashSortFilterProxyModel::setShowCrackedRowsOnly(bool showCrackedOnly,
                                                      bool shouldInvalidateFilter)
{
    m_showCrackedRowsOnly = showCrackedOnly;
    if(shouldInvalidateFilter)
    {
        invalidateFilter();
    }
}

void HashSortFilterProxyModel::checkBoxHasChanged()
{
    if(m_showCheckedRowsOnly)
    {
        invalidateFilter();
    }
}

void HashSortFilterProxyModel::crackingHasChanged()
{
    if(m_showCrackedRowsOnly)
    {
        invalidateFilter();
    }
}

bool HashSortFilterProxyModel::filterAcceptsRow(int                sourceRow,
                                                const QModelIndex &sourceParent) const
{
    bool        isAccepted = false;
    QModelIndex index;
    int         currentColumn = 0;

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    if(m_showCheckedRowsOnly &&
       (sourceModel()->data(index0, Qt::CheckStateRole) == Qt::Unchecked))
    {
        return false;
    }

    QModelIndex indexPassword =
        sourceModel()->index(sourceRow, PasswordFileModel::PASSWORD_COL,
                             sourceParent);
    if(m_showCrackedRowsOnly &&
       sourceModel()->data(indexPassword).toString().isEmpty())
    {
        return false;
    }

    if(filterRegExp().isEmpty())
    {
        return true;
    }

    while((isAccepted == false) && (currentColumn < m_filteredColumns.count()))
    {
        index = sourceModel()->index(sourceRow, m_filteredColumns[currentColumn],
                                     sourceParent);
        if(sourceModel()->data(index).toString().contains(filterRegExp()))
        {
            isAccepted = true;
        }
        currentColumn++;
    }
    return isAccepted;
}

bool HashSortFilterProxyModel::lessThan(const QModelIndex &left,
                                        const QModelIndex &right) const
{
    QString leftData  = sourceModel()->data(left).toString();
    QString rightData = sourceModel()->data(right).toString();
    int     result    = leftData.compare(rightData, Qt::CaseInsensitive);

    if(result == 0)
    {
        return left.row() < right.row();
    }
    else if(result < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
