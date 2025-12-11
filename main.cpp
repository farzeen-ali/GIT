#include <iostream>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

// ---------------- STRUCTURES ----------------

struct Transaction {
    int id;
    string type;
    float amount;
    string note;
    string date; // YYYY-MM
};

struct Account {
    string name;
    int pin;
};

// ---------------- PIN VALIDATION ----------------

bool isValidPIN(const string &pin) {
    if (pin.length() != 4) return false;
    for (char c : pin) if (!isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

// ---------------- SAFE STRING -> INT FOR PIN ----------------
// (No exception handling)
bool stringToInt(const string &s, int &num) {
    if (!isValidPIN(s)) return false;
    num = (s[0]-'0')*1000 + (s[1]-'0')*100 + (s[2]-'0')*10 + (s[3]-'0');
    return true;
}

// ---------------- ACCOUNT FUNCTIONS ----------------

void saveAccount(Account acc) {
    ofstream file("account.txt");
    if (!file) {
        cout << "Error saving account!\n";
        return;
    }
    file << acc.name << endl;
    file << acc.pin << endl;
}

bool loadAccount(Account &acc) {
    ifstream file("account.txt");
    if (!file) return false;

    string pinLine;

    if (!getline(file, acc.name)) return false;
    if (!getline(file, pinLine)) return false;

    int tempPin;
    if (!stringToInt(pinLine, tempPin)) {
        cout << "Invalid input!\n";
        return false;
    }

    acc.pin = tempPin;
    return true;
}

// -------- CREATE ACCOUNT --------

void createAccount(Account &acc) {
    cout << "\n--- Create Your Financial Account ---\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter your full name: ";
    getline(cin, acc.name);

    string pinInput;
    int attempts = 0;

    while (attempts < 3) {
        cout << "Set a 4-digit PIN: ";
        cin >> pinInput;

        if (isValidPIN(pinInput)) {
            stringToInt(pinInput, acc.pin);
            saveAccount(acc);
            cout << "Account created successfully!\n";
            return;
        }

        cout << "Invalid input!\n";
        attempts++;
    }

    cout << "Too many invalid attempts.\n";
}

// -------- CHECK PIN --------

bool checkPIN(Account acc) {
    string pinInput;
    int attempts = 0;
    int temp;

    while (attempts < 3) {
        cout << "Enter PIN: ";
        cin >> pinInput;

        if (!stringToInt(pinInput, temp)) {
            cout << "Invalid input!\n";
        } else if (temp == acc.pin) {
            cout << "Access granted.\n";
            return true;
        } else {
            cout << "Incorrect PIN!\n";
        }

        attempts++;
    }

    cout << "Too many failed attempts.\n";
    return false;
}

// -------- RESET PIN --------

void resetPIN(Account &acc) {
    string oldPinInput;
    int attempts = 0;
    int temp;

    while (attempts < 3) {
        cout << "Enter current PIN: ";
        cin >> oldPinInput;

        if (stringToInt(oldPinInput, temp) && temp == acc.pin) break;

        cout << "Invalid input!\n";
        attempts++;
    }

    if (attempts == 3) {
        cout << "Too many invalid attempts.\n";
        return;
    }

    string newPin;
    attempts = 0;

    while (attempts < 3) {
        cout << "Enter NEW 4-digit PIN: ";
        cin >> newPin;

        if (stringToInt(newPin, acc.pin)) {
            saveAccount(acc);
            cout << "PIN reset successfully!\n";
            return;
        }

        cout << "Invalid input!\n";
        attempts++;
    }

    cout << "Too many invalid attempts.\n";
}

// ---------------- TRANSACTION FUNCTIONS ----------------

void addTransaction(Transaction record[], int &totalRecords) {
    cout << "\nEnter type (income/expense/saving): ";
    cin >> record[totalRecords].type;

    if (record[totalRecords].type != "income" &&
        record[totalRecords].type != "expense" &&
        record[totalRecords].type != "saving") {
        cout << "Invalid input!\n";
        return;
    }

    cout << "Enter amount: ";
    if (!(cin >> record[totalRecords].amount)) {
        cout << "Invalid input!\n";
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    cout << "Enter note: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, record[totalRecords].note);

    cout << "Enter date (YYYY-MM): ";
    cin >> record[totalRecords].date;

    record[totalRecords].id = totalRecords + 1;
    totalRecords++;

    cout << "Transaction added.\n";
}

void checkBalance(Transaction record[], int totalRecords) {
    float income = 0, expense = 0, savings = 0;

    for (int i = 0; i < totalRecords; i++) {
        if (record[i].type == "income") income += record[i].amount;
        else if (record[i].type == "saving") savings += record[i].amount;
        else if (record[i].type == "expense") expense += record[i].amount;
    }

    cout << "\n--- Current Balance ---\n";
    cout << "Main Balance: " << income - expense << endl;
    cout << "Savings Balance: " << savings << endl;
    cout << "Total Balance: " << (income - expense + savings) << endl;
}

void showTransactions(Transaction record[], int totalRecords) {
    cout << "\n--- Transaction History ---\n";

    for (int i = 0; i < totalRecords; i++) {
        cout << "ID: " << record[i].id
             << " | Type: " << record[i].type
             << " | Amount: " << record[i].amount
             << " | Note: " << record[i].note
             << " | Date: " << record[i].date << endl;
    }
}

void showTotalIncome(Transaction record[], int totalRecords) {
    float income = 0;

    for (int i = 0; i < totalRecords; i++)
        if (record[i].type == "income")
            income += record[i].amount;

    cout << "\nTotal Income: " << income << endl;
}

void showTotalExpense(Transaction record[], int totalRecords) {
    float expense = 0;

    for (int i = 0; i < totalRecords; i++)
        if (record[i].type == "expense")
            expense += record[i].amount;

    cout << "\nTotal Expense: " << expense << endl;
}

// ------------ UPDATED MONTHLY SUMMARY (MERGED) ------------

void monthlySummary(Transaction record[], int totalRecords) {
    string month;
    float income = 0, expense = 0;

    cout << "Enter month (YYYY-MM): ";
    cin >> month;

    // -------- VALIDATION FOR YYYY-MM --------
    if (month.length() != 7 ||
        month[4] != '-' ||
        !isdigit(month[0]) || !isdigit(month[1]) ||
        !isdigit(month[2]) || !isdigit(month[3]) ||
        !isdigit(month[5]) || !isdigit(month[6]))
    {
        cout << "Invalid input!\n";
        return;
    }

    string mm = month.substr(5, 2);
    int monthValue = stoi(mm);

    if (monthValue < 1 || monthValue > 12) {
        cout << "Invalid input!\n";
        return;
    }
    // ----------------------------------------

    for (int i = 0; i < totalRecords; i++) {
        if (record[i].date == month) {
            if (record[i].type == "income")
                income += record[i].amount;
            else if (record[i].type == "expense")
                expense += record[i].amount;
        }
    }

    cout << "\n--- Summary for " << month << " ---\n";
    cout << "Total Income: " << income << endl;
    cout << "Total Expense: " << expense << endl;
    cout << "Balance: " << (income - expense) << endl;
}

// ---------------- FILE HANDLING ----------------

void saveData(Transaction record[], int totalRecords) {
    ofstream file("finance.txt");

    if (!file) {
        cout << "Invalid input!\n";
        return;
    }

    for (int i = 0; i < totalRecords; i++) {
        file << record[i].id << " "
             << record[i].type << " "
             << record[i].amount << " "
             << record[i].note << " "
             << record[i].date << endl;
    }

    cout << "Data saved.\n";
}

int loadData(Transaction record[]) {
    ifstream file("finance.txt");
    int totalRecords = 0;

    if (!file) return 0;

    while (file >> record[totalRecords].id) {
        file >> record[totalRecords].type;
        file >> record[totalRecords].amount;
        file >> record[totalRecords].note;
        file >> record[totalRecords].date;
        totalRecords++;
    }
    return totalRecords;
}

// ---------------- MAIN PROGRAM ----------------

int main() {
    Transaction record[500];
    int totalRecords = loadData(record);

    Account acc;
    bool hasAccount = loadAccount(acc);

    int choice;

    do {
        cout << "\n--- Personal Finance Manager ---\n";
        cout << "1. Create Account\n";
        cout << "2. Add Transaction\n";
        cout << "3. Check Current Balance (PIN)\n";
        cout << "4. Total Income (PIN)\n";
        cout << "5. Total Expense (PIN)\n";
        cout << "6. View Transaction History\n";
        cout << "7. Monthly Summary (PIN)\n";
        cout << "8. Reset PIN\n";
        cout << "9. Save & Exit\n";

        cout << "Enter choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input!\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                createAccount(acc);
                hasAccount = loadAccount(acc);
                break;

            case 2:
                if (hasAccount)
                    addTransaction(record, totalRecords);
                else
                    cout << "Create an account first.\n";
                break;

            case 3:
                if (hasAccount && checkPIN(acc))
                    checkBalance(record, totalRecords);
                break;

            case 4:
                if (hasAccount && checkPIN(acc))
                    showTotalIncome(record, totalRecords);
                break;

            case 5:
                if (hasAccount && checkPIN(acc))
                    showTotalExpense(record, totalRecords);
                break;

            case 6:
                showTransactions(record, totalRecords);
                break;

            case 7:
                if (hasAccount && checkPIN(acc))
                    monthlySummary(record, totalRecords);
                break;

            case 8:
                if (hasAccount)
                    resetPIN(acc);
                else
                    cout << "No account found.\n";
                break;

            case 9:
                saveData(record, totalRecords);
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid input!\n";
        }

    } while (choice != 9);

    return 0;
}
