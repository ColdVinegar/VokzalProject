#ifndef STATION_H
#define STATION_H

#include "passenger.h"
#include "tariff.h"
#include "ticket.h"
#include "discount.h"
#include <vector>
#include <memory>

class Station {
private:
    std::vector<std::unique_ptr<Passenger>> passengers;
    std::vector<std::unique_ptr<Tariff>> tariffs;
    std::vector<std::unique_ptr<Ticket>> tickets;
    DiscountManager* discountManager;

public:
    Station() = default;

    void connectDiscountManager(DiscountManager* discountManager);

    // Добавление
    void addPassenger(std::unique_ptr<Passenger> passenger);
    void addTariff(std::unique_ptr<Tariff> tariff);
    void buyTicket(Passenger* passenger, Tariff* tariff);

    // Удаление
    bool removePassenger(int passport);
    bool removeTariff(const std::string& name);
    bool removeTicket(int ticketIndex);

    // Поиск
    Passenger* findPassengerByPassport(int passport) const;
    Passenger* getPassengerAt(int index) const;
    Tariff* getTariffAt(int index) const;
    Tariff* getTariffByName(const std::string& name) const;
    Ticket* getTicketAt(int index) const;

    // Получение списков
    std::vector<Passenger*> getAllPassengers() const;
    std::vector<Tariff*> getAllTariffs() const;
    std::vector<Ticket*> getAllTickets() const;

    // Счётчики
    size_t getPassengerCount() const;
    size_t getTariffCount() const;
    size_t getTicketCount() const;
    bool isEmpty();

    // Статистика
    std::vector<Ticket*> getTicketsByPassport(int passport) const;
    std::vector<Ticket*> getTicketsByTariff(const std::string& tariffName) const;
    Tariff* getCheapestTariff() const;
    float getTotalRevenue(bool withoutDiscounts) const;

    // Сохранение/загрузка
    bool saveToFile(const std::string& filename, bool isAuto, bool automode) const;
    bool loadFromFile(const std::string& filename, bool* isAuto, bool automode, bool isCheck);
    void clearAllData();
};

#endif // STATION_H
