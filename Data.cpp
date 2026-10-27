#include "class.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

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

bool Data::getCandidateByID(int candidateID, Candidate& candidate) {
    vector<string> lines;
    if (!readFile("candidates.txt", lines))
        return false;
    for (const string& line : lines) {
        if (line.empty() || line[0] == '#')
            continue;
        stringstream ss(line);
        string candidateIDText, userIDText;
        if (!getline(ss, candidateIDText, '|') ||
            !getline(ss, userIDText, '|'))
            continue;
        try {
            int savedCandidateID = stoi(candidateIDText);
            int userID = stoi(userIDText);

            if (savedCandidateID == candidateID) {
                return getCandidateByUserID(userID, candidate);
            }
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

bool Data::getCompanyByID(int companyID, Company& company) {
    vector<string> lines;
    if (!readFile("companies.txt", lines)) {
        return false;
    }
    for (const string& line : lines) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
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
            if (stoi(companyIDText) != companyID) {
                continue;
            }
            company.IDcompany = companyID;
            company.setIDuser(stoi(userIDText));
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

bool Data::saveJobSkill(const Job& job) {
    vector<string> lines;
    if (!readFile("job_skill.txt", lines)) {
        return false;
    }
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.empty()) {
            continue;
        }
        if (fields[0] == to_string(job.IDjob)) {
            lines.erase(lines.begin() + i);
            i--;
        }
    }
    const vector<Skill>& skills = job.getRequiredSkill();
    for (int i = 0; i < (int)skills.size(); i++) {
        stringstream ss;
        ss << job.IDjob << '|'
           << skills[i].name << '|'
           << skills[i].level;
        lines.push_back(ss.str());
    }
    return writeFile("job_skill.txt", lines);
}

bool Data::loadJobSkill(Job& job) {
    vector<string> lines;
    if (!readFile("job_skill.txt", lines)) {
        return false;
    }
    vector<Skill> skills;
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() != 3) {
            continue;
        }
        if (fields[0] != to_string(job.IDjob)) {
            continue;
        }
        try {
            Skill skill{};
            skill.name = fields[1];
            skill.level = stoi(fields[2]);
            skills.push_back(skill);
        }
        catch (...) {
            continue;
        }
    }
    job.requiredSkills = skills;
    return true;
}

bool Data::saveJobCertificate(const Job& job) {
    vector<string> lines;
    if (!readFile("job_certificate.txt", lines)) {
        return false;
    }
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.empty()) {
            continue;
        }
        if (fields[0] == to_string(job.IDjob)) {
            lines.erase(lines.begin() + i);
            i--;
        }
    }
    const vector<CertificateRequirementGroup>& groups = job.getCertificateGroups();
    for (int i = 0; i < (int)groups.size(); i++) {
        for (int j = 0; j < (int)groups[i].alternatives.size(); j++) {
            stringstream ss;
            ss << job.IDjob << '|'
               << i + 1 << '|'
               << groups[i].alternatives[j].certificateTypeID << '|'
               << groups[i].alternatives[j].minValue << '|'
               << (groups[i].isRequired ? 1 : 0);
            lines.push_back(ss.str());
        }
    }
    return writeFile("job_certificate.txt", lines);
}

bool Data::loadJobCertificate(Job& job) {
    vector<string> lines;
    if (!readFile("job_certificate.txt", lines)) {
        return false;
    }
    vector<CertificateRequirementGroup> groups;
    vector<int> groupIDs;
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() != 5) {
            continue;
        }
        if (fields[0] != to_string(job.IDjob)) {
            continue;
        }
        try {
            int groupID = stoi(fields[1]);
            int certificateTypeID = stoi(fields[2]);
            double minValue = stod(fields[3]);
            bool isRequired = (stoi(fields[4]) != 0);
            int index = -1;
            for (int j = 0; j < (int)groupIDs.size(); j++) {
                if (groupIDs[j] == groupID) {
                    index = j;
                    break;
                }
            }
            if (index == -1) {
                CertificateRequirementGroup group;
                group.isRequired = isRequired;
                groups.push_back(group);
                groupIDs.push_back(groupID);
                index = (int)groups.size() - 1;
            }

            CertificateRequirement requirement;
            requirement.certificateTypeID = certificateTypeID;
            requirement.minValue = minValue;
            groups[index].alternatives.push_back(requirement);
        }
        catch (...) {
            continue;
        }
    }
    job.certificateGroups = groups;
    return true;
}

bool Data::getJobsByCompanyID(int companyID, vector<Job>& jobs) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    jobs.clear();
    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 11 || fields[0].empty()
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
                job.nameMajor = fields[10];
                if (!loadJobSkill(job)) {
                    continue;
                }
                if (!loadJobCertificate(job)) {
                    continue;
                }
                jobs.push_back(job);
            }
        }
        catch (...) {
            continue;
        }
    }
    return true;
}

bool Data::getAllJobs(vector<Job>& jobs) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }

    jobs.clear();

    for (int i = 0; i < lines.size(); i++) {
        vector<string> fields = splitLine(lines[i]);

        if (fields.size() < 11 || fields[0].empty()
            || fields[0][0] == '#') {
            continue;
        }

        try {
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
            job.nameMajor = fields[10];

            if (!loadJobSkill(job)) {
                continue;
            }

            if (!loadJobCertificate(job)) {
                continue;
            }

            jobs.push_back(job);
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
        if (fields.size() < 11 || fields[0].empty() || fields[0][0] == '#') {
            continue;
        }
        try {
            if (stoi(fields[0]) != jobID) {
                continue;
            }
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
            job.nameMajor = fields[10];
            if (!loadJobSkill(job)) {
                return false;
            }
            if (!loadJobCertificate(job)) {
                return false;
            }
            return true;
        }
        catch (...) {
            continue;
        }
    }
    return false;
}


bool Data::saveJob(const Job& job) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 2) {
            continue;
        }
        try {
            if (stoi(fields[0]) == job.getIDjob()) {
                return false;
            }
        }
        catch (...) {
            continue;
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
       << (job.status ? 1 : 0) << '|'
       << job.nameMajor;
    lines.push_back(ss.str());
    if (!writeFile("jobs.txt", lines)) {
        return false;
    }
    if (!saveJobSkill(job)) {
        return false;
    }
    if (!saveJobCertificate(job)) {
        return false;
    }
    return true;
}


bool Data::updateJob(const Job& job) {
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    bool found = false;
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 11) {
            continue;
        }
        try {
            if (stoi(fields[0]) == job.getIDjob() && stoi(fields[1]) == job.IDcompany) {
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
                   << (job.status ? 1 : 0) << '|'
                   << job.nameMajor;
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
        return false;
    }
    if (!writeFile("jobs.txt", lines)) {
        return false;
    }
    if (!saveJobSkill(job)) {
        return false;
    }
    if (!saveJobCertificate(job)) {
        return false;
    }
    return true;
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
    for (int i = 0; i < (int)lines.size(); i++) {
        if (lines[i].empty() || lines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(lines[i]);
        if (fields.size() < 2) {
            continue;
        }
        try {
            if (stoi(fields[0]) == jobID && stoi(fields[1]) == companyID) {
                lines.erase(lines.begin() + i);
                found = true;
                break;
            }
        }
        catch (...) {
            continue;
        }
    }
    if (!found) {
        return false;
    }
    if (!writeFile("jobs.txt", lines)) {
        return false;
    }
    vector<string> skillLines;
    if (!readFile("job_skill.txt", skillLines)) {
        return false;
    }
    for (int i = 0; i < (int)skillLines.size(); i++) {
        if (skillLines[i].empty() || skillLines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(skillLines[i]);
        if (fields.empty()) {
            continue;
        }
        try {
            if (stoi(fields[0]) == jobID) {
                skillLines.erase(skillLines.begin() + i);
                i--;
            }
        }
        catch (...) {
            continue;
        }
    }
    if (!writeFile("job_skill.txt", skillLines)) {
        return false;
    }
    vector<string> certificateLines;
    if (!readFile("job_certificate.txt", certificateLines)) {
        return false;
    }
    for (int i = 0; i < (int)certificateLines.size(); i++) {
        if (certificateLines[i].empty() || certificateLines[i][0] == '#') {
            continue;
        }
        vector<string> fields = splitLine(certificateLines[i]);
        if (fields.empty()) {
            continue;
        }
        try {
            if (stoi(fields[0]) == jobID) {
                certificateLines.erase(certificateLines.begin() + i);
                i--;
            }
        }
        catch (...) {
            continue;
        }
    }
    if (!writeFile("job_certificate.txt", certificateLines)) {
        return false;
    }
    return true;
}

bool Data::getAllJobs(vector<Job>& jobs) {
    jobs.clear();
    vector<string> lines;
    if (!readFile("jobs.txt", lines)) {
        return false;
    }
    for (const string& line : lines) {
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);
        string jobIDText;
        if (!getline(ss, jobIDText, '|')) {
            continue;
        }
        try {
            int jobID = stoi(jobIDText);
            Job job;
            if (getJobByID(jobID, job)) {
                jobs.push_back(job);
            }
        } catch (...) {
            continue;
        }
    }
    return true;
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
        string levelStr, appliedAt, status;
        getline(ss, idStr, '|');
        getline(ss, candidateStr, '|');
        getline(ss, jobStr, '|');
        getline(ss, levelStr, '|');
        getline(ss, appliedAt, '|');
        getline(ss, status);
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
        string levelStr, appliedAt, status;
        getline(ss, idStr, '|');
        getline(ss, candidateStr, '|');
        getline(ss, jobStr, '|');
        getline(ss, levelStr, '|');
        getline(ss, appliedAt, '|');
        getline(ss, status);
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
        string levelStr, appliedAt, oldStatus;
        getline(ss, idStr, '|');
        getline(ss, candidateStr, '|');
        getline(ss, jobStr, '|');
        getline(ss, levelStr, '|');
        getline(ss, appliedAt, '|');
        getline(ss, oldStatus);

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
                << appliedAt << "|"
                << status;

        lines[i] = newLine.str();
        found = true;
        break;
    }
    if (!found)
        return false;
    return writeFile("applications.txt", lines);
}

bool Data::updateApplicationScore(int applicationID, float score) {
    vector<string> lines;
    if (!readFile("applications.txt", lines))
        return false;
    bool found = false;
    for (string& line : lines) {
        if (line.empty() || line[0] == '#')
            continue;
        stringstream ss(line);
        vector<string> fields;
        string field;
        while (getline(ss, field, '|')) {
            fields.push_back(field);
        }
        if (fields.size() < 6)
            continue;
        try {
            if (stoi(fields[0]) != applicationID)
                continue;
            ostringstream scoreText;
            scoreText << fixed << setprecision(2) << score;
            fields[3] = scoreText.str();
            line = "";
            for (int i = 0; i < (int)fields.size(); i++) {
                if (i > 0)
                    line += "|";
                line += fields[i];
            }
            found = true;
            break;
        }
        catch (...) {
            continue;
        }
    }
    if (!found)
        return false;
    return writeFile("applications.txt", lines);
}

bool Data::saveApplication(const Application& application) {
    vector<string> lines;
    if (!readFile("applications.txt", lines))
        return false;
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
         << application.appliedAt << "|"
         << application.status;
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

    return writeFile("candidates.txt", lines);
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
        if (fields.empty()) {
            continue;
        }

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

    if (!readFile("candidate_certificate.txt", lines)) {
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

    return writeFile("candidate_certificate.txt", lines);
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
