
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <ctime>
#include <conio.h>
using namespace std;

struct Account {
    string username;
    string password;
    string accType;
    string cvv;
    float balance;
};

unordered_map<string, Account> loadAccounts() {
    unordered_map<string, Account> accounts;
    ifstream file("users.txt");
    string uname, pwd, type, cvv;
    float bal;

    while (file >> uname >> pwd >> type >> cvv >> bal) {
        accounts[uname] = {uname, pwd, type, cvv, bal};
    }
    file.close();
    return accounts;
}

void saveAccounts(const unordered_map<string, Account>& accounts) {
    ofstream file("users.txt");
    for (unordered_map<string, Account>::const_iterator it = accounts.begin(); it != accounts.end(); ++it) {
        const Account& acc = it->second;
        file << acc.username << " " << acc.password << " " << acc.accType << " " << acc.cvv << " " << acc.balance << endl;
    }
    file.close();
}

string getTimestamp() {
    time_t now = time(0);
    char* dt = ctime(&now);
    string ts(dt);
    ts.pop_back(); // Remove newline
    return ts;
}

void writeReceipt(const string& operation, float amount, const string& user, const string& details = "") {
    ofstream rec("transactions.txt", ios::app);
    rec << getTimestamp() << " | User: " << user << " | Operation: " << operation << " | Amount: $" << amount;
    if (!details.empty()) rec << " | Details: " << details;
    rec << endl;
    rec.close();
}

string getHiddenPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        } else if (ch != '\b') {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

void registerUser() {
    unordered_map<string, Account> accounts = loadAccounts();
    Account acc;

    cout << "\n-- Registration --\nEnter Username: ";
    cin >> acc.username;

    if (accounts.find(acc.username) != accounts.end()) {
        cout << "Username already exists.\n";
        return;
    }

    cout << "Enter Password: "; 
    acc.password = getHiddenPassword();
    cout << "Select Account Type (Saving/Current): "; cin >> acc.accType;
    cout << "Set a 4-digit CVV PIN: "; cin >> acc.cvv;
    acc.balance = 0.0;

    accounts[acc.username] = acc;
    saveAccounts(accounts);
    writeReceipt("Registration", 0.0, acc.username);
    cout << "Registration successful.\n";
}

void loginUser() {
    unordered_map<string, Account> accounts = loadAccounts();
    string username, password;

    cout << "\n-- Login --\nUsername: "; cin >> username;
    cout << "Password: "; password = getHiddenPassword();

    if (accounts.find(username) == accounts.end() || accounts[username].password != password) {
        cout << "Invalid credentials.\n";
        return;
    }

    string pin;
    cout << "Enter your CVV PIN to access account: "; cin >> pin;

    if (accounts[username].cvv != pin) {
        cout << "Invalid CVV.\n";
        return;
    }

    int option;
    do {
        cout << "\nWelcome, " << username << "!\n";
        cout << "1. Deposit\n2. Withdraw\n3. Check Balance\n4. Send Money\n5. Exit\nChoice: ";
        cin >> option;

        if (option == 1) {
            float amt;
            cout << "Enter deposit amount: ";
            cin >> amt;
            accounts[username].balance += amt;
            cout << "Deposited successfully.\n";
            writeReceipt("Deposit", amt, username);
        } else if (option == 2) {
            float amt;
            cout << "Enter withdrawal amount: ";
            cin >> amt;
            if (amt > accounts[username].balance) {
                cout << "Insufficient balance.\n";
            } else {
                accounts[username].balance -= amt;
                cout << "Withdrawn successfully.\n";
                writeReceipt("Withdraw", amt, username);
            }
        } else if (option == 3) {
            cout << "Your balance is: $" << accounts[username].balance << endl;
            writeReceipt("Check Balance", 0.0, username);
        } else if (option == 4) {
            cout << "Select Method to Send Money:\n";
            cout << "1. PayPal\n2. NayaPay\n3. SadaPay\n4. Paytm\n5. Bank Transfer\nChoice: ";
            int method; cin >> method;
            string accNum;
            float amt;
            cout << "Enter recipient account number or ID: ";
            cin >> accNum;
            cout << "Enter amount to send: ";
            cin >> amt;

            if (amt > accounts[username].balance) {
                cout << "Insufficient balance.\n";
            } else {
                accounts[username].balance -= amt;
                cout << "Money sent successfully.\n";
                string methodStr = (method == 1 ? "PayPal" : method == 2 ? "NayaPay" : method == 3 ? "SadaPay" : method == 4 ? "Paytm" : "Bank Transfer");
                writeReceipt("Send Money", amt, username, methodStr + " to " + accNum);
            }
        }
    } while (option != 5);

    saveAccounts(accounts);
    cout << "Logged out.\n";
}

int main() {
    int choice;
    do {
        cout << "\n==== Welcome to n1x5 Bank ====\n";
        cout << "1. Register\n2. Login\n3. Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) registerUser();
        else if (choice == 2) loginUser();
    } while (choice != 3);

    return 0;
}
