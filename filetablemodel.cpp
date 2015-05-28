/*
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#include "filetablemodel.h"

#include <QFile>

#define FIELD_SEPARATOR ':'

// NOTE: Model could be resizable but it is easier to not
//       implement it. Instead it is possible to load full
//       file into memory and count rows. However it needs to
//       place file loading into model. Though it seems to be
//       good.

FileTableModel::FileTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // We make it as object field because we could not make class field.
    m_columns << tr("User") << tr("Password") << tr("Hash") << tr("GECOS");
}

bool FileTableModel::readFile(const QStringList &fileNames)
{
    m_data.clear();

    // We use vector of vectors to store data. It should work faster
    // than with lists. But it is easier to fill table using lists as
    // of they could change their size easily. So we build vector of
    // lists and then convert it to vector of vectors.

    QVector<QStringList> data(columnCount());
    for(int fileCount = 0; fileCount < fileNames.size(); fileCount++)
    {
        // We read and parse the file.
        // We create and fill our internal model representation.
        QFile file(fileNames[fileCount]);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line.remove(QRegExp("\\r?\\n"));
            QStringList fields = line.split(FIELD_SEPARATOR);
            int column = 0;
            // NOTE: When we want we change lists we use [] as of .at()
            //       gives us only const.
            if (fields.size() == 1) {
                // Lonely hash
                data[column++].append("?");
                data[column++].append("");
                data[column++].append(fields.at(0));
            } else if (fields.size() >= 3 && fields.at(2).indexOf(QRegExp("^[0-9a-fA-F]{32}$")) == 0) {
                // Pwdump format
                data[column++].append(fields.at(0));
                data[column++].append("");
                data[column++].append(fields.at(2));
                fields.removeAt(2);
                fields.removeAt(0);
                data[column++].append(fields.join(FIELD_SEPARATOR));
            } else {
                // user:hash:other
                data[column++].append(fields.at(0));
                data[column++].append("");
                data[column++].append(fields.at(1));
                fields.removeAt(1);
                fields.removeAt(0);
                data[column++].append(fields.join(FIELD_SEPARATOR));
            }
            for (; column < columnCount(); column++)
                data[column].append("");
        }
    }
    // We convert our lists into vectors to store data.
    for (int column = 0; column < columnCount(); column++) {
        m_data << data.at(column).toVector();
    }
    return true;
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
    if (!index.isValid() || role != Qt::DisplayRole ||
            index.column() >= columnCount() || index.row() >= rowCount())
        return QVariant();
    return m_data.at(index.column()).at(index.row());
}

bool FileTableModel::setData(const QModelIndex &index,
                             const QVariant &value,
                             int role)
{
    // We validate arguments.
    if (!index.isValid() || role != Qt::EditRole ||
            index.column() >= columnCount() || index.row() >= rowCount())
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

    return QVariant();
}

