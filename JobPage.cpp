#include "JobPage.h"
#include "StyleHelper.h"
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

namespace {
QString formatCertificateRequirementName(int typeId, double minValue) {
    switch (typeId) {
        case 1: {
            QString v = QString::number(minValue, 'f', 1);
            return QString("IELTS >= %1").arg(v);
        }
        case 2: {
            int score = static_cast<int>(std::round(minValue));
            return QString("TOEIC >= %1").arg(score);
        }
        case 3: {
            QString v = (minValue == static_cast<int>(minValue)) ? QString::number(static_cast<int>(minValue)) : QString::number(minValue, 'f', 1);
            return QString("TOEFL / HSK >= %1").arg(v);
        }
        case 4: {
            int n = static_cast<int>(std::round(minValue));
            if (n >= 1 && n <= 5) return QString("JLPT N%1 trở lên").arg(n);
            return QString("JLPT >= %1").arg(minValue);
        }
        case 5: {
            int lvl = static_cast<int>(std::round(minValue));
            return QString("Chứng chỉ Cloud / AWS (Level %1)").arg(lvl > 0 ? lvl : 1);
        }
        default: {
            QString v = (minValue == static_cast<int>(minValue)) ? QString::number(static_cast<int>(minValue)) : QString::number(minValue, 'f', 1);
            return QString("Chứng chỉ chuyên ngành (>= %1)").arg(v);
        }
    }
}
}

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
    headerCardLayout->setSpacing(12);

    QHBoxLayout *brandHeaderRow = new QHBoxLayout();
    brandHeaderRow->setSpacing(14);
    brandHeaderRow->setAlignment(Qt::AlignVCenter);

    companyLogoLabel = new QLabel(headerCard);
    companyLogoLabel->setFixedSize(54, 54);
    companyLogoLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *titleTextCol = new QVBoxLayout();
    titleTextCol->setSpacing(3);
    titleTextCol->setAlignment(Qt::AlignVCenter);
    jobTitleLabel = new QLabel("Lập trình viên C++ / Qt GUI", headerCard);
    jobTitleLabel->setStyleSheet("font-size: 22px; font-weight: 700; color: #1E1E1E;");

    companyNameLabel = new QLabel("Công ty Cổ phần Công nghệ ABC", headerCard);
    companyNameLabel->setStyleSheet("font-size: 15px; color: #6C757D; font-weight: 500;");

    titleTextCol->addWidget(jobTitleLabel);
    titleTextCol->addWidget(companyNameLabel);

    brandHeaderRow->addWidget(companyLogoLabel, 0, Qt::AlignVCenter);
    brandHeaderRow->addLayout(titleTextCol, 1);

    headerCardLayout->addLayout(brandHeaderRow);

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

    QLabel *skillsHeader = new QLabel("⚡ Kỹ năng yêu cầu:", reqCard);
    skillsHeader->setStyleSheet("font-weight: 700; font-size: 13px; color: #1E1E1E; margin-top: 8px;");
    reqCardLayout->addWidget(skillsHeader);

    skillsContainer = new QWidget(reqCard);
    skillsContainer->setStyleSheet("background: transparent;");
    skillsLayout = new QVBoxLayout(skillsContainer);
    skillsLayout->setContentsMargins(0, 2, 0, 4);
    skillsLayout->setSpacing(6);
    reqCardLayout->addWidget(skillsContainer);

    QLabel *certsHeader = new QLabel("📜 Chứng chỉ bắt buộc & ưu tiên:", reqCard);
    certsHeader->setStyleSheet("font-weight: 700; font-size: 13px; color: #1E1E1E; margin-top: 8px;");
    reqCardLayout->addWidget(certsHeader);

    certsContainer = new QWidget(reqCard);
    certsContainer->setStyleSheet("background: transparent;");
    certsLayout = new QVBoxLayout(certsContainer);
    certsLayout->setContentsMargins(0, 2, 0, 4);
    certsLayout->setSpacing(6);
    reqCardLayout->addWidget(certsContainer);

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

    companyLogoLabel->setPixmap(StyleHelper::getRoundedBadge("", StyleHelper::getInitials(job.company, "VT"), 54, 12));
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
    QString compDesc = job.companyDesc.isEmpty() ? "Doanh nghiệp công nghệ phát triển giải pháp phần mềm." : job.companyDesc;
    companyInfoLabel->setText(
        QString("• Giới thiệu: %1\n• Địa chỉ: %2\n• Website: %3\n• Đánh giá uy tín: 4.8 / 5.0")
        .arg(compDesc, job.companyAddress, job.companyWebsite)
    );

    majorLabel->setText(job.major);
    gpaLabel->setText(QString::number(job.minGpa, 'f', 2));
    expLabel->setText(QString("%1 tháng").arg(job.minExp));

    // Clear old items
    auto clearLayout = [](QLayout *l, auto &self) -> void {
        if (!l) return;
        QLayoutItem *item;
        while ((item = l->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            else if (item->layout()) self(item->layout(), self);
            delete item;
        }
    };
    clearLayout(skillsLayout, clearLayout);
    clearLayout(certsLayout, clearLayout);

    // Skills: Hiển thị dạng badge tag gọn gàng, không dùng bảng ô vuông chiếm diện tích
    if (job.skills.isEmpty()) {
        QLabel *emptySkill = new QLabel("Không yêu cầu kỹ năng bắt buộc", skillsContainer);
        emptySkill->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        skillsLayout->addWidget(emptySkill);
    } else {
        QHBoxLayout *tagsRow = new QHBoxLayout();
        tagsRow->setSpacing(6);
        for (const QString &skill : job.skills) {
            QLabel *tag = new QLabel(skill, skillsContainer);
            tag->setStyleSheet("background-color: #F5EBE1; color: #754533; font-size: 11px; font-weight: 600; border-radius: 10px; padding: 4px 10px;");
            tagsRow->addWidget(tag);
        }
        tagsRow->addStretch();
        skillsLayout->addLayout(tagsRow);
    }

    // Certs: Hiển thị phân loại rõ ràng Bắt buộc & Ưu tiên với tên chứng chỉ rõ ràng
    if (job.requiredCerts.isEmpty() && job.preferredCerts.isEmpty() && job.certs.isEmpty()) {
        QLabel *emptyCert = new QLabel("Không yêu cầu chứng chỉ ngoại ngữ / chuyên môn", certsContainer);
        emptyCert->setStyleSheet("color: #6C757D; font-size: 12px; font-style: italic;");
        certsLayout->addWidget(emptyCert);
    } else {
        // 1. Nhóm chứng chỉ BẮT BUỘC (Điều kiện tiên quyết)
        if (!job.requiredCerts.isEmpty()) {
            QHBoxLayout *badgeRow = new QHBoxLayout();
            badgeRow->setSpacing(6);
            QLabel *badge = new QLabel("BẮT BUỘC", certsContainer);
            badge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-size: 10px; font-weight: 800; border-radius: 4px; padding: 2px 6px;");
            QLabel *desc = new QLabel("Điều kiện tiên quyết:", certsContainer);
            desc->setStyleSheet("color: #DC2626; font-size: 11px; font-weight: 600;");
            badgeRow->addWidget(badge);
            badgeRow->addWidget(desc);
            badgeRow->addStretch();
            certsLayout->addLayout(badgeRow);

            for (const QString &c : job.requiredCerts) {
                QLabel *certLbl = new QLabel("• " + c, certsContainer);
                certLbl->setWordWrap(true);
                certLbl->setStyleSheet("color: #1E1E1E; font-size: 12px; font-weight: 600; padding-left: 6px;");
                certsLayout->addWidget(certLbl);
            }
        }

        // 2. Nhóm chứng chỉ ƯU TIÊN (Cộng điểm đánh giá)
        if (!job.preferredCerts.isEmpty()) {
            if (!job.requiredCerts.isEmpty()) {
                certsLayout->addSpacing(6);
            }
            QHBoxLayout *badgeRow = new QHBoxLayout();
            badgeRow->setSpacing(6);
            QLabel *badge = new QLabel("ƯU TIÊN", certsContainer);
            badge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-size: 10px; font-weight: 800; border-radius: 4px; padding: 2px 6px;");
            QLabel *desc = new QLabel("Cộng điểm đánh giá hồ sơ:", certsContainer);
            desc->setStyleSheet("color: #1B7F49; font-size: 11px; font-weight: 600;");
            badgeRow->addWidget(badge);
            badgeRow->addWidget(desc);
            badgeRow->addStretch();
            certsLayout->addLayout(badgeRow);

            for (const QString &c : job.preferredCerts) {
                QLabel *certLbl = new QLabel("• " + c, certsContainer);
                certLbl->setWordWrap(true);
                certLbl->setStyleSheet("color: #333333; font-size: 12px; font-weight: 500; padding-left: 6px;");
                certsLayout->addWidget(certLbl);
            }
        }

        // 3. Fallback cho callers chỉ truyền job.certs
        if (job.requiredCerts.isEmpty() && job.preferredCerts.isEmpty()) {
            for (const QString &cert : job.certs) {
                QLabel *certLbl = new QLabel("• " + cert, certsContainer);
                certLbl->setWordWrap(true);
                certLbl->setStyleSheet("color: #333333; font-size: 12px; font-weight: 500;");
                certsLayout->addWidget(certLbl);
            }
        }
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

    detail.companyDesc =
        QString::fromStdString(company.getDescription());

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

    detail.requiredCerts.clear();
    detail.preferredCerts.clear();
    detail.certs.clear();

    for (const auto &group : job.getCertificateGroups()) {
        QStringList alternatives;

        for (const auto &certificate : group.alternatives) {
            alternatives.append(formatCertificateRequirementName(
                certificate.certificateTypeID, certificate.minValue
            ));
        }

        if (!alternatives.isEmpty()) {
            QString groupText = alternatives.join("  hoặc  ");

            if (group.isRequired) {
                detail.requiredCerts.append(groupText);
                detail.certs.append("[Bắt buộc] " + groupText);
            } else {
                detail.preferredCerts.append(groupText);
                detail.certs.append("[Ưu tiên] " + groupText);
            }
        }
    }

    loadJob(detail);
    QString logoPath = StyleHelper::getCompanyLogoPath(company.getIDcompany());
    QString compInitials = StyleHelper::getInitials(QString::fromStdString(company.getNameCompany()), "VT");
    companyLogoLabel->setPixmap(StyleHelper::getRoundedBadge(logoPath, compInitials, 54, 12));
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
