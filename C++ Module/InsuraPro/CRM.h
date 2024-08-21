#ifndef CRM_H
#define CRM_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include "nlohmann/json.hpp"

// For convenience
#include "Customer.h"
#include "Interaction.h"

// Ensure compatibility with C++17 or later for std::filesystem
#if __cplusplus < 201703L
#error "This code requires C++17 or later."
#endif

using json = nlohmann::json;

class CRM {
public:
    CRM() : filename("InsuraProCRM.json"), idCounter(0) {}

    // Add a customer with a unique ID
    bool addCustomer(const std::string& name, const std::string& surname) {
        bool isUnique = false;
        int id = 0;

        while (!isUnique) {
            id = generateUniqueId();
            isUnique = true; // Assume the ID is unique unless found otherwise

            for (const auto& customer : customers) {
                if (customer.getId() == id) {
                    isUnique = false; 
                }
            }
        }


        customers.emplace_back(name, surname, id);
        return true;
    }

    // Display all customers and their interactions
    void displayCustomers() const {
        for (const auto& customer : customers) {
            std::cout << "ID: " << customer.getId()
                      << ", Name: " << customer.getName()
                      << ", Surname: " << customer.getSurname() << std::endl;
            std::cout << "-- Interactions --" << std::endl;
            for (const auto& interaction : customer.getInteractions()) {
                std::cout << "Type: " << interaction.getType()
                          << ", Details: " << interaction.getDetails() << std::endl;
            }
            std::cout << std::endl;
        }
    }

    // Display a specific customer and their interactions by ID
    void displayOnlyCustomerById(int id) const {
        for (const auto& customer : customers) {
            if (customer.getId() == id) {
                std::cout << "ID: " << customer.getId()
                        << ", Name: " << customer.getName()
                        << ", Surname: " << customer.getSurname() << std::endl;
                return;
            }
        }
        // If no customer with the given ID is found
        std::cout << "Customer with ID " << id << " not found." << std::endl;
    }

    // Display a specific customer and their interactions by ID
    void displayFullCustomerById(int id) const {
        for (const auto& customer : customers) {
            if (customer.getId() == id) {
                std::cout << "ID: " << customer.getId()
                        << ", Name: " << customer.getName()
                        << ", Surname: " << customer.getSurname() << std::endl;
                std::cout << "-- Interactions --" << std::endl;
                for (const auto& interaction : customer.getInteractions()) {
                    std::cout << "Type: " << interaction.getType()
                            << ", Details: " << interaction.getDetails() << std::endl;
                }
                std::cout << std::endl;
                return; 
            }
        }
        // If no customer with the given ID is found
        std::cout << "Customer with ID " << id << " not found." << std::endl;
    }

    // Search for a customer by ID or name
    std::vector<Customer> searchCustomer(const std::string& query) const {
        // In case of name and surname, there might be more matches
        std::vector<Customer> matches;
        for (const auto& customer : customers) {
            if (customer.getName() == query || customer.getSurname() == query || std::to_string(customer.getId()) == query) {
                matches.push_back(customer);
            }
        }
        return matches;
    }

    // Edit a customer's details by their ID
    void editCustomer(int id, const std::string& newName, const std::string& newSurname) {
        for (auto& customer : customers) {
            if (customer.getId() == id) {
                customer.setName(newName);
                customer.setSurname(newSurname);
                std::cout << "Customer details updated successfully!" << std::endl;
                return;
            }
        }
        std::cout << "Customer not found." << std::endl;
    }

    // Delete a customer by their ID
    void deleteCustomer(int id) {
        auto it = std::remove_if(customers.begin(), customers.end(),
                                 [id](const Customer& customer) {
                                     return customer.getId() == id;
                                 });
        if (it != customers.end()) {
            customers.erase(it, customers.end());
            std::cout << "Customer deleted successfully!" << std::endl;
        } else {
            std::cout << "Customer not found." << std::endl;
        }
    }

    // Add an interaction to a customer by ID
    bool addInteractionToCustomer(int id, const Interaction& interaction) {
        for (auto& customer : customers) {
            if (customer.getId() == id) {
                customer.addInteraction(interaction);
                return true;
            }
        }
        return false;
    }

    bool saveData() const {
        std::filesystem::path filePath = filename;
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
            return false;
        }

        json j;

        // Convert customer data to JSON
        for (const auto& customer : customers) {
            json customerJson;
            customerJson["id"] = customer.getId();
            customerJson["name"] = customer.getName();
            customerJson["surname"] = customer.getSurname();

            // Convert interactions to JSON
            json interactionsJson = json::array();
            for (const auto& interaction : customer.getInteractions()) {
                json interactionJson;
                interactionJson["type"] = interaction.getType();
                interactionJson["details"] = interaction.getDetails();
                interactionsJson.push_back(interactionJson);
            }

            customerJson["interactions"] = interactionsJson;
            j["customers"].push_back(customerJson);
        }

        // Write JSON to file
        file << j.dump(4); // dump(4) formats the JSON with an indentation of 4 spaces

        file.close();
        return true;
    }

    // Load customer data from a file
    bool loadData() {
        std::filesystem::path filePath = filename;
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for reading: " << filePath << std::endl;
            return false;
        }

        json j;
        file >> j;  // Parse the JSON file into the json object

        customers.clear();
        int maxId = 0;

        // Iterate over the JSON array of customers
        for (const auto& customerJson : j["customers"]) {
            int id = customerJson["id"];
            std::string name = customerJson["name"];
            std::string surname = customerJson["surname"];

            customers.emplace_back(name, surname, id);

            // Update maxId if the current id is greater
            if (id > maxId) {
                maxId = id;
            }

            // Load interactions
            for (const auto& interactionJson : customerJson["interactions"]) {
                std::string type = interactionJson["type"];
                std::string details = interactionJson["details"];
                customers.back().addInteraction(Interaction(type, details));
            }
        }

        file.close();
        return true;
    }

private:
    // Unique ID generator for new customers
    int generateUniqueId() {
        return ++idCounter;
    }

    int idCounter; // Counter to keep track of unique IDs
    std::vector<Customer> customers;
    std::string filename;
};

#endif // CRM_H
