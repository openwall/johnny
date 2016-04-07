/*
 * Copyright (c) 2011-2015 Shinnok <admin at shinnok.com>
 * Parts Copyright (c) 2012 Aleksey Cherepanov <aleksey.4erepanov@gmail.com>
 * Parts Copyright (c) 2015 Mathieu Laprise <mathieu.laprise@polymtl.ca>
 * See LICENSE dist-file for details.
 */

#include "passwordfilemodel.h"
#include "hashtypechecker.h"

#include <QBrush>
#include <QFile>
#include <QFont>

#define FIELD_SEPARATOR ':'

// NOTE: Model could be resizable but it is easier to not
//       implement it. Instead it is possible to load full
//       file into memory and count rows. However it needs to
//       place file loading into model. Though it seems to be
//       good.

PasswordFileModel::PasswordFileModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // We make it as object field because we could not make class field.
    // When changing order of this, the enum FileTableModel::Columns should be
    // changed accordingly
    m_columns << tr("User") << tr("Password") << tr("Hash") << tr("Formats")
              << tr("GECOS");
}

bool PasswordFileModel::readFiles(const QStringList &fileNames)
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
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while(!file.atEnd())
        {
            QString line = file.readLine();
            line.remove(QRegExp("\\r?\\n"));
            QStringList fields = line.split(FIELD_SEPARATOR);
            int         column = 0;
            // NOTE: When we want we change lists we use [] as of .at()
            //       gives us only const.
            if(fields.size() == 1)
            {
                // Lonely hash
                data[column++].append("?");
                data[column++].append("");
                data[column++].append(fields.at(0));
            }
            else if((fields.size() >= 3) &&
                    (fields.at(2).indexOf(QRegExp("^[0-9a-fA-F]{32}$")) == 0))
            {
                // Pwdump format
                data[column++].append(fields.at(0));
                data[column++].append("");
                data[column++].append(fields.at(2));
                fields.removeAt(2);
                fields.removeAt(0);
                data[column++].append(fields.join(QString(FIELD_SEPARATOR)));
            }
            else
            {
                // user:hash:other
                data[column++].append(fields.at(0));
                data[column++].append("");
                data[column++].append(fields.at(1));
                fields.removeAt(1);
                fields.removeAt(0);
                data[column++].append(fields.join(QString(FIELD_SEPARATOR)));
            }
            for(; column < columnCount(); column++)
                data[column].append("");
        }
        file.close();
    }
    // We convert our lists into vectors to store data.
    for(int column = 0; column < columnCount(); column++)
    {
        if(column == FORMAT_COL)
        {
            m_data << QVector<QString>(rowCount());
        }
        m_data << data.at(column).toVector();
    }
    m_checkedRows.fill(Qt::Checked, rowCount());
    return true;
}

void PasswordFileModel::fillHashTypes(const QStringList &listHashTypes)
{
    for(int i = 0; (i < listHashTypes.size()) && (i < rowCount()); i++)
    {
        setData(index(i, FORMAT_COL), listHashTypes[i]);
    }
}

int PasswordFileModel::rowCount(const QModelIndex & /* parent */) const
{
    // For this size we use size of the first column.
    // NOTE: Sizes of all columns should be equal.
    return m_data.at(0).size();
}

int PasswordFileModel::columnCount(const QModelIndex & /* parent */) const
{
    return m_columns.size();
}

QVariant PasswordFileModel::data(const QModelIndex &index, int role) const
{
    // We validate arguments.
    if(!index.isValid() || (index.column() >= columnCount()) ||
       (index.row() >= rowCount()))
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole:
        return m_data.at(index.column()).at(index.row());
        break;
    case Qt::CheckStateRole:
        if((index.column() == 0) && (index.row() < m_checkedRows.count()))
        {
            return m_checkedRows[index.row()];
        }
        else
        {
            return QVariant();
        }
        break;

    case Qt::FontRole:
        if((index.column() == 0) &&
           (!m_data.at(PASSWORD_COL).at(index.row()).isEmpty()))
        {
            QFont font;
            font.setBold(true);
            return font;
        }
        else if((index.column() == PASSWORD_COL) &&
                (m_rowsWithEmptyPasswords.contains(index.row())))
        {
            // Special case empty password ("")
            QFont font;
            font.setItalic(true);
            return font;
        }
        else
        {
            return QVariant();
        }
        break;

    case Qt::BackgroundRole:
        // Show differently cracked passwords
        if((index.row() < m_checkedRows.count()) &&
           (m_checkedRows.at(index.row()) == Qt::Unchecked))
        {
            return QVariant(QColor("#EEEEEE")); // a kind of light-gray
        }
        else
        {
            return QVariant();
        }
        break;

    case Qt::ForegroundRole:
        // Special case empty password ("")
        if((index.column() == PASSWORD_COL) &&
           (m_rowsWithEmptyPasswords.contains(index.row())))
        {
            return QVariant(QBrush(Qt::darkGray));
        }
        else
        {
            return QVariant();
        }
        break;

    case Qt::TextAlignmentRole:
        // Special case empty password ("")
        if((index.column() == PASSWORD_COL) &&
           (m_rowsWithEmptyPasswords.contains(index.row())))
        {
            return Qt::AlignCenter;
        }
        else
        {
            return QVariant();
        }

    default:
        return QVariant();
    }
}

bool PasswordFileModel::setData(const QModelIndex &index, const QVariant &value,
                                int role)
{
    if(!index.isValid() || (index.column() >= columnCount()) ||
       (index.row() >= rowCount()))
        return false;

    QString strValue = value.toString();
    switch(role)
    {
    case Qt::EditRole:
        // We replace data in our table.
        if((index.column() == PASSWORD_COL) && (strValue.isEmpty()))
        {
            strValue = "**NULL PASS**";
            m_rowsWithEmptyPasswords.append(index.row());
        }
        m_data[index.column()].replace(index.row(), strValue);
        break;

    case Qt::CheckStateRole:
        if((index.column() == 0) && (index.row() < m_checkedRows.count()))
        {
            int checkState = value.toInt();
            if(checkState == UNCHECKED_PROGRAMMATICALLY)
            {
                m_checkedRows[index.row()] = Qt::Unchecked;
            }
            else if(checkState == Qt::Unchecked)
            {
                m_checkedRows[index.row()] = Qt::Unchecked;
                emit rowUncheckedByUser();
            }
            else
            {
                m_checkedRows[index.row()] = Qt::Checked;
            }

            QVector<int> role;
            role.push_back(Qt::BackgroundColorRole);
            for(int i = 1; i < columnCount(); i++)
            {
                QModelIndex index2 = this->index(index.row(), i);
                emit dataChanged(index2, index2);
            }
        }
        else
        {
            return false;
        }
        break;

    default:
        return false;
    }
    // We notice all that we changed our state.
    emit dataChanged(index, index);
    return true;
}

QVariant PasswordFileModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
    // We only display header data. It is readonly.
    if(role != Qt::DisplayRole)
        return QVariant();
    // For vertical header we return string numbers.
    if(orientation == Qt::Vertical)
        return QString("%1").arg(section + 1);
    // For horizontal header we return names from fields array.
    if(orientation == Qt::Horizontal)
        return m_columns[section];

    return QVariant();
}

Qt::ItemFlags PasswordFileModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsUserCheckable;
    return flags;
}
