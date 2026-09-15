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

void Company::createJob() {
    Job job;
    cout << "\n===== TAO CONG VIEC MOI =====\n";
    job.inputJob(IDcompany);
    jobs.push_back(job);
    cout << "\nTao cong viec thanh cong!\n";
    job.displayJob();
}