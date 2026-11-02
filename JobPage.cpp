#include "JobPage.h"
#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>
#include <QLocale>
#include <QString>

JobPage::JobPage(QWidget *parent)
    : QWidget(parent) {
    setupUI();

}

void JobPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Top Action Bar
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    backBtn = new QPushButton("← Quay lại danh sách", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 8px; padding: 7px 15px; font-weight: 500;");

    applyBtn = new QPushButton("★ Nộp hồ sơ ứng tuyển", this);
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 8px; padding: 8px 20px; font-weight: 600; font-size: 14px;");

    topBarLayout->addWidget(backBtn);
    topBarLayout->addStretch();
    topBarLayout->addWidget(applyBtn);
    mainLayout->addLayout(topBarLayout);

    // Scroll Area for details
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *contentWidget = new QWidget(scrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(16);

    // Header Card
    QFrame *headerCard = new QFrame(contentWidget);
    headerCard->setStyleSheet("background: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 20px;");
    QVBoxLayout *headerCardLayout = new QVBoxLayout(headerCard);
    headerCardLayout->setSpacing(8);

    jobTitleLabel = new QLabel("Lập trình viên C++ / Qt GUI", headerCard);
    jobTitleLabel->setStyleSheet("font-size: 22px; font-weight: 700; color: #1E1E1E;");

    companyNameLabel = new QLabel("Công ty Cổ phần Công nghệ ABC", headerCard);
    companyNameLabel->setStyleSheet("font-size: 15px; color: #6C757D; font-weight: 500;");

    headerCardLayout->addWidget(jobTitleLabel);
    headerCardLayout->addWidget(companyNameLabel);

    // Quick Stats Bar
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(12);

    salaryLabel = new QLabel("💰 Lương: 18,000,000 VNĐ", headerCard);
    salaryLabel->setStyleSheet("background: #FCE8E2; color: #B95334; border: 1px solid #F5D3C7; padding: 6px 12px; border-radius: 6px; font-weight: 600;");

    locationLabel = new QLabel("📍 Đà Nẵng", headerCard);
    locationLabel->setStyleSheet("background: #F5EBE1; color: #754533; border: 1px solid #ECE5DE; padding: 6px 12px; border-radius: 6px; font-weight: 500;");

    deadlineLabel = new QLabel("⏰ Hạn nộp: 2026-11-30", headerCard);
    deadlineLabel->setStyleSheet("background: #FEF3E2; color: #B76E00; border: 1px solid #FDE68A; padding: 6px 12px; border-radius: 6px; font-weight: 500;");

    statusLabel = new QLabel("● Đang tuyển", headerCard);
    statusLabel->setStyleSheet("background: #E3F7EB; color: #1B7F49; border: 1px solid #C4EDD4; padding: 6px 12px; border-radius: 6px; font-weight: 600;");

    scoreLabel = new QLabel("🎯 Phù hợp: 92%", headerCard);
    scoreLabel->setStyleSheet("background: #E3F7EB; color: #1B7F49; border: 1px solid #C4EDD4; padding: 6px 12px; border-radius: 6px; font-weight: 700;");

    statsLayout->addWidget(salaryLabel);
    statsLayout->addWidget(locationLabel);
    statsLayout->addWidget(deadlineLabel);
    statsLayout->addWidget(statusLabel);
    statsLayout->addWidget(scoreLabel);
    statsLayout->addStretch();

    headerCardLayout->addLayout(statsLayout);
    contentLayout->addWidget(headerCard);

    // Requirements & Description Grid
    QHBoxLayout *gridColsLayout = new QHBoxLayout();
    gridColsLayout->setSpacing(16);

    // Left Column: Description & Company
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(16);

    QFrame *descCard = new QFrame(contentWidget);
    descCard->setStyleSheet("background: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 18px;");
    QVBoxLayout *descCardLayout = new QVBoxLayout(descCard);
    descCardLayout->setSpacing(8);

    QLabel *descTitle = new QLabel("Mô tả công việc", descCard);
    descTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");
    descLabel = new QLabel(descCard);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #1E1E1E; line-height: 1.5; font-size: 13px;");
    descCardLayout->addWidget(descTitle);
    descCardLayout->addWidget(descLabel);
    leftCol->addWidget(descCard);

    QFrame *companyCard = new QFrame(contentWidget);
    companyCard->setStyleSheet("background: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 18px;");
    QVBoxLayout *companyCardLayout = new QVBoxLayout(companyCard);
    companyCardLayout->setSpacing(8);

    QLabel *compTitle = new QLabel("Thông tin doanh nghiệp", companyCard);
    compTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");
    companyInfoLabel = new QLabel(companyCard);
    companyInfoLabel->setWordWrap(true);
    companyInfoLabel->setStyleSheet("color: #6C757D; font-size: 13px;");
    companyCardLayout->addWidget(compTitle);
    companyCardLayout->addWidget(companyInfoLabel);
    leftCol->addWidget(companyCard);

    gridColsLayout->addLayout(leftCol, 3);

    // Right Column: Requirements, Skills, Certificates
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(16);

    QFrame *reqCard = new QFrame(contentWidget);
    reqCard->setStyleSheet("background: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 12px; padding: 18px;");
    QVBoxLayout *reqCardLayout = new QVBoxLayout(reqCard);
    reqCardLayout->setSpacing(10);

    QLabel *reqTitle = new QLabel("Yêu cầu ứng viên", reqCard);
    reqTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #252525;");
    reqCardLayout->addWidget(reqTitle);

    QFormLayout *reqForm = new QFormLayout();
    reqForm->setSpacing(8);

    majorLabel = new QLabel("Công nghệ thông tin", reqCard);
    gpaLabel = new QLabel("3.20 / 4.0", reqCard);
    expLabel = new QLabel("12 tháng kinh nghiệm", reqCard);

    reqForm->addRow("Chuyên ngành:", majorLabel);
    reqForm->addRow("GPA tối thiểu:", gpaLabel);
    reqForm->addRow("Kinh nghiệm:", expLabel);
    reqCardLayout->addLayout(reqForm);

    QLabel *skillsHeader = new QLabel("Kỹ năng yêu cầu:", reqCard);
    skillsHeader->setStyleSheet("font-weight: 600; color: #1e293b; margin-top: 6px;");
    reqCardLayout->addWidget(skillsHeader);

    skillsTable = new QTableWidget(reqCard);
    skillsTable->setColumnCount(1);
    skillsTable->setHorizontalHeaderLabels(QStringList() << "Yêu cầu kỹ năng");
    skillsTable->horizontalHeader()->setStretchLastSection(true);
    skillsTable->setMaximumHeight(120);
    reqCardLayout->addWidget(skillsTable);

    QLabel *certsHeader = new QLabel("Chứng chỉ bắt buộc & ưu tiên:", reqCard);
    certsHeader->setStyleSheet("font-weight: 600; color: #1e293b; margin-top: 6px;");
    reqCardLayout->addWidget(certsHeader);

    certsTable = new QTableWidget(reqCard);
    certsTable->setColumnCount(1);
    certsTable->setHorizontalHeaderLabels(QStringList() << "Yêu cầu chứng chỉ");
    certsTable->horizontalHeader()->setStretchLastSection(true);
    certsTable->setMaximumHeight(100);
    reqCardLayout->addWidget(certsTable);

    rightCol->addWidget(reqCard);
    gridColsLayout->addLayout(rightCol, 2);

    contentLayout->addLayout(gridColsLayout);
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);

    // Connections
    connect(backBtn, &QPushButton::clicked, this, &JobPage::onBackClicked);
    connect(applyBtn, &QPushButton::clicked, this, &JobPage::onApplyClicked);
}

void JobPage::loadJob(const MockJobDetail &job) {
    currentJob = job;

    jobTitleLabel->setText(job.title);
    companyNameLabel->setText(job.company);
    salaryLabel->setText(QString("💰 Lương: %1 VNĐ").arg(QLocale(QLocale::Vietnamese).toString(job.salary)));
    locationLabel->setText(QString("📍 %1").arg(job.location));
    deadlineLabel->setText(QString("⏰ Hạn: %1").arg(job.deadline));

    statusLabel->setText(job.isOpen ? "● Đang tuyển" : "● Đã đóng");

    if (job.isOpen) {
        statusLabel->setStyleSheet(
            "background: #E8F5E9; color: #2E7D32;"
            "border: 1px solid #C8E6C9; padding: 6px 12px;"
            "border-radius: 6px; font-weight: 600;"
            );
    } else {
        statusLabel->setStyleSheet(
            "background: #FEE2E2; color: #B91C1C;"
            "border: 1px solid #FECACA; padding: 6px 12px;"
            "border-radius: 6px; font-weight: 600;"
            );
    }

    scoreLabel->setVisible(job.hasMatchScore);
    if (job.hasMatchScore) {
        scoreLabel->setText(QString("🎯 Phù hợp: %1%").arg(job.matchScore));
    }

    applyBtn->setEnabled(job.isOpen);
    applyBtn->setText(
        job.isOpen ? "★ Nộp hồ sơ ứng tuyển" : "Công việc đã đóng"
        );

    descLabel->setText(job.description);
    companyInfoLabel->setText(
        QString("• Địa chỉ: %1\n• Website: %2\n• Đánh giá uy tín: 4.8 / 5.0")
        .arg(job.companyAddress, job.companyWebsite)
    );

    majorLabel->setText(job.major);
    gpaLabel->setText(QString::number(job.minGpa, 'f', 2));
    expLabel->setText(QString("%1 tháng").arg(job.minExp));

    // Skills
    skillsTable->setRowCount(job.skills.size());
    for (int i = 0; i < job.skills.size(); ++i) {
        skillsTable->setItem(i, 0, new QTableWidgetItem("✓ " + job.skills[i]));
    }

    // Certs
    certsTable->setRowCount(job.certs.size());
    for (int i = 0; i < job.certs.size(); ++i) {
        certsTable->setItem(i, 0, new QTableWidgetItem("✓ " + job.certs[i]));
    }
}

void JobPage::loadJob(const Job &job, const Company &company, double matchScore) {
    MockJobDetail detail;

    detail.id = job.getIDjob();
    detail.title = QString::fromStdString(job.getTitle());
    detail.company = QString::fromStdString(company.getNameCompany());
    detail.location = QString::fromStdString(job.getLocation());
    detail.salary = job.getSalary();
    detail.deadline = QString::fromStdString(job.getDeadline());
    detail.minGpa = job.getMinGPA();
    detail.minExp = job.getMinExperience();
    detail.major = QString::fromStdString(job.getNameMajor());
    detail.description = QString::fromStdString(job.getDescription());

    detail.companyWebsite =
        QString::fromStdString(company.getWebsite());

    detail.companyAddress =
        QString::fromStdString(company.getAddressCompany());

    detail.isOpen = job.getStatus();
    if (matchScore >= 0.0) {
        detail.hasMatchScore = true;
        detail.matchScore = static_cast<int>(std::round(matchScore));
    } else {
        detail.hasMatchScore = false;
    }

    for (const auto &skill : job.getRequiredSkill()) {
        detail.skills.append(QString::fromStdString(skill.name));
    }

    for (const auto &group : job.getCertificateGroups()) {
        QStringList alternatives;

        for (const auto &certificate : group.alternatives) {
            alternatives.append(
                QString("Loại chứng chỉ #%1 (mức tối thiểu: %2)")
                    .arg(certificate.certificateTypeID)
                    .arg(certificate.minValue)
                );
        }

        if (!alternatives.isEmpty()) {
            QString groupText = alternatives.join(" / ");

            if (group.isRequired) {
                groupText.prepend("[Bắt buộc] ");
            } else {
                groupText.prepend("[Tùy chọn] ");
            }

            detail.certs.append(groupText);
        }
    }

    loadJob(detail);
}

void JobPage::onBackClicked() {
    emit backRequested();
}

void JobPage::onApplyClicked() {
    emit applyRequested(currentJob.id);
}

void JobPage::setApplyState(bool hasApplied, const QString &statusText) {
    if (hasApplied) {
        applyBtn->setText("✓ " + (statusText.isEmpty() ? "Đã nộp đơn" : statusText));
        applyBtn->setEnabled(false);
        applyBtn->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; border: 1px solid #C4EDD4; border-radius: 8px; padding: 8px 20px; font-weight: 600; font-size: 14px;");
    } else {
        applyBtn->setText("★ Nộp hồ sơ ứng tuyển");
        applyBtn->setEnabled(true);
        applyBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 8px; padding: 8px 20px; font-weight: 600; font-size: 14px;");
    }
}
