#ifndef HASHSORTFILTERPROXYMODEL_H
#define HASHSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QList>

class HashSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    HashSortFilterProxyModel(QObject *parent = 0);
    void setFilteredColumns(const QList<int> &index, bool shouldInvalidateFilter = true);
    QList<int> filteredColumns();

public slots:
    void setShowCheckedRowsOnly(bool showCheckedOnly, bool shouldInvalidateFilter = true);
    void setShowCrackedRowsOnly(bool showCrackedOnly, bool shouldInvalidateFilter = true);
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
