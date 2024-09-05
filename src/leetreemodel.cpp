#include <leetreemodel.h>


LeeTreeModel::LeeTreeModel(QObject *parent)
    :QFileSystemModel(parent)
{
    //setSty
}

LeeTreeModel::~LeeTreeModel()
{
    deleteLater();
}

QVariant LeeTreeModel::data(const QModelIndex &index, int role) const
{

    if (role == Qt::CheckStateRole && index.column() < 1)
    {
        if(checklist.contains(index))
        {
            return checklist[index];
        }
        else
        {
            return Qt::Unchecked;
        }
    }
    return QFileSystemModel::data(index, role);
}

bool LeeTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    if (role == Qt::CheckStateRole)
    {
        setNodeCheckState(index, value, role);
        setParentNodeCheckState(index);
    }
    this->setReadOnly(false);
    //QApplication::restoreOverrideCursor();
    return QFileSystemModel::setData(index, value, role);

}

Qt::ItemFlags LeeTreeModel::flags(const QModelIndex &index) const
{

    Qt::ItemFlags flag = index.column() > 1 ? Qt::NoItemFlags : Qt::ItemIsUserCheckable ;

    return QFileSystemModel::flags(index) | flag;
}

void LeeTreeModel::RemoveUnCheckItem(QString inItem)
{
    for(int i = 0 ; i < CheckedItems.count(); i++)
    {
        if(CheckedItems[i]==inItem)
            CheckedItems.removeAt(i);
    }

}

bool LeeTreeModel::setNodeCheckState(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        Qt::CheckState currentNodeCheckState = static_cast<Qt::CheckState>(value.toUInt());
        if(checklist.contains(index))
        {
            checklist[index] = currentNodeCheckState;

        }
        else
        {
            checklist.insert(index, currentNodeCheckState);
        }
        setChildNodesCheck(index, value);
        QString item = index.model()->data(index).toString();
        value == 2 ? AddCheckedItem(item) : RemoveUnCheckItem(item);

        emit dataChanged(index, index);
        return true;
    }
    return QFileSystemModel::setData(index, value, role);

}

bool LeeTreeModel::setChildNodesCheck(const QModelIndex &index, const QVariant &value)
{
    int i = 0;
    if(canFetchMore(index))
    {
        fetchMore(index);
        QEventLoop eventLoop;
        QObject::connect(this, SIGNAL(directoryLoaded(QString)), &eventLoop, SLOT(quit()));

        eventLoop.exec(); //blocks untill directoryLoaded signal is fired
    }
    int childrenCount = rowCount(index);
    QModelIndex child;
    for(i = 0; i < childrenCount; i++)
    {
        child = QFileSystemModel::index(i, 0, index);
        setNodeCheckState(child, value, Qt::CheckStateRole);
    }
    return true;
}

bool LeeTreeModel::setParentNodeCheckState(const QModelIndex &currentNodeIndex)
{

    QModelIndex parentNodeIndex = parent(currentNodeIndex);
    if(parentNodeIndex.isValid())
    {
        Qt::CheckState parentNodeCheckState = getCheckStateAccordingToChildren(parentNodeIndex);
        if(checklist.contains(parentNodeIndex))
        {
            checklist[parentNodeIndex] = parentNodeCheckState;
        }
        else
        {
            checklist.insert(parentNodeIndex, parentNodeCheckState);
        }
        emit dataChanged(parentNodeIndex, parentNodeIndex);
        return setParentNodeCheckState(parentNodeIndex);
    }
    return true;
}

Qt::CheckState LeeTreeModel::getCheckStateAccordingToChildren(const QModelIndex &currentNodeIndex)
{
    Qt::CheckState checkState = Qt::Unchecked;
    int childrenCheckCount = 0;
    bool isAnyChildChecked = false;
    bool isAnyChildPartiallyChecked = false;
    if(hasChildren(currentNodeIndex))
    {
        int i;
        int childrenCount = rowCount(currentNodeIndex);
        QModelIndex childIndex;
        for(i = 0; i < childrenCount; i++)
        {
            childIndex = QFileSystemModel::index(i, 0, currentNodeIndex);
            if(checklist.contains(childIndex))
            {
                Qt::CheckState currentCheckState = checklist[childIndex];
                if(currentCheckState == Qt::Checked)
                {
                    childrenCheckCount++;
                    isAnyChildChecked = true;
                }
                else if(currentCheckState == Qt::PartiallyChecked)
                {
                    isAnyChildPartiallyChecked = true;
                }
            }
        }
        if(childrenCount == childrenCheckCount)
        {
            checkState = Qt::Checked;
        }
        else if(isAnyChildChecked || isAnyChildPartiallyChecked)
        {
            checkState = Qt::PartiallyChecked;
        }
    }
    return checkState;
}



