#include "class.h"
#include <iostream>
using namespace std;

Company::Company() 
    : User(), IDcompany(0) {
}

Company::Company(int id, string email, string password)
    : User(id, email, password, "COMPANY"), IDcompany(id) {
}

void Company::inputCompany() {
    cin.ignore();
    cout << "Nhap ten cong ty: ";
    getline(cin, nameCompany);
    cout << "Nhap mo ta cong ty: ";
    getline(cin, description);
    cout << "Nhap dia chi: ";
    getline(cin, addressCompany);
    cout << "Nhap so dien thoai: ";
    getline(cin, phoneCompany);
    cout << "Nhap website: ";
    getline(cin, website);
}

void Company::displayCompany() {
    cout << "\n===== THONG TIN CONG TY =====\n";
    cout << "ID: " << IDcompany << endl;
    cout << "Ten cong ty: " << nameCompany << endl;
    cout << "Mo ta: " << description << endl;
    cout << "Dia chi: " << addressCompany << endl;
    cout << "So dien thoai: " << phoneCompany << endl;
    cout << "Website: " << website << endl;
}

void Company::updateCompany() {
    int choice;
    do {
        cout << "\n===== CAP NHAT THONG TIN CONG TY =====\n";
        cout << "1. Cap nhat ten cong ty\n";
        cout << "2. Cap nhat so dien thoai\n";
        cout << "3. Cap nhat dia chi\n";
        cout << "4. Cap nhat mo ta\n";
        cout << "5. Cap nhat website\n";
        cout << "0. Hoan tat\n";
        cout << "Lua chon: ";
        cin >> choice;
        cin.ignore();
        switch(choice) {
            case 1:
                cout << "Nhap ten cong ty moi: ";
                getline(cin, nameCompany);
                break;

            case 2: 
                cout << "Nhap so dien thoai moi: ";
                getline(cin, phoneCompany);
                break;

            case 3:
                cout << "Nhap dia chi moi: ";
                getline(cin, addressCompany);
                break;

            case 4:
                cout << "Nhap mo ta moi: ";
                getline(cin, description);
                break;

            case 5:
                cout << "Nhap website moi: ";
                getline(cin, website);
                break;

            case 0:
                cout << "Da cap nhat thong tin cong ty!\n";
                break;

            default: 
                cout << "Lua chon khong hop le!\n";
        }
    } while (choice != 0);
}

void Company::createJob() {
    Job job;
    cout << "\n===== TAO CONG VIEC MOI =====\n";
    job.inputJob(IDcompany);
    jobs.push_back(job);
    cout << "\nTao cong viec thanh cong!\n";
    job.displayJob();
}

void Company::displayJobs() {
    if (jobs.empty()) {
        cout << "nCong ty hien khong co cong viec nao!\n";
        return;
    }
    cout << "\n===== DANH SACH CONG VIEC =====\n";
    for (int i = 0; i < jobs.size(); i++) {
        cout << "\n----- Cong viec " << i+1 << "-----\n";
        jobs[i].displayJob();
    }
}

void Company::updateJob() {
    if (jobs.empty()) {
        cout << "\nCong ty hien khong co cong viec nao!\n";
        return;
    }
    displayJobs();
    int id;
    cout << "Nhap ID cong viec can cap nhat: ";
    cin >> id;
    for (int i = 0; i < jobs.size(); i++) {
        if (jobs[i].getIDjob() == id) {
            jobs[i].updateJob();
            cout << "Cap nhat thanh cong!\n";
            return;
        }
    }
    cout << "\nKhong tim thay cong viec co ID " << id << "!\n";
}

void Company::deleteJob() {
    if (jobs.empty()) {
        cout << "\nCong ty hien khong co cong viec nao!\n";
        return;
    }
    displayJobs();
    int id;
    cout << "Nhap ID cong viec can xoa: ";
    cin >> id;
    for (int i = 0; i < jobs.size(); i++) {
        if (jobs[i].getIDjob() == id) {
            char confirm;
            cout << "Ban co chac chan muon xoa cong viec nay? (y/n): ";
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                jobs.erase(jobs.begin() + i);
                cout << "Xoa thanh cong!\n";
            } else {
                cout << "Da huy thao tac xoa!\n";
            }
            return;
        }
    }
    cout << "Khong tim thay cong viec co ID " << id << "!\n";
}

void Company::closeJob() {
    if (jobs.empty()) {
        cout << "\nCong ty hien khong co cong viec nao!\n";
        return;
    }
    displayJobs();
    int id;
    cout << "\nNhap ID cong viec can dong: ";
    cin >> id;
    for (int i = 0; i < jobs.size(); i++) {
        if (jobs[i].getIDjob() == id) {
            if (!jobs[i].checkStatus()) {
                cout << "Cong viec nay da duoc dong!\n";
                return;
            }
            char confirm;
            cout << "Ban co chac chan muon dong cong viec nay? (y/n): ";
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                jobs[i].setStatus(false);
                cout << "\nDong thanh cong!\n";
            } else {
                cout << "\nDa huy thao tac dong!\n";
            }
            return;
        }
    }
    cout << "\nKhong tim thay cong viec co ID " << id << "!\n";
}