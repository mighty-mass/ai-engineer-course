#ifndef INTERACTION_H
#define INTERACTION_H

#include <string>
#include <iostream>

class Interaction {
public:
    Interaction(const std::string& type, const std::string& details)
        : type(type), details(details) {}

    std::string getType() const { return type; }
    std::string getDetails() const { return details; }

private:
    std::string type;  // e.g., "Appointment", "Contract"
    std::string details;
};

#endif // INTERACTION_H