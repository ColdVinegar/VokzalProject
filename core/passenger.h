#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>

class Passenger {
private:
    int passportNumber;
    std::string firstName;
    std::string lastName;

public:
    Passenger(int passport, const std::string& fname, const std::string& lname);

    int getPassport() const;
    std::string getFirstName() const;
    std::string getLastName() const;

    void setPassport(int newPassport);
    void setFName(const std::string& newFName);
    void setLName(const std::string& newLName);

    std::string getFullName() const;
    std::string getInfo() const;
};

#endif // PASSENGER_H
