import os
import string

# Simple base Contact class
# I've added the built-in methods for print and comparison
# Sorting contacts to reduce searching time (for huge use cases)
class Contact:
    def __init__(self, *args) -> None:
        self.surname = args[0][0]
        self.name = args[0][1]
        self.phone_number = args[0][2]
    
    def get_surname(self):
        return f"{self.surname}"
    
    def get_name(self):
        return f"{self.name}"
    
    # def get_full_name(self):
    #     return f"{self.surname} {self.name}"

    def get_phone_number(self):
        return f"{self.phone_number}"
    
    def get_csv_format(self):
        return f"{self.surname},{self.name},{self.phone_number}"
    
    def __repr__(self) -> str:
        return f"{self.surname} {self.name}: {self.phone_number}"
    
    def __lt__(self, other):
        return self.surname < other.surname
    

# Child class that represent a list of contacts
# I've moved from simple list to a dictionary to reduce the amount of contact to iterate
# on searching operation. For now, I've emulated the one on mobile phones that use the first 
# letter of the surname
class ContactEase(Contact):
    def __init__(self) -> None:
        self.contact_list = {x:[] for x in string.ascii_uppercase}
        self.CONTACTS_FILE_NAME = "ContactEase.csv"

    def _contact_list_exists(self):
        return os.path.isfile(self.CONTACTS_FILE_NAME)

    # Save the contact list as a CSV for redability
    # We don't need a JSON structure and can be easily substituted with a database
    def load_contact_list(self):
        # Check existing contact list
        if self._contact_list_exists():
            with open(self.CONTACTS_FILE_NAME) as f:
                header = True
                for line in f.readlines():
                    if header:
                        header = False
                    else:
                        self.add_contact(line.split(","))
            return True

        return False

    # Quick search reducing the total search by 1/26 (alphabeth letters)
    # and traverse only once the list of contact for that letters
    # O(n) for our nerd friends
    def search_contacts_by_surname(self, surname):
        first_letter = surname[0].upper()

        contacts_found = []
        for contact in self.contact_list[first_letter]:
                if contact.get_surname().lower() == surname.lower():
                    contacts_found.append(contact)
        
        return contacts_found

    # Since it was requested I added also the search based on names
    # Not faster as the surname one, but still reduced to level 1 depth
    def search_contacts_by_name(self, name):
        all_contacts = []
        for value in  self.contact_list.values():
            all_contacts.extend(value)

        contacts_found = []
        for contact in all_contacts:
            if contact.get_name().lower() == name.lower():
                contacts_found.append(contact)
        
        return contacts_found

    # Create the CSV with all the contacts
    def save_contact_list(self):
        try:
            with open(self.CONTACTS_FILE_NAME, "w") as f:
                f.writelines("surname,name,phone_number\n")
                for x in self.contact_list:
                    f.writelines([contact.get_csv_format() for contact in self.contact_list[x]])
            return True
        except Exception as e:
            print(e.with_traceback())
            return False

    # Add a new Contact object to our contact list
    def add_contact(self, *args):
        surname = args[0][0]
        if len(args[0]) == 3:
            first_letter = surname[0].upper()
            self.contact_list[first_letter].append(Contact(args[0]))
            self.contact_list[first_letter] = sorted(self.contact_list[first_letter])
            return True
        return False

    # Print out all the contact list divided by the initial character
    def show_contact_list(self):
        for x in self.contact_list:
            if len(self.contact_list[x]) > 0:
                print(f"\n{x.upper()}")
                for contact in self.contact_list[x]:
                    print(contact)

        print(f"\n---> Total: {sum([len(self.contact_list[x]) for x in self.contact_list])}")


    # Remove a Contact from the list
    # Tricky part on how to handle duplicates, but I've decided to emulate the functionality
    # of our phone, where you can have multiple copy of the same contact
    # NOTE: we could ask the user if he want to merge duplicated contacts
    def remove_contact(self, surname, name=None, phone_number=None):
        first_letter = surname[0].upper()

        index = 0        
        flag = True
        
        while flag:
            for contact in self.contact_list[first_letter]:
                if contact.get_surname().lower() == surname.lower():
                    if name is None and phone_number is None:
                        flag = False

                    if phone_number is not None and contact.get_name().lower() == name.lower():
                        flag = False

                    if name is not None and contact.get_phone_number() == phone_number:
                        flag = False
                    
                    if contact.get_phone_number() == phone_number and contact.get_name().lower() == name.lower():
                        flag = False
                else:
                    index+=1
        
        if index < len(self.contact_list[first_letter]):
            del self.contact_list[first_letter][index]
            return True
        else:
            return False    
