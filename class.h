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

        // updateCV();
        //bool loadCandidate(int candidateID);
        //bool applyJob(Data& dt, int jobID);

        int getIDcandidate() const { return IDcandidate; }
        PersonalInfo getPersonalInfo() const { return personal; }
        ProfessionalInfo getProfessionalInfo() const { return professional; }
        vector<Experience> getExperiences() const { return experiences; }

        void setIDcandidate(int id) { IDcandidate = id; }
        void setPersonalInfo(const PersonalInfo& p) { personal = p; }
        void setProfessionalInfo(const ProfessionalInfo& p) { professional = p; }
        void setExperiences(const vector<Experience>& e) { experiences = e; }
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
        string getTitle() const { return title; }
        string getDescription() const { return description; }
        string getLocation() const { return location; }
        float getMinGPA() const { return minGPA; }
        int getMinExperience() const { return minExperience; }
        string getDeadline() const { return deadline; }
        int getSalary() const { return salary; }
        bool getStatus() const { return status; }
        string getNameMajor() const { return nameMajor; }

        void setIDjob(int id) { IDjob = id; }
        void setIDcompany(int id) { IDcompany = id; }
        void setTitle(const string& s) { title = s; }
        void setDescription(const string& s) { description = s; }
        void setLocation(const string& s) { location = s; }
        void setMinGPA(float gpa) { minGPA = gpa; }
        void setMinExperience(int exp) { minExperience = exp; }
        void setDeadline(const string& d) { deadline = d; }
        void setSalary(int s) { salary = s; }
        void setStatus(bool s) { status = s; }
        void setNameMajor(const string& major) { nameMajor = major; }

        //bool loadJob(int jobID);
        //bool isExpired() const;
        //bool isEligible(const Candidate& candidate) const;
};

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
        void updateCompany();

        void createJob();
        void displayJobs();
        void displayJobDetail();

        void updateJob();
        void deleteJob();
        void closeJob();

        int getIDcompany() const { return IDcompany; }
        string getNameCompany() const { return nameCompany; }
        string getPhoneCompany() const { return phoneCompany; }
        string getAddressCompany() const { return addressCompany; }
        string getDescription() const { return description; }
        string getWebsite() const { return website; }
        vector<Job> getJobs() const { return jobs; }

        void setIDcompany(int id) { IDcompany = id; }
        void setNameCompany(string name) { nameCompany = name; }
        void setPhoneCompany(string phone) { phoneCompany = phone; }
        void setAddressCompany(string address) { addressCompany = address; }
        void setDescription(string des) { description = des; }
        void setWebsite(string web) { website = web; }

        //bool loadCompany(int companyID);
        //void displayApplications();
        //void updateApplicationStatus(int applicationID);
};

class Application {
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

        int getIDappli() const { return IDappli; }
        int getIDcandidate() const { return IDcandidate; }
        int getIDjob() const { return IDjob; }
        float getAppropriateLevel() const { return appropriateLevel; }
        string getStatus() const { return status; }
        string getAppliedAt() const { return appliedAt; }
        void setStatus(const string& status) { this->status = status; }

        //bool submit(Data& dt);
        //bool updateStatus(Data& dt);
        //void displayApplication();
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

        bool getCompanyByUserID(int userID, Company& company);
};

#endif
