#ifndef TICKET_H
#define TICKET_H

#include "passenger.h"
#include "tariff.h"
#include <memory>

class Ticket {
private:
    Passenger* passenger;
    Tariff* tariff;

public:
    Ticket(Passenger* pass, Tariff* tariff);

    Passenger* getPassenger() const;
    Tariff* getTariff() const;

    void setPassenger(Passenger* newPass);
    void setTariff(Tariff* newTariff);

    int getPassportNumber() const;
    std::string getDestination() const;
    float getPrice(bool whithoutDiscount) const;

    std::string getInfo() const;
};

#endif // TICKET_H
