#include "station.h"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iostream>

void Station::connectDiscountManager(DiscountManager* dM)
{
    discountManager = dM;
}

void Station::addPassenger(std::unique_ptr<Passenger> passenger)
{
    passengers.push_back(std::move(passenger));
}

void Station::addTariff(std::unique_ptr<Tariff> tariff)
{
    tariffs.push_back(std::move(tariff));
}

void Station::buyTicket(Passenger* passenger, Tariff* tariff)
{
    tickets.push_back(std::make_unique<Ticket>(passenger, tariff));
}

// Удаление пассажира по паспорту
bool Station::removePassenger(int passport)
{
    // Проверка на наличие билетов
    for (const auto& ticket : tickets) {
        if (ticket->getPassportNumber() == passport) {
            return false;
        }
    }

    auto it = std::find_if(passengers.begin(), passengers.end(),
                           [passport](const std::unique_ptr<Passenger>& p) {
                               return p->getPassport() == passport;
                           });

    if (it != passengers.end()) {
        passengers.erase(it);
        return true;
    }

    return false;
}

// Удаление тарифа по названию
bool Station::removeTariff(const std::string& name)
{
    // Проверка на наличие билетов
    for (const auto& ticket : tickets) {
        if (ticket->getDestination() == name) {
            return false;
        }
    }

    auto it = std::find_if(tariffs.begin(), tariffs.end(),
                           [&name](const std::unique_ptr<Tariff>& t) {
                               return t->getName() == name;
                           });

    if (it != tariffs.end()) {
        tariffs.erase(it);
        return true;
    }

    return false;
}

// Удаление билета по индексу
bool Station::removeTicket(int ticketIndex)
{
    if (ticketIndex >= 0 && static_cast<size_t>(ticketIndex) < tickets.size()) {
        tickets.erase(tickets.begin() + ticketIndex);
        return true;
    }
    return false;
}

// Поиск пассажира по паспорту
Passenger* Station::findPassengerByPassport(int passport) const
{
    auto it = std::find_if(passengers.begin(), passengers.end(),
                           [passport](const std::unique_ptr<Passenger>& p) {
                               return p->getPassport() == passport;
                           });

    return (it != passengers.end()) ? it->get() : nullptr;
}

// Получение пассажира по индексу
Passenger* Station::getPassengerAt(int index) const
{
    if (index >= 0 && static_cast<size_t>(index) < passengers.size()) {
        return passengers[index].get();
    }
    return nullptr;
}

// Получение тарифа по индексу
Tariff* Station::getTariffAt(int index) const
{
    if (index >= 0 && static_cast<size_t>(index) < tariffs.size()) {
        return tariffs[index].get();
    }
    return nullptr;
}

// Получение тарифа по названию
Tariff* Station::getTariffByName(const std::string& name) const
{
    auto it = std::find_if(tariffs.begin(), tariffs.end(),
                           [&name](const std::unique_ptr<Tariff>& t) {
                               return t->getName() == name;
                           });

    return (it != tariffs.end()) ? it->get() : nullptr;
}

// Получение билета по индексу
Ticket* Station::getTicketAt(int index) const
{
    if (index >= 0 && static_cast<size_t>(index) < tickets.size()) {
        return tickets[index].get();
    }
    return nullptr;
}

// Получение всех пассажиров
std::vector<Passenger*> Station::getAllPassengers() const
{
    std::vector<Passenger*> result;
    for (const auto& p : passengers) {
        result.push_back(p.get());
    }
    return result;
}

// Получение всех тарифов
std::vector<Tariff*> Station::getAllTariffs() const
{
    std::vector<Tariff*> result;
    for (const auto& t : tariffs) {
        result.push_back(t.get());
    }
    return result;
}

// Получение всех билетов
std::vector<Ticket*> Station::getAllTickets() const
{
    std::vector<Ticket*> result;
    for (const auto& t : tickets) {
        result.push_back(t.get());
    }
    return result;
}

// Получение количества пассажиров
size_t Station::getPassengerCount() const
{
    return passengers.size();
}

// Получение количества тарифов
size_t Station::getTariffCount() const
{
    return tariffs.size();
}

// Получение количества билетов
size_t Station::getTicketCount() const
{
    return tickets.size();
}

// Получение билетов по номеру паспорта
std::vector<Ticket*> Station::getTicketsByPassport(int passport) const
{
    std::vector<Ticket*> result;
    for (const auto& ticket : tickets) {
        if (ticket->getPassportNumber() == passport) {
            result.push_back(ticket.get());
        }
    }
    return result;
}

// Получение билетов по названию тарифа
std::vector<Ticket*> Station::getTicketsByTariff(const std::string& tariffName) const
{
    std::vector<Ticket*> result;
    for (const auto& ticket : tickets) {
        if (ticket->getDestination() == tariffName) {
            result.push_back(ticket.get());
        }
    }
    return result;
}

// Получение самого дешевого тарифа
Tariff* Station::getCheapestTariff() const
{
    if (tariffs.empty()) {
        return nullptr;
    }

    auto it = std::min_element(tariffs.begin(), tariffs.end(),
                               [](const std::unique_ptr<Tariff>& a, const std::unique_ptr<Tariff>& b) {
                                   return a->calculatePrice(false) < b->calculatePrice(false);
                               });

    return it->get();
}

// Расчет общей выручки (со скидками/ без скидок)
float Station::getTotalRevenue(bool withoutDiscounts) const
{
    float sum = 0;
    for (const auto& t : tickets) {
        sum+=t->getPrice(withoutDiscounts);
    }
    return sum;
}

// Сохранение в файл
bool Station::saveToFile(const std::string& filename, bool isAuto, bool automode) const
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Указываем, статус автосохранения
    if (automode){
        file << "[META]\n";
        if (isAuto) file << "AUTO";
        else file << "NOTAUTO";
        file << "\n";
    }

    // Сохраняем пассажиров
    file << "\n[PASSENGERS]\n";
    for (const auto& p : passengers) {
        file << p->getPassport() << "|"
             << p->getFirstName() << "|"
             << p->getLastName() << "\n";
    }

    // Сохраняем скидки
    file << "\n[DISCOUNTS]\n";
    auto discounts = discountManager->getAllDiscounts();
    for (const auto& d : discounts) {
        file << d.name << "|"
             << d.description << "|"
             << d.percentage << "\n";
    }

    // Сохраняем тарифы
    file << "\n[TARIFFS]\n";
    for (const auto& t : tariffs) {
        file << t->getName() << "|"
             << t->getBasePrice() << "|"
             << static_cast<int>(t->getVType()) << "|"
             << t->getDiscount()->getDiscountInfo().name << "\n";
    }

    // Сохраняем билеты
    file << "\n[TICKETS]\n";
    for (const auto& ticket : tickets) {
        file << ticket->getPassportNumber() << "|"
             << ticket->getDestination() << "\n";
    }

    file.close();
    return true;
}

// Загрузка из файла
bool Station::loadFromFile(const std::string& filename, bool* isAuto, bool automode, bool isCheck)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Очищаем текущие данные
    clearAllData();

    std::string line;
    std::string section;

    bool dataAuto = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line == "[META]") {
            section = "META";
        } else if (line == "[PASSENGERS]") {
            section = "PASSENGERS";
        } else if (line == "[DISCOUNTS]") {
            section = "DISCOUNTS";
        } else if (line == "[TARIFFS]") {
            section = "TARIFFS";
        } else if (line == "[TICKETS]") {
            section = "TICKETS";
        } else if (line[0] == '[') {
            section = "";
        } else if (!section.empty()) {
            std::istringstream iss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(iss, token, '|')) {
                tokens.push_back(token);
            }

            if (section == "META" && tokens.size() >= 1) {
                try {
                    std::string data = tokens[0];
                    if (data == "AUTO") dataAuto = true;
                    else if (data == "NOTAUTO") dataAuto = false;
                    if (isCheck){
                        file.close();
                        *isAuto = dataAuto;
                        return true;
                    }
                } catch (...) {
                    if (isCheck){
                        file.close();
                        *isAuto = false;
                        return false;
                    }
                }
            } else if (section == "PASSENGERS" && tokens.size() >= 3){
                try {
                    int passport = std::stoi(tokens[0]);
                    std::string firstName = tokens[1];
                    std::string lastName = tokens[2];

                    addPassenger(std::make_unique<Passenger>(passport, firstName, lastName));
                } catch (...) {
                    // Пропускаем некорректные записи
                }
            } else if (section == "DISCOUNTS" && tokens.size() >= 3){
                try {
                    std::string name = tokens[0];
                    std::string discr = tokens[1];
                    float perc = std::stof(tokens[2]);

                    if (name != "Без скидки"){
                        DiscountInfo discountInfo(name, perc, discr);
                        discountManager->addCustomDiscount(discountInfo);
                    }
                } catch (...) {}
            } else if (section == "TARIFFS" && tokens.size() >= 4) {
                try {
                    std::string name = tokens[0];
                    float price = std::stof(tokens[1]);
                    VagonType type = static_cast<VagonType>(std::stoi(tokens[2]));
                    std::string discountName = tokens[3];

                    auto discount = discountManager->getDiscountByName(discountName);

                    addTariff(std::make_unique<Tariff>(name, price, type, std::move(discount)));
                } catch (...) {}
            } else if (section == "TICKETS" && tokens.size() >= 2) {
                try {
                    int passport = std::stoi(tokens[0]);
                    std::string tariffName = tokens[1];

                    Passenger* passenger = findPassengerByPassport(passport);
                    Tariff* tariff = getTariffByName(tariffName);

                    if (passenger && tariff) {
                        buyTicket(passenger, tariff);
                    }
                } catch (...) {}
            }
        }
    }

    file.close();
    return true;
}

// Проверка на наличие данных
bool Station::isEmpty(){
    size_t total = this->getPassengerCount()+this->getTariffCount()+this->getTicketCount();
    if (total) return false;
    return true;
}

// Очистка всех данных
void Station::clearAllData()
{
    passengers.clear();
    tariffs.clear();
    tickets.clear();
}
