//<LICENSE>

#ifndef GENERICCONVERSION_H
#define GENERICCONVERSION_H

#include <QDialog>

#include <logging/logger.h>

namespace Ui {
class GenericConversion;
}

class GenericConversion : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit GenericConversion(QWidget *parent = 0);
    ~GenericConversion();

private slots:
    void on_pushButton_BrowseFileMask_clicked();

    void on_pushButton_Preview_clicked();

    void on_pushButton_BrowseDest_clicked();

    void on_pushButton_ConvertAll_clicked();

    void on_checkBox_DispLevels_toggled(bool checked);

    void on_comboBox_DataType_currentIndexChanged(int index);

private:
    Ui::GenericConversion *ui;
    void UpdateDialog();
    void UpdateConfig();

    std::string m_fname;
    std::string m_destPath;
    std::string m_destMask;
    int         m_sizeX;
    int         m_sizeY;
    int         m_offset;
    int         m_stride;
    bool        m_customStride;
    bool        m_rotate;
    bool        m_customLevels;
    int         m_type;
    int         m_levelLo;
    int         m_levelHi;
    int         m_firstImg;
    int         m_lastImg;
};

#endif // GENERICCONVERSION_H
