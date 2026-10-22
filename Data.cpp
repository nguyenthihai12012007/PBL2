#include "class.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

bool Data::readFile(const string& filename, vector<string>& lines) {
    ifstream file(filename);
    if (!file.is_open()) 
        return false;
    lines.clear();
    string line;
    while(getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    return true;
}

bool Data::writeFile(const string& filename, vector<string>& lines) {
    ofstream file(filename);
    if (!file.is_open()) 
        return false;
    for (const string& line: lines) {
        file << line << '\n';
    }
    file.close();
    return true;
}

int Data::getNextID(const string& filename) {
    vector<string> lines;
    if (!readFile(filename, lines)) {
        return 1;
    }
    int maxID = 0;
    for (const string& line : lines) {
        stringstream ss(line);
        string idText;
        if (getline(ss, idText, '|')) {
            try {
                int id = stoi(idText);
                if (id > maxID) {
                    maxID = id;
                }
            }
            catch (...) {
                continue;
            }
        }
    }
    return maxID + 1;
}

bool Data::findUserByEmail(const string& email, User& user) {
    vector<string> lines;
    if (!readFile("users.txt", lines)) {
        return false;
    }
    for (const string& line : lines) {
        stringstream ss(line);
        string idText, savedEmail, password, role;
        if (getline(ss, idText, '|') && getline(ss, savedEmail, '|')
            && getline(ss, password, '|') && getline(ss, role)) {
            if (savedEmail == email) {
                int id;
                try {
                    id = stoi(idText);
                } catch (...) {
                    return false;
                }
                user = User(id, savedEmail, password, role);
                return true;
            }
        }
    }
    return false;
}

bool Data::emailExists(const string& email) {
    vector<string> lines;
    if (!readFile("users.txt", lines)) {
        return false;
    }
    for (const string& line : lines) {
        stringstream ss(line);
        string id, savedEmail;
        if (getline(ss, id, '|') &&
            getline(ss, savedEmail, '|')) {
            if (savedEmail == email) {
                return true;
            }
        }
    }
    return false;
}

bool Data::registerUser(const User& user) {
    if (emailExists(user.getEmail())) {
        return false;
    }
    ofstream file("users.txt", ios::app);
    if (!file.is_open()) {
        return false;
    }
    file << user.getIDuser() << "|"
         << user.getEmail() << "|"
         << user.getPassword() << "|"
         << user.getRole() << '\n';

    bool success = file.good();
    file.close();
    return success;
}

bool Data::getCandidateByUserID(int userID, Candidate& candidate) {
    vector<string> lines;
    if (!readFile("candidates.txt", lines)) {
        return false;
    }
    for (const string& line : lines) {
        stringstream ss(line);
        string candidateIDText, userIDText;
        string fullName, birthday, gender;
        string phone, address, major, school, educationLevel;
        string gpaText;

        if (!getline(ss, candidateIDText, '|') ||
            !getline(ss, userIDText, '|') ||
            !getline(ss, fullName, '|') ||
            !getline(ss, birthday, '|') ||
            !getline(ss, gender, '|') ||
            !getline(ss, phone, '|') ||
            !getline(ss, address, '|') ||
            !getline(ss, major, '|') ||
            !getline(ss, school, '|') ||
            !getline(ss, educationLevel, '|') ||
            !getline(ss, gpaText)) {
            continue;
        }

        try {
            int savedUserID = stoi(userIDText);
            if (savedUserID != userID) {
                continue;
            }
            candidate.IDcandidate = stoi(candidateIDText);
            candidate.setIDuser(savedUserID);
            candidate.setRole("CANDIDATE");

            candidate.personal.fullName = fullName;
            candidate.personal.birthday = birthday;
            candidate.personal.gender = gender;
            candidate.personal.phoneCandidate = phone;
            candidate.personal.addressCandidate = address;

            candidate.professional.major = major;
            candidate.professional.school = school;
            candidate.professional.educationLevel = educationLevel;
            candidate.professional.GPA = stod(gpaText);
            return true;
        }
        catch (...) {
            continue;
        }
    }
    return false;
}

bool Data::getCompanyByUserID(int userID, Company& company) {
    vector<string> lines;
    if (!readFile("companies.txt", lines)) {
        return false;
    }
    for (const string& line : lines) {
        stringstream ss(line);
        string companyIDText, userIDText;
        string nameCompany, phoneCompany;
        string addressCompany, description, website;

        if (!getline(ss, companyIDText, '|') ||
            !getline(ss, userIDText, '|') ||
            !getline(ss, nameCompany, '|') ||
            !getline(ss, phoneCompany, '|') ||
            !getline(ss, addressCompany, '|') ||
            !getline(ss, description, '|') ||
            !getline(ss, website)) {
            continue;
        }

        try {
            int savedUserID = stoi(userIDText);
            if (savedUserID != userID) {
                continue;
            }

            company.IDcompany = stoi(companyIDText);
            company.setIDuser(savedUserID);
            company.setRole("COMPANY");

            company.nameCompany = nameCompany;
            company.phoneCompany = phoneCompany;
            company.addressCompany = addressCompany;
            company.description = description;
            company.website = website;
            return true;
        }
        catch (...) {
            continue;
        }
    }
    return false;
}
