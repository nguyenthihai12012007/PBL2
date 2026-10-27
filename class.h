#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
using namespace std;

class Data;

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

        int getIDuser() const { return IDuser; }
        string getEmail() const { return email; }
        string getRole() const { return role; }
        string getPassword() const { return password; }

        void setIDuser(int id) { IDuser = id; }
        void setEmail(const string& e) { email = e; }
        void setPassword(const string& p) { password = p; }
        void setRole(const string& r) { role = r; }

        bool login(Data& dt, const string& email, const string& password);
};


struct PersonalInfo {
    string fullName;
    string birthday;
    string gender;
    string phoneCandidate;
    string addressCandidate;
};

struct Skill {
    int IDskill;
    string name;
    int level;
};

struct CertificateRequirement {
    int certificateTypeID;
    double minValue;
};

struct Certificate {
    int IDcertificate;
    int certificateTypeID;
    string organization;
    double value; 
};

struct CertificateRequirementGroup {
    vector<CertificateRequirement> alternatives;
    bool isRequired;
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

        string getCompanyName() const { return companyName; }
        string getPosition() const { return position; }
        string getDescription() const { return description; }
        string getStartDate() const { return startDate; }
        string getEndDate() const { return endDate; }
        int getIDexperience() const { return IDexperience; }
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
        friend class Data;
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

        const vector<Skill>& getSkill() const;
        const vector<Certificate>& getCertificate() const;       
        int getTimeExperience() const;
        double getGPACandidate() const { return professional.GPA ;}

        int getIDcandidate() const { return IDcandidate; }
        PersonalInfo getPersonalInfo() const { return personal; }
        ProfessionalInfo getProfessionalInfo() const { return professional; }
        vector<Experience> getExperiences() const { return experiences; }

        void setIDCandidate(int id) { IDcandidate = id;}

        // updateCV();
        //bool loadCandidate(int candidateID);
        //bool applyJob(Data& dt, int jobID);
};

class Job {
        friend class Data;
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
        vector<CertificateRequirementGroup> certificateGroups;

    public :
        Job();

        void inputRequiredSkills();
        void inputCertificateGroups();
        void inputJob(int companyID);
        void displayCertificate();
        void displayJob();
        void updateJob();
        bool checkStatus() { return status; }

        const vector<Skill>& getRequiredSkill() const;
        const vector<CertificateRequirementGroup>& getCertificateGroups() const;

        int getIDjob() const { return IDjob; }
        int getIDcompany() const { return IDcompany; }
        int getMinExperience() const { return minExperience; }
        float getMinGPA() const { return minGPA; }
        bool getStatus() const { return status; }
        string getDeadline() const { return deadline; }
        string getNameMajor() const { return nameMajor; }

        void setIDjob(int id) { IDjob = id; }
        void setStatus(bool s) { status = s; }

        bool isExpired() const;
        bool isEligible(const Candidate& candidate) const;
};

struct JobMatchResult {
    Job job;
    double score;
};

vector<JobMatchResult> matching(
    const vector<Job>& jobs,
    const Candidate& candidate
);

class Company : public User {
        friend class Data;
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
        void updateCompany(Data& dt);

        void createJob(Data& dt);
        void displayJobs(Data& dt);
        void displayJobDetail(Data& dt);

        void updateJob(Data& dt);
        void deleteJob(Data& dt);
        void closeJob(Data& dt);

        int getIDcompany() const { return IDcompany; }
        void setIDcompany(int id) { IDcompany = id; }
        void displayApplications(Data& dt);
};

class Application {
        friend class Data;
        int IDappli;
        int IDcandidate;
        int IDjob;
        float appropriateLevel;
        string status;
        string appliedAt;

    public :
        Application() {
            IDappli = 0;
            IDcandidate = 0;
            IDjob = 0;
            appropriateLevel = 0.0f;
            status = "";
            appliedAt = "";
        }

        Application(int id, int candidateID, int jobID, float level, string status, string appliedAt) {
            IDappli = id;
            IDcandidate = candidateID;
            IDjob = jobID;
            appropriateLevel = level;
            this->status = status;
            this->appliedAt = appliedAt;
        }

        string getStatus() const { return status; }
        int getIDcandidate() const { return IDcandidate; }
        int getIDjob() const { return IDjob; }
        int getIDappli() const { return IDappli; }
        void setAppropriateLevel(float level) { appropriateLevel = level; }
        void setStatus(const string& s) { status = s; }
        void displayApplication() const;
};

class Data {
    public:
        bool readFile(const string& filename, vector<string>& lines);
        bool writeFile(const string& filename, vector<string>& lines);
        int getNextID(const string& filename);

        bool findUserByEmail(const string& email, User& user);
        bool emailExists(const string& email);
        bool registerUser(const User& user);

        bool getCandidateByUserID(int userID, Candidate& candidate);
        bool getCandidateByID(int candidateID, Candidate& candidate); 

        bool getCompanyByUserID(int userID, Company& company);
        bool saveCompany(const Company& company);

        bool saveJobSkill(const Job& job);
        bool loadJobSkill(Job& job);
        bool saveJobCertificate(const Job& job);
        bool loadJobCertificate(Job& job);
        
        bool getJobsByCompanyID(int companyID, vector<Job>& jobs);
        bool getJobByID(int jobID, Job& job);
        bool saveJob(const Job& job);
        bool updateJob(const Job& job);
        bool closeJob(int jobID, int companyID);
        bool deleteJob(int jobID, int companyID);

        bool getApplicationsByCompanyID(int companyID, vector<Application>& applications);
        bool getApplicationByID(int applicationID, Application& application);
        bool updateApplicationStatus(int applicationID, const string& status);
        bool updateApplicationScore(int applicationID, float score);
        bool saveApplication(const Application& application);

        bool saveCandidate(const Candidate& candidate);
        bool saveCandidateSkill (const Candidate& candidate);
        bool saveCandidateCertificate (const Candidate& candidate);
        bool saveCandidateExperience(const Candidate& candidate);

        bool loadCandidateSkill(Candidate& candidate);
        bool loadCandidateCertificate(Candidate& candidate);
        bool loadCandidateExperience(Candidate& candidate);

        bool getAllJobs(vector<Job>& jobs);
};

#endif
