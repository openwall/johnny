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


    bool readFile(const QString &fileName);
    void fillHashTypes(const QStringList &listHashTypes);

private:
    enum Columns {USER_COL,PASSWORD_COL,HASH_COL,FORMATS_COL,GECOS_COL};
    QVector<QVector<QString> > m_data;

    QVector<QString> m_columns;
};

#endif // FILETABLEMODEL_H
