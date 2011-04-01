
#include <QtGui>

#include "tablemodel.h"

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    for(int i=0; i < TABLE_ROWS; i++)
        for(int j=0; j < TABLE_COLUMNS; j++)
            table.append("");
}

TableModel::~TableModel()
{
    table.empty();
}

int TableModel::rowCount(const QModelIndex & /* parent */) const
{
    return TABLE_ROWS;
}

int TableModel::columnCount(const QModelIndex & /* parent */) const
{
    return TABLE_COLUMNS;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    return table.at(index.row() * TABLE_COLUMNS + index.column());
}

bool TableModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    table.replace(index.row() * TABLE_COLUMNS + index.column(), value.toString());

    emit TableModel::dataChanged(index, index);

    return true;
}

QVariant TableModel::headerData(int section/* section */,
                                Qt::Orientation orientation/* orientation */,
                                int role) const
{
    //if (role == Qt::SizeHintRole)
     //   return QSize(1, 1);

    if (role != Qt::DisplayRole)
             return QVariant();

    if (orientation == Qt::Vertical)
        return QString("%1").arg(section);
    if (orientation == Qt::Horizontal)
        switch(section){
            case 0: return QString(tr("User"));
                break;
            case 1: return QString(tr("Hash"));
                break;
        }

    return QVariant();
}
