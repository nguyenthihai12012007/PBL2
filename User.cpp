#include "class.h"
#include "Database.h"
#include <iostream>
using namespace std;

bool User::login(Database& db, const string& email, const string& password) {
    User account;
    if (!db.getUserByEmail(email, account)) {
        cout << "Khong tim thay tai khoan hoac loi truy van!\n";
        return false;
    }
    if (account.getPassword() != password) {
        cout << "Mat khau khong dung!\n";
        return false;
    }
    setIDuser(account.getIDuser());
    setEmail(account.getEmail());
    setRole(account.getRole());
    cout << "Dang nhap thanh cong!\n";
    return true;
}
