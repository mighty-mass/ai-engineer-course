#include <cstdlib>  // For system()
#include <chrono>   // For std::chrono
#include <thread>   // For std::this_thread::sleep_for
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "InsuraPro/CRM.h"
#include "InsuraPro/Customer.h"
#include "InsuraPro/Interaction.h"

void clearScreen(int seconds) {
    // Print countdown before clearing the screen
    std::cout << "\n\nScreen will be cleared in\n";
    for (int i = 1; i < seconds; ++i) {
        std::cout << (seconds - i) << " seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Clear the screen based on the operating system
    #ifdef _WIN32
        // Windows
        system("cls");
    #else
        // Linux and other Unix-like systems
        system("clear");
    #endif
}

// Function to print the title
void printTitle(const std::string& title) {
    std::string border(title.length(), '#');
    std::cout << border << std::endl;
    std::cout << title << std::endl;
    std::cout << border << std::endl;
}

// Function to check input against allowed responses
std::string checkInput(const std::string& text, const std::vector<std::string>& allowedResponses) {
    std::string value;
    std::cout << text;
    std::getline(std::cin, value);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

    while (std::find(allowedResponses.begin(), allowedResponses.end(), value) == allowedResponses.end()) {
        std::cout << "Seems that your choice is not allowed. Please try again. " << std::endl;
        std::cout << text;
        std::getline(std::cin, value);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
    }

    return value;
}

// Function to create a vector of strings from "1" to a given number
std::vector<std::string> createChoiceList(int maxNumber) {
    std::vector<std::string> stringVector;
    for (int i = 1; i <= maxNumber; ++i) {
        stringVector.push_back(std::to_string(i));
    }
    return stringVector;
}


// Function to look for a customer, generalised for all use cases
int findIdinCRM(CRM crm){
    std::string name;

    std::cout << "Enter customer info (id, name or surname): ";
    std::getline(std::cin >> std::ws, name);
    auto results = crm.searchCustomer(name);
    if (results.empty()) {
        return -1;
    } else {
        if (results.size() > 1){
            std::vector<std::string> allowedChoices;

            std::cout << "Found the following customers: " << std::endl;
            for (const auto& result : results) {
                crm.displayOnlyCustomerById(result.getId());
                allowedChoices.push_back(std::to_string(result.getId()));
            }

            int id = std::stoi(checkInput("Choose the customer ID: ", allowedChoices));
            return id;
        }

        return results[0].getId();
    }
}

// Function to display the menu and return the number of options
int displayMenu() {
    printTitle("InsuraPro Menu");
    std::vector<std::string> menuOptions = {
        "1) Add Customer",
        "2) Display Customers",
        "3) Search Customer",
        "4) Edit Customer",
        "5) Delete Customer",
        "6) Manage Interactions",
        "7) Exit"
    };

    for (const auto& option : menuOptions) {
        std::cout << option << std::endl;
    }

    return menuOptions.size();
}

int main() {
    std::string choice;
    CRM crm;
    std::string name, surname, type, details;

    printTitle("Welcome to InsuraPro!");

    // Load the CRM from a JSON
    if (crm.loadData()) {
        std::cout << "Found a CRM backup! Loading data..." << std::endl;
    } else {
        std::cout << "Empty CRM initialized" << std::endl;
    }
    clearScreen(4);

    do {
        int choiceLength = displayMenu();
        std::vector<std::string> allowedResponses = createChoiceList(choiceLength);

        choice = checkInput("Choose an option using the respective number [1-" + std::to_string(choiceLength) + "]: ", allowedResponses);

        clearScreen(1);
        // Handle the menu choice
        switch (std::stoi(choice)) {
            // Add new customer
            case 1:
                std::cout << "Enter customer name: ";
                std::getline(std::cin >> std::ws, name); // using std::ws to ignore leading whitespace
                std::cout << "Enter customer surname: ";
                std::getline(std::cin, surname);
                crm.addCustomer(name, surname); // ID will be assigned by CRM
                std::cout << "Customer added successfully! " << std::endl;
                break;
            // Display all the customers (intercations included)
            case 2:
                crm.displayCustomers();
                break;
            // Search for a customer
            case 3: {
                int id = findIdinCRM(crm);
                crm.displayFullCustomerById(id);
                break;
            }
            // Search for a customer and edit its information
            case 4: {
                int id = findIdinCRM(crm);
                std::cout << "Enter new customer name: ";
                std::getline(std::cin, name);
                std::cout << "Enter new customer surname: ";
                std::getline(std::cin, surname);
                crm.editCustomer(id, name, surname);
                std::cout << "Customer sucessfully updated!" << std::endl;
                break;
            }
            // Delete a customer
            case 5: {
                int id = findIdinCRM(crm);
                crm.deleteCustomer(id);
                std::cout << "Customer deleted!" << std::endl;
                break;
            }
            // Edit interactions informations of a specific customer
            case 6: {
                int id = findIdinCRM(crm);
                std::cout << "Enter interaction type (e.g., Appointment, Contract): ";
                std::getline(std::cin, type);
                std::cout << "Enter interaction details: ";
                std::getline(std::cin, details);
                crm.addInteractionToCustomer(id, Interaction(type, details));
                std::cout << "Interaction added to customer!" << std::endl;
                break;
            }
            // Exit and save data in a JSON format
            case 7:
                if (crm.saveData()) {
                    std::cout << "CRM backup created!" << std::endl;
                } else {
                    std::cout << "Error while creating CRM backup. All data will be lost!" << std::endl;
                }
                break;
            default:
                std::cout << "Invalid option. Please try again. " << std::endl;
                break;
        }
    } while (choice != "7");

    printTitle("Goodbye!");
    return 0;
}
