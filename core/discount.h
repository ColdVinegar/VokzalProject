#ifndef DISCOUNT_H
#define DISCOUNT_H

#include <string>
#include <vector>
#include <memory>
#include "types.h"

class DiscountStrategy {
protected:
    DiscountInfo info;

public:
    DiscountStrategy(const std::string& name = "", float percentage = 0.0f,
                     const std::string& description = "")
        : info(name, percentage, description) {}

    virtual ~DiscountStrategy() = default;

    // Основной метод применения скидки
    virtual float applyDiscount(float price) const = 0;

    // Получение информации о скидке
    virtual DiscountInfo getDiscountInfo() const { return info; }
    virtual std::string getDiscountName() const { return info.name; }
    virtual float getDiscountPercentage() const { return info.percentage; }
    virtual std::string getDiscountDescription() const { return info.description; }

    // Установка информации о скидке
    virtual void setDiscountInfo(const DiscountInfo& newInfo) { info = newInfo; }

    // Клонирование скидки
    virtual std::unique_ptr<DiscountStrategy> clone() const = 0;
};

// Базовая скидка "Без скидки"
class NoDiscount : public DiscountStrategy {
public:
    NoDiscount();

    float applyDiscount(float price) const override;
    std::string getDiscountName() const override;
    float getDiscountPercentage() const override;
    std::unique_ptr<DiscountStrategy> clone() const override;
};

// Класс для создания пользовательских скидок
class CustomDiscount : public DiscountStrategy {
public:
    CustomDiscount(const DiscountInfo& discountInfo);
    CustomDiscount(const std::string& name, float percentage,
                   const std::string& description = "");

    float applyDiscount(float price) const override;
    std::unique_ptr<DiscountStrategy> clone() const override;
};

// Класс для управления всеми доступными скидками
class DiscountManager {
private:
    std::vector<std::unique_ptr<DiscountStrategy>> availableDiscounts;

public:
    DiscountManager();

    // Получение всех доступных скидок
    std::vector<DiscountInfo> getAllDiscounts() const;

    // Получение скидки по имени
    std::unique_ptr<DiscountStrategy> getDiscountByName(const std::string& name) const;

    // Получение скидки по индексу
    std::unique_ptr<DiscountStrategy> getDiscountByIndex(size_t index) const;

    // Добавление новой пользовательской скидки
    bool addCustomDiscount(const DiscountInfo& discountInfo);

    // Удаление скидки по имени (кроме "Без скидки")
    bool removeDiscount(const std::string& name);

    // Редактирование существующей скидки
    bool editDiscount(const std::string& oldName, const DiscountInfo& newInfo);

    // Проверка существования скидки
    bool discountExists(const std::string& name) const;

    // Получение количества скидок
    size_t getDiscountCount() const;

    // Очистка всех пользовательских скидок
    void clearCustomDiscounts();
};

#endif // DISCOUNT_H
