#include "class.h"
#include <iostream>

using namespace std;

int main() {
    Candidate candidate(1, "candidate@gmail.com", "123456");
    cout << "===== NHAP THONG TIN UNG VIEN =====\n";
    candidate.inputCandidate();
    candidate.displayCandidate();

    Company company(2, "company@gmail.com", "123456");
    cout << "\n\n===== NHAP THONG TIN CONG TY =====\n";
    company.inputCompany();
    company.displayCompany();

    company.createJob();

    return 0;
}