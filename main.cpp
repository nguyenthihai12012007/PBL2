#include <iostream>
#include <string>
#include "class.h"
#include "Database.h"
#include "menuCandidate.h"

using namespace std;

void companyMenu(Company& company);

int main() {
    Database db;
    if (!db.connect()) {
        cout << "Ket noi MySQL that bai!" << endl;
        return 1;
    }
    cout << "Ket noi MySQL thanh cong!" << endl;
    int roleChoice;
    do {
        cout << "\n====================================\n";
        cout << "     HE THONG QUAN LY VIEC LAM\n";
        cout << "====================================\n";
        cout << "1. Dang nhap voi vai tro Candidate\n";
        cout << "2. Dang nhap voi vai tro Company\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "------------------------------------\n";
        cout << "Lua chon cua ban: ";
        cin >> roleChoice;
        cin.ignore(10000, '\n');

        switch (roleChoice) {
            case 1:
            case 2: {
                string email, password;
                cout << "Nhap email: ";
                getline(cin, email);
                cout << "Nhap mat khau: ";
                getline(cin, password);
                User user;
                if (!db.getUserByEmail(email, user)) {
                    cout << "Email khong ton tai hoac co loi truy van!\n";
                    break;
                }
                if (user.getPassword() != password) {
                    cout << "Mat khau khong dung!\n";
                    break;
                }
                if (roleChoice == 1) {
                    if (user.getRole() != "CANDIDATE") {
                        cout << "Tai khoan khong co vai tro Candidate!\n";
                        break;
                    }
                    Candidate candidate;
                    if (!db.getCandidateByUserID(user.getIDuser(), candidate)) {
                        cout << "Khong tim thay ho so Candidate!\n";
                        break;
                    }
                    cout << "\nDang nhap Candidate thanh cong!\n";
                    menuCandidate(candidate);
                } else {
                    if (user.getRole() != "COMPANY") {
                        cout << "Tai khoan khong co vai tro Company!\n";
                        break;
                    }
                    Company company;
                    if (!db.getCompanyByUserID(user.getIDuser(), company)) {
                        cout << "Khong tim thay ho so Company!\n";
                        break;
                    }
                    cout << "\nDang nhap Company thanh cong!\n";
                    companyMenu(company);
                }
                break;
            }

            case 0:
                cout << "\nCam on ban da su dung chuong trinh. Tam biet!\n";
                break;

            default:
                cout << "\nLua chon khong hop le! Vui long chon lai.\n";
        }

    } while (roleChoice != 0);

    db.disconnect();

    return 0;
}
