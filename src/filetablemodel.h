/*
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#ifndef FILETABLEMODEL_H
#define FILETABLEMODEL_H

#include "hashtypechecker.h"

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include <QString>
#define UNCHECKED_PROGRAMMATICALLY 3

class FileTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileTableModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole);
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    bool readFiles(const QStringList &fileNames);
    void fillHashTypes(const QStringList &listHashTypes);
    Qt::ItemFlags flags(const QModelIndex & index) const;

    enum TableColumns {USER_COL,PASSWORD_COL,HASH_COL,FORMATS_COL,GECOS_COL};

signals:
    void rowUncheckedByUser();

private:
    QVector<QVector<QString> > m_data;
    QVector<Qt::CheckState> m_checkedStates;
    QVector<int> m_rowsWithEmptyPasswords;
    QVector<QString> m_columns;
};

#endif // FILETABLEMODEL_H
