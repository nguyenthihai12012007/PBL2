#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
using namespace std;


class User {
        int IDuser;
        string email;
        string password;
        string role;

    public:
        User() {
            IDuser = 0;
            email = "";
            password = "";
            role = "";
        }

        User(int id, string e, string p, string r) {
            IDuser = id;
            email = e;
            password = p;
            role = r;
        }

        // login()
};


struct PersonalInfo {
    string fullName;
    string birthday;
    string gender;
    string phoneCandidate;
    string addressCandidate;
};


struct Skill {
    string name;
    int level;
};


struct Certificate {
    string name;
    string organization;
    string levelOrScore;
};


class Experience {
        int IDexperience;
        string companyName;
        string position;
        string description;
        string startDate;
        string endDate;

    public:
        Experience() {
            IDexperience = 0;
            companyName = "";
            position = "";
            description = "";
            startDate = "";
            endDate = "";
        }

        Experience(int id, string company, string pos,
                   string des, string start, string end) {
            IDexperience = id;
            companyName = company;
            position = pos;
            description = des;
            startDate = start;
            endDate = end;
        }

        void inputExperience();
        void displayExperience();
};


struct ProfessionalInfo {
    string major;
    string school;
    string educationLevel;
    float GPA;

    vector<Skill> skills;
    vector<Certificate> certificates;
};


class Candidate : public User {
        int IDcandidate;
        PersonalInfo personal;
        ProfessionalInfo professional;

        vector<Experience> experiences;

    public:
        Candidate();

        Candidate(int id, string email, string password);

        void inputCandidate();
        void displayCandidate();
        void updateCandidate();
        
        void addSkill();
        void updateSkill();
        void deleteSkill();

        void addCertificate();
        void updateCertificate();
        void deleteCertificate();

        void addExperience();
        void updateExperience();
        void deleteExperience();

        // updateCV();
        // applyJob();
};

class Job {
        int IDjob;
        int IDcompany;
        string title;
        string description;
        string location;
        float minGPA;
        int minExperience;
        string deadline;
        int salary;
        bool status;
        string nameMajor;
        vector<Skill> requiredSkills;
        //vector<Certificate> requiredCertificates;

    public :
        Job();
        void inputJob(int companyID);
        void displayJob();
        void updateJob();
        bool checkStatus();
        int getIDjob() const;
        void setStatus(bool s);
};

class Company : public User {
        int IDcompany;
        string nameCompany;
        string phoneCompany;
        string addressCompany;
        string description;
        string website;
        vector<Job> jobs;

    public :
        Company();
        Company(int id, string email, string password);
        void inputCompany();
        void displayCompany();
        void updateCompany();
        void createJob();
        void displayJobs();
        void updateJob();
        void deleteJob();
        void closeJob();
};

class Application {
        int IDappli;
        int IDcandidate;
        int IDJob;
        float appropriateLevel;

    public :
        //submit();
        //updateStatus();
};

#endif