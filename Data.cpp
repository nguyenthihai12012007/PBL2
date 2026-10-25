#include "class.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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
        if (line.empty() || line[0] == '#')
            continue;
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
        if (line.empty() || line[0] == '#')
            continue;
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
        if (line.empty() || line[0] == '#')
            continue;
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
        if (line.empty() || line[0] == '#')
            continue;
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
        if (line.empty() || line[0] == '#')
            continue;
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

static vector<string> splitLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream ss(line);
    while (getline(ss, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

bool Data::saveCompany(const Company& company) {
    vector<string> lines;
    if (!readFile("companies.txt", lines)) {
        return false;
    }
    bool found = false;
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 7 || fields[0].empty()) {
            continue;
        }
        try {
            if (stoi(fields[0]) == company.IDcompany) {
                stringstream ss;
                ss << company.IDcompany << '|'
                   << company.getIDuser() << '|'
                   << company.nameCompany << '|'
                   << company.phoneCompany << '|'
                   << company.addressCompany << '|'
                   << company.description << '|'
                   << company.website;
                lines[i] = ss.str();
                found = true;
                break;
            }
        }
        catch (...) {
            continue;
        }    
    }
    if (!found) {
        stringstream ss;
        ss << company.IDcompany << '|'
           << company.getIDuser() << '|'
           << company.nameCompany << '|'
           << company.phoneCompany << '|'
           << company.addressCompany << '|'
           << company.description << '|'
           << company.website;
        lines.push_back(ss.str());
    }
    return writeFile("companies.txt", lines);
}

bool Data::getJobsByCompanyID(int companyID, vector<Job>& jobs) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    jobs.clear();
    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 10 || fields[0].empty()
            || fields[0][0] == '#') {
            continue;
        }
        try {
            if (stoi(fields[1]) == companyID) {
                Job job;
                job.IDjob = stoi(fields[0]);
                job.IDcompany = stoi(fields[1]);
                job.title = fields[2];
                job.description = fields[3];
                job.location = fields[4];
                job.minGPA = stof(fields[5]);
                job.minExperience = stoi(fields[6]);
                job.deadline = fields[7];
                job.salary = stoi(fields[8]);
                job.status = (stoi(fields[9]) != 0);
                jobs.push_back(job);
            }
        }
        catch (...) {
            continue;
        }
    }
    return true;
}

bool Data::getJobByID(int jobID, Job& job) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 10 || fields[0].empty()
            || fields[0][0] == '#') {
            continue;
        }
        if (stoi(fields[0]) == jobID) {
            job.IDjob = stoi(fields[0]);
            job.IDcompany = stoi(fields[1]);
            job.title = fields[2];
            job.description = fields[3];
            job.location = fields[4];
            job.minGPA = stof(fields[5]);
            job.minExperience = stoi(fields[6]);
            job.deadline = fields[7];
            job.salary = stoi(fields[8]);
            job.status = (stoi(fields[9]) != 0);
            return true;
        }
    }
    return false;
}

bool Data::saveJob(const Job& job) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() >= 2 && !fields[0].empty()
            && fields[0][0] != '#') {
            if (stoi(fields[0]) == job.getIDjob()) {
                return false;
            }
        }
    }
    stringstream ss;
    ss << job.IDjob << '|'
       << job.IDcompany << '|'
       << job.title << '|'
       << job.description << '|'
       << job.location << '|'
       << job.minGPA << '|'
       << job.minExperience << '|'
       << job.deadline << '|'
       << job.salary << '|'
       << (job.status ? 1 : 0);
    lines.push_back(ss.str());
    return writeFile("jobs.txt", lines);
}

bool Data::updateJob(const Job& job) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    bool found = false;
    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() >= 10 && !fields[0].empty()
            && fields[0][0] != '#') {
            if (stoi(fields[0]) == job.getIDjob()
                && stoi(fields[1]) == job.getIDcompany()) {
                stringstream ss;
                ss << job.IDjob << '|'
                   << job.IDcompany << '|'
                   << job.title << '|'
                   << job.description << '|'
                   << job.location << '|'
                   << job.minGPA << '|'
                   << job.minExperience << '|'
                   << job.deadline << '|'
                   << job.salary << '|'
                   << (job.status ? 1 : 0);
                lines[i] = ss.str();
                found = true;
                break;
            }
        }
    }
    if (!found) {
        return false;
    }
    return writeFile("jobs.txt", lines);
}

bool Data::closeJob(int jobID, int companyID) {
    vector<Job> jobs;
    if (!getJobsByCompanyID(companyID, jobs)) {
        return false;
    }
    for (int i = 0; i < jobs.size(); i++) {
        if (jobs[i].getIDjob() == jobID) {
            if (!jobs[i].checkStatus()) {
                return false;
            }
            jobs[i].setStatus(false);
            return updateJob(jobs[i]);
        }
    }
    return false;
}

bool Data::deleteJob(int jobID, int companyID) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    bool found = false;
    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() >= 2 && !fields[0].empty()
            && fields[0][0] != '#') {
            if (stoi(fields[0]) == jobID
                && stoi(fields[1]) == companyID) {
                lines.erase(lines.begin() + i);
                found = true;
                break;
            }
        }
    }
    if (!found) {
        return false;
    }
    return writeFile("jobs.txt", lines);
}

bool Data::getApplicationsByCompanyID(int companyID, vector<Application>& applications) {
    vector<string> applicationLines;
    vector<string> jobLines;

    if (!readFile("applications.txt", applicationLines))
        return false;
    if (!readFile("jobs.txt", jobLines))
        return false;

    applications.clear();
    vector<int> jobIDs;
    for (int i = 0; i < (int)jobLines.size(); i++) {
        if (jobLines[i].empty() || jobLines[i][0] == '#')
            continue;
        stringstream ss(jobLines[i]);
        string jobIDStr, companyIDStr;
        getline(ss, jobIDStr, '|');
        getline(ss, companyIDStr, '|');
        if (jobIDStr.empty() || companyIDStr.empty())
            continue;
        int jobID = stoi(jobIDStr);
        int ownerID = stoi(companyIDStr);
        if (jobID > 0 && ownerID == companyID)
            jobIDs.push_back(jobID);
    }
    for (int i = 0; i < (int)applicationLines.size(); i++) {
        if (applicationLines[i].empty() || applicationLines[i][0] == '#')
            continue;
        stringstream ss(applicationLines[i]);
        string idStr, candidateStr, jobStr;
        string levelStr, status, appliedAt;
        getline(ss, idStr, '|');
        getline(ss, candidateStr, '|');
        getline(ss, jobStr, '|');
        getline(ss, levelStr, '|');
        getline(ss, status, '|');
        getline(ss, appliedAt, '|');
        if (idStr.empty() || candidateStr.empty() ||
            jobStr.empty() || levelStr.empty())
            continue;
        int id = stoi(idStr);
        int candidateID = stoi(candidateStr);
        int jobID = stoi(jobStr);
        if (id <= 0 || candidateID <= 0 || jobID <= 0)
            continue;

        bool belongsToCompany = false;
        for (int j = 0; j < (int)jobIDs.size(); j++) {
            if (jobIDs[j] == jobID) {
                belongsToCompany = true;
                break;
            }
        }
        if (belongsToCompany) {
            Application application;
            application.IDappli = id;
            application.IDcandidate = candidateID;
            application.IDjob = jobID;
            application.appropriateLevel = stof(levelStr);
            application.status = status;
            application.appliedAt = appliedAt;
            applications.push_back(application);
        }
    }
    return true;
}

bool Data::getApplicationByID(int applicationID, Application& application) {
    vector<string> lines;
    if (!readFile("applications.txt", lines))
        return false;
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#')
            continue;
        stringstream ss(lines[i]);
        string idStr, candidateStr, jobStr;
        string levelStr, status, appliedAt;
        getline(ss, idStr, '|');
        getline(ss, candidateStr, '|');
        getline(ss, jobStr, '|');
        getline(ss, levelStr, '|');
        getline(ss, status, '|');
        getline(ss, appliedAt, '|');
        if (idStr.empty() || candidateStr.empty() ||
            jobStr.empty() || levelStr.empty())
            continue;
        int id = stoi(idStr);
        if (id != applicationID || id <= 0)
            continue;
        application.IDappli = id;
        application.IDcandidate = stoi(candidateStr);
        application.IDjob = stoi(jobStr);
        application.appropriateLevel = stof(levelStr);
        application.status = status;
        application.appliedAt = appliedAt;
        return true;
    }
    return false;
}

bool Data::updateApplicationStatus(int applicationID, const string& status) {
    if (status != "PENDING" && status != "ACCEPTED" && status != "REJECTED") {
        return false;
    }

    vector<string> lines;
    if (!readFile("applications.txt", lines))
        return false;

    bool found = false;
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#')
            continue;
        stringstream ss(lines[i]);
        string idStr, candidateStr, jobStr;
        string levelStr, oldStatus, appliedAt;
        getline(ss, idStr, '|');
        getline(ss, candidateStr, '|');
        getline(ss, jobStr, '|');
        getline(ss, levelStr, '|');
        getline(ss, oldStatus, '|');
        getline(ss, appliedAt, '|');

        if (idStr.empty())
            continue;

        int id = stoi(idStr);
        if (id != applicationID || id <= 0)
            continue;

        stringstream newLine;
        newLine << idStr << "|"
                << candidateStr << "|"
                << jobStr << "|"
                << levelStr << "|"
                << status << "|"
                << appliedAt;

        lines[i] = newLine.str();
        found = true;
        break;
    }
    if (!found)
        return false;
    return writeFile("applications.txt", lines);
}

bool Data::saveApplication(const Application& application) {
    vector<string> lines;
    if (!readFile("applications.txt", lines))
        lines.clear();
    if (application.IDappli <= 0 ||
        application.IDcandidate <= 0 ||
        application.IDjob <= 0) {
        return false;
    }
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#')
            continue;
        stringstream ss(lines[i]);
        string idStr;
        getline(ss, idStr, '|');
        if (idStr.empty())
            continue;
        if (stoi(idStr) == application.IDappli)
            return false;
    }

    stringstream line;
    line << application.IDappli << "|"
         << application.IDcandidate << "|"
         << application.IDjob << "|"
         << application.appropriateLevel << "|"
         << application.status << "|"
         << application.appliedAt;
    lines.push_back(line.str());
    return writeFile("applications.txt", lines);
}

bool Data::saveCandidate (const Candidate& candidate) {
    if (candidate.IDcandidate <= 0 || candidate.getIDuser() <= 0) {
        return false;
    }

    vector <string> lines;

    if (!readFile("candidates.txt", lines)) {
        return false;
    }

    stringstream ss;


    ss << candidate.IDcandidate << '|'
       << candidate.getIDuser() << '|'
       << candidate.personal.fullName << '|'
       << candidate.personal.birthday << '|'
       << candidate.personal.gender << '|'
       << candidate.personal.phoneCandidate << '|'
       << candidate.personal.addressCandidate << '|'
       << candidate.professional.major << '|'
       << candidate.professional.school << '|'
       << candidate.professional.educationLevel << '|'
       << candidate.professional.GPA;

    bool found = false;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector <string> fields = splitLine(lines[i]);

        if (fields[0] == to_string(candidate.IDcandidate)) {
            lines[i] = ss.str();
            found = true;
            break;
        }
    }

    if (!found) {
        lines.push_back(ss.str());
    }

    return writeFile("candidate.txt", lines);
}

bool Data::saveCandidateSkill (const Candidate& candidate) {
    if (candidate.IDcandidate <= 0 || candidate.getIDuser() <= 0) {
        return false;
    }

    vector<string> lines;

    if(!readFile("candidate_skill.txt", lines)) {
        return false;
    }

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector <string> fields = splitLine(lines[i]);

        if (fields[0] == to_string(candidate.IDcandidate)) {
            lines.erase(lines.begin() + i);
            i--;
        }
    }

    const vector<Skill>& skill = candidate.getSkill();

    for (int i = 0; i < skill.size(); i++) {
        stringstream ss;

        ss << candidate.IDcandidate << '|'
           << skill[i].name << '|'
           << skill[i].level;

        lines.push_back(ss.str());
    }

    return writeFile("candidate_skill.txt", lines);
}

bool Data::saveCandidateCertificate (const Candidate& candidate) {
    if (candidate.IDcandidate <= 0 || candidate.getIDuser() <= 0) {
        return false;
    }

    vector <string> lines;

    if (!readFile("candidate_certificate", lines)) {
        return false;
    }

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector <string> fields = splitLine(lines[i]);

        if (fields[0] == to_string(candidate.IDcandidate)) {
            lines.erase(lines.begin() + i);
            i--;
        }
    }

    const vector <Certificate>& certificate = candidate.getCertificate();

    for (int i = 0; i < certificate.size(); i++) {
        stringstream ss;

        ss << candidate.IDcandidate << '|'
            << certificate[i].certificateTypeID << '|'
            << certificate[i].organization << '|'
            << certificate[i].value;

        lines.push_back(ss.str());
    }

    return writeFile("candidatecertificate.txt", lines);
}

bool Data::saveCandidateExperience(const Candidate& candidate) {
    if (candidate.IDcandidate <= 0 || candidate.getIDuser() <= 0) {
        return false;
    }

    vector<string> lines;

    if (!readFile("candidate_experience.txt", lines)) {
        return false;
    }

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector<string> fields = splitLine(lines[i]);

        if (fields.empty()) {
            continue;
        }

        if (fields[0] == to_string(candidate.IDcandidate)) {
            lines.erase(lines.begin() + i);
            i--;
        }
    }

    vector<Experience> experiences = candidate.getExperiences();

    for (int i = 0; i < experiences.size(); i++) {
        stringstream ss;

        ss << candidate.IDcandidate << '|'
           << experiences[i].getIDexperience() << '|'
           << experiences[i].getCompanyName() << '|'
           << experiences[i].getPosition() << '|'
           << experiences[i].getDescription() << '|'
           << experiences[i].getStartDate() << '|'
           << experiences[i].getEndDate();

        lines.push_back(ss.str());
    }

    return writeFile("candidate_experience.txt", lines);
}

bool Data::loadCandidateSkill(Candidate& candidate) {
    if (candidate.getIDcandidate() <= 0) {
        return false;
    }

    vector<string> lines;

    if (!readFile("candidate_skill.txt", lines)) {
        return false;
    }

    vector<Skill> skills;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector<string> fields = splitLine(lines[i]);

        if (fields.size() != 3) {
            continue;
        }

        if (fields[0] == to_string(candidate.getIDcandidate())) {
            Skill skill{};

            skill.name = fields[1];
            skill.level = stoi(fields[2]);
            skills.push_back(skill);
        }
    }

    candidate.professional.skills = skills;

    return true;
}

bool Data::loadCandidateCertificate(Candidate& candidate) {
    if (candidate.getIDcandidate() <= 0) {
        return false;
    }

    vector<string> lines;

    if (!readFile("candidate_certificate.txt", lines)) {
        return false;
    }

    vector<Certificate> certificates;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector<string> fields = splitLine(lines[i]);

        if (fields.size() != 4) {
            continue;
        }

        if (fields[0] == to_string(candidate.getIDcandidate())) {
            Certificate certificate{};

            certificate.organization = fields[2];

            try {
                certificate.certificateTypeID = stoi(fields[1]);
                certificate.value = stod(fields[3]);
            }
            catch (...) {
                return false;
            }

            certificates.push_back(certificate);
        }
    }

    candidate.professional.certificates = certificates;

    return true;
}

bool Data::loadCandidateExperience(Candidate& candidate) {
    if (candidate.getIDcandidate() <= 0) {
        return false;
    }

    vector<string> lines;

    if (!readFile("candidate_experience.txt", lines)) {
        return false;
    }

    vector<Experience> experiences;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }

        vector<string> fields = splitLine(lines[i]);

        if (fields.size() != 7) {
            continue;
        }

        if (fields[0] == to_string(candidate.getIDcandidate())) {
            int experienceID;

            try {
                experienceID = stoi(fields[1]);
            }
            catch (...) {
                return false;
            }

            Experience experience(
                experienceID,
                fields[2],
                fields[3],
                fields[4],
                fields[5],
                fields[6]
            );

            experiences.push_back(experience);
        }
    }

    candidate.experiences = experiences;

    return true;
}