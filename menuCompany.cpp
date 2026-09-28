#include <iostream>
#include "class.h"

void companyInfoMenu(Company& company) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "     QUAN LY THONG TIN CONG TY\n";
        cout << "====================================\n";
        cout << "1. Xem thong tin cong ty\n";
        cout << "2. Cap nhat thong tin cong ty\n";
        cout << "0. Quay lai\n";
        cout << "------------------------------------\n";
        cout << "Lua chon: ";
        cin >> choice;
        switch (choice) {
            case 1:
                company.displayCompany();
                break;
            
            case 2:
                company.updateCompany();
                break;
            
            case 0:
                cout << "Quay lại.\n";
                break;
            
            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while(choice != 0);
}

void companyJobMenu(Company& company) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "          QUAN LY CONG VIEC\n";
        cout << "====================================\n";
        cout << "1. Tao cong viec moi\n";
        cout << "2. Xem danh sach cong viec\n";
        cout << "3. Xem chi tiet cong viec\n";
        cout << "4. Cap nhat cong viec\n";
        cout << "5. Dong cong viec\n";
        cout << "6. Xoa cong viec\n";
        cout << "0. Quay lai\n";
        cout << "------------------------------------\n";
        cout << "Lua chon: ";
        cin >> choice;
        switch (choice) {
            case 1:
                company.createJob();
                break;

            case 2:
                company.displayJobs();
                break;

            case 3:
                company.displayJobDetail();
                break;

            case 4:
                company.updateJob();
                break;

            case 5:
                company.closeJob();
                break;
            
            case 6:
                company.deleteJob();
                break;

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while(choice != 0);
}

void companyCandidateMenu() {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "          QUAN LY UNG VIEN\n";
        cout << "====================================\n";
        cout << "1. Xem danh sach ung vien\n";
        cout << "2. Tim kiem ung vien\n";
        cout << "3. Xem ho so ung vien\n";
        cout << "0. Quay lai\n";
        cout << "------------------------------------\n";
        cout << "Lua chon: ";
        cin >> choice;
        switch (choice) {
            case 1:
                //viewCandidateList();
                break;

            case2:
                //searchCandidateList();
                break;

            case 3:
                //viewCandidateProfile();
                break;

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while(choice != 0);
}

void companyApplicationMenu() {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "        QUAN LY DON UNG TUYEN\n";
        cout << "====================================\n";
        cout << "1. Xem danh sach don ung tuyen\n";
        cout << "2. Xem chi tiet don ung tuyen\n";
        cout << "0. Quay lai\n";
        cout << "------------------------------------\n";
        cout << "Lua chon: ";
        cin >> choice;
        switch (choice) {
            case 1:
                //viewApplicationList();
                break;
            
            case 2:
                //viewApplicationDetails();
                break;

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    }while (choice != 0);
}

void companyMenu(Company& company) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "            COMPANY MENU\n";
        cout << "====================================\n";
        cout << "1. Quan ly thong tin cong ty\n";
        cout << "2. Quan ly cong viec\n";
        cout << "3. Quan ly ung vien\n";
        cout << "4. Quan ly don ung tuyen\n";
        cout << "0. Dang xuat\n";
        cout << "------------------------------------\n";
        cout << "Lua chon: ";
        cin >> choice;
        switch (choice) {
            case 1:
                companyInfoMenu(company);
                break;
            
            case 2:
                companyJobMenu(company);
                break;

            case 3:
                companyCandidateMenu();
                break;

            case 4:
                companyApplicationMenu();
                break;

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    }while (choice != 0);
}