#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "core/discount.h"
#include "core/passenger.h"
#include "core/tariff.h"
#include "core/ticket.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    autoMode = false;

    // Настройка моделей
    setupModels();

    // Обновление таблиц
    refreshAllTables();

    // Подключение менеджера скидок к станции
    station.connectDiscountManager(&discountManager);

    showStatusMessage("Система управления вокзалом запущена");

    bool isAuto;

    if (station.loadFromFile("data.backup", &isAuto, true, true)) {
        if (isAuto){
            autoMode = true;
            ui->checkBoxAutosave->setChecked(true);
        } else autoMode = false;
    } else {
        showStatusMessage("Бэкап не обнаружен");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModels()
{
    // Модель для тарифов
    tariffsModel = new QStandardItemModel(this);
    tariffsProxyModel = new QSortFilterProxyModel(this);
    tariffsModel->setHorizontalHeaderLabels({"Название", "Тип вагона", "Базовая цена", "Скидка", "Итоговая цена"});
    tariffsProxyModel->setSourceModel(tariffsModel);
    tariffsProxyModel->setSortRole(Qt::EditRole);
    ui->tableTariffs->setModel(tariffsProxyModel);
    ui->tableTariffs->horizontalHeader()->setStretchLastSection(true);

    // Модель для скидок
    discountsModel = new QStandardItemModel(this);
    discountsProxyModel = new QSortFilterProxyModel(this);
    discountsModel->setHorizontalHeaderLabels({"Название", "Размер, %", "Описание"});
    discountsProxyModel->setSourceModel(discountsModel);
    discountsProxyModel->setSortRole(Qt::EditRole);
    ui->tableDiscounts->setModel(discountsProxyModel);
    ui->tableDiscounts->horizontalHeader()->setStretchLastSection(true);

    // Модель для пассажиров
    passengersModel = new QStandardItemModel(this);
    passesProxyModel = new QSortFilterProxyModel(this);
    passengersModel->setHorizontalHeaderLabels({"Паспорт", "Фамилия", "Имя"});
    passesProxyModel->setSourceModel(passengersModel);
    passesProxyModel->setSortRole(Qt::EditRole);
    ui->tablePasses->setModel(passesProxyModel);
    ui->tablePasses->horizontalHeader()->setStretchLastSection(true);

    // Модель для билетов
    ticketsModel = new QStandardItemModel(this);
    ticketsProxyModel = new QSortFilterProxyModel(this);
    ticketsModel->setHorizontalHeaderLabels({"Паспорт", "Пассажир", "Направление", "Стоимость"});
    ticketsProxyModel->setSourceModel(ticketsModel);
    ticketsProxyModel->setSortRole(Qt::EditRole);
    ui->tableTickets->setModel(ticketsProxyModel);
    ui->tableTickets->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::refreshTariffsTable()
{
    tariffsModel->removeRows(0, tariffsModel->rowCount());

    auto tariffs = station.getAllTariffs();
    for (size_t i = 0; i < tariffs.size(); ++i) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(tariffs[i]->getName()));
        row << new QStandardItem(QString::fromStdString(tariffs[i]->getVagonTypeString()));

        double basePrice = tariffs[i]->getBasePrice();
        QStandardItem* baseItem = new QStandardItem();
        baseItem->setText(QString::number(basePrice, 'f', 2));
        baseItem->setData(basePrice, Qt::EditRole);
        row << baseItem;

        auto discountInfo = tariffs[i]->getDiscount()->getDiscountInfo();
        row << new QStandardItem(QString("%1 (%2%)")
                                     .arg(QString::fromStdString(discountInfo.name))
                                     .arg(discountInfo.percentage, 0, 'f', 1));

        double finalPrice = tariffs[i]->calculatePrice(false);
        QStandardItem* finalItem = new QStandardItem();
        finalItem->setText(QString::number(finalPrice, 'f', 2));
        finalItem->setData(finalPrice, Qt::EditRole);
        row << finalItem;

        tariffsModel->appendRow(row);
    }
    ui->tableTariffs->resizeColumnsToContents();
}

void MainWindow::refreshDiscountsTable()
{
    discountsModel->removeRows(0, discountsModel->rowCount());

    auto discounts = discountManager.getAllDiscounts();
    for (const auto& discount : discounts) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(discount.name));

        double perc = discount.percentage;
        QStandardItem* baseItem = new QStandardItem();
        baseItem->setText(QString::number(perc, 'f', 2));
        baseItem->setData(perc, Qt::EditRole);
        row << baseItem;

        row << new QStandardItem(QString::fromStdString(discount.description));

        discountsModel->appendRow(row);
    }

    ui->tableDiscounts->resizeColumnsToContents();
}

void MainWindow::refreshPassengersTable()
{
    passengersModel->removeRows(0, passengersModel->rowCount());

    auto passengers = station.getAllPassengers();
    for (size_t i = 0; i < passengers.size(); ++i) {
        QList<QStandardItem*> row;
        int passport = passengers[i]->getPassport();
        QStandardItem* baseItem = new QStandardItem();
        baseItem->setText(QString::number(passport, 'i', 0));
        baseItem->setData(passport, Qt::EditRole);
        row << baseItem;

        row << new QStandardItem(QString::fromStdString(passengers[i]->getLastName()));
        row << new QStandardItem(QString::fromStdString(passengers[i]->getFirstName()));

        passengersModel->appendRow(row);
    }
    ui->tablePasses->resizeColumnsToContents();
}

void MainWindow::refreshTicketsTable()
{
    ticketsModel->removeRows(0, ticketsModel->rowCount());

    auto tickets = station.getAllTickets();
    for (size_t i = 0; i < tickets.size(); ++i) {
        QList<QStandardItem*> row;

        QStandardItem* baseItem = new QStandardItem();
        int passport = tickets[i]->getPassportNumber();
        baseItem->setText(QString::number(passport, 'i', 0));
        baseItem->setData(passport, Qt::EditRole);
        row << baseItem;

        row << new QStandardItem(QString::fromStdString(tickets[i]->getPassenger()->getFullName()));
        row << new QStandardItem(QString::fromStdString(tickets[i]->getDestination()));

        QStandardItem* baseItem2 = new QStandardItem();
        double basePrice = tickets[i]->getPrice(false);
        baseItem2->setText(QString::number(basePrice, 'f', 2));
        baseItem2->setData(basePrice, Qt::EditRole);
        row << baseItem2;

        ticketsModel->appendRow(row);
    }
    ui->tableTickets->resizeColumnsToContents();
}

void MainWindow::refreshAllTables()
{
    refreshTariffsTable();
    refreshPassengersTable();
    refreshTicketsTable();
    refreshDiscountsTable();

    wasSaved = false;

    if(autoMode) station.saveToFile("data.backup", true, true);
}

void MainWindow::showStatusMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

bool MainWindow::validatePassport(const QString& passportStr, int& passport) const
{
    bool ok;
    passport = passportStr.toInt(&ok);
    return ok && passport > 0 && passport <= 999999;
}

bool MainWindow::validatePerc(float perc) const
{
    return perc > 0.0f && perc <= 100.0f;
}

bool MainWindow::validatePrice(const QString& priceStr, float& price) const
{
    bool ok;
    price = priceStr.toFloat(&ok);
    return ok && price > 0 && price <= 10000;
}

QString MainWindow::getSelectedTariffName() const
{
    QModelIndexList selection = ui->tableTariffs->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        int row = selection.first().row();
        return tariffsModel->item(row, 0)->text();
    }
    return QString();
}

int MainWindow::getSelectedPassengerPassport() const
{
    QModelIndexList selection = ui->tablePasses->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        int row = selection.first().row();
        bool ok;
        int passport = passengersModel->item(row, 0)->text().toInt(&ok);
        return ok ? passport : -1;
    }
    return -1;
}

int MainWindow::getSelectedTicketIndex() const
{
    QModelIndexList selection = ui->tableTickets->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        return selection.first().row();
    }
    return -1;
}

bool MainWindow::addPassenger(const QVariantMap& data)
{
    QString lastName = data.value("lastName").toString().trimmed();
    QString firstName = data.value("firstName").toString().trimmed();
    QString passportStr = data.value("passport").toString().trimmed();

    bool ok;
    int passportNum = passportStr.toInt(&ok);
    if (!ok) {
        return false;
    }

    if (!validatePassport(passportStr, passportNum)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный номер паспорта (6 цифр)");
        return false;
    }

    if (lastName.isEmpty() || firstName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Фамилия и имя должны быть заполнены");
        return false;
    }

    if (station.findPassengerByPassport(passportNum)) {
        QMessageBox::warning(this, "Ошибка", "Пассажир с таким паспортом уже существует");
        return false;
    }

    station.addPassenger(std::make_unique<Passenger>(
        passportNum,
        firstName.toStdString(),
        lastName.toStdString()
        ));

    return true;
}

bool MainWindow::addTariff(const QVariantMap& data)
{
    QString name = data.value("name").toString().trimmed();
    QString priceStr = data.value("price").toString().trimmed();
    QString vagonTypeStr = data.value("vagonType").toString();
    QString discountName = data.value("discountName").toString();
    auto discount = discountManager.getDiscountByName(discountName.toStdString());

    float price;
    if (!validatePrice(priceStr, price)) {
        QMessageBox::warning(this, "Ошибка", "Некорректная цена (0-10000)");
        return false;
    }

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Название тарифа должно быть заполнено");
        return false;
    }

    if (station.getTariffByName(name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", "Тариф с таким названием уже существует");
        return false;
    }

    VagonType vagonType = SIT;
    if (vagonTypeStr == "PLAC") vagonType = PLAC;
    else if (vagonTypeStr == "KUPE") vagonType = KUPE;

    station.addTariff(std::make_unique<Tariff>(
        name.toStdString(),
        price,
        vagonType,
        std::move(discount)
        ));

    return true;
}

bool MainWindow::addTicket(const QVariantMap& data)
{
    int passengerIndex = data.value("passengerIndex").toInt();
    int tariffIndex = data.value("tariffIndex").toInt();

    Passenger* passenger = station.getPassengerAt(passengerIndex);
    Tariff* tariff = station.getTariffAt(tariffIndex);

    if (!passenger || !tariff) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить данные пассажира или тарифа");
        return false;
    }

    auto tickets = station.getTicketsByPassport(passenger->getPassport());
    for (const auto& ticket : tickets) {
        if (ticket->getDestination() == tariff->getName()) {
            QMessageBox::warning(this, "Ошибка", "У пассажира уже есть билет на это направление");
            return false;
        }
    }

    station.buyTicket(passenger, tariff);
    return true;
}

bool MainWindow::addDiscount(const QVariantMap& data)
{
    QString Name = data.value("name").toString().trimmed();
    QString Discr = data.value("discr").toString().trimmed();
    float Perc = data.value("perc").toFloat();

    if (!validatePerc(Perc)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный размер скидки (должен быть 0-100%)");
        return false;
    }

    if (Name.isEmpty() || Discr.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Название и описание должны быть заполнены");
        return false;
    }

    if (discountManager.getDiscountByName(Name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", "Скидка с таким названием уже существует");
        return false;
    }

    discountManager.addCustomDiscount(DiscountInfo(Name.toStdString(), Perc, Discr.toStdString()));

    return true;
}

bool MainWindow::deletePassenger(int passport)
{
    if (station.removePassenger(passport)) {
        showStatusMessage(QString("Пассажир с паспортом %1 удален").arg(passport));
        return true;
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить пассажира. Возможно, у него есть купленные билеты");
        return false;
    }
}

bool MainWindow::deleteTariff(const QString& name)
{
    if (station.removeTariff(name.toStdString())) {
        showStatusMessage(QString("Тариф \"%1\" удален").arg(name));
        return true;
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить тариф. Возможно, есть купленные билеты с этим тарифом");
        return false;
    }
}

bool MainWindow::deleteTicket(int index)
{
    if (station.removeTicket(index)) {
        showStatusMessage("Билет удален");
        return true;
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить билет");
        return false;
    }
}

bool MainWindow::deleteDiscount(const QString& name)
{
    auto tariffs = station.getAllTariffs();
    bool ok = true;

    for (const auto& t : tariffs) {
        if (t->getDiscountInfo().name == name){
            ok = false;
            break;
        }
    }

    if (ok){
        if (discountManager.removeDiscount(name.toStdString())) {
            showStatusMessage("Скидка удалена");
            return true;
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить скидку");
            return false;
        }
    }else{
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить скидку. Возможно, есть тарифы, использующие данную скидку");
        return false;
    }
}

void MainWindow::on_confirmAdd_clicked()
{
    int mode = 1;
    if (ui->radioAddTariff->isChecked()) {
        mode = 1;
    } else if (ui->radioAddPass->isChecked()) {
        mode = 2;
    } else if (ui->radioAddTicket->isChecked()) {
        mode = 3;
    } else if (ui->radioAddDiscount->isChecked()){
        mode = 4;
    }

    AddingDialog* dialog = new AddingDialog(mode, &station, &discountManager, false, 0, QMap<QString, QString>(), this);

    connect(dialog, &AddingDialog::acceptedWithData,
            this, &MainWindow::onAddingDialogAccepted);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}


void MainWindow::on_editDataButton_clicked()
{
    QList<QModelIndexList> sells(4);
    sells[0] = ui->tableTariffs->selectionModel()->selectedRows();
    sells[1] = ui->tablePasses->selectionModel()->selectedRows();
    sells[2] = ui->tableTickets->selectionModel()->selectedRows();
    sells[3] = ui->tableDiscounts->selectionModel()->selectedRows();

    size_t sumsize = 0;
    for (int i = 0; i < 4; ++i) {
        sumsize+=sells[i].size();
    }

    if (sumsize == 0){
        QMessageBox::warning(this, "Ошибка", "Не выбрана строка. Для этого перейдите во вкладку 'Информация' и выберите ОДНУ строку которую хотите изменить");
        return;
    } else if (sumsize > 1) {
        QMessageBox::warning(this, "Ошибка", "Выбрано несколько строк. Перейдите во вкладку 'Информация' и снимите выделение с остальных строк");
        return;
    }

    QModelIndexList selection;
    int editMode;
    for (int i = 0; i < 4; ++i) {
        if (sells[i].size()) {
            selection = sells[i];
            editMode = i+1;
        }
    }

    int row = selection.first().row();
    QMap<QString, QString> data;

    bool success = false;

    switch (editMode) {
    case 1: { // Тариф
        data["name"] = tariffsModel->item(row, 0)->text();
        data["price"] = tariffsModel->item(row, 2)->text();
        QString vagon = tariffsModel->item(row, 1)->text();;
        if (vagon == "Сидячий") vagon = "0";
        else if (vagon == "Плацкарт") vagon = "1";
        else vagon = "2";
        data["vtype"] = vagon;
        auto discounts = discountManager.getAllDiscounts();
        int index = 0;
        for (const auto& d : discounts) {
            QString tname = tariffsModel->item(row, 3)->text();
            QStringList tokens = tname.split(" ");
            QString currname = "";
            for (int i = 0; i < tokens.size()-1; ++i) {
                currname+=" "+tokens[i];
            }
            if (" "+d.name == currname.toStdString()) break;
            index++;
        }
        data["discindex"] = QString("%1").arg(index);

        auto tariffs = station.getAllTariffs();
        index = 0;
        for (const auto& t : tariffs) {
            if (t->getName() == data["name"]) break;
            index++;
        }
        data["selfindex"] = QString("%1").arg(index);
        success = true;
        break;
    }
    case 2: { // Пассажир
        data["lname"] = passengersModel->item(row, 1)->text();
        data["fname"] = passengersModel->item(row, 2)->text();
        data["passp"] = passengersModel->item(row, 0)->text();

        auto passes = station.getAllPassengers();
        int index = 0;
        for (const auto& p : passes) {
            if (QString("%1").arg(p->getPassport()) == data["passp"]) break;
            index++;
        }
        data["selfindex"] = QString("%1").arg(index);
        success = true;
        break;
    }
    case 3: { // Билет
        auto passes = station.getAllPassengers();
        int index = 0;
        for (const auto& p : passes) {
            if (QString("%1").arg(p->getPassport()) == ticketsModel->item(row, 0)->text()) break;
            index++;
        }
        data["passindex"]  = QString("%1").arg(index);
        auto tariffs = station.getAllTariffs();
        index = 0;
        for (const auto& t : tariffs) {
            if (QString("%1").arg(t->getName()) == ticketsModel->item(row, 2)->text()) break;
            index++;
        }
        data["tariffindex"]  = QString("%1").arg(index);

        auto tickets = station.getAllTickets();
        index = 0;
        for (const auto& t : tickets) {
            if (t->getTariff()->getName() == ticketsModel->item(row, 2)->text() && QString("%1").arg(t->getPassportNumber()) == ticketsModel->item(row, 0)->text()) break;
            index++;
        }
        data["selfindex"] = QString("%1").arg(index);
        success = true;
        break;
    }
    case 4: { // Скидка
        data["name"] = discountsModel->item(row, 0)->text();
        data["perc"] = discountsModel->item(row, 1)->text();
        data["discr"] = discountsModel->item(row, 2)->text();
        auto discounts = discountManager.getAllDiscounts();
        int index = 0;
        for (const auto& d : discounts) {
            if (QString("%1").arg(d.name) == data["name"]) break;
            index++;
        }
        data["selfindex"] = QString("%1").arg(index);

        if (data["name"] == "Без скидки") success = false;
        else success = true;
        break;
    }
    }

    if (success) {
        AddingDialog* dialog = new AddingDialog(editMode, &station, &discountManager, true, editMode, data, this);

        connect(dialog, &AddingDialog::acceptedWithData,
                this, &MainWindow::onEditDialogAccepted);

        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->exec();
    }
}

void MainWindow::onAddingDialogAccepted(int mode, QString selfindex, const QVariantMap& data)
{
    bool success = false;

    switch (mode) {
    case 1: // Тариф
        success = addTariff(data);
        break;
    case 2: // Пассажир
        success = addPassenger(data);
        break;
    case 3: // Билет
        success = addTicket(data);
        break;
    case 4: // Скидка
        success = addDiscount(data);
        break;
    }

    if (success) {
        refreshAllTables();
        showStatusMessage("Данные успешно добавлены");
    }
}

void MainWindow::onEditDialogAccepted(int mode, QString selfindex, const QVariantMap& data)
{
    bool success = false;

    switch (mode) {
    case 1: { // Тариф
        if (station.getTariffByName(data["name"].toString().toStdString()) && station.getTariffAt(selfindex.toInt())->getName() != data["name"].toString()) {
            QMessageBox::warning(this, "Ошибка", "Тариф с таким названием уже существует");
            break;
        }
        station.getTariffAt(selfindex.toInt())->setName(data["name"].toString().toStdString());
        station.getTariffAt(selfindex.toInt())->setBasePrice(data["price"].toFloat());
        VagonType vagon;
        std::string vdata = data["vagonType"].toString().toStdString();
        if (vdata == "SIT") vagon = SIT;
        else if (vdata == "PLAC") vagon = PLAC;
        else vagon = KUPE;
        station.getTariffAt(selfindex.toInt())->setVType(vagon);
        station.getTariffAt(selfindex.toInt())->setDiscountFromManager(discountManager, data["discountName"].toString().toStdString());
        success = true;
        break;
    }
    case 2:{  // Пассажир
        if (station.findPassengerByPassport(data["passport"].toInt()) && station.getPassengerAt(selfindex.toInt())->getPassport() != data["passport"].toInt()) {
            QMessageBox::warning(this, "Ошибка", "Пассажир с таким паспортом уже существует");
            break;
        }
        station.getPassengerAt(selfindex.toInt())->setLName(data["lastName"].toString().toStdString());
        station.getPassengerAt(selfindex.toInt())->setFName(data["firstName"].toString().toStdString());
        station.getPassengerAt(selfindex.toInt())->setPassport(data["passport"].toInt());
        success = true;
        break;
    }
    case 3: { // Билет
        auto tickets = station.getTicketsByPassport(station.getPassengerAt(data["passengerIndex"].toInt())->getPassport());
        bool stop = false;
        for (const auto& ticket : tickets) {
            if (ticket->getDestination() == station.getTariffAt(data["tariffIndex"].toInt())->getName()) {
                QMessageBox::warning(this, "Ошибка", "У пассажира уже есть билет на это направление");
                stop = true;
                break;
            }
        }
        if (stop) break;
        (*station.getTicketAt(selfindex.toInt())).setPassenger(station.getPassengerAt(data["passengerIndex"].toInt()));
        (*station.getTicketAt(selfindex.toInt())).setTariff(station.getTariffAt(data["tariffIndex"].toInt()));
        success = true;
        break;
    }
    case 4: { // Скидка
        if (discountManager.getDiscountByName(data["name"].toString().toStdString()) && discountManager.getDiscountByIndex(selfindex.toInt())->getDiscountName() != data["name"].toString()) {
            QMessageBox::warning(this, "Ошибка", "Скидка с таким названием уже существует");
            break;
        }
        DiscountInfo info = DiscountInfo(data["name"].toString().toStdString(), data["perc"].toFloat(), data["discr"].toString().toStdString());
        discountManager.editDiscount(data["name"].toString().toStdString(), info);
        success = true;
        break;
    }
    }

    if (success) {
        refreshAllTables();
        showStatusMessage("Данные успешно изменены");
    }
}

void MainWindow::on_confirmDel_clicked()
{
    int mode = 1;
    if (ui->radioDelTariff->isChecked()) {
        mode = 1;
    } else if (ui->radioDelPass->isChecked()) {
        mode = 2;
    } else if (ui->radioDelTicket->isChecked()) {
        mode = 3;
    } else if (ui->radioDelDiscount->isChecked()) {
        mode = 4;
    }

    DelDialog* dialog = new DelDialog(mode, &station, &discountManager, this);

    connect(dialog, &DelDialog::acceptedWithIndex,
            this, &MainWindow::onDelDialogAccepted);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::onDelDialogAccepted(int mode, int selectedIndex)
{
    bool success = false;

    switch (mode) {
    case 1: // Тариф
        if (selectedIndex >= 0 && selectedIndex < station.getTariffCount()) {
            auto tariff = station.getTariffAt(selectedIndex);
            if (tariff) {
                success = deleteTariff(QString::fromStdString(tariff->getName()));
            }
        }
        break;

    case 2: // Пассажир
        if (selectedIndex >= 0 && selectedIndex < station.getPassengerCount()) {
            auto passenger = station.getPassengerAt(selectedIndex);
            if (passenger) {
                success = deletePassenger(passenger->getPassport());
            }
        }
        break;

    case 3: // Билет
        success = deleteTicket(selectedIndex);
        break;

    case 4: // Скидка
        if (selectedIndex >= 0 && selectedIndex < discountManager.getDiscountCount()) {
            auto discount = discountManager.getDiscountByIndex(selectedIndex);
            if (discount) {
                success = deleteDiscount(QString::fromStdString(discount->getDiscountName()));
            }
        }
        break;
    }

    if (success) {
        refreshAllTables();
    }
}

void MainWindow::on_ticketByPassButton_clicked()
{
    QModelIndexList selection = ui->tablePasses->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите пассажира. Для этого перейдите во вкладку 'Информация' и выберите строку с необходимым пассажиром");
        return;
    }

    if (selection.size() > 1) {
        QMessageBox::warning(this, "Ошибка", "Нельзя выбрать более 1 пассажира, проверьте выделение во вкладке 'Информация'");
        return;
    }

    int row = selection.first().row();
    int passport = passengersModel->item(row, 0)->text().toInt();

    auto tickets = station.getTicketsByPassport(passport);

    if (tickets.empty()) {
        QMessageBox::information(this, "Информация", "У выбранного пассажира нет купленных билетов");
        return;
    }

    QString info = QString("Билеты пассажира %1 %2:\n\n")
                       .arg(passengersModel->item(row, 1)->text())
                       .arg(passengersModel->item(row, 2)->text());

    float total = 0;
    for (size_t i = 0; i < tickets.size(); ++i) {
        info += QString("%1. %2 - %3 руб.\n")
                    .arg(i + 1)
                    .arg(QString::fromStdString(tickets[i]->getDestination()))
                    .arg(tickets[i]->getPrice(false), 0, 'f', 2);
        total += tickets[i]->getPrice(false);
    }

    info += QString("\nОбщая стоимость: %1 руб.").arg(total, 0, 'f', 2);

    QMessageBox::information(this, "Билеты пассажира", info);
}

void MainWindow::on_passesByTariffButton_clicked()
{
    QModelIndexList selection = ui->tableTariffs->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите тариф. Для этого перейдите во вкладку 'Информация' и выберите строку с необходимым тарифом");
        return;
    }

    if (selection.size() > 1) {
        QMessageBox::warning(this, "Ошибка", "Нельзя выбрать более 1 тарифа, проверьте выделение во вкладке 'Информация'");
        return;
    }

    int row = selection.first().row();
    QString tariffName = tariffsModel->item(row, 0)->text();

    auto tickets = station.getTicketsByTariff(tariffName.toStdString());

    if (tickets.empty()) {
        QMessageBox::information(this, "Информация", "По выбранному тарифу не продано билетов");
        return;
    }

    QString info = QString("Пассажиры по тарифу \"%1\":\n\n").arg(tariffName);

    for (size_t i = 0; i < tickets.size(); ++i) {
        info += QString("%1. %2 (паспорт: %3)\n")
                    .arg(i + 1)
                    .arg(QString::fromStdString(tickets[i]->getPassenger()->getFullName()))
                    .arg(tickets[i]->getPassportNumber());
    }

    info += QString("\nВсего билетов: %1").arg(tickets.size());

    QMessageBox::information(this, "Пассажиры тарифа", info);
}

void MainWindow::on_totalRevenueButton_clicked()
{
    float total = station.getTotalRevenue(false);
    float discounts = station.getTotalRevenue(true)-total;

    QString info = QString("Выручка всего: %1\n").arg(total);
    info += QString("Без учёта скидок: %1\n").arg(total+discounts);
    info += QString("Сумма убытка по скидкам: %1").arg(discounts);

    QMessageBox::information(this, "Финансовая сводка", info);
}

void MainWindow::on_openBDButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть базу данных", "", "Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (station.loadFromFile(fileName.toStdString(), nullptr, false, false)) {
        refreshAllTables();
        wasSaved = true;
        showStatusMessage("База данных загружена из файла: " + fileName);
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить базу данных из файла");
    }
}

void MainWindow::on_saveBDButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить базу данных", "station_data.txt",
                                                    "Текстовые файлы (*.txt);;Все файлы (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (station.saveToFile(fileName.toStdString(), false, false)) {
        showStatusMessage("База данных сохранена в файл: " + fileName);
        wasSaved  = true;
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить базу данных в файл");
    }
}


void MainWindow::on_checkBoxAutosave_checkStateChanged(const Qt::CheckState &arg1)
{
    if (ui->checkBoxAutosave->isChecked())
    {
        if(!wasSaved && (!station.isEmpty() || discountManager.getDiscountCount() > 1)){
            askToSave("Данные были изменены.\nВместо них будут загружены данные из бэкапа.\nХотите сохранить изменения в отдельный файл?", true);
        }
        autoMode = true;
        if (station.loadFromFile("data.backup", nullptr, true, false)) {
            refreshAllTables();
            showStatusMessage("Бэкап успешно загружен, автосохранение включено");
            station.saveToFile("data.backup", true, true);
        } else {
            showStatusMessage("Бэкап не обнаружен, автосохранение включено");
        }
    } else
    {
        station.saveToFile("data.backup", false, true);
        wasSaved = true;
        autoMode = false;
        showStatusMessage("Бэкап сохранён, автосохранение выключено");
    }
}


void MainWindow::on_restoreDataButtons_clicked(QAbstractButton *button)
{
    if (button->text() == "Reset"){
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset data", "Сохранить бэкап, отключить автосохранение и очистить данные в текущей сессии?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes){
            if (ui->checkBoxAutosave->isChecked()) ui->checkBoxAutosave->setChecked(false);
            station.clearAllData();
            discountManager.clearCustomDiscounts();
            refreshAllTables();
        }
    }

    if (button->text() == "Restore Defaults"){
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Restore data", "Удалить бэкап и очистить данные в текущей сессии?\nПосле удаления бекап восстановить НЕВОЗМОЖНО", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes){
            station.clearAllData();
            refreshAllTables();
            if (ui->checkBoxAutosave->isChecked()) ui->checkBoxAutosave->setChecked(false);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (wasSaved){
        event->accept();
        return;
    }
    if (askToSave("Данные были изменены.\nХотите сохранить изменения?", false)) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::askToSave(const QString& message, bool noCancel){
    if (autoMode || (station.isEmpty() && discountManager.getDiscountCount() == 1)) return true;

    QMessageBox::StandardButton ret;
    if (noCancel) ret = QMessageBox::warning(this, "Сохранение данных", message,QMessageBox::Save | QMessageBox::Discard);
    else ret = QMessageBox::warning(this, "Сохранение данных", message,QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:
        ui->saveBDButton->click();
        return true;

    case QMessageBox::Discard:
        return true;

    case QMessageBox::Cancel:
    default:
        return false;
    }
}

