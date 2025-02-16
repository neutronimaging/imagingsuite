//#include <QApplication>
//#include <QFileDialog>
//#include <QStringList>
#include <logging/logger.h>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>


class ModuleFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT 
    kipl::logging::Logger logger;
public:
    ModuleFilterProxyModel( const QString &rootPath = QDir::rootPath(), 
                            const QString &filter = "*.*", 
                            QObject *parent = nullptr);


    QString rootPath() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    std::unique_ptr<QFileSystemModel> fileSystemModel;
    QString filterString;
};