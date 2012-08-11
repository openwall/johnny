/*
 * Copyright © 2011,2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>.  See LICENSE.
 */

#include "filetablemodel.h"

#include <QFile>

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
    // TODO: we leave object in inconsistent state.
}

// TODO: more codes? Other way to describe reason?
// TODO: this should not be called twice. Either check that it was
//       called or begin cleaning m_data.
bool FileTableModel::readFile(const QString &fileName)
{
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
    // TODO: Restore could call us with empty name. We crash.
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    while (!file.atEnd()) {
        QString line = file.readLine();
        // TODO: right? Should not we keep \r in middle of line?
        line.remove(QRegExp("\\r?\\n"));
        // TODO: Parse gecos well.
        // TODO: Make customizable separator. It should be an option.
        QStringList fields = line.split(':');
        // TODO: Is it safe to show untrusted input in gui?
        int column = 0;
        // NOTE: When we want we change lists we use [] as of .at()
        //       gives us only const.
        if (fields.size() == 1) {
            // Lonely hash
            // TODO: Special mark to show that ? is not from file. Color? How?
            data[column++].append("?");
            data[column++].append("");
            data[column++].append(fields.at(0));
        } else if (fields.size() >= 3 && fields.at(2).indexOf(QRegExp("^[0-9a-fA-F]{32}$")) == 0) {
            // Pwdump format
            data[column++].append(fields.at(0));
            data[column++].append("");
            data[column++].append(fields.at(2));
            // TODO: It is not good to pack it so. Parse gecos well.
            // TODO: split and join back seem slower than optimal.
            fields.removeAt(2);
            fields.removeAt(0);
            data[column++].append(fields.join(":"));
        } else {
            // user:hash:other
            data[column++].append(fields.at(0));
            data[column++].append("");
            data[column++].append(fields.at(1));
            fields.removeAt(1);
            fields.removeAt(0);
            data[column++].append(fields.join(":"));
        }
        for (; column < columnCount(); column++)
            data[column].append("");
    }
    // We convert our lists into vectors to store data.
    for (int column = 0; column < columnCount(); column++) {
        m_data << data.at(column).toVector();
    }
    // TODO: Should we emit a signal to notice all that we changed our
    //       state?

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

