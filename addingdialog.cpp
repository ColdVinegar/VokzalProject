#include "addingdialog.h"
#include "ui_addingdialog.h"
#include "core/discount.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>

AddingDialog::AddingDialog(int mode, Station* station, DiscountManager* discountManager, bool isEdit, int editMode, QMap<QString, QString> editData, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddingDialog)
    , currentMode(mode)
    , station(station)
    , discountManager(discountManager)
    , isEdit(isEdit)
    , editData(editData)
{
    ui->setupUi(this);

    if (isEdit) {
        currentMode = editMode;
        this->setWindowTitle("Изменение");
    }

    switch(currentMode) {
    case 1:
        setTariffMode(editData);
        break;
    case 2:
        setPassMode(editData);
        break;
    case 3:
        setTicketMode(editData);
        break;
    case 4:
        setDiscountMode(editData);
        break;
    }

    populateComboBoxes(editData);

    // Подключение валидации ввода
    connect(ui->add_line_1, &QLineEdit::textChanged, this, &AddingDialog::validateInputs);
    if (ui->add_line_2) {
        connect(ui->add_line_2, &QLineEdit::textChanged, this, &AddingDialog::validateInputs);
    }
    if (ui->add_line_3) {
        connect(ui->add_line_3, &QLineEdit::textChanged, this, &AddingDialog::validateInputs);
    }

    ui->buttonBox->disconnect();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddingDialog::onAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddingDialog::onReject);

    validateInputs();
}

void AddingDialog::setTariffMode(QMap<QString, QString> editData)
{
    ui->add_label_1->setText("Название тарифа");

    if (ui->add_label_2) ui->add_label_2->hide();
    if (ui->add_line_2) ui->add_line_2->hide();
    if (ui->line_2) ui->line_2->hide();

    ui->add_label_3->setText("Базовая стоимость");

    if (ui->add_line_3) {
        ui->add_line_3->setInputMask("");

        QRegularExpression regExp("^"
                                  "(?:0|[1-9]\\d{0,3})"
                                  "(?:\\.\\d{0,2})?"
                                  "|"
                                  "10000"
                                  "$");

        QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this);
        ui->add_line_3->setValidator(validator);
    }

    if (ui->AddVTypeLabel) ui->AddVTypeLabel->show();
    if (ui->comboVTypeAdd) ui->comboVTypeAdd->show();
    if (ui->AddDiscLabel) ui->AddDiscLabel->show();
    if (ui->comboDiscountAdd) ui->comboDiscountAdd->show();

    if (ui->comboVTypeAdd) {
        ui->comboVTypeAdd->addItem("Сидячий", "SIT");
        ui->comboVTypeAdd->addItem("Плацкарт", "PLAC");
        ui->comboVTypeAdd->addItem("Купе", "KUPE");
    }

    if (!editData.isEmpty()){
        ui->add_line_1->setText(editData["name"]);
        ui->add_line_3->setText(editData["price"]);
        ui->comboVTypeAdd->setCurrentIndex(editData["vtype"].toInt());
    }

    if (ui->add_line_1) ui->add_line_1->setFocus();
}

void AddingDialog::setPassMode(QMap<QString, QString> editData)
{
    if (ui->line_3) ui->line_3->hide();
    if (ui->AddDiscLabel) ui->AddDiscLabel->hide();
    if (ui->comboDiscountAdd) ui->comboDiscountAdd->hide();
    if (ui->AddVTypeLabel) ui->AddVTypeLabel->hide();
    if (ui->comboVTypeAdd) ui->comboVTypeAdd->hide();

    ui->add_label_1->setText("Фамилия");
    ui->add_label_2->setText("Имя");
    ui->add_label_3->setText("Паспорт (6 цифр)");

    if (ui->add_line_3) {
        ui->add_line_3->setValidator(nullptr);
        ui->add_line_3->setInputMask(">999999;#");
    }

    if (!editData.isEmpty()){
        ui->add_line_1->setText(editData["lname"]);
        ui->add_line_2->setText(editData["fname"]);
        ui->add_line_3->setText(editData["passp"]);
    }

    if (ui->add_line_1) ui->add_line_1->setFocus();
}

void AddingDialog::setTicketMode(QMap<QString, QString> editData)
{
    if (ui->add_label_1) ui->add_label_1->hide();
    if (ui->add_line_1) ui->add_line_1->hide();
    if (ui->line) ui->line->hide();
    if (ui->add_label_2) ui->add_label_2->hide();
    if (ui->add_line_2) ui->add_line_2->hide();
    if (ui->line_2) ui->line_2->hide();
    if (ui->add_label_3) ui->add_label_3->hide();
    if (ui->add_line_3) ui->add_line_3->hide();
    if (ui->line_3) ui->line_3->hide();

    if (ui->AddDiscLabel) ui->AddDiscLabel->setText("Тариф");
    if (ui->AddVTypeLabel) ui->AddVTypeLabel->setText("Пассажир");
}

void AddingDialog::setDiscountMode(QMap<QString, QString> editData)
{
    if (ui->line_3) ui->line_3->hide();
    if (ui->AddDiscLabel) ui->AddDiscLabel->hide();
    if (ui->comboDiscountAdd) ui->comboDiscountAdd->hide();
    if (ui->AddVTypeLabel) ui->AddVTypeLabel->hide();
    if (ui->comboVTypeAdd) ui->comboVTypeAdd->hide();

    ui->add_label_1->setText("Название");
    ui->add_label_2->setText("Описание");
    ui->add_label_3->setText("Размер скидки, %");

    if (ui->add_line_3) {
        ui->add_line_3->setInputMask("");
        QRegularExpression regExp("^"
                                  "(?:0|[1-9]\\d{0,1})"
                                  "(?:\\.\\d{0,2})?"
                                  "|"
                                  "100"
                                  "$");

        QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this);
        ui->add_line_3->setValidator(validator);
    }


    if (!editData.isEmpty()){
        ui->add_line_1->setText(editData["name"]);
        ui->add_line_2->setText(editData["discr"]);
        ui->add_line_3->setText(editData["perc"]);
    }

    if (ui->add_line_1) ui->add_line_1->setFocus();
}

void AddingDialog::populateComboBoxes(QMap<QString, QString> editData)
{
    if (currentMode == 1) { // Тариф - заполняем скидки
        if (ui->comboDiscountAdd) {
            ui->comboDiscountAdd->clear();
            auto discounts = discountManager->getAllDiscounts();
            for (const auto& discount : discounts) {
                QString displayText = QString("%1 (%2%)")
                .arg(QString::fromStdString(discount.name))
                    .arg(discount.percentage, 0, 'f', 1);
                ui->comboDiscountAdd->addItem(displayText,
                                              QVariantList() << QString::fromStdString(discount.name) << discount.percentage);
            }
            if (!editData.isEmpty()) ui->comboDiscountAdd->setCurrentIndex(editData["discindex"].toInt());
        }
    } else if (currentMode == 3) { // Билет - заполняем пассажиров и тарифы
        if (ui->comboVTypeAdd) {
            ui->comboVTypeAdd->clear();
            auto passengers = station->getAllPassengers();
            for (size_t i = 0; i < passengers.size(); ++i) {
                QString displayText = QString("%1 %2 (паспорт: %3)")
                                          .arg(QString::fromStdString(passengers[i]->getLastName()))
                                          .arg(QString::fromStdString(passengers[i]->getFirstName()))
                                          .arg(passengers[i]->getPassport());
                ui->comboVTypeAdd->addItem(displayText, static_cast<int>(i));
            }
            if (!editData.isEmpty()) ui->comboVTypeAdd->setCurrentIndex(editData["passindex"].toInt());
        }

        if (ui->comboDiscountAdd) {
            ui->comboDiscountAdd->clear();
            auto tariffs = station->getAllTariffs();
            for (size_t i = 0; i < tariffs.size(); ++i) {
                QString displayText = QString("%1 - %2 руб.")
                                          .arg(QString::fromStdString(tariffs[i]->getName()))
                                          .arg(tariffs[i]->calculatePrice(false), 0, 'f', 2);
                ui->comboDiscountAdd->addItem(displayText, static_cast<int>(i));
            }
            if (!editData.isEmpty()) ui->comboDiscountAdd->setCurrentIndex(editData["tariffindex"].toInt());
        }
    }
}

void AddingDialog::validateInputs()
{
    bool valid = true;

    switch (currentMode) {
    case 1: // Тариф
        if (ui->add_line_1 && ui->add_line_3 && ui->comboVTypeAdd && ui->comboDiscountAdd) {
            valid = !ui->add_line_1->text().trimmed().isEmpty() &&
                    !ui->add_line_3->text().trimmed().isEmpty() &&
                    ui->comboVTypeAdd->currentIndex() >= 0 &&
                    ui->comboDiscountAdd->currentIndex() >= 0;
        } else {
            valid = false;
        }
        break;

    case 2: // Пассажир
        if (ui->add_line_1 && ui->add_line_2 && ui->add_line_3) {
            valid = !ui->add_line_1->text().trimmed().isEmpty() &&
                    !ui->add_line_2->text().trimmed().isEmpty() &&
                    ui->add_line_3->text().trimmed().length() == 6;
        } else {
            valid = false;
        }
        break;

    case 3: // Билет
        if (ui->comboVTypeAdd && ui->comboDiscountAdd) {
            valid = ui->comboVTypeAdd->currentIndex() >= 0 &&
                    ui->comboDiscountAdd->currentIndex() >= 0;
        } else {
            valid = false;
        }
        break;

    case 4: // Скидка
        if (ui->add_line_1 && ui->add_line_2 && ui->add_line_3) {
            valid = !ui->add_line_1->text().trimmed().isEmpty() &&
                    !ui->add_line_2->text().trimmed().isEmpty() &&
                    !ui->add_line_3->text().trimmed().isEmpty();
        } else {
            valid = false;
        }
        break;
    }

    QPushButton* okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (okButton) {
        okButton->setEnabled(valid);
    }
}

bool AddingDialog::collectData(QVariantMap& data)
{
    switch (currentMode) {
    case 1:
        return collectTariffData(data);
    case 2:
        return collectPassengerData(data);
    case 3:
        return collectTicketData(data);
    case 4:
        return collectDiscountData(data);
    }

    return false;
}

bool AddingDialog::collectTariffData(QVariantMap& data)
{
    if (!ui->add_line_1 || !ui->add_line_3 || !ui->comboVTypeAdd || !ui->comboDiscountAdd) {
        return false;
    }

    QString name = ui->add_line_1->text().trimmed();
    QString price = ui->add_line_3->text().trimmed();

    if (name.isEmpty() || price.isEmpty()) {
        return false;
    }

    // Проверка на наличие
    if (!isEdit && station->getTariffByName(name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", "Тариф с таким названием уже существует");
        return false;
    }

    data["name"] = name;
    data["price"] = price;
    data["vagonType"] = ui->comboVTypeAdd->currentData().toString();

    QVariantList discountData = ui->comboDiscountAdd->currentData().toList();
    if (discountData.size() >= 2) {
        data["discountName"] = discountData[0].toString();
        data["discountPercentage"] = discountData[1].toFloat();
    }

    return true;
}

bool AddingDialog::collectPassengerData(QVariantMap& data)
{
    if (!ui->add_line_1 || !ui->add_line_2 || !ui->add_line_3) {
        return false;
    }

    QString lastName = ui->add_line_1->text().trimmed();
    QString firstName = ui->add_line_2->text().trimmed();
    QString passport = ui->add_line_3->text().trimmed();

    if (lastName.isEmpty() || firstName.isEmpty() || passport.length() != 6) {
        return false;
    }

    bool ok;
    int passportNum = passport.toInt(&ok);
    if (!ok) {
        return false;
    }

    // Проверка на наличие
    if (!isEdit && station->findPassengerByPassport(passportNum)) {
        QMessageBox::warning(this, "Ошибка", "Пассажир с таким паспортом уже существует");
        return false;
    }

    data["lastName"] = lastName;
    data["firstName"] = firstName;
    data["passport"] = passport;

    return true;
}

bool AddingDialog::collectTicketData(QVariantMap& data)
{
    if (!ui->comboVTypeAdd || !ui->comboDiscountAdd) {
        return false;
    }

    int passengerIndex = ui->comboVTypeAdd->currentData().toInt();
    int tariffIndex = ui->comboDiscountAdd->currentData().toInt();

    if (passengerIndex < 0 || tariffIndex < 0) {
        return false;
    }

    auto passenger = station->getPassengerAt(passengerIndex);
    auto tariff = station->getTariffAt(tariffIndex);

    if (!passenger || !tariff) {
        return false;
    }

    // Проверка на наличие
    if (!isEdit){
        auto tickets = station->getTicketsByPassport(passenger->getPassport());
        for (const auto& ticket : tickets) {
            if (ticket->getDestination() == tariff->getName()) {
                QMessageBox::warning(this, "Ошибка", "У пассажира уже есть билет на это направление");
                return false;
            }
        }
    }

    data["passengerIndex"] = passengerIndex;
    data["tariffIndex"] = tariffIndex;

    return true;
}

bool AddingDialog::collectDiscountData(QVariantMap& data)
{
    if (!ui->add_line_1 || !ui->add_line_2 || !ui->add_line_3) {
        return false;
    }

    QString Name = ui->add_line_1->text().trimmed();
    QString Discr = ui->add_line_2->text().trimmed();
    QString Perc = ui->add_line_3->text().trimmed();

    if (Name.isEmpty() || Discr.isEmpty()) {
        return false;
    }

    // Проверка на наличие
    if (!isEdit && discountManager->getDiscountByName(Name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", "Скидка с таким названием уже существует");
        return false;
    }

    bool ok;
    float percNum = Perc.toFloat(&ok);
    if (!ok) {
        return false;
    }

    data["name"] = Name;
    data["discr"] = Discr;
    data["perc"] = percNum;

    return true;
}

void AddingDialog::onAccept()
{
    QVariantMap data;
    if (collectData(data)) {
        emit acceptedWithData(currentMode, editData["selfindex"], data);
        accept();
    }
}

void AddingDialog::onReject()
{
    reject();
}

AddingDialog::~AddingDialog()
{
    delete ui;
}
