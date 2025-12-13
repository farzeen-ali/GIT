#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <vector>

using namespace std;

// ---------------- STRUCTS ----------------

struct Account {
    string username;
    size_t pinHash;
};

struct Transaction {
    int id;
    string username;
    string type;
    float amount;
    string note;
    string date;
};

// ---------------- UTILITIES ----------------

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

size_t hashPIN(const string& pin) {
    return hash<string>{}(pin);
}

float calculateMainBalance(Transaction records[], int total, const string& user) {
    float income = 0, expense = 0;

    for (int i = 0; i < total; i++) {
        if (records[i].username != user) continue;

        if (records[i].type == "income")
            income += records[i].amount;
        else if (records[i].type == "expense")
            expense += records[i].amount;
    }
    return income - expense;
}


// ---------------- ACCOUNT FILE HANDLING ----------------

vector<Account> loadAccounts() {
    vector<Account> accounts;
    ifstream file("account.txt");
    string line;

    while (getline(file, line)) {
        size_t pos = line.find('|');
        if (pos == string::npos) continue;

        Account acc;
        acc.username = line.substr(0, pos);
        acc.pinHash = stoull(line.substr(pos + 1));
        accounts.push_back(acc);
    }
    file.close();
    return accounts;
}

void saveAllAccounts(const vector<Account>& accounts) {
    ofstream file("account.txt", ios::trunc);
    for (const auto& acc : accounts) {
        file << acc.username << "|" << acc.pinHash << endl;
    }
    file.close();
}

bool userExists(const vector<Account>& accounts, const string& user) {
    for (const auto& a : accounts)
        if (a.username == user)
            return true;
    return false;
}

bool isValidDate(const string& date) {
    // Format: YYYY-MM
    if (date.length() != 7 || date[4] != '-')
        return false;

    string year = date.substr(0, 4);
    string monthStr = date.substr(5, 2);

    if (!all_of(year.begin(), year.end(), ::isdigit) ||
        !all_of(monthStr.begin(), monthStr.end(), ::isdigit))
        return false;

    int month = stoi(monthStr);
    return (month >= 1 && month <= 12);
}

int findUserIndex(const vector<Account>& accounts, const string& user) {
    for (int i = 0; i < accounts.size(); i++)
        if (accounts[i].username == user)
            return i;
    return -1;
}

// ---------------- ACCOUNT OPERATIONS ----------------

void createAccount(vector<Account>& accounts) {
    Account acc;
    string pin;

    cout << "Enter username: ";
    getline(cin >> ws, acc.username);

    if (userExists(accounts, acc.username)) {
        cout << "User already exists\n";
        return;
    }

    while (true) {
        cout << "Set 4-digit PIN: ";
        cin >> pin;
        if (pin.length() == 4 && all_of(pin.begin(), pin.end(), ::isdigit))
            break;
        cout << "Invalid PIN\n";
    }

    acc.pinHash = hashPIN(pin);
    accounts.push_back(acc);
    saveAllAccounts(accounts);

    cout << "Account created successfully\n";
}

bool verifyPIN(const Account& acc) {
    string pin;
    cout << "Enter PIN: ";
    cin >> pin;
    return hashPIN(pin) == acc.pinHash;
}

void resetPIN(vector<Account>& accounts) {
    string user;
    cout << "Enter username: ";
    getline(cin >> ws, user);

    int idx = findUserIndex(accounts, user);
    if (idx == -1) {
        cout << "User not found\n";
        return;
    }

    if (!verifyPIN(accounts[idx])) {
        cout << "Wrong PIN\n";
        return;
    }

    string newPin;
    while (true) {
        cout << "Enter new 4-digit PIN: ";
        cin >> newPin;
        if (newPin.length() == 4 && all_of(newPin.begin(), newPin.end(), ::isdigit))
            break;
        cout << "Invalid PIN\n";
    }

    accounts[idx].pinHash = hashPIN(newPin);
    saveAllAccounts(accounts);

    cout << "PIN reset successful\n";
}

// ---------------- TRANSACTIONS ----------------

int loadTransactions(Transaction records[]) {
    ifstream file("finance.txt");
    string line;
    int count = 0;

    while (getline(file, line)) {
        size_t pos, prev = 0;

        pos = line.find('|'); records[count].username = line.substr(prev, pos - prev);
        prev = pos + 1;

        pos = line.find('|', prev); records[count].id = stoi(line.substr(prev, pos - prev));
        prev = pos + 1;

        pos = line.find('|', prev); records[count].type = line.substr(prev, pos - prev);
        prev = pos + 1;

        pos = line.find('|', prev); records[count].amount = stof(line.substr(prev, pos - prev));
        prev = pos + 1;

        pos = line.find('|', prev); records[count].note = line.substr(prev, pos - prev);
        prev = pos + 1;

        records[count].date = line.substr(prev);
        count++;
    }
    file.close();
    return count;
}

void saveTransaction(const Transaction& t) {
    ofstream file("finance.txt", ios::app);
    file << t.username << "|" << t.id << "|" << t.type << "|"
         << t.amount << "|" << t.note << "|" << t.date << endl;
    file.close();
}

void addTransaction(Transaction records[], int& total, const string& user) {
    Transaction t;
    t.id = total + 1;
    t.username = user;

    while (true) {
        cout << "Type (income/expense/saving): ";
        cin >> t.type;
        if (t.type == "income" || t.type == "expense" || t.type == "saving")
            break;
        cout << "Invalid type\n";
    }

    cout << "Amount: ";
    cin >> t.amount;

    if (t.type == "expense") {
        float currentBalance = calculateMainBalance(records, total, user);
        if (t.amount > currentBalance) {
            cout << "❌ Insufficient balance! Expense not allowed.\n";
            return;
        }
    }

    clearInput();
    cout << "Note: ";
    getline(cin, t.note);

    while (true) {
        cout << "Date (YYYY-MM): ";
        cin >> t.date;
        if (isValidDate(t.date))
            break;
        cout << "Invalid date\n";
    }

    records[total++] = t;
    saveTransaction(t);

    cout << "Transaction saved\n";
}

void checkBalance(Transaction records[], int total, const string& user) {
    float income = 0, expense = 0, saving = 0;

    for (int i = 0; i < total; i++) {
        if (records[i].username != user) continue;

        if (records[i].type == "income") income += records[i].amount;
        else if (records[i].type == "expense") expense += records[i].amount;
        else if (records[i].type == "saving") saving += records[i].amount;
    }

    float mainBalance = income - expense;
    float totalBalance = mainBalance + saving;

    cout << "\nBalance\n";
    cout << "Main Balance: " << mainBalance << endl;
    cout << "Savings: " << saving << endl;
    cout << "Total Balance: " << totalBalance << endl;
}

void viewTransactions(Transaction records[], int total, const string& user) {
    cout << "\nTransactions for " << user << ":\n";
    for (int i = 0; i < total; i++) {
        if (records[i].username != user) continue;

        cout << records[i].id << " | " << records[i].type << " | "
             << records[i].amount << " | " << records[i].note
             << " | " << records[i].date << endl;
    }
}

// ---------------- MAIN ----------------

int main() {
    vector<Account> accounts = loadAccounts();

    Transaction records[500];
    int totalRecords = loadTransactions(records);

    int choice;
    string user;

    do {
        cout << "\n--- Personal Finance Manager ---\n";
        cout << "1. Create Account\n";
        cout << "2. Reset PIN\n";
        cout << "3. Add Transaction\n";
        cout << "4. Check Balance\n";
        cout << "5. View Transactions\n";
        cout << "6. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createAccount(accounts);
                break;

            case 2:
                resetPIN(accounts);
                break;

            case 3:
                cout << "Username: ";
                 getline(cin >> ws, user);
                if (userExists(accounts, user) &&
                    verifyPIN(accounts[findUserIndex(accounts, user)]))
                    addTransaction(records, totalRecords, user);
                else
                    cout << "Authentication failed\n";
                break;

            case 4:
                cout << "Username: ";
                getline(cin >> ws, user);
                if (userExists(accounts, user) &&
                    verifyPIN(accounts[findUserIndex(accounts, user)]))
                    checkBalance(records, totalRecords, user);
                else
                    cout << "Authentication failed\n";
                break;

            case 5:
                cout << "Username: ";
                getline(cin >> ws, user);
                if (userExists(accounts, user) &&
                    verifyPIN(accounts[findUserIndex(accounts, user)]))
                    viewTransactions(records, totalRecords, user);
                else
                    cout << "Authentication failed\n";
                break;
        }

    } while (choice != 6);
    cout<<"\nThank You!"<<endl;
    return 0;
}