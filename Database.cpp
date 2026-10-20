#include "Database.h"
#include <iostream>
#include <string>
#include <cstring>
#include <mysql.h>

using namespace std;

Database::Database() {
    conn = mysql_init(nullptr);
}

Database::~Database() {
    disconnect();
    if (conn != nullptr) {
        mysql_close(conn);
        conn = nullptr;
    }
}

bool Database::connect() {
    if (conn == nullptr)
        conn = mysql_init(nullptr);
    if (conn == nullptr) {
        cerr << "Khong khoi tao duoc MySQL!\n";
        return false;
    }
    if (mysql_real_connect(
            conn,
            "localhost",
            "root",
            "matkhau mysql",
            "JobSystem",
            3306,
            nullptr,
            0
        ) == nullptr) {
            cerr << "Loi ket noi MySQL: " << mysql_error(conn) << endl;
            return false;
    }
    mysql_set_character_set(conn, "utf8mb4");
    cout << "Ket noi MySQL thanh cong!\n";
    return true;
}

void Database::disconnect() {
    if (conn != nullptr) {
        mysql_close(conn);
        conn = nullptr;
    }
}

bool Database::isConnected() {
    return conn != nullptr && mysql_ping(conn) == 0;
}

bool Database::getUserByEmail(const string& email, User& user) {
    if (conn == nullptr) 
        return false;
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) {
        return false;
    }
    const char* query =
        "SELECT user_id, email, password, role "
        "FROM User WHERE email = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }
    MYSQL_BIND param[1]{};
    unsigned long emailLength = email.size();
    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = const_cast<char*>(email.c_str());
    param[0].buffer_length = emailLength;
    param[0].length = &emailLength;
    if (mysql_stmt_bind_param(stmt, param) != 0 || mysql_stmt_execute(stmt) != 0) {
        cerr << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    } 
    MYSQL_BIND output[4]{};
    int userID = 0;
    char emailBuffer[256]{};
    char passwordBuffer[256]{};
    char roleBuffer[32]{};
    unsigned long emailSize = 0;
    unsigned long passwordSize = 0;
    unsigned long roleSize = 0;

    output[0].buffer_type = MYSQL_TYPE_LONG;
    output[0].buffer = &userID;

    output[1].buffer_type = MYSQL_TYPE_STRING;
    output[1].buffer = emailBuffer;
    output[1].buffer_length = sizeof(emailBuffer);
    output[1].length = &emailSize;

    output[2].buffer_type = MYSQL_TYPE_STRING;
    output[2].buffer = passwordBuffer;
    output[2].buffer_length = sizeof(passwordBuffer);
    output[2].length = &passwordSize;

    output[3].buffer_type = MYSQL_TYPE_STRING;
    output[3].buffer = roleBuffer;
    output[3].buffer_length = sizeof(roleBuffer);
    output[3].length = &roleSize;

    if (mysql_stmt_bind_result(stmt, output) != 0) {
        mysql_stmt_close(stmt);
        return false;
    }
    int fetchStatus = mysql_stmt_fetch(stmt);
    if (fetchStatus != 0) {
        mysql_stmt_close(stmt);
        return false;
    }
    user.IDuser = userID;
    user.email = emailBuffer;
    user.password = passwordBuffer;
    user.role = roleBuffer;
    mysql_stmt_close(stmt);
    return true;
}

bool Database::registerUser(const User& user) {
    if (conn == nullptr || mysql_ping(conn) != 0) {
        cerr << "Chua ket noi MySQL!\n";
        return false;
    }
    const char* query =
        "INSERT INTO User (email, password, role) "
        "VALUES (?, ?, ?)";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) {
        cerr << "Khong khoi tao duoc statement!\n";
        return false;
    }
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Loi prepare: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }
    string email = user.getEmail();
    string password = user.getPassword();
    string role = user.getRole();

    MYSQL_BIND param[3]{};
    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = email.data();
    param[0].buffer_length = email.size();

    param[1].buffer_type = MYSQL_TYPE_STRING;
    param[1].buffer = password.data();
    param[1].buffer_length = password.size();

    param[2].buffer_type = MYSQL_TYPE_STRING;
    param[2].buffer = role.data();
    param[2].buffer_length = role.size();

    bool success = false;
    if (mysql_stmt_bind_param(stmt, param) == 0 &&
        mysql_stmt_execute(stmt) == 0) {
        success = true;
    } else {
        cerr << "Loi dang ky: " << mysql_stmt_error(stmt) << endl;
    }
    mysql_stmt_close(stmt);
    return success;
}

bool Database::getJobs(vector<Job>& jobs) {
    if (conn == nullptr || mysql_ping(conn) != 0) {
        cerr << "Chua ket noi MySQL!\n";
        return false;
    }
    const char* query =
        "SELECT job_id, company_id, title, description, "
        "location, min_GPA, min_experience, "
        "deadline, salary, status "
        "FROM Job";
    if (mysql_query(conn, query) != 0) {
        cerr << "Loi truy van Job: " << mysql_error(conn) << endl;
        return false;
    }
    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        cerr << "Loi doc ket qua: " << mysql_error(conn) << endl;
        return false;
    }
    jobs.clear();
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        Job job;
        job.setIDjob(row[0] ? stoi(row[0]) : 0);
        job.setIDcompany(row[1] ? stoi(row[1]) : 0);
        job.setTitle(row[2] ? row[2] : "");
        job.setDescription(row[3] ? row[3] : "");
        job.setLocation(row[4] ? row[4] : "");
        job.setMinGPA(row[5] ? stof(row[5]) : 0.0f);
        job.setMinExperience(row[6] ? stoi(row[6]) : 0);
        job.setDeadline(row[7] ? row[7] : "");
        job.setSalary(row[8] ? stoi(row[8]) : 0);
        job.setStatus(row[9] ? stoi(row[9]) != 0 : false);

        jobs.push_back(job);
    }
    mysql_free_result(result);
    return true;
}

bool Database::insertJob(const Job& job) {
    if (conn == nullptr)
        return false;
    const char* query =
        "INSERT INTO Job "
        "(company_id, title, description, location, "
        "min_GPA, min_experience, deadline, salary, status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr)
        return false;
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }
    int companyID = job.getIDcompany();
    float gpa = job.getMinGPA();
    int experience = job.getMinExperience();
    int salary = job.getSalary();
    int status = job.getStatus() ? 1 : 0;
    string title = job.getTitle();
    string description = job.getDescription();
    string location = job.getLocation();
    string deadline = job.getDeadline();

    MYSQL_BIND bind[9] = {};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &companyID;

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = title.data();
    bind[1].buffer_length = title.size();

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = description.data();
    bind[2].buffer_length = description.size();

    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = location.data();
    bind[3].buffer_length = location.size();

    bind[4].buffer_type = MYSQL_TYPE_FLOAT;
    bind[4].buffer = &gpa;

    bind[5].buffer_type = MYSQL_TYPE_LONG;
    bind[5].buffer = &experience;

    bind[6].buffer_type = MYSQL_TYPE_STRING;
    bind[6].buffer = deadline.data();
    bind[6].buffer_length = deadline.size();

    bind[7].buffer_type = MYSQL_TYPE_LONG;
    bind[7].buffer = &salary;

    bind[8].buffer_type = MYSQL_TYPE_LONG;
    bind[8].buffer = &status;

    bool success = false;
    if (mysql_stmt_bind_param(stmt, bind) == 0 && mysql_stmt_execute(stmt) == 0) {
        success = true;
    } else {
        cerr << mysql_stmt_error(stmt) << endl;
    }
    mysql_stmt_close(stmt);
    return success;
}

bool Database::updateJob(const Job& job) {
    if (conn == nullptr) 
        return false;
    const char* query =
        "UPDATE Job SET title=?, description=?, location=?, "
        "min_GPA=?, min_experience=?, deadline=?, salary=?, "
        "status=? WHERE job_id=? AND company_id=?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) 
        return false;
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }
    string title = job.getTitle();
    string description = job.getDescription();
    string location = job.getLocation();
    string deadline = job.getDeadline();
    float gpa = job.getMinGPA();
    int experience = job.getMinExperience();
    int salary = job.getSalary();
    int status = job.getStatus() ? 1 : 0;
    int jobID = job.getIDjob();
    int companyID = job.getIDcompany();

    MYSQL_BIND b[10] = {};
    b[0].buffer_type = MYSQL_TYPE_STRING;
    b[0].buffer = title.data();
    b[0].buffer_length = title.size();

    b[1].buffer_type = MYSQL_TYPE_STRING;
    b[1].buffer = description.data();
    b[1].buffer_length = description.size();

    b[2].buffer_type = MYSQL_TYPE_STRING;
    b[2].buffer = location.data();
    b[2].buffer_length = location.size();

    b[3].buffer_type = MYSQL_TYPE_FLOAT;
    b[3].buffer = &gpa;

    b[4].buffer_type = MYSQL_TYPE_LONG;
    b[4].buffer = &experience;

    b[5].buffer_type = MYSQL_TYPE_STRING;
    b[5].buffer = deadline.data();
    b[5].buffer_length = deadline.size();

    b[6].buffer_type = MYSQL_TYPE_LONG;
    b[6].buffer = &salary;

    b[7].buffer_type = MYSQL_TYPE_LONG;
    b[7].buffer = &status;

    b[8].buffer_type = MYSQL_TYPE_LONG;
    b[8].buffer = &jobID;

    b[9].buffer_type = MYSQL_TYPE_LONG;
    b[9].buffer = &companyID;

    bool success = false;
    if (mysql_stmt_bind_param(stmt, b) == 0 && mysql_stmt_execute(stmt) == 0) {
        success = true;
    } else {
        cerr << mysql_stmt_error(stmt) << endl;
    }
    mysql_stmt_close(stmt);
    return success;
}

bool Database::deleteJob(int jobID, int companyID) {
    if (conn == nullptr || mysql_ping(conn) != 0) {
        cerr << "Chua ket noi MySQL!\n";
        return false;
    }
    const char* query = "DELETE FROM Job WHERE job_id = ? AND company_id = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) {
        cerr << "Khong khoi tao duoc statement!" << endl;
        return false;
    }
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Loi prepare: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[2] = {};
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &jobID;
    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &companyID;
    bool success = false;
    if (mysql_stmt_bind_param(stmt, bind) == 0 && mysql_stmt_execute(stmt) == 0) {
        success = mysql_stmt_affected_rows(stmt) > 0;
        if (!success) 
            cout << "Khong tim thay cong viec hoac ban khong co quyen xoa!\n";
    } else {
        cerr << "Loi xoa cong viec: " << mysql_stmt_error(stmt) << endl;
    }
    mysql_stmt_close(stmt);
    return success;
}

bool Database::getCompanies(vector<Company>& companies) {
    if (conn == nullptr) {
        return false;
    }
    const char* query =
        "SELECT c.company_id, c.user_id, "
        "u.email, u.password, "
        "c.name_company, c.phone, c.address, "
        "c.description, c.website "
        "FROM Company c "
        "JOIN User u ON c.user_id = u.user_id";
    if (mysql_query(conn, query) != 0) {
        cerr << "Loi doc Company: " << mysql_error(conn) << endl;
        return false;
    }
    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        cerr << "Loi lay ket qua: " << mysql_error(conn) << endl;
        return false;
    }
    MYSQL_ROW row;
    companies.clear();
    while ((row = mysql_fetch_row(result)) != nullptr) {
        int companyID = row[0] ? stoi(row[0]) : 0;
        int userID = row[1] ? stoi(row[1]) : 0;
        string email = row[2] ? row[2] : "";
        string password = row[3] ? row[3] : "";
        Company company(userID, email, password);
        company.setIDcompany(companyID);
        company.setNameCompany(row[4] ? row[4] : "");
        company.setPhoneCompany(row[5] ? row[5] : "");
        company.setAddressCompany(row[6] ? row[6] : "");
        company.setDescription(row[7] ? row[7] : "");
        company.setWebsite(row[8] ? row[8] : "");
        companies.push_back(company);
    }
    mysql_free_result(result);
    return true;
}

bool Database::getCompanyByUserID(int userID, Company& company) {
    if (conn == nullptr || mysql_ping(conn) != 0) {
        cerr << "Chua ket noi MySQL!\n";
        return false;
    }
    const char* query =
        "SELECT c.company_id, u.email, u.password, "
        "c.name_company, c.phone, c.address, "
        "c.description, c.website "
        "FROM Company c "
        "JOIN User u ON c.user_id = u.user_id "
        "WHERE c.user_id = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) {
        cerr << "Khong khoi tao duoc statement!\n";
        return false;
    }
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Loi prepare: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND param[1]{};
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &userID;
    if (mysql_stmt_bind_param(stmt, param) != 0 ||
        mysql_stmt_execute(stmt) != 0) {
        cerr << "Loi truy van Company: "
             << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    int companyID = 0;
    char email[256]{};
    char password[256]{};
    char nameCompany[151]{};
    char phone[21]{};
    char address[256]{};
    char description[4096]{};
    char website[256]{};

    unsigned long emailLength = 0;
    unsigned long passwordLength = 0;
    unsigned long nameLength = 0;
    unsigned long phoneLength = 0;
    unsigned long addressLength = 0;
    unsigned long descriptionLength = 0;
    unsigned long websiteLength = 0;

    MYSQL_BIND output[8]{};
    output[0].buffer_type = MYSQL_TYPE_LONG;
    output[0].buffer = &companyID;

    output[1].buffer_type = MYSQL_TYPE_STRING;
    output[1].buffer = email;
    output[1].buffer_length = sizeof(email);
    output[1].length = &emailLength;

    output[2].buffer_type = MYSQL_TYPE_STRING;
    output[2].buffer = password;
    output[2].buffer_length = sizeof(password);
    output[2].length = &passwordLength;

    output[3].buffer_type = MYSQL_TYPE_STRING;
    output[3].buffer = nameCompany;
    output[3].buffer_length = sizeof(nameCompany);
    output[3].length = &nameLength;

    output[4].buffer_type = MYSQL_TYPE_STRING;
    output[4].buffer = phone;
    output[4].buffer_length = sizeof(phone);
    output[4].length = &phoneLength;

    output[5].buffer_type = MYSQL_TYPE_STRING;
    output[5].buffer = address;
    output[5].buffer_length = sizeof(address);
    output[5].length = &addressLength;

    output[6].buffer_type = MYSQL_TYPE_STRING;
    output[6].buffer = description;
    output[6].buffer_length = sizeof(description);
    output[6].length = &descriptionLength;

    output[7].buffer_type = MYSQL_TYPE_STRING;
    output[7].buffer = website;
    output[7].buffer_length = sizeof(website);
    output[7].length = &websiteLength;

    if (mysql_stmt_bind_result(stmt, output) != 0) {
        cerr << "Loi bind result: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }
    int fetchStatus = mysql_stmt_fetch(stmt);
    if (fetchStatus != 0) {
        if (fetchStatus != MYSQL_NO_DATA) {
            cerr << "Loi doc Company: " << mysql_stmt_error(stmt) << endl;
        }
        mysql_stmt_close(stmt);
        return false;
    }

    company.IDuser = userID;
    company.setIDcompany(companyID);
    company.setEmail(email);
    company.setPassword(password);
    company.setNameCompany(nameCompany);
    company.setPhoneCompany(phone);
    company.setAddressCompany(address);
    company.setDescription(description);
    company.setWebsite(website);
    mysql_stmt_close(stmt);
    return true;
}

bool Database::getCandidateByUserID(int userID, Candidate& candidate) {
    if (conn == nullptr || mysql_ping(conn) != 0) {
        cerr << "Chua ket noi MySQL!\n";
        return false;
    }
    const char* query =
        "SELECT c.candidate_id, u.email, u.password, "
        "c.full_name, c.birthday, c.gender, c.phone, c.address, "
        "c.major, c.school, c.education_level, c.GPA "
        "FROM Candidate c "
        "JOIN User u ON c.user_id = u.user_id "
        "WHERE c.user_id = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (stmt == nullptr) {
        cerr << "Khong khoi tao duoc statement!\n";
        return false;
    }
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        cerr << "Loi prepare: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND param[1]{};
    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &userID;
    if (mysql_stmt_bind_param(stmt, param) != 0 ||
        mysql_stmt_execute(stmt) != 0) {
        cerr << "Loi truy van Candidate: "
             << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }

    int candidateID = 0;
    char email[256]{};
    char password[256]{};
    char fullName[101]{};
    char birthday[11]{};
    char gender[21]{};
    char phone[21]{};
    char address[256]{};
    char major[101]{};
    char school[151]{};
    char educationLevel[51]{};
    float GPA = 0.0f;

    unsigned long emailLength = 0;
    unsigned long passwordLength = 0;
    unsigned long fullNameLength = 0;
    unsigned long birthdayLength = 0;
    unsigned long genderLength = 0;
    unsigned long phoneLength = 0;
    unsigned long addressLength = 0;
    unsigned long majorLength = 0;
    unsigned long schoolLength = 0;
    unsigned long educationLevelLength = 0;

    MYSQL_BIND output[12]{};
    output[0].buffer_type = MYSQL_TYPE_LONG;
    output[0].buffer = &candidateID;

    output[1].buffer_type = MYSQL_TYPE_STRING;
    output[1].buffer = email;
    output[1].buffer_length = sizeof(email);
    output[1].length = &emailLength;

    output[2].buffer_type = MYSQL_TYPE_STRING;
    output[2].buffer = password;
    output[2].buffer_length = sizeof(password);
    output[2].length = &passwordLength;

    output[3].buffer_type = MYSQL_TYPE_STRING;
    output[3].buffer = fullName;
    output[3].buffer_length = sizeof(fullName);
    output[3].length = &fullNameLength;

    output[4].buffer_type = MYSQL_TYPE_STRING;
    output[4].buffer = birthday;
    output[4].buffer_length = sizeof(birthday);
    output[4].length = &birthdayLength;

    output[5].buffer_type = MYSQL_TYPE_STRING;
    output[5].buffer = gender;
    output[5].buffer_length = sizeof(gender);
    output[5].length = &genderLength;

    output[6].buffer_type = MYSQL_TYPE_STRING;
    output[6].buffer = phone;
    output[6].buffer_length = sizeof(phone);
    output[6].length = &phoneLength;

    output[7].buffer_type = MYSQL_TYPE_STRING;
    output[7].buffer = address;
    output[7].buffer_length = sizeof(address);
    output[7].length = &addressLength;

    output[8].buffer_type = MYSQL_TYPE_STRING;
    output[8].buffer = major;
    output[8].buffer_length = sizeof(major);
    output[8].length = &majorLength;

    output[9].buffer_type = MYSQL_TYPE_STRING;
    output[9].buffer = school;
    output[9].buffer_length = sizeof(school);
    output[9].length = &schoolLength;

    output[10].buffer_type = MYSQL_TYPE_STRING;
    output[10].buffer = educationLevel;
    output[10].buffer_length = sizeof(educationLevel);
    output[10].length = &educationLevelLength;

    output[11].buffer_type = MYSQL_TYPE_FLOAT;
    output[11].buffer = &GPA;

    if (mysql_stmt_bind_result(stmt, output) != 0) {
        cerr << "Loi bind result: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return false;
    }
    int fetchStatus = mysql_stmt_fetch(stmt);
    if (fetchStatus != 0) {
        if (fetchStatus != MYSQL_NO_DATA) {
            cerr << "Loi doc Candidate: " << mysql_stmt_error(stmt) << endl;
        }
        mysql_stmt_close(stmt);
        return false;
    }

    candidate.IDuser = userID;
    candidate.setIDcandidate(candidateID);
    candidate.setEmail(email);
    candidate.setPassword(password);

    candidate.personal.fullName = fullName;
    candidate.personal.birthday = birthday;
    candidate.personal.gender = gender;
    candidate.personal.phoneCandidate = phone;
    candidate.personal.addressCandidate = address;

    candidate.professional.major = major;
    candidate.professional.school = school;
    candidate.professional.educationLevel = educationLevel;
    candidate.professional.GPA = GPA;
    mysql_stmt_close(stmt);
    return true;
}