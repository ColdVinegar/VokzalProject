#include "deldialog.h"
#include "ui_deldialog.h"
#include "core/discount.h"
#include <QMessageBox>

DelDialog::DelDialog(int mode, Station* station, DiscountManager* discountManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DelDialog)
    , currentMode(mode)
    , station(station)
    , discountManager(discountManager)
{
    ui->setupUi(this);

    switch(currentMode) {
    case 1:
        setTariffMode();
        break;
    case 2:
        setPassMode();
        break;
    case 3:
        setTicketMode();
        break;
    case 4:
        setDiscountMode();
        break;
    }

    populateComboBox();

    ui->buttonBox->disconnect();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DelDialog::onAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DelDialog::onReject);
}

void DelDialog::setTariffMode()
{
    ui->delLabel->setText("Выберите тариф для удаления");
    ui->delCombo->setPlaceholderText("Тариф");
}

void DelDialog::setPassMode()
{
    ui->delLabel->setText("Выберите пассажира для удаления");
    ui->delCombo->setPlaceholderText("Пассажир");
}

void DelDialog::setTicketMode()
{
    ui->delLabel->setText("Выберите билет для удаления");
    ui->delCombo->setPlaceholderText("Билет");
}

void DelDialog::setDiscountMode()
{
    ui->delLabel->setText("Выберите скидку для удаления");
    ui->delCombo->setPlaceholderText("Скидка");
}

void DelDialog::populateComboBox()
{
    if (!ui->delCombo) return;

    ui->delCombo->clear();

    switch (currentMode) {
    case 1: // Тарифы
    {
        auto tariffs = station->getAllTariffs();
        for (size_t i = 0; i < tariffs.size(); ++i) {
            QString displayText = QString("%1 - %2 руб.")
                                      .arg(QString::fromStdString(tariffs[i]->getName()))
                                      .arg(tariffs[i]->calculatePrice(false), 0, 'f', 2);
            ui->delCombo->addItem(displayText, static_cast<int>(i));
        }
        break;
    }
    case 2: // Пассажиры
    {
        auto passengers = station->getAllPassengers();
        for (size_t i = 0; i < passengers.size(); ++i) {
            QString displayText = QString("%1 %2 (паспорт: %3)")
                                      .arg(QString::fromStdString(passengers[i]->getLastName()))
                                      .arg(QString::fromStdString(passengers[i]->getFirstName()))
                                      .arg(passengers[i]->getPassport());
            ui->delCombo->addItem(displayText, static_cast<int>(i));
        }
        break;
    }
    case 3: // Билеты
    {
        auto tickets = station->getAllTickets();
        for (size_t i = 0; i < tickets.size(); ++i) {
            QString displayText = QString("%1 -> %2 - %3 руб.")
                                      .arg(QString::fromStdString(tickets[i]->getPassenger()->getFullName()))
                                      .arg(QString::fromStdString(tickets[i]->getDestination()))
                                      .arg(tickets[i]->getPrice(false), 0, 'f', 2);
            ui->delCombo->addItem(displayText, static_cast<int>(i));
        }
        break;
    }
    case 4: // Скидки
    {
        auto discounts = discountManager->getAllDiscounts();
        for (size_t i = 1; i < discounts.size(); ++i) { // От 1, т.к. под 0 индексом хранится системная скидка 0%
            QString displayText = QString("%1 - %2 \%")
                                      .arg(QString::fromStdString(discounts[i].name))
                                      .arg(discounts[i].percentage, 0, 'f', 1);
            ui->delCombo->addItem(displayText, static_cast<int>(i));
        }
        break;
    }
    }

    if (ui->delCombo->count() == 0) {
        ui->delCombo->addItem("Нет данных для удаления", -1);
        ui->delCombo->setEnabled(false);

        QPushButton* okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
        if (okButton) {
            okButton->setEnabled(false);
        }
    }
}

void DelDialog::onAccept()
{
    if (!ui->delCombo) {
        reject();
        return;
    }

    int selectedIndex = ui->delCombo->currentData().toInt();

    if (selectedIndex < 0) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для удаления");
        return;
    }

    QString itemText = ui->delCombo->currentText();
    QString message;

    switch (currentMode) {
    case 1:
        message = QString("Вы уверены, что хотите удалить тариф:\n\"%1\"?").arg(itemText);
        break;
    case 2:
        message = QString("Вы уверены, что хотите удалить пассажира:\n\"%1\"?").arg(itemText);
        break;
    case 3:
        message = QString("Вы уверены, что хотите удалить билет:\n\"%1\"?").arg(itemText);
        break;
    case 4:
        message = QString("Вы уверены, что хотите удалить скидку:\n\"%1\"?").arg(itemText);
        break;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение удаления",
                                                              message, QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes){
        emit acceptedWithIndex(currentMode, selectedIndex);
        accept();
    }
}

void DelDialog::onReject()
{
    reject();
}

DelDialog::~DelDialog()
{
    delete ui;
}
