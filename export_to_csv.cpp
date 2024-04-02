#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

static int contactCount = 0;

class Contact {
public:
    string name, phoneNumber, email;
};

class HashTable {
public:
    int size;
    vector<Contact> *table;
};

void rehash(HashTable *ht) {
    cout << "Rehashing" << endl;
    int oldSize = ht->size;
    ht->size = 2 * oldSize;
    vector<Contact> *newTable = new vector<Contact>[ht->size];

    // Copy existing contacts to the new table
    for (int i = 0; i < oldSize; i++) {
        for (int j = 0; j < ht->table[i].size(); j++) {
            Contact c = ht->table[i][j];
            int index = c.name.length() % ht->size;
            while (newTable[index].size() != 0) {
                index = (index + 1) % ht->size;
            }
            newTable[index].push_back(c);
        }
    }
    // Delete old table and update ht->table
    delete[] ht->table;
    ht->table = newTable;
}

void insertContact(HashTable *ht, Contact c) {
    int index = c.name.length() % ht->size;
    float loadFactor = (float)contactCount / (float)ht->size;
    if (loadFactor > 0.75) {
        rehash(ht);
    }
    // Collision Handling using Linear Probing
    while (ht->table[index].size() != 0) {
        index = (index + 1) % ht->size;
    }
    ht->table[index].push_back(c);
    contactCount++;
}

bool contactExists(HashTable *ht, string phoneNumber) {
    for (int i = 0; i < ht->size; ++i) {
        for (auto contact : ht->table[i]) {
            if (contact.phoneNumber == phoneNumber) {
                return true;
            }
        }
    }
    return false;
}

void deleteContact(HashTable *ht, string phoneNumber) {
    for (int i = 0; i < ht->size; ++i) {
        for (auto it = ht->table[i].begin(); it != ht->table[i].end(); ++it) {
            if (it->phoneNumber == phoneNumber) {
                ht->table[i].erase(it);
                contactCount--;
                cout << "Contact deleted successfully" << endl;
                return;
            }
        }
    }
    cout << "Contact not found" << endl;
}

void exportToCSV(const vector<Contact>& contacts, const string& filename) {
    ofstream outputFile(filename);

    if (!outputFile.is_open()) {
        cerr << "Error opening file " << filename << endl;
        return;
    }

    // Write header row
    outputFile << "Name,Phone Number,Email" << endl;

    // Write each contact to the file
    for (const auto& contact : contacts) {
        outputFile << contact.name << "," << contact.phoneNumber << "," << contact.email << endl;
    }

    outputFile.close();
}

int main() {
    HashTable *ht = new HashTable();
    Contact c;
    ht->size = 10;
    ht->table = new vector<Contact>[ht->size];

    ifstream file("contacts.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, c.name, ',');
        getline(ss, c.phoneNumber, ',');
        getline(ss, c.email, ',');
        insertContact(ht, c);
    }

    while(true) {
        int choice;
        cout << "1. Add a Contact" << endl;
        cout << "2. Delete a Contact" << endl;
        cout << "3. Export Contacts to CSV" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        string phoneNumber;

        switch(choice) {
            case 1:
                cout << "Enter the Name: ";
                cin.ignore();
                getline(cin, c.name);
                cout << "Enter the Phone Number: ";
                getline(cin, c.phoneNumber);
                // Check if the Phone Number is Valid
                if(c.phoneNumber.length() != 10) {
                    cout << "Invalid Phone Number" << endl;
                    break;
                }
                // Search if the Contact exists with the same number
                if (contactExists(ht, c.phoneNumber)) {
                    cout << "Contact already exists" << endl;
                    break;
                }
                cout << "Enter the Email: ";
                cin >> c.email;
                insertContact(ht, c);
                break;
            case 2:
                cout << "Enter the Phone Number to delete: ";
                cin.ignore(); // Clear the input buffer
                getline(cin, phoneNumber);
                deleteContact(ht, phoneNumber);
                break;
            case 3:
                exportToCSV(*ht->table, "contacts.csv");
                cout << "Contacts exported to contacts.csv successfully." << endl;
                break;
            case 4:
                cout << "Exiting" << endl;
                return 0;
            default:
                cout << "Invalid choice" << endl;
                break;
        }

        ofstream outfile("contacts.txt");
        for (int i = 0; i < ht->size; i++) {
            for (int j = 0; j < ht->table[i].size(); j++) {
                outfile << ht->table[i][j].name << "," << ht->table[i][j].phoneNumber << "," << ht->table[i][j].email << endl;
            }
        }
    }
    file.close();
}