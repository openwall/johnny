/*
 * Copyright © 2011 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#include "filetablemodel.h"

#include <QFile>

// NOTE: Model could be resizable but it is easier to not
//       implement it. Instead it is possible to load full
//       file into memory and count rows. However it needs to
//       place file loading into model. Though it seems to be
//       good.

FileTableModel::FileTableModel(const QString &fileName, QObject *parent)
    : QAbstractTableModel(parent)
{
    // We make it as object field because we could not make class field.
    m_columns << tr("User") << tr("Hash");
    // We use vector of vectors to store data. It should work faster
    // than with lists. But it is easier to fill table using lists as
    // of they could change their size easily. So we build vector of
    // lists and then convert it to vector of vectors.
    // TODO: It seems to be not optimal structure. May pointers be faster?
    //       How does it work being assigned? Does it make full copy?
    // TODO: Is string an appropriate type for fields? May bytes be
    //       better?
    QVector<QStringList> data(columnCount());
    // We read and parse the file.
    // We create and fill our internal model representation.
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        // TODO: Notice user that file could not be opened.
        // TODO: As of we in constructor we should raise an exception.
        return;
    while (!file.atEnd()) {
        QString line = file.readLine();
        // To parse the line we split it by colon and take first
        // two fields.
        // TODO: We have more than two fields. Parse them too. 
        QStringList fields = line.split(':');
        // TODO: Is it safe to show untrusted input in gui?
        int column;
        for (column = 0; column < columnCount() && column < fields.size(); column++)
            // NOTE: When we want we change lists we use [] as of .at()
            //       gives us only const.
            data[column].append(fields.at(column));
        // Line in file could contain fewer amount of fields than we
        // want. So we fill our table with empty values.
        // We continue column traversing.
        // NOTE: It is not possible to skip such lines because later we
        //       will have more fields and not all should be presented.
        for (; column < columnCount(); column++)
            data[column].append("");
    }
    // We convert our lists into vectors to store data.
    for (int column = 0; column < columnCount(); column++) {
        m_data << data.at(column).toVector();
    }
    // TODO: Should we emit a signal to notice all that we changed our
    //       state?
}

int FileTableModel::rowCount(const QModelIndex &/* parent */) const
{
    // For this size we use size of the first column.
    // NOTE: Sizes of all columns should be equal.
    return m_data.at(0).size();
}

int FileTableModel::columnCount(const QModelIndex &/* parent */) const
{
    return m_columns.size();
}

QVariant FileTableModel::data(const QModelIndex &index,
                              int role) const
{
    // We validate arguments.
    // TODO: Check bounds.
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    return m_data.at(index.column()).at(index.row());
}

bool FileTableModel::setData(const QModelIndex &index,
                             const QVariant &value,
                             int role)
{
    // We validate arguments.
    // TODO: Check bounds.
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    // We replace data in our table.
    m_data[index.column()].replace(index.row(), value.toString());
    // We notice all that we changed our state.
    emit dataChanged(index, index);
    return true;
}

QVariant FileTableModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const
{
    // We only display header data. It is readonly.
    if (role != Qt::DisplayRole)
        return QVariant();
    // For vertical header we return string numbers.
    if (orientation == Qt::Vertical)
        return QString("%1").arg(section + 1);
    // For horizontal header we return names from fields array.
    if (orientation == Qt::Horizontal)
        return m_columns[section];
    // TODO: Could we rich this place? Should we do something special
    //       here?
    return QVariant();
}

