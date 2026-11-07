#ifndef JOB_PAGE_H
#define JOB_PAGE_H

#include "class.h"
#include <QWidget>
#include <QStringList>

class QLabel;
class QPushButton;
class QVBoxLayout;

struct MockJobDetail {
    int id = 1;
    bool isOpen = true;
    bool hasMatchScore = false;
    QString title = "Lập trình viên C++ / Qt GUI";
    QString company = "Công ty Cổ phần Công nghệ ABC";
    QString location = "Đà Nẵng";
    int salary = 18000000;
    QString deadline = "2026-11-30";
    float minGpa = 3.2f;
    int minExp = 12; // months
    QString major = "Công nghệ thông tin";
    QString description = "Tham gia thiết kế và phát triển các ứng dụng Desktop hiện đại bằng C++17 và Qt Widgets / QML. Xây dựng module nghiệp vụ, tối ưu hóa hiệu năng và kết nối cơ sở dữ liệu.";
    QString companyWebsite = "https://abctechnology.vn";
    QString companyAddress = "Khu Công viên Phần mềm Đà Nẵng, Hải Châu, Đà Nẵng";
    QString companyDesc = "Công ty phát triển phần mềm và giải pháp ứng dụng web hiện đại";
    QStringList skills;
    QStringList certs;
    QStringList requiredCerts;  // Nhóm chứng chỉ bắt buộc (tiên quyết)
    QStringList preferredCerts; // Nhóm chứng chỉ ưu tiên (điểm cộng)
    int matchScore = 92;
};

class JobPage : public QWidget {
    Q_OBJECT

public:
    explicit JobPage(QWidget *parent = nullptr);
    ~JobPage() override = default;

    void loadJob(const MockJobDetail &job);
    void loadJob(const Job &job, const Company &company, double matchScore = -1.0);
    void setApplyState(bool hasApplied, const QString &statusText = "");

signals:
    void backRequested();
    void applyRequested(int jobId);

private slots:
    void onBackClicked();
    void onApplyClicked();

private:
    void setupUI();

    MockJobDetail currentJob;

    QPushButton *backBtn;
    QPushButton *applyBtn;

    QLabel *companyLogoLabel;
    QLabel *jobTitleLabel;
    QLabel *companyNameLabel;
    QLabel *salaryLabel;
    QLabel *locationLabel;
    QLabel *deadlineLabel;
    QLabel *statusLabel;
    QLabel *scoreLabel;

    QLabel *majorLabel;
    QLabel *gpaLabel;
    QLabel *expLabel;
    QLabel *descLabel;
    QLabel *companyInfoLabel;

    QWidget *skillsContainer;
    QVBoxLayout *skillsLayout;
    QWidget *certsContainer;
    QVBoxLayout *certsLayout;
};

#endif // JOB_PAGE_H
