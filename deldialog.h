#ifndef DELDIALOG_H
#define DELDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include "core/station.h"

namespace Ui {
class DelDialog;
}

class DelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DelDialog(int mode, Station* station, DiscountManager* discountManager, QWidget *parent = nullptr);
    ~DelDialog();

signals:
    void acceptedWithIndex(int mode, int selectedIndex);

private slots:
    void onAccept();
    void onReject();

private:
    Ui::DelDialog *ui;
    int currentMode;
    Station* station;
    DiscountManager* discountManager;

    void setTariffMode();
    void setPassMode();
    void setTicketMode();
    void setDiscountMode();

    void populateComboBox();
};

#endif // DELDIALOG_H
