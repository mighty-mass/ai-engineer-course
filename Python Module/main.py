
from ContactEase.ContactEase import ContactEase
import re
import os
import time



# Extra: phone number validation. ChatGPT provided a better version of the one i wrote
PHONE_NUMBER_PATTERN = re.compile(r'''
    ^                             # Start of the string
    (\+|00)?                      # Optional international prefix (+ or 00)
    (\d{1,3})?                    # Optional country code (1 to 3 digits)
    [\s.-]?                       # Optional separator (space, dot, or dash)
    (\(?\d{2,4}\)?)?              # Optional area code (2 to 4 digits, optional parentheses)
    [\s.-]?                       # Optional separator (space, dot, or dash)
    (\d{2,4})                     # First part of the phone number (2 to 4 digits)
    [\s.-]?                       # Optional separator (space, dot, or dash)
    (\d{2,4})                     # Second part of the phone number (2 to 4 digits)
    [\s.-]?                       # Optional separator (space, dot, or dash)
    (\d{2,4})?                    # Optional third part of the phone number (2 to 4 digits)
    $                             # End of the string
''', re.VERBOSE)

def clear_screen(seconds):
    # Keep the scren clear after every iteration with the menu
    # Choose your version based on your os
    # 4 seconds seems a reasonable time to double check the results
    print("\n\nScreen will be cleared in")
    for i in range(1,seconds):
        print(f"{seconds-i} seconds")
        time.sleep(1)

    if os.name == 'nt':
        # Windows
        os.system('cls')
    else:
        # Linux
        os.system('clear')

def check_input(text, allowed_responses):
    value= input(text)
    value = value.lower().strip()

    while value not in allowed_responses:
        print("Seems that your chioce is not allowed. Please try again")
        value= input(text)
        value = value.lower()

    return value

# ChatGPT helped correctly set up the padding and length
def print_title(title):
    border_char = '#'
    title_length = len(title)
    border_length = title_length + 4  # Adding 4 for padding and borders

    # Print the border, title, and border
    print("\n")
    print(border_char * border_length)
    print(f"{border_char} {title} {border_char}")
    print(border_char * border_length)
    print("\n")

# Previously written using only prints but still used a manual value for menu size
def display_menu():
    print_title("ContactEase Menu")
    menu_options = [
        "1) Add a contact",
        "2) Remove a contact",
        "3) Modify a contact",
        "4) Show all contacts",
        "5) Search a contact",
        "6) Exit\n\n"
    ]
    for option in menu_options:
        print(option)
    
    return len(menu_options)


# Search and delete method moved to his own function so it can be used both for modify and remove
def search_and_collect(rubrica, method="delete"):
    surname = input(f"Which contact would you like to {method}? [surname]: ")
    matching_contacts = rubrica.search_contacts_by_surname(surname)
    if len(matching_contacts) > 2:
        print(f"I found more than one. Which contact would you like to {method}?")
        for x in matching_contacts:
            print(f"{matching_contacts.index(x)+1}) {x}")
        c_choice = check_input(f"Choose one option using the respective number [1-{len(matching_contacts)}]: ", [f"{i}" for i in range(1, len(matching_contacts)+1)])

        matching_contact = matching_contacts[int(c_choice)-1]
        rubrica.remove_contact(matching_contact.get_surname(), matching_contact.get_name(),matching_contact.get_phone_number())
        return matching_contact
    elif len(matching_contacts) == 1:
        rubrica.remove_contact(surname)
        return matching_contacts[0]
    
    return None

if __name__ == "__main__":
    rubrica = ContactEase()
    print_title("Welcome to ContactEase")
    print("Looking for saved contact list...")

    if rubrica.load_contact_list():
        print("Saved contact list loaded!")
    else:
        print("No saved contact list have been found")

    clear_screen(4)

    # Loop menu 
    choice = None
    while choice!= "exit":
        OPTIONS = display_menu()
        choice = check_input(f"Choose one option using the respective number [1-{OPTIONS}]: ", [f"{i}" for i in range(1, OPTIONS+1)])
        #print(choice)

        clear_screen(1)
        if choice == "1":
            #Add a New Contact
            name = input("New contact name: ")
            surname = input("New contact surname: ")
            phone_number = "fake"
            while not PHONE_NUMBER_PATTERN.match(phone_number):
                phone_number = input("New contact phone number, digit only [prefix supported]: ")

            if rubrica.add_contact([surname, name, phone_number]):
                print("New contact added to your contact list")
            else:
                print("Impossible to add the new contact")

        elif choice == "2":
            #Remove a Contact
            rm_contact = search_and_collect(rubrica)

            if rm_contact is not None:
                print(f"\nContact '{rm_contact}' has been delete")
            else:
                print("\nNo contact matched the criteria")

        elif choice == "3":
            #Modify a Contact
            modify_contact = search_and_collect(rubrica,"modify")

            if modify_contact:
                name = input(f"Modify contact name [{modify_contact.get_name()}]: ")
                surname = input(f"Modify contact surname [{modify_contact.get_surname()}]: ")
                phone_number = "fake"
                while not PHONE_NUMBER_PATTERN.match(string=phone_number) and phone_number!="":
                    phone_number = input(f"Modify contact phone number [{modify_contact.get_phone_number()}], digit only [prefix supported]: ")

                name = name or modify_contact.get_name()
                surname = surname or modify_contact.get_surname()
                phone_number= phone_number or modify_contact.get_phone_number()

                if rubrica.add_contact([surname, name, phone_number]):
                    print("Contact modified. Added to your contact list")
                    print(f"Previous ---> {modify_contact}")
                    print(f"Current ---> {surname} {name}: {phone_number}")

                else:
                    print("Impossible to modify the chosen contact")
                
        elif choice == "4":
            #Show all contacts
            print_title("Contact List")
            rubrica.show_contact_list()

        elif choice == "5":
            #Search a contact (or all the matched ones)
            choice = check_input(f"Search via surname or name? [s/n]:", ["s","n"])
            if choice == "s":
                surname = input("Which surname are you looking for? [surname]: ")
                matching_contacts = rubrica.search_contacts_by_surname(surname)
            else:
                name = input("Which name are you looking for?  [name]: ")
                matching_contacts = rubrica.search_contacts_by_name(name)
            
            if len(matching_contacts) > 0:
                print_title("Contact(s) Found")
                for contact in matching_contacts:
                    print(contact)
            else:
                print("No matching contact found")

        else:
            # Save and Exit
            if rubrica.save_contact_list():
                print("Contact list saved successfully")
            else:
                print("Somenthing went wrong while saving the contact list")
            
            print("\n")
            print_title("Have a nice day!")
            break
        

        