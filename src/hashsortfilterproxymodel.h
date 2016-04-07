/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#ifndef HASHSORTFILTERPROXYMODEL_H
#define HASHSORTFILTERPROXYMODEL_H

#include <QList>
#include <QSortFilterProxyModel>

class HashSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    HashSortFilterProxyModel(QObject *parent = 0);
    void setFilteredColumns(const QList<int> &index,
                            bool              shouldInvalidateFilter = true);
    QList<int> filteredColumns();

public slots:
    void setShowCheckedRowsOnly(bool showCheckedOnly,
                                bool shouldInvalidateFilter = true);
    void setShowCrackedRowsOnly(bool showCrackedOnly,
                                bool shouldInvalidateFilter = true);
    void checkBoxHasChanged();
    void crackingHasChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QList<int> m_filteredColumns;
    bool       m_showCheckedRowsOnly;
    bool       m_showCrackedRowsOnly;
};
#endif // HASHSORTFILTERPROXYMODEL_H
