#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <vector>
#include "Interaction.h"

class Customer {
public:
    // Constructor to initialize a customer with an ID, name, and surname
    Customer(const std::string& name, const std::string& surname, int id)
        : name(name), surname(surname), id(id) {}

    // Getters and setters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getSurname() const { return surname; }
    void setName(const std::string& newName) { name = newName; }
    void setSurname(const std::string& newSurname) { surname = newSurname; }

    // Add an interaction to the customer
    void addInteraction(const Interaction& interaction) {
        interactions.push_back(interaction);
    }

    // Get a list of interactions for the customer
    std::vector<Interaction> getInteractions() const {
        return interactions;
    }

private:
    // ID for the customer
    // will be managed as unique by the CRM class
    int id; 
    std::string name;
    std::string surname;
    std::vector<Interaction> interactions;
};

#endif // CUSTOMER_H
