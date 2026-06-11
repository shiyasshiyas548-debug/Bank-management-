#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

class Account {
private:
    int accountNumber;
    string holderName;
    string accountType;
    double balance;

public:
    Account()
        : accountNumber(0), holderName(""), accountType("Savings"), balance(0.0) {}

    Account(int accNo, const string& name, const string& type, double initialBalance)
        : accountNumber(accNo), holderName(name),
          accountType(type), balance(initialBalance) {}

    int getAccountNumber() const { return accountNumber; }
    string getHolderName() const { return holderName; }
    string getAccountType() const { return accountType; }
    double getBalance() const { return balance; }

    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "Deposit amount must be positive.\n";
            return false;
        }

        balance += amount;
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "Withdrawal amount must be positive.\n";
            return false;
        }

        if (amount > balance) {
            cout << "Insufficient balance.\n";
            return false;
        }

        balance -= amount;
        return true;
    }

    void display() const {
        cout << "\n----------------------------------\n";
        cout << "Account Number : " << accountNumber << "\n";
        cout << "Holder Name    : " << holderName << "\n";
        cout << "Account Type   : " << accountType << "\n";
        cout << fixed << setprecision(2);
        cout << "Balance        : $" << balance << "\n";
        cout << "----------------------------------\n";
    }

    void writeToFile(ofstream& ofs) const {
        ofs << accountNumber << "|"
            << holderName << "|"
            << accountType << "|"
            << fixed << setprecision(2)
            << balance << "\n";
    }

    static Account readFromLine(const string& line) {
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);

        int accNo = stoi(line.substr(0, p1));
        string name = line.substr(p1 + 1, p2 - p1 - 1);
        string type = line.substr(p2 + 1, p3 - p2 - 1);
        double bal = stod(line.substr(p3 + 1));

        return Account(accNo, name, type, bal);
    }
};

class Bank {
private:
    const string DATA_FILE = "accounts.dat";
    vector<Account> accounts;

    void loadFromFile() {
        accounts.clear();

        ifstream file(DATA_FILE);
        if (!file.is_open())
            return;

        string line;
        while (getline(file, line)) {
            if (!line.empty())
                accounts.push_back(Account::readFromLine(line));
        }

        file.close();
    }

    void saveToFile() const {
        ofstream file(DATA_FILE, ios::trunc);

        if (!file.is_open()) {
            cerr << "Unable to save data.\n";
            return;
        }

        for (const auto& account : accounts)
            account.writeToFile(file);

        file.close();
    }

    int nextAccountNumber() const {
        if (accounts.empty())
            return 1001;

        int maxNumber = 0;

        for (const auto& account : accounts)
            maxNumber = max(maxNumber, account.getAccountNumber());

        return maxNumber + 1;
    }

    Account* findAccount(int accountNumber) {
        for (auto& account : accounts) {
            if (account.getAccountNumber() == accountNumber)
                return &account;
        }

        return nullptr;
    }

public:
    Bank() {
        loadFromFile();
    }

    ~Bank() {
        saveToFile();
    }

    void createAccount() {
        string name, type;
        double initialDeposit;

        cout << "\nEnter Account Holder Name: ";
        cin.ignore();
        getline(cin, name);

        int choice;
        cout << "Account Type (1-Savings, 2-Current): ";
        cin >> choice;

        type = (choice == 2) ? "Current" : "Savings";

        cout << "Initial Deposit Amount: ";
        cin >> initialDeposit;

        if (initialDeposit < 0) {
            cout << "Invalid amount.\n";
            return;
        }

        int accountNumber = nextAccountNumber();

        accounts.emplace_back(
            accountNumber,
            name,
            type,
            initialDeposit
        );

        saveToFile();

        cout << "\nAccount created successfully.\n";
        cout << "Account Number: " << accountNumber << "\n";
    }

    void deposit() {
        int accountNumber;
        double amount;

        cout << "\nEnter Account Number: ";
        cin >> accountNumber;

        Account* account = findAccount(accountNumber);

        if (!account) {
            cout << "Account not found.\n";
            return;
        }

        cout << "Enter Deposit Amount: ";
        cin >> amount;

        if (account->deposit(amount)) {
            saveToFile();
            cout << "New Balance: $" << fixed
                 << setprecision(2)
                 << account->getBalance() << "\n";
        }
    }

    void withdraw() {
        int accountNumber;
        double amount;

        cout << "\nEnter Account Number: ";
        cin >> accountNumber;

        Account* account = findAccount(accountNumber);

        if (!account) {
            cout << "Account not found.\n";
            return;
        }

        cout << "Enter Withdrawal Amount: ";
        cin >> amount;

        if (account->withdraw(amount)) {
            saveToFile();
            cout << "New Balance: $" << fixed
                 << setprecision(2)
                 << account->getBalance() << "\n";
        }
    }

    void checkBalance() {
        int accountNumber;

        cout << "\nEnter Account Number: ";
        cin >> accountNumber;

        Account* account = findAccount(accountNumber);

        if (!account) {
            cout << "Account not found.\n";
            return;
        }

        account->display();
    }

    void listAllAccounts() const {
        if (accounts.empty()) {
            cout << "\nNo accounts available.\n";
            return;
        }

        cout << "\n";
        cout << left
             << setw(12) << "Acc No"
             << setw(25) << "Name"
             << setw(12) << "Type"
             << "Balance\n";

        cout << string(60, '-') << "\n";

        for (const auto& account : accounts) {
            cout << left
                 << setw(12) << account.getAccountNumber()
                 << setw(25) << account.getHolderName()
                 << setw(12) << account.getAccountType()
                 << "$" << fixed << setprecision(2)
                 << account.getBalance() << "\n";
        }

        cout << "\nTotal Accounts: "
             << accounts.size() << "\n";
    }

    void deleteAccount() {
        int accountNumber;

        cout << "\nEnter Account Number to delete: ";
        cin >> accountNumber;

        auto it = remove_if(
            accounts.begin(),
            accounts.end(),
            [accountNumber](const Account& account) {
                return account.getAccountNumber() == accountNumber;
            }
        );

        if (it == accounts.end()) {
            cout << "Account not found.\n";
            return;
        }

        accounts.erase(it, accounts.end());

        saveToFile();

        cout << "Account deleted successfully.\n";
    }
};

void printMenu() {
    cout << "\n===== BANK MANAGEMENT SYSTEM =====\n";
    cout << "1. Create New Account\n";
    cout << "2. Deposit Money\n";
    cout << "3. Withdraw Money\n";
    cout << "4. Check Balance\n";
    cout << "5. List All Accounts\n";
    cout << "6. Delete Account\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    Bank bank;
    int choice;

    do {
        printMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                bank.createAccount();
                break;

            case 2:
                bank.deposit();
                break;

            case 3:
                bank.withdraw();
                break;

            case 4:
                bank.checkBalance();
                break;

            case 5:
                bank.listAllAccounts();
                break;

            case 6:
                bank.deleteAccount();
                break;

            case 0:
                cout << "\nThank you for using the system.\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}