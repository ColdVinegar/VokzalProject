#include "ticket.h"
#include <sstream>
#include <iomanip>

Ticket::Ticket(Passenger* pass, Tariff* tariff)
    : passenger(pass), tariff(tariff) {
}

Passenger* Ticket::getPassenger() const {
    return passenger;
}

Tariff* Ticket::getTariff() const {
    return tariff;
}

void Ticket::setPassenger(Passenger* newPass){
    passenger = newPass;
}

void Ticket::setTariff(Tariff* newTariff){
    tariff = newTariff;
}

int Ticket::getPassportNumber() const {
    return passenger->getPassport();
}

std::string Ticket::getDestination() const {
    return tariff->getName();
}

float Ticket::getPrice(bool withoutDiscount) const {
    return tariff->calculatePrice(withoutDiscount);
}

std::string Ticket::getInfo() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Пассажир: " << passenger->getFullName() << "\n"
        << "Паспорт: " << passenger->getPassport() << "\n"
        << "Направление: " << tariff->getName() << "\n"
        << "Тип вагона: " << tariff->getVagonTypeString() << "\n"
        << "Стоимость: " << getPrice(false) << " руб.";

    return oss.str();
}
