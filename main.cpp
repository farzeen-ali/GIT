#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Structure for transaction
struct Transaction {
    int id;
    string type;
    float amount;
    string note;
    string date; // YYYY-MM
};

// User account structure
struct Account {
    string name;
    int pin;
};

// ---------------- ACCOUNT FUNCTIONS ----------------

// Save account to file
void saveAccount(Account acc) {
    ofstream file("account.txt");
    file << acc.name << endl;
    file << acc.pin;
    file.close();
}

// Load account if exists
bool loadAccount(Account &acc) {
    ifstream file("account.txt");
    if (!file) return false;
    file >> acc.name;
    file >> acc.pin;
    return true;
}

// Create new user account
void createAccount(Account &acc) {
    cout << "\n--- Create Your Financial Account ---\n";
    cout << "Enter your name: ";
    cin >> acc.name;

    cout << "Set a 4-digit PIN: ";
    cin >> acc.pin;

    saveAccount(acc);

    cout << "Account created successfully!\n";
}

// Check PIN for secure features
bool checkPIN(Account acc) {
    int pin;
    cout << "Enter PIN: ";
    cin >> pin;

    if (pin == acc.pin) {
        cout << "Access granted.\n";
        return true;
    } else {
        cout << "Incorrect PIN!\n";
        return false;
    }
}

// Reset PIN
void resetPIN(Account &acc) {
    int oldPin;
    cout << "Enter current PIN: ";
    cin >> oldPin;

    if (oldPin != acc.pin) {
        cout << "Incorrect PIN! Cannot reset.\n";
        return;
    }

    cout << "Enter NEW PIN: ";
    cin >> acc.pin;

    saveAccount(acc);

    cout << "PIN reset successfully!\n";
}

// ---------------- TRANSACTION FUNCTIONS ----------------

void addTransaction(Transaction record[], int &totalRecords) {
    cout << "\nEnter type (income/expense/saving): ";
    cin >> record[totalRecords].type;

    cout << "Enter amount: ";
    cin >> record[totalRecords].amount;

    cout << "Enter note: ";
    cin.ignore();
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
        if (record[i].type == "income")
            income += record[i].amount;
        else if (record[i].type == "saving")
            savings += record[i].amount;
        else
            expense += record[i].amount;
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

    for (int i = 0; i < totalRecords; i++) {
        if (record[i].type == "income")
            income += record[i].amount;
    }

    cout << "\nTotal Income: " << income << endl;
}

void showTotalExpense(Transaction record[], int totalRecords) {
    float expense = 0;

    for (int i = 0; i < totalRecords; i++) {
        if (record[i].type == "expense")
            expense += record[i].amount;
    }

    cout << "\nTotal Expense: " << expense << endl;
}

void monthlySummary(Transaction record[], int totalRecords) {
    string month;
    float income = 0, expense = 0;

    cout << "Enter month (YYYY-MM): ";
    cin >> month;

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

    for (int i = 0; i < totalRecords; i++) {
        file << record[i].id << " "
             << record[i].type << " "
             << record[i].amount << " "
             << record[i].note << " "
             << record[i].date << endl;
    }

    file.close();
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

    file.close();
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
        cout << "1. Create Your Account\n";
        cout << "2. Reset PIN\n";
        cout << "3. Add Transaction\n";
        cout << "4. Check Current Balance (PIN)\n";
        cout << "5. View Transaction History\n";
        cout << "6. Total Income (PIN)\n";
        cout << "7. Total Expense (PIN)\n";
        cout << "8. Monthly Summary (PIN)\n";
        cout << "9. Save & Exit\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createAccount(acc);
                hasAccount = true;
                break;

            case 2:
                if (hasAccount)
                    resetPIN(acc);
                else
                    cout << "No account found. Create account first.\n";
                break;

            case 3:
                if (hasAccount)
                    addTransaction(record, totalRecords);
                else
                    cout << "Create an account first.\n";
                break;

            case 4:
                if (hasAccount && checkPIN(acc))
                    checkBalance(record, totalRecords);
                break;

            case 5:
                showTransactions(record, totalRecords);
                break;

            case 6:
                if (hasAccount && checkPIN(acc))
                    showTotalIncome(record, totalRecords);
                break;

            case 7:
                if (hasAccount && checkPIN(acc))
                    showTotalExpense(record, totalRecords);
                break;

            case 8:
                if (hasAccount && checkPIN(acc))
                    monthlySummary(record, totalRecords);
                break;

            case 9:
                saveData(record, totalRecords);
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 9);

    return 0;
}
