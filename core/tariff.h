#ifndef TARIFF_H
#define TARIFF_H

#include "types.h"
#include "discount.h"
#include <string>
#include <memory>

class Tariff {
private:
    std::string name;
    float basePrice;
    VagonType vagonType;
    std::unique_ptr<DiscountStrategy> discountStrategy;

public:
    Tariff(const std::string& name, float price, VagonType type,
           std::unique_ptr<DiscountStrategy> discount = std::make_unique<NoDiscount>());

    // Запрещаем копирование
    Tariff(const Tariff&) = delete;
    Tariff& operator=(const Tariff&) = delete;

    // Разрешаем перемещение
    Tariff(Tariff&&) = default;
    Tariff& operator=(Tariff&&) = default;

    virtual ~Tariff() = default;

    std::string getName() const;
    float getBasePrice() const;
    VagonType getVType() const;
    DiscountStrategy* getDiscount() const;
    DiscountInfo getDiscountInfo() const;

    void setName(const std::string& newName);
    void setBasePrice(float newPrice);
    void setVType(VagonType newType);
    void setDiscountFromManager(DiscountManager& manager, const std::string& discountName);

    // Расчет цены
    float calculatePrice(bool withoutDiscount = false) const;

    // Информация
    std::string getVagonTypeString() const;
    std::string getInfo() const;
};

#endif // TARIFF_H
