#include "passenger.h"
#include <sstream>

Passenger::Passenger(int passport, const std::string& fname, const std::string& lname)
    : passportNumber(passport), firstName(fname), lastName(lname) {
}

int Passenger::getPassport() const {
    return passportNumber;
}

std::string Passenger::getFirstName() const {
    return firstName;
}

std::string Passenger::getLastName() const {
    return lastName;
}

void Passenger::setPassport(int newPassport){
    passportNumber = newPassport;
}

void Passenger::setFName(const std::string& newFName){
    firstName = newFName;
}

void Passenger::setLName(const std::string& newLName){
    lastName = newLName;
}

std::string Passenger::getFullName() const {
    return lastName + " " + firstName;
}

std::string Passenger::getInfo() const {
    std::ostringstream oss;
    oss << "Паспорт: " << passportNumber
        << ", ФИО: " << getFullName();
    return oss.str();
}
