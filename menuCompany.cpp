#include <iostream>
#include "class.h"

void companyInfoMenu(Company& company, Data& dt) {
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
                company.updateCompany(dt);
                break;
            
            case 0:
                cout << "Quay lại.\n";
                break;
            
            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while(choice != 0);
}

void companyJobMenu(Company& company, Data& dt) {
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
                company.createJob(dt);
                break;

            case 2:
                company.displayJobs(dt);
                break;

            case 3:
                company.displayJobDetail(dt);
                break;

            case 4:
                company.updateJob(dt);
                break;

            case 5:
                company.closeJob(dt);
                break;
            
            case 6:
                company.deleteJob(dt);
                break;

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while(choice != 0);
}

void companyApplicationMenu(Company& company, Data& dt) {
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
                company.displayApplications(dt);
                break;
            
            case 2: {
                int id;
                cout << "Nhap ma don ung tuyen: ";
                cin >> id;
                Application application;
                if (dt.getApplicationByID(id, application)) { 
                    application.displayApplication();
                } else 
                    cout << "Khong tim thay don ung tuyen!\n";
                break;
            }

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    }while (choice != 0);
}

void companyMenu(Company& company, Data& dt) {
    int choice;
    do {
        cout << "\n====================================\n";
        cout << "            COMPANY MENU\n";
        cout << "====================================\n";
        cout << "1. Quan ly thong tin cong ty\n";
        cout << "2. Quan ly cong viec\n";
        cout << "3. Quan ly don ung tuyen\n";
        cout << "0. Dang xuat\n";
        cout << "------------------------------------\n";
        cout << "Lua chon: ";
        cin >> choice;
        switch (choice) {
            case 1:
                companyInfoMenu(company, dt);
                break;
            
            case 2:
                companyJobMenu(company, dt);
                break;

            case 3:
                companyApplicationMenu(company, dt);
                break;

            case 0:
                cout << "Quay lai.\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }
    }while (choice != 0);
}
