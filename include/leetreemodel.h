#ifndef LEETREEMODEL_H
#define LEETREEMODEL_H

#include <QTreeView>
#include <QVector>
#include <QFileSystemModel>
#include <QApplication>


class LeeTreeModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit LeeTreeModel(QObject *parent = Q_NULLPTR);
    ~LeeTreeModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    QStringList GetCheckedItem(){return CheckedItems;}

    void AddCheckedItem(QString inItem){CheckedItems.push_back(inItem);}

    void RemoveUnCheckItem(QString inItem);
private:
    QMap<QPersistentModelIndex, Qt::CheckState> checklist;

    bool setNodeCheckState(const QModelIndex & index, const QVariant & value, int role);
    bool setChildNodesCheck (const QModelIndex & index, const QVariant & value);

    bool setParentNodeCheckState(const QModelIndex& currentNodeIndex);
    Qt::CheckState getCheckStateAccordingToChildren(const QModelIndex& currentNodeIndex);

    QStringList CheckedItems;
};



#endif // LEETREEMODEL_H
