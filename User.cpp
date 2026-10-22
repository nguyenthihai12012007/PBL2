#include "class.h"
#include <iostream>
using namespace std;

bool User::login(Data& dt, const string& email, const string& password) {
    User account;
    if (!dt.findUserByEmail(email, account)) {
        cout << "Email khong ton tai!\n";
        return false;
    }
    if (account.getPassword() != password) {
        cout << "Mat khau khong dung!\n";
        return false;
    }
    setIDuser(account.getIDuser());
    setEmail(account.getEmail());
    setPassword(account.getPassword());
    setRole(account.getRole());
    cout << "\nDang nhap thanh cong!\n";
    cout << "Vai tro: " << getRole() << endl;
    return true;
}
