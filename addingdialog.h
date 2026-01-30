#ifndef ADDINGDIALOG_H
#define ADDINGDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMap>
#include "core/station.h"
#include "core/discount.h"

namespace Ui {
class AddingDialog;
}

class AddingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddingDialog(int mode, Station* station, DiscountManager* discountManager, bool isEdit, int editMode, QMap<QString, QString> editData, QWidget *parent = nullptr);
    ~AddingDialog();

signals:
    void acceptedWithData(int mode, QString selfindex, const QVariantMap& data);

private slots:
    void onAccept();
    void onReject();

    void validateInputs();

private:
    Ui::AddingDialog *ui;
    int currentMode;
    Station* station;
    DiscountManager* discountManager;
    int isEdit = false;
    QMap<QString, QString> editData;

    void setTariffMode(QMap<QString, QString> editData);
    void setPassMode(QMap<QString, QString> editData);
    void setTicketMode(QMap<QString, QString> editData);
    void setDiscountMode(QMap<QString, QString> editData);

    bool collectData(QVariantMap& data);
    bool collectTariffData(QVariantMap& data);
    bool collectPassengerData(QVariantMap& data);
    bool collectTicketData(QVariantMap& data);
    bool collectDiscountData(QVariantMap& data);

    void populateComboBoxes(QMap<QString, QString> editData);
};

#endif // ADDINGDIALOG_H
