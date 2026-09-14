#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
using namespace std;

class User {
        string email;
        string password;
    public:
        User() {
            email = "";
            password = "";
        }
        User(string e, string p) {
            email = e;
            password = p;
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
};

struct ProfessionalInfo {
    string major;
    float GPA;
    string experience;
    vector<Skill> skills;
    vector<Certificate> certificates;
};

class Candidate : public User {
        int IDcandidate;
        PersonalInfo personal;
        ProfessionalInfo professional;
    public :
        Candidate();
        Candidate(int id, string email, string password);

        void inputCandidate();
        void displayCandidate();

        //updateCV();
        //applyJob();
};

class Recruiter : public User {
        int IDcompany;
        string nameCompany;
        string phoneCompany;
        string addressCompany;
        string description;
    public :
        //createJob();
        //updateJob();
        //updateCompany();
};

class Job {
        int IDjob;
        string nameMajor;
        // requiredSkill;
        string requiredExperience;
        float requiredGPA;
        // requiredCertificate;
        int salary;
        bool status;
    public :
        //displayJob();
        //updateJob();
        //closeJob();
        //checkStatus();
};

class Application {
        int IDappli;
        int IDcandidate;
        int IDJob;
        float appropriateLevel;
    public :
        //submic();
        //updateStatus();
};

#endif