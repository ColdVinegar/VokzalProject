#ifndef TYPES_H
#define TYPES_H

#include <string>

typedef enum {
    SIT,
    PLAC,
    KUPE
} VagonType;

struct DiscountInfo {
    std::string name;
    float percentage;
    std::string description;

    DiscountInfo(const std::string& n = "", float p = 0.0f,
                 const std::string& d = "")
        : name(n), percentage(p), description(d) {}
};

#endif // TYPES_H
