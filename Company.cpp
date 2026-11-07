#include "class.h"
#include "Matching.h"
#include <iostream>
#include <limits>
using namespace std;

Company::Company() 
    : User(), IDcompany(0) {
}

Company::Company(int id, string email, string password)
    : User(id, email, password, "COMPANY"), IDcompany(id) {
}

bool Company::addJob(const Job& job, Data& dt) {
    if (job.getIDcompany() != IDcompany) {
        return false;
    }
    if (dt.saveJob(job)) {
        jobs.push_back(job);
        return true;
    }
    return false;
}

bool Company::closeJob(int jobID, Data& dt) {
    if (dt.closeJob(jobID, IDcompany)) {
        for (auto& j : jobs) {
            if (j.getIDjob() == jobID) {
                j.setStatus(false);
                break;
            }
        }
        return true;
    }
    return false;
}

bool Company::deleteJob(int jobID, Data& dt) {
    if (dt.deleteJob(jobID, IDcompany)) {
        for (auto it = jobs.begin(); it != jobs.end(); ++it) {
            if (it->getIDjob() == jobID) {
                jobs.erase(it);
                break;
            }
        }
        return true;
    }
    return false;
}

bool Company::updateJob(const Job& job, Data& dt) {
    if (job.getIDcompany() != IDcompany) {
        return false;
    }
    if (dt.updateJob(job)) {
        for (size_t i = 0; i < jobs.size(); ++i) {
            if (jobs[i].getIDjob() == job.getIDjob()) {
                jobs[i] = job;
                break;
            }
        }
        return true;
    }
    return false;
}

bool Company::updateCompany(const string& name, const string& phone, const string& address, const string& website, const string& desc, Data& dt) {
    nameCompany = name;
    phoneCompany = phone;
    addressCompany = address;
    this->website = website;
    description = desc;
    return dt.saveCompany(*this);
}