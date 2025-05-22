#include <QApplication>
#include <QFileDialog>
#include <QStringList>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <iostream>
#include <memory>

class CustomFilterProxyModel : public QSortFilterProxyModel {
public:
    CustomFilterProxyModel(const QString &rootPath = QDir::rootPath(), const QString &filter = "PreProcessingModules.dylib", QObject *parent = nullptr) 
        : QSortFilterProxyModel(parent), filterString(filter) {
        fileSystemModel = std::make_unique<QFileSystemModel>();
        fileSystemModel->setRootPath(rootPath);
        setSourceModel(fileSystemModel.get());
    }

    QString rootPath() const {
        return fileSystemModel->rootPath();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QString fileName = sourceModel()->data(index).toString();
        return fileName.contains(filterString);
    }

private:
    std::unique_ptr<QFileSystemModel> fileSystemModel;
    QString filterString;
};

void openFileDialog(const QString &customPath = QDir::rootPath(), const QString &filter = "PreProcessingModules.dylib") {
    QFileDialog dialog;
    auto proxyModel = std::make_unique<CustomFilterProxyModel>(customPath, filter);

    dialog.setProxyModel(proxyModel.get());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec()) {
        QStringList selectedFiles = dialog.selectedFiles();
        for (const QString &file : selectedFiles) {
            std::cout << "Selected file: " << file.toStdString() << std::endl;
        }
    }

    // Obtain the root path from the proxy model
    QString rootPath = proxyModel->rootPath();
    std::cout << "Root path: " << rootPath.toStdString() << std::endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QString customPath = argc > 1 ? argv[1] : QDir::rootPath();
    QString filter = argc > 2 ? argv[2] : "PreProcessingModules.dylib";
    openFileDialog(customPath, filter);
    return 0;
}
