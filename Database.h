
#ifndef DATABASE_H
#define DATABASE_H

#include "class.h"
#include <mysql.h>
#include <vector>

class Database {
    private:
        MYSQL* conn;

    public:
        Database();
        ~Database();

        bool connect();
        void disconnect();
        bool isConnected();

        bool getUserByEmail(const std::string& email, User& user);
        bool registerUser(const User& user);
        //bool getUserByID(int userID, User& user);
        //bool updateUser(const User& user);
        //bool deleteUser(int userID);
        //bool getUsers(vector<User>& users);

        bool getJobs(std::vector<Job>& jobs);
        //bool getJobByID(int jobID, Job& job);
        //bool getJobsByCompanyID(int companyID, std::vector<Job>& jobs);
        bool insertJob(const Job& job);
        bool updateJob(const Job& job);
        bool deleteJob(int jobID, int companyID);
        //bool updateJobStatus(int jobID, bool status);

        bool getCompanies(std::vector<Company>& companies);
        //bool getCompanyByID(int companyID, Company& company);
        bool getCompanyByUserID(int userID, Company& company);
        //bool insertCompany(const Company& company);
        //bool updateCompany(const Company& company);

        //bool getCandidates(std::vector<Candidate>& candidates);
        //bool getCandidateByID(int candidateID, Candidate& candidate);
        bool getCandidateByUserID(int userID, Candidate& candidate);
        //bool insertCandidate(const Candidate& candidate);
        //bool updateCandidate(const Candidate& candidate);

        //bool getCandidateSkills(int candidateID, std::vector<Skill>& skills);
        //bool getCandidateCertificates(int candidateID, std::vector<Certificate>& certificates);
        //bool getCandidateExperiences(int candidateID, std::vector<Experience>& experiences);

        //bool getApplicationsByCandidateID(int candidateID, std::vector<Application>& applications); 
        //bool getApplicationsByJobID(int jobID, std::vector<Application>& applications);
        //bool insertApplication(const Application& application);
        ///bool updateApplicationStatus(int applicationID, const string& status);

        //string getLastError() const;
};

#endif
