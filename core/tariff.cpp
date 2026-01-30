#include "tariff.h"
#include <sstream>
#include <iomanip>

Tariff::Tariff(const std::string& name, float price, VagonType type,
               std::unique_ptr<DiscountStrategy> discount)
    : name(name), basePrice(price), vagonType(type),
    discountStrategy(std::move(discount)) {
}

std::string Tariff::getName() const {
    return name;
}

float Tariff::getBasePrice() const {
    return basePrice;
}

VagonType Tariff::getVType() const {
    return vagonType;
}

DiscountStrategy* Tariff::getDiscount() const {
    return discountStrategy.get();
}

DiscountInfo Tariff::getDiscountInfo() const {
    if (discountStrategy) {
        return discountStrategy->getDiscountInfo();
    }
    return DiscountInfo("Без скидки", 0.0f);
}

void Tariff::setName(const  std::string& newName){
    name = newName;
}

void Tariff::setBasePrice(float newPrice){
    basePrice = newPrice;
}

void Tariff::setVType(VagonType newType){
    vagonType = newType;
}

// Установка скидки из менеджера
void Tariff::setDiscountFromManager(DiscountManager& manager, const std::string& discountName) {
    auto discount = manager.getDiscountByName(discountName);
    if (discount) {
        discountStrategy = std::move(discount);
    }
}

// Расчет цены
float Tariff::calculatePrice(bool withoutDiscount) const {
    float multiplier = 1.0f;
    switch (vagonType) {
    case PLAC: multiplier = 1.5f; break;
    case KUPE: multiplier = 2.0f; break;
    case SIT:  multiplier = 1.0f; break;
    }

    float price = basePrice * multiplier;

    if (withoutDiscount) {
        return price;
    }

    return discountStrategy->applyDiscount(price);
}

std::string Tariff::getVagonTypeString() const {
    switch (vagonType) {
    case PLAC: return "Плацкарт";
    case KUPE: return "Купе";
    case SIT:  return "Сидячий";
    default:   return "Неизвестный";
    }
}

std::string Tariff::getInfo() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    DiscountInfo discountInfo = getDiscountInfo();

    oss << "Направление: " << name << "\n"
        << "Тип вагона: " << getVagonTypeString() << "\n"
        << "Базовая цена: " << basePrice << "\n"
        << "Цена с учетом типа: " << calculatePrice(true) << "\n"
        << "Скидка: " << discountInfo.name
        << " (" << discountInfo.percentage << "%)\n";

    if (!discountInfo.description.empty()) {
        oss << "Описание: " << discountInfo.description << "\n";
    }

    oss << "Итоговая цена: " << calculatePrice(false);

    return oss.str();
}
