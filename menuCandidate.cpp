#include "class.h"
#include "menuCandidate.h"
#include <iostream>
using namespace std;

void menuUpdateCandidate() {
    cout << "\n===== CAP NHAT THONG TIN CANDIDATE =====\n";

    cout << "1. Cap nhat ho ten\n";
    cout << "2. Cap nhat ngay sinh\n";
    cout << "3. Cap nhat gioi tinh\n";
    cout << "4. Cap nhat so dien thoai\n";
    cout << "5. Cap nhat dia chi\n";
    cout << "6. Cap nhat chuyen nganh\n";
    cout << "7. Cap nhat truong\n";
    cout << "8. Cap nhat trinh do hoc van\n";
    cout << "9. Cap nhat GPA\n";
    cout << "0. Quay lai\n";
}

void menuSkill(Candidate& candidate) {
    int choice;

    do {
        cout << "\n===== QUAN LY KY NANG =====\n";
        cout << "1. Them ky nang\n";
        cout << "2. Sua ky nang\n";
        cout << "3. Xoa ky nang\n";
        cout << "0. Quay lai\n";

        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                candidate.addSkill();
                break;

            case 2:
                candidate.updateSkill();
                break;

            case 3:
                candidate.deleteSkill();
                break;

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);
}

void menuCertificate(Candidate& candidate) {
    int choice;

    do {
        cout << "\n===== QUAN LY CHUNG CHI =====\n";
        cout << "1. Them chung chi\n";
        cout << "2. Sua chung chi\n";
        cout << "3. Xoa chung chi\n";
        cout << "0. Quay lai\n";

        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                candidate.addCertificate();
                break;

            case 2:
                candidate.updateCertificate();
                break;

            case 3:
                candidate.deleteCertificate();
                break;

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);
}

void menuExperience(Candidate& candidate) {
    int choice;

    do {
        cout << "\n===== QUAN LY KINH NGHIEM =====\n";
        cout << "1. Them kinh nghiem\n";
        cout << "2. Sua kinh nghiem\n";
        cout << "3. Xoa kinh nghiem\n";
        cout << "0. Quay lai\n";

        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                candidate.addExperience();
                break;

            case 2:
                candidate.updateExperience();
                break;

            case 3:
                candidate.deleteExperience();
                break;

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);
}

void menuCandidate(Candidate& candidate) {
    int choice;

    do {
        cout << "\n===== MENU CANDIDATE =====\n";
        cout << "1. Xem ho so\n";
        cout << "2. Cap nhat thong tin ca nhan\n";
        cout << "3. Quan ly ky nang\n";
        cout << "4. Quan ly chung chi\n";
        cout << "5. Quan ly kinh nghiem\n";
        cout << "0. Dang xuat\n";

        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                candidate.displayCandidate();
                break;

            case 2:
                candidate.updateCandidate();
                break;

            case 3:
                menuSkill(candidate);
                break;

            case 4:
                menuCertificate(candidate);
                break;

            case 5:
                menuExperience(candidate);
                break;

            case 0:
                cout << "Dang xuat...\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);
}