#include "discount.h"
#include <algorithm>

NoDiscount::NoDiscount()
    : DiscountStrategy("Без скидки", 0.0f, "Полная стоимость") {}

float NoDiscount::applyDiscount(float price) const {
    return price;
}

std::string NoDiscount::getDiscountName() const {
    return "Без скидки";
}

float NoDiscount::getDiscountPercentage() const {
    return 0.0f;
}

std::unique_ptr<DiscountStrategy> NoDiscount::clone() const {
    return std::make_unique<NoDiscount>();
}

CustomDiscount::CustomDiscount(const DiscountInfo& discountInfo)
    : DiscountStrategy(discountInfo.name, discountInfo.percentage,
                       discountInfo.description) {}

CustomDiscount::CustomDiscount(const std::string& name, float percentage,
                               const std::string& description)
    : DiscountStrategy(name, percentage, description) {}

float CustomDiscount::applyDiscount(float price) const {
    DiscountInfo info = getDiscountInfo();
    if (info.percentage < 0) return price;
    if (info.percentage > 100) return 0;

    return price * (1.0f - info.percentage / 100.0f);
}

std::unique_ptr<DiscountStrategy> CustomDiscount::clone() const {
    return std::make_unique<CustomDiscount>(getDiscountInfo());
}

DiscountManager::DiscountManager() {
    availableDiscounts.push_back(std::make_unique<NoDiscount>());
}

std::vector<DiscountInfo> DiscountManager::getAllDiscounts() const {
    std::vector<DiscountInfo> result;
    for (const auto& discount : availableDiscounts) {
        result.push_back(discount->getDiscountInfo());
    }
    return result;
}

std::unique_ptr<DiscountStrategy> DiscountManager::getDiscountByName(const std::string& name) const {
    auto it = std::find_if(availableDiscounts.begin(), availableDiscounts.end(),
                           [&name](const std::unique_ptr<DiscountStrategy>& discount) {
                               return discount->getDiscountInfo().name == name;
                           });

    if (it != availableDiscounts.end()) {
        return (*it)->clone();
    }

    return nullptr;
}

std::unique_ptr<DiscountStrategy> DiscountManager::getDiscountByIndex(size_t index) const {
    if (index < availableDiscounts.size()) {
        return availableDiscounts[index]->clone();
    }

    return nullptr;
}

bool DiscountManager::addCustomDiscount(const DiscountInfo& discountInfo) {
    if (discountInfo.name.empty()) {
        return false;
    }

    if (discountInfo.percentage < 0 || discountInfo.percentage > 100) {
        return false;
    }

    if (discountExists(discountInfo.name)) {
        return false;
    }

    if (discountInfo.name == "Без скидки") {
        return false;
    }

    availableDiscounts.push_back(
        std::make_unique<CustomDiscount>(discountInfo)
        );

    return true;
}

bool DiscountManager::removeDiscount(const std::string& name) {
    if (name == "Без скидки") {
        return false;
    }

    auto it = std::find_if(availableDiscounts.begin(), availableDiscounts.end(),
                           [&name](const std::unique_ptr<DiscountStrategy>& discount) {
                               return discount->getDiscountInfo().name == name;
                           });

    if (it != availableDiscounts.end()) {
        availableDiscounts.erase(it);
        return true;
    }

    return false;
}

bool DiscountManager::editDiscount(const std::string& oldName, const DiscountInfo& newInfo) {
    if (oldName == "Без скидки") {
        return false;
    }

    if (newInfo.name.empty()) {
        return false;
    }

    if (newInfo.percentage < 0 || newInfo.percentage > 100) {
        return false;
    }

    auto it = std::find_if(availableDiscounts.begin(), availableDiscounts.end(),
                           [&oldName](const std::unique_ptr<DiscountStrategy>& discount) {
                               return discount->getDiscountInfo().name == oldName;
                           });

    if (it != availableDiscounts.end()) {
        if (oldName != newInfo.name && discountExists(newInfo.name)) {
            return false;
        }

        (*it)->setDiscountInfo(newInfo);
        return true;
    }

    return false;
}


bool DiscountManager::discountExists(const std::string& name) const {
    return std::any_of(availableDiscounts.begin(), availableDiscounts.end(),
                       [&name](const std::unique_ptr<DiscountStrategy>& discount) {
                           return discount->getDiscountInfo().name == name;
                       });
}

size_t DiscountManager::getDiscountCount() const {
    return availableDiscounts.size();
}

void DiscountManager::clearCustomDiscounts() {
    availableDiscounts.erase(
        std::remove_if(availableDiscounts.begin(), availableDiscounts.end(),
                       [](const std::unique_ptr<DiscountStrategy>& discount) {
                           return discount->getDiscountInfo().name != "Без скидки";
                       }),
        availableDiscounts.end()
        );
}
