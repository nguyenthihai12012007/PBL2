DROP DATABASE IF EXISTS JobSystem;

CREATE DATABASE IF NOT EXISTS JobSystem;

USE JobSystem;

CREATE TABLE IF NOT EXISTS User (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    email VARCHAR(255) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    role ENUM('CANDIDATE', 'COMPANY') NOT NULL
);

CREATE TABLE IF NOT EXISTS Candidate (
    candidate_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL UNIQUE,

    full_name VARCHAR(100) NOT NULL,
    birthday DATE,
    gender VARCHAR(20),
    phone VARCHAR(20),
    address VARCHAR(255),

    major VARCHAR(100),
    school VARCHAR(150),
    education_level VARCHAR(50),
    GPA DECIMAL(3,2),

    FOREIGN KEY (user_id)
        REFERENCES User(user_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Company (
    company_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL UNIQUE,

    name_company VARCHAR(150) NOT NULL,
    phone VARCHAR(20),
    address VARCHAR(255),
    description TEXT,
    website VARCHAR(255),

    FOREIGN KEY (user_id)
        REFERENCES User(user_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Experience (
    experience_id INT AUTO_INCREMENT PRIMARY KEY,
    candidate_id INT NOT NULL,

    company_name VARCHAR(150),
    position VARCHAR(100),
    description TEXT,
    start_date DATE,
    end_date DATE,

    FOREIGN KEY (candidate_id)
        REFERENCES Candidate(candidate_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Skill (
    skill_id INT AUTO_INCREMENT PRIMARY KEY,
    skill_name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS Candidate_Skill (
    candidate_id INT NOT NULL,
    skill_id INT NOT NULL,
    skill_level INT NOT NULL,

    PRIMARY KEY (candidate_id, skill_id),

    FOREIGN KEY (candidate_id)
        REFERENCES Candidate(candidate_id)
        ON DELETE CASCADE,

    FOREIGN KEY (skill_id)
        REFERENCES Skill(skill_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Certificate (
    certificate_id INT AUTO_INCREMENT PRIMARY KEY,
    candidate_id INT NOT NULL,

    name VARCHAR(150) NOT NULL,
    organization VARCHAR(150),

    FOREIGN KEY (candidate_id)
        REFERENCES Candidate(candidate_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Job (
    job_id INT AUTO_INCREMENT PRIMARY KEY,
    company_id INT NOT NULL,

    title VARCHAR(150) NOT NULL,
    description TEXT,
    location VARCHAR(255),

    min_GPA DECIMAL(3,2),
    min_experience INT,

    deadline DATE,
    salary INT,

    status BOOLEAN DEFAULT TRUE,

    FOREIGN KEY (company_id)
        REFERENCES Company(company_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Job_Skill (
    job_id INT NOT NULL,
    skill_id INT NOT NULL,

    required_level INT NOT NULL,
    weight DECIMAL(5,2) DEFAULT 1.0,
    is_required BOOLEAN DEFAULT FALSE,

    PRIMARY KEY (job_id, skill_id),

    FOREIGN KEY (job_id)
        REFERENCES Job(job_id)
        ON DELETE CASCADE,

    FOREIGN KEY (skill_id)
        REFERENCES Skill(skill_id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Application (
    application_id INT AUTO_INCREMENT PRIMARY KEY,

    candidate_id INT NOT NULL,
    job_id INT NOT NULL,

    appropriate_level DECIMAL(5,2),

    application_date DATETIME DEFAULT CURRENT_TIMESTAMP,

    status ENUM(
        'PENDING',
        'ACCEPTED',
        'REJECTED'
    ) DEFAULT 'PENDING',

    UNIQUE(candidate_id, job_id),

    FOREIGN KEY (candidate_id)
        REFERENCES Candidate(candidate_id)
        ON DELETE CASCADE,

    FOREIGN KEY (job_id)
        REFERENCES Job(job_id)
        ON DELETE CASCADE
);

INSERT INTO User (email, password, role)
VALUES
('candidate1@gmail.com', '123456', 'CANDIDATE'),
('candidate2@gmail.com', '123456', 'CANDIDATE'),
('company1@gmail.com', '123456', 'COMPANY'),
('company2@gmail.com', '123456', 'COMPANY');

INSERT INTO Candidate
(user_id, full_name, birthday, gender, phone, address, major, school, education_level, GPA)
VALUES
(1, 'Nguyen Van A', '2007-05-10', 'Nam', '0901234567',
 'Da Nang', 'Cong nghe thong tin', 'Dai hoc Bach khoa Da Nang',
 'Dai hoc', 3.25),

(2, 'Tran Thi B', '2007-08-20', 'Nu', '0912345678',
 'Da Nang', 'Ky thuat may tinh', 'Dai hoc Bach khoa Da Nang',
 'Dai hoc', 3.60);

INSERT INTO Company
(user_id, name_company, phone, address, description, website)
VALUES
(3, 'ABC Technology',
 '0905555555',
 'Da Nang',
 'Cong ty phat trien phan mem',
 'https://abc.com'),

(4, 'XYZ Software',
 '0916666666',
 'Da Nang',
 'Cong ty phat trien ung dung',
 'https://xyz.com');

INSERT INTO Experience 
(candidate_id, company_name, position, description, start_date, end_date) 
VALUES 
(1, 'ABC Technology', 
 'Software Developer', 
 'Tham gia phát triển ứng dụng bằng C++ và MySQL', 
 '2025-06-01', 
 '2025-08-31'), 

(1, 'XYZ Software', 
 'Intern Developer', 
 'Hỗ trợ phát triển và kiểm thử phần mềm', 
 '2025-09-01', 
 '2026-01-31'), 
 
(2, 'ABC Technology', 
 'Java Developer Intern', 
 'Tham gia phát triển ứng dụng Java', 
 '2025-06-01', 
 '2025-08-31');

INSERT INTO Skill (skill_name)
VALUES
('C++'),
('Java'),
('SQL'),
('MySQL'),
('HTML'),
('CSS'),
('JavaScript');

SELECT * FROM User;

SELECT * FROM Candidate;

SELECT * FROM Company;

SELECT * FROM Experience;

SELECT * FROM Skill;

DESCRIBE User;

DESCRIBE Candidate;

SHOW DATABASES;