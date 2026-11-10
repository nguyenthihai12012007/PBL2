#include "CandidatePage.h"
#include "Matching.h"
#include "CustomMessageBox.h"
#include "StyleHelper.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QProgressBar>
#include <QInputDialog>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QListView>
#include <QCheckBox>
#include <QFormLayout>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace {
void safeClearLayout(QLayout *layout) {
    if (!layout) return;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        } else if (item->layout()) {
            safeClearLayout(item->layout());
        }
        delete item;
    }
}
}

CandidatePage::CandidatePage(Data &data, JobSearchEngine &searchEngine, QWidget *parent)
    : QWidget(parent), m_data(data), m_searchEngine(searchEngine) {
    setupUI();
}

void CandidatePage::setupUI() {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background-color: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(28, 20, 28, 28);
    contentLayout->setSpacing(20);

    // =========================================================================
    // THANH TÌM KIẾM & BỘ LỌC TOÀN DIỆN (FULL-WIDTH ACTION BAR)
    // Duy nhất một thanh tìm kiếm trên trang chủ ứng viên, bố trí rộng rãi & hợp lý
    // =========================================================================
    filterBarCard = new QFrame(scrollContent);
    filterBarCard->setObjectName("candidateFilterCard");
    filterBarCard->setStyleSheet(
        "#candidateFilterCard {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 12px;"
        "}"
    );
    QHBoxLayout *filterLayout = new QHBoxLayout(filterBarCard);
    filterLayout->setSpacing(12);
    filterLayout->setContentsMargins(12, 10, 12, 10);

    inpageSearchEdit = new QLineEdit(filterBarCard);
    inpageSearchEdit->setPlaceholderText("🔍  Tìm kiếm việc làm theo vị trí, kỹ năng, công ty...");
    inpageSearchEdit->setStyleSheet(
        "QLineEdit {"
        "   background-color: #FBF8F5;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 8px;"
        "   padding: 8px 14px;"
        "   font-size: 13px;"
        "   color: #1E1E1E;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #B95334;"
        "   background-color: #FFFFFF;"
        "}"
    );

    const QString comboStyle =
        "QComboBox {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 8px;"
        "   padding: 7px 10px;"
        "   font-size: 12px;"
        "   color: #1E1E1E;"
        "}"
        "QComboBox:hover {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #B95334;"
        "}"
        "QComboBox:on, QComboBox:pressed, QComboBox:focus {"
        "   background-color: #FFFFFF;"
        "   color: #1E1E1E;"
        "   border: 1.5px solid #B95334;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   background: transparent;"
        "   width: 18px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #FFFFFF;"
        "   color: #1E1E1E;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 8px;"
        "   padding: 4px;"
        "   outline: none;"
        "   selection-background-color: #FFF4ED;"
        "   selection-color: #B95334;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "   color: #1E1E1E;"
        "   background-color: #FFFFFF;"
        "   padding: 6px 10px;"
        "   min-height: 24px;"
        "   border-radius: 4px;"
        "}"
        "QComboBox QAbstractItemView::item:hover,"
        "QComboBox QAbstractItemView::item:selected {"
        "   background-color: #FFF4ED;"
        "   color: #B95334;"
        "}";

    inpageLocCombo = new QComboBox(filterBarCard);
    inpageLocCombo->setView(new QListView(inpageLocCombo));
    inpageLocCombo->addItem("📍 Tất cả địa điểm", "");
    inpageLocCombo->addItem("📍 Đà Nẵng", "Đà Nẵng");
    inpageLocCombo->addItem("📍 Hà Nội", "Hà Nội");
    inpageLocCombo->addItem("📍 TP. Hồ Chí Minh", "Hồ Chí Minh");
    inpageLocCombo->setStyleSheet(comboStyle);

    inpageMajorCombo = new QComboBox(filterBarCard);
    inpageMajorCombo->setView(new QListView(inpageMajorCombo));
    inpageMajorCombo->addItem("🎓 Tất cả chuyên ngành", "");
    inpageMajorCombo->addItem("🎓 Công nghệ thông tin", "Công nghệ thông tin");
    inpageMajorCombo->addItem("🎓 Kỹ thuật phần mềm", "Kỹ thuật phần mềm");
    inpageMajorCombo->addItem("🎓 Khoa học máy tính", "Khoa học máy tính");
    inpageMajorCombo->addItem("🎓 Hệ thống thông tin", "Hệ thống thông tin");
    inpageMajorCombo->addItem("🎓 Trí tuệ nhân tạo", "Trí tuệ nhân tạo");
    inpageMajorCombo->addItem("🎓 Mạng máy tính", "Mạng máy tính");
    inpageMajorCombo->addItem("🎓 An toàn thông tin", "An toàn thông tin");
    inpageMajorCombo->setStyleSheet(comboStyle);

    clearFilterBtn = new QPushButton("✕ Đặt lại", filterBarCard);
    clearFilterBtn->setCursor(Qt::PointingHandCursor);
    clearFilterBtn->setToolTip("Xóa bộ lọc và hiển thị lại toàn bộ việc làm");
    clearFilterBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #F8F9FA;"
        "   color: #6C757D;"
        "   border: 1px solid #D1D5DB;"
        "   border-radius: 8px;"
        "   padding: 7px 14px;"
        "   font-size: 12px;"
        "   font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FEE2E2;"
        "   color: #DC2626;"
        "   border-color: #FCA5A5;"
        "}"
    );

    filterLayout->addWidget(inpageSearchEdit, 1);
    filterLayout->addWidget(inpageLocCombo, 0);
    filterLayout->addWidget(inpageMajorCombo, 0);
    filterLayout->addWidget(clearFilterBtn, 0);
    contentLayout->addWidget(filterBarCard);

    connect(inpageSearchEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_filterKeyword = text;
        buildJobList();
    });
    connect(inpageLocCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        buildJobList();
    });
    connect(inpageMajorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        buildJobList();
    });
    connect(clearFilterBtn, &QPushButton::clicked, this, [this]() {
        inpageSearchEdit->clear();
        inpageLocCombo->setCurrentIndex(0);
        inpageMajorCombo->setCurrentIndex(0);
        m_filterKeyword.clear();
        buildJobList();
    });

    // Main 2-Column Dashboard Layout
    QHBoxLayout *dashboardLayout = new QHBoxLayout();
    dashboardLayout->setSpacing(24);
    dashboardLayout->setAlignment(Qt::AlignTop);

    // =========================================================================
    // LEFT COLUMN (~65%): Danh sách việc làm phù hợp
    // =========================================================================
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(16);

    // Section Header
    QHBoxLayout *secHeaderLayout = new QHBoxLayout();
    secHeaderLayout->setSpacing(10);

    QLabel *iconBadge = new QLabel("💼", scrollContent);
    iconBadge->setStyleSheet("background-color: #FCE8E2; font-size: 14px; border-radius: 6px; padding: 6px;");

    QVBoxLayout *titleTextLayout = new QVBoxLayout();
    titleTextLayout->setSpacing(2);
    secTitle = new QLabel("Cơ hội việc làm đang tuyển dụng", scrollContent);
    secTitle->setStyleSheet("font-size: 17px; font-weight: 700; color: #1E1E1E;");
    secSub = new QLabel("Khám phá tất cả các vị trí việc làm còn hiệu lực trong hệ thống", scrollContent);
    secSub->setStyleSheet("font-size: 12px; color: #6C757D;");
    titleTextLayout->addWidget(secTitle);
    titleTextLayout->addWidget(secSub);

    viewAllBtn = new QPushButton("Xem tất cả →", scrollContent);
    viewAllBtn->setCursor(Qt::PointingHandCursor);
    viewAllBtn->setStyleSheet("color: #B95334; font-size: 13px; font-weight: 600; background: transparent; border: none;");
    viewAllBtn->setVisible(false); // Bỏ nút "Xem tất cả" theo Yêu cầu 4

    secHeaderLayout->addWidget(iconBadge);
    secHeaderLayout->addLayout(titleTextLayout);
    secHeaderLayout->addStretch();
    secHeaderLayout->addWidget(viewAllBtn);
    leftCol->addLayout(secHeaderLayout);

    // Jobs Container
    jobsListLayout = new QVBoxLayout();
    jobsListLayout->setSpacing(16);
    leftCol->addLayout(jobsListLayout);
    leftCol->addStretch();

    dashboardLayout->addLayout(leftCol, 64);

    // =========================================================================
    // RIGHT COLUMN (~36%): Hồ sơ cá nhân & Đơn ứng tuyển gần đây
    // =========================================================================
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(16);

    // Card 1: Hồ sơ của tôi
    QFrame *profileCard = new QFrame(scrollContent);
    profileCard->setObjectName("cardBox");
    profileCard->setStyleSheet(
        "#cardBox {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 10px;"
        "   padding: 14px;"
        "}"
    );
    profileContentLayout = new QVBoxLayout(profileCard);
    profileContentLayout->setSpacing(12);
    rightCol->addWidget(profileCard);

    // Card 2: Đơn ứng tuyển gần đây
    QFrame *recentAppsCard = new QFrame(scrollContent);
    recentAppsCard->setObjectName("cardBox");
    recentAppsCard->setStyleSheet(
        "#cardBox {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 10px;"
        "   padding: 14px;"
        "}"
    );
    recentAppsLayout = new QVBoxLayout(recentAppsCard);
    recentAppsLayout->setSpacing(12);
    rightCol->addWidget(recentAppsCard);

    rightCol->addStretch();
    dashboardLayout->addLayout(rightCol, 36);

    contentLayout->addLayout(dashboardLayout);
    scrollArea->setWidget(scrollContent);
    rootLayout->addWidget(scrollArea);

    connect(viewAllBtn, &QPushButton::clicked, this, &CandidatePage::showAllJobsDialog);
}

void CandidatePage::setSession(int userId) {
    m_userId = userId;

    // 1. Tải hồ sơ ứng viên từ cơ sở dữ liệu thật
    if (m_data.getCandidateByUserID(userId, m_candidate)) {
        m_data.loadCandidateSkill(m_candidate);
        m_data.loadCandidateCertificate(m_candidate);
        m_data.loadCandidateExperience(m_candidate);
    } else {
        // Fallback tạo đối tượng rỗng với ID nếu chưa có trong candidates.txt
        m_candidate = Candidate();
        m_candidate.setIDCandidate(m_data.getNextID("candidates.txt"));
        m_candidate.setIDuser(userId);
        m_candidate.setRole("CANDIDATE");
    }

    // 2. Tải đơn ứng tuyển của ứng viên từ JobSearchEngine (O(1))
    m_applications = m_searchEngine.getApplicationsForCandidate(m_candidate.getIDcandidate());

    populateDashboard();
}

void CandidatePage::populateDashboard() {
    buildProfileCard();
    buildRecentApplicationsCard();
    buildJobList();
}

int CandidatePage::calculateProfileCompleteness() const {
    int score = 0;
    const auto &p = m_candidate.getPersonalInfo();
    const auto &prof = m_candidate.getProfessionalInfo();

    // 1. Thông tin cá nhân cơ bản (tối đa 25%)
    if (!p.fullName.empty()) score += 10;
    if (!p.phoneCandidate.empty()) score += 5;
    if (!p.addressCandidate.empty()) score += 5;
    if (!p.birthday.empty()) score += 5;

    // 2. Học vấn & GPA (tối đa 25%)
    if (!prof.major.empty()) score += 10;
    if (!prof.school.empty()) score += 10;
    if (prof.GPA > 0.0f) score += 5;

    // 3. Kỹ năng (tối đa 20%)
    if (!prof.skills.empty()) {
        score += std::min(20, static_cast<int>(prof.skills.size()) * 5);
    }

    // 4. Chứng chỉ (tối đa 15%)
    if (!prof.certificates.empty()) {
        score += std::min(15, static_cast<int>(prof.certificates.size()) * 8);
    }

    // 5. Kinh nghiệm (tối đa 15%)
    if (!m_candidate.getExperiences().empty()) {
        score += std::min(15, static_cast<int>(m_candidate.getExperiences().size()) * 8);
    }

    return std::min(100, score);
}

void CandidatePage::buildProfileCard() {
    // Xóa nội dung cũ trong profile layout an toàn, tránh double free
    safeClearLayout(profileContentLayout);

    // Header Profile
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);

    QLabel *headerTitle = new QLabel("👤 Hồ sơ năng lực", this);
    headerTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");

    QPushButton *editBtn = new QPushButton("✏ Chỉnh sửa", this);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setStyleSheet("color: #B95334; font-size: 12px; font-weight: 600; background: transparent; border: none;");
    connect(editBtn, &QPushButton::clicked, this, &CandidatePage::onEditProfileClicked);

    headerLayout->addWidget(headerTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(editBtn);
    profileContentLayout->addLayout(headerLayout);

    // Avatar và Thông tin cá nhân
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(14);
    infoLayout->setAlignment(Qt::AlignVCenter);

    std::string fullName = m_candidate.getPersonalInfo().fullName;
    if (fullName.empty()) fullName = "Ứng viên";

    QString initials = "UV";
    QString qName = QString::fromStdString(fullName).trimmed();
    QStringList parts = qName.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2) {
        initials = parts.first().left(1).toUpper() + parts.last().left(1).toUpper();
    } else if (!parts.isEmpty()) {
        initials = parts.first().left(2).toUpper();
    }

    avatarCircleLabel = new QLabel(this);
    avatarCircleLabel->setAlignment(Qt::AlignCenter);
    avatarCircleLabel->setFixedSize(68, 68);
    avatarCircleLabel->setCursor(Qt::PointingHandCursor);
    avatarCircleLabel->setToolTip("Nhấn để đổi ảnh đại diện");
    avatarCircleLabel->installEventFilter(this);
    QString avPath = StyleHelper::getCandidateAvatarPath(m_candidate.getIDcandidate());
    avatarCircleLabel->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 68));

    QVBoxLayout *avatarCol = new QVBoxLayout();
    avatarCol->setSpacing(4);
    avatarCol->setAlignment(Qt::AlignCenter);
    avatarCol->addWidget(avatarCircleLabel, 0, Qt::AlignCenter);

    QPushButton *changeAvatarBtn = new QPushButton("📷 Đổi ảnh", this);
    changeAvatarBtn->setCursor(Qt::PointingHandCursor);
    changeAvatarBtn->setStyleSheet("background-color: transparent; color: #B95334; font-size: 11px; font-weight: 600; border: none; padding: 2px;");
    connect(changeAvatarBtn, &QPushButton::clicked, this, &CandidatePage::onChangeAvatarClicked);
    avatarCol->addWidget(changeAvatarBtn, 0, Qt::AlignCenter);

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(3);
    textLayout->setAlignment(Qt::AlignVCenter);

    candidateNameLabel = new QLabel(qName, this);
    candidateNameLabel->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");

    std::string major = m_candidate.getProfessionalInfo().major;
    if (major.empty()) major = "Chưa cập nhật chuyên ngành";
    candidateTitleLabel = new QLabel(QString::fromStdString(major), this);
    candidateTitleLabel->setStyleSheet("font-size: 12px; color: #B95334; font-weight: 600;");

    std::string address = m_candidate.getPersonalInfo().addressCandidate;
    if (address.empty()) address = "Chưa cập nhật địa chỉ";
    candidateLocationLabel = new QLabel("📍 " + QString::fromStdString(address), this);
    candidateLocationLabel->setStyleSheet("font-size: 12px; color: #6C757D;");

    std::string phone = m_candidate.getPersonalInfo().phoneCandidate;
    candidateEmailLabel = new QLabel("📞 " + QString::fromStdString(phone.empty() ? "Chưa có SĐT" : phone), this);
    candidateEmailLabel->setStyleSheet("font-size: 12px; color: #6C757D;");

    textLayout->addWidget(candidateNameLabel);
    textLayout->addWidget(candidateTitleLabel);
    textLayout->addWidget(candidateLocationLabel);
    textLayout->addWidget(candidateEmailLabel);

    infoLayout->addLayout(avatarCol);
    infoLayout->addLayout(textLayout, 1);
    infoLayout->addStretch();
    profileContentLayout->addLayout(infoLayout);

    // Mức độ hoàn thiện hồ sơ (Tính toán động thực tế)
    int completeness = calculateProfileCompleteness();

    QVBoxLayout *progressBox = new QVBoxLayout();
    progressBox->setSpacing(6);

    QHBoxLayout *progressHeader = new QHBoxLayout();
    QLabel *progressTitle = new QLabel("Mức độ hoàn thiện hồ sơ", this);
    progressTitle->setStyleSheet("font-size: 12px; color: #1E1E1E; font-weight: 500;");
    progressPercentLabel = new QLabel(QString::number(completeness) + "%", this);
    progressPercentLabel->setStyleSheet("font-size: 12px; color: #B95334; font-weight: 700;");
    progressHeader->addWidget(progressTitle);
    progressHeader->addStretch();
    progressHeader->addWidget(progressPercentLabel);
    progressBox->addLayout(progressHeader);

    profileProgressBar = new QProgressBar(this);
    profileProgressBar->setRange(0, 100);
    profileProgressBar->setValue(completeness);
    profileProgressBar->setTextVisible(false);
    profileProgressBar->setFixedHeight(8);
    progressBox->addWidget(profileProgressBar);
    profileContentLayout->addLayout(progressBox);

    // Khối Học vấn & GPA
    const auto &prof = m_candidate.getProfessionalInfo();
    QVBoxLayout *eduBox = new QVBoxLayout();
    eduBox->setSpacing(6);

    QLabel *eduTitle = new QLabel("🎓 Học vấn & Điểm GPA", this);
    eduTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
    eduBox->addWidget(eduTitle);

    QFrame *eduCard = new QFrame(this);
    eduCard->setStyleSheet("background-color: #FAF6F2; border: none; border-left: 3px solid #B95334; border-radius: 4px; padding: 6px 10px;");
    QVBoxLayout *eduCardLay = new QVBoxLayout(eduCard);
    eduCardLay->setSpacing(4);

    QString schoolStr = QString::fromStdString(prof.school.empty() ? "Chưa cập nhật trường" : prof.school);
    QString eduLevelStr = QString::fromStdString(prof.educationLevel.empty() ? "Đại học" : prof.educationLevel);
    QLabel *schoolLbl = new QLabel(QString("🏫 %1 (%2)").arg(schoolStr).arg(eduLevelStr), eduCard);
    schoolLbl->setStyleSheet("font-size: 12px; color: #1E1E1E; font-weight: 500;");

    QString gpaStr = QString("📊 Điểm GPA: %1 / 4.00").arg(QString::number(prof.GPA, 'f', 2));
    QLabel *gpaLbl = new QLabel(gpaStr, eduCard);
    gpaLbl->setStyleSheet("font-size: 12px; color: #B95334; font-weight: 700;");

    eduCardLay->addWidget(schoolLbl);
    eduCardLay->addWidget(gpaLbl);
    eduBox->addWidget(eduCard);
    profileContentLayout->addLayout(eduBox);

    // Khối Kỹ năng nổi bật
    QVBoxLayout *skillsBox = new QVBoxLayout();
    skillsBox->setSpacing(8);

    QHBoxLayout *skillsHeader = new QHBoxLayout();
    QLabel *skillsTitle = new QLabel("⚡ Kỹ năng chuyên môn", this);
    skillsTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
    QPushButton *addSkillBtn = new QPushButton("+ Thêm kỹ năng", this);
    addSkillBtn->setCursor(Qt::PointingHandCursor);
    addSkillBtn->setStyleSheet("color: #B95334; font-size: 12px; font-weight: 600; background: transparent; border: none;");
    connect(addSkillBtn, &QPushButton::clicked, this, &CandidatePage::onAddSkillClicked);

    skillsHeader->addWidget(skillsTitle);
    skillsHeader->addStretch();
    skillsHeader->addWidget(addSkillBtn);
    skillsBox->addLayout(skillsHeader);

    QGridLayout *skillsTagsGrid = new QGridLayout();
    skillsTagsGrid->setSpacing(6);

    const auto &candSkills = m_candidate.getSkill();
    if (!candSkills.empty()) {
        for (int i = 0; i < static_cast<int>(candSkills.size()); ++i) {
            const auto &sk = candSkills[i];

            QFrame *pill = new QFrame(this);
            pill->setStyleSheet("QFrame { background-color: #F5EBE1; border-radius: 12px; }");
            QHBoxLayout *pillLay = new QHBoxLayout(pill);
            pillLay->setContentsMargins(10, 2, 6, 2);
            pillLay->setSpacing(6);

            QPushButton *skillBtn = new QPushButton(QString::fromStdString(sk.name) + QString(" (Lv %1)").arg(sk.level), pill);
            skillBtn->setCursor(Qt::PointingHandCursor);
            skillBtn->setToolTip("Nhấp để chỉnh sửa kỹ năng này");
            skillBtn->setStyleSheet("color: #754533; font-size: 11px; font-weight: 600; background: transparent; border: none; padding: 0;");
            connect(skillBtn, &QPushButton::clicked, this, [this, i]() {
                onEditSkillClicked(i);
            });

            QPushButton *delBtn = new QPushButton("✕", pill);
            delBtn->setCursor(Qt::PointingHandCursor);
            delBtn->setToolTip("Xóa kỹ năng");
            delBtn->setFixedSize(16, 16);
            delBtn->setStyleSheet("QPushButton { color: #A36B5E; font-size: 11px; font-weight: 700; background: transparent; border: none; border-radius: 8px; } QPushButton:hover { color: #DC2626; background-color: #EBDAD0; }");
            connect(delBtn, &QPushButton::clicked, this, [this, i]() {
                onDeleteSkillClicked(i);
            });

            pillLay->addWidget(skillBtn);
            pillLay->addWidget(delBtn);
            skillsTagsGrid->addWidget(pill, i / 2, i % 2);
        }
        skillsBox->addLayout(skillsTagsGrid);
    } else {
        QLabel *emptySkillLbl = new QLabel("Chưa cập nhật kỹ năng chuyên môn", this);
        emptySkillLbl->setStyleSheet("color: #6C757D; font-size: 11px; font-style: italic;");
        skillsBox->addWidget(emptySkillLbl);
    }
    profileContentLayout->addLayout(skillsBox);

    // Khối Chứng chỉ ngoại ngữ & Tin học
    QVBoxLayout *certBox = new QVBoxLayout();
    certBox->setSpacing(8);

    QHBoxLayout *certHeader = new QHBoxLayout();
    QLabel *certTitle = new QLabel("📜 Chứng chỉ đạt được", this);
    certTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
    QPushButton *addCertBtn = new QPushButton("+ Thêm chứng chỉ", this);
    addCertBtn->setCursor(Qt::PointingHandCursor);
    addCertBtn->setStyleSheet("color: #B95334; font-size: 12px; font-weight: 600; background: transparent; border: none;");
    connect(addCertBtn, &QPushButton::clicked, this, &CandidatePage::onAddCertificateClicked);

    certHeader->addWidget(certTitle);
    certHeader->addStretch();
    certHeader->addWidget(addCertBtn);
    certBox->addLayout(certHeader);

    const auto &certs = m_candidate.getCertificate();
    if (!certs.empty()) {
        for (int i = 0; i < static_cast<int>(certs.size()); ++i) {
            const auto &c = certs[i];
            QString typeName = "Chứng chỉ";
            if (c.certificateTypeID == 1) typeName = "IELTS";
            else if (c.certificateTypeID == 2) typeName = "TOEIC";
            else if (c.certificateTypeID == 3) typeName = "TOEFL / HSK";
            else if (c.certificateTypeID == 4) typeName = "JLPT (Tiếng Nhật)";
            else if (c.certificateTypeID == 5) typeName = "Cloud / AWS";
            else typeName = "Chuyên ngành";

            QString orgStr = QString::fromStdString(c.organization.empty() ? "Tổ chức uy tín" : c.organization);
            QString valStr;
            if (c.certificateTypeID == 1) { // IELTS dạng 7.0, 6.5
                valStr = QString::number(c.value, 'f', 1);
            } else if (c.certificateTypeID == 2) { // TOEIC dạng số nguyên 700, 650
                valStr = QString::number(static_cast<int>(std::round(c.value)));
            } else if (c.certificateTypeID == 4) { // JLPT dạng N3, N2...
                int n = static_cast<int>(std::round(c.value));
                valStr = (n >= 1 && n <= 5) ? QString("N%1").arg(n) : QString::number(n);
            } else {
                valStr = (c.value == static_cast<int>(c.value)) ? QString::number(static_cast<int>(c.value)) : QString::number(c.value, 'f', 1);
            }

            QString certLine = QString("• %1: %2 (%3)").arg(typeName).arg(valStr).arg(orgStr);

            QHBoxLayout *certRow = new QHBoxLayout();
            certRow->setSpacing(8);

            QLabel *cLbl = new QLabel(certLine, this);
            cLbl->setStyleSheet("font-size: 12px; color: #333333;");
            certRow->addWidget(cLbl, 1);

            QPushButton *editCertBtn = new QPushButton("✏ Sửa", this);
            editCertBtn->setCursor(Qt::PointingHandCursor);
            editCertBtn->setStyleSheet("QPushButton { color: #4B5563; font-size: 11px; font-weight: 600; background: transparent; border: 1px solid #ECE5DE; border-radius: 4px; padding: 2px 8px; } QPushButton:hover { color: #B95334; border-color: #B95334; background-color: #FFFFFF; }");
            connect(editCertBtn, &QPushButton::clicked, this, [this, i]() {
                onEditCertificateClicked(i);
            });
            certRow->addWidget(editCertBtn);

            QPushButton *delCertBtn = new QPushButton("🗑 Xóa", this);
            delCertBtn->setCursor(Qt::PointingHandCursor);
            delCertBtn->setStyleSheet("QPushButton { color: #DC2626; font-size: 11px; font-weight: 600; background: transparent; border: 1px solid #FCA5A5; border-radius: 4px; padding: 2px 8px; } QPushButton:hover { background-color: #FEE2E2; }");
            connect(delCertBtn, &QPushButton::clicked, this, [this, i]() {
                onDeleteCertificateClicked(i);
            });
            certRow->addWidget(delCertBtn);

            certBox->addLayout(certRow);
        }
    } else {
        QLabel *emptyCertLbl = new QLabel("Chưa cập nhật chứng chỉ ngoại ngữ hoặc chuyên môn", this);
        emptyCertLbl->setStyleSheet("color: #6C757D; font-size: 11px; font-style: italic;");
        certBox->addWidget(emptyCertLbl);
    }
    profileContentLayout->addLayout(certBox);

    // Khối Kinh nghiệm làm việc
    QVBoxLayout *expBox = new QVBoxLayout();
    expBox->setSpacing(8);

    QHBoxLayout *expHeader = new QHBoxLayout();
    QLabel *expTitle = new QLabel("💼 Kinh nghiệm làm việc", this);
    expTitle->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
    QPushButton *addExpBtn = new QPushButton("+ Thêm kinh nghiệm", this);
    addExpBtn->setCursor(Qt::PointingHandCursor);
    addExpBtn->setStyleSheet("color: #B95334; font-size: 12px; font-weight: 600; background: transparent; border: none;");
    connect(addExpBtn, &QPushButton::clicked, this, &CandidatePage::onAddExperienceClicked);

    expHeader->addWidget(expTitle);
    expHeader->addStretch();
    expHeader->addWidget(addExpBtn);
    expBox->addLayout(expHeader);

    const auto &exps = m_candidate.getExperiences();
    if (!exps.empty()) {
        for (int i = 0; i < static_cast<int>(exps.size()); ++i) {
            const auto &e = exps[i];
            QFrame *expItem = new QFrame(this);
            expItem->setStyleSheet("background-color: #FAF6F2; border: none; border-left: 3px solid #B95334; border-radius: 4px; padding: 6px 10px;");
            QVBoxLayout *expItemLay = new QVBoxLayout(expItem);
            expItemLay->setSpacing(4);

            QHBoxLayout *topRow = new QHBoxLayout();
            topRow->setSpacing(8);

            QLabel *posLbl = new QLabel(QString("• %1 @ %2")
                .arg(QString::fromStdString(e.getPosition()))
                .arg(QString::fromStdString(e.getCompanyName())), expItem);
            posLbl->setStyleSheet("font-size: 12px; font-weight: 600; color: #1E1E1E;");
            topRow->addWidget(posLbl, 1);

            QPushButton *editExpBtn = new QPushButton("✏ Sửa", expItem);
            editExpBtn->setCursor(Qt::PointingHandCursor);
            editExpBtn->setStyleSheet("QPushButton { color: #4B5563; font-size: 11px; font-weight: 600; background: transparent; border: 1px solid #ECE5DE; border-radius: 4px; padding: 2px 8px; } QPushButton:hover { color: #B95334; border-color: #B95334; background-color: #FFFFFF; }");
            connect(editExpBtn, &QPushButton::clicked, this, [this, i]() {
                onEditExperienceClicked(i);
            });
            topRow->addWidget(editExpBtn);

            QPushButton *delExpBtn = new QPushButton("🗑 Xóa", expItem);
            delExpBtn->setCursor(Qt::PointingHandCursor);
            delExpBtn->setStyleSheet("QPushButton { color: #DC2626; font-size: 11px; font-weight: 600; background: transparent; border: 1px solid #FCA5A5; border-radius: 4px; padding: 2px 8px; } QPushButton:hover { background-color: #FEE2E2; }");
            connect(delExpBtn, &QPushButton::clicked, this, [this, i]() {
                onDeleteExperienceClicked(i);
            });
            topRow->addWidget(delExpBtn);

            expItemLay->addLayout(topRow);

            QLabel *dateLbl = new QLabel(QString("%1 → %2")
                .arg(QString::fromStdString(e.getStartDate()))
                .arg(QString::fromStdString(e.getEndDate())), expItem);
            dateLbl->setStyleSheet("font-size: 11px; color: #6C757D;");
            expItemLay->addWidget(dateLbl);

            if (!e.getDescription().empty()) {
                QLabel *descLbl = new QLabel(QString::fromStdString(e.getDescription()), expItem);
                descLbl->setStyleSheet("font-size: 11px; color: #555555;");
                descLbl->setWordWrap(true);
                expItemLay->addWidget(descLbl);
            }
            expBox->addWidget(expItem);
        }
    } else {
        QLabel *emptyExpLbl = new QLabel("Chưa cập nhật kinh nghiệm làm việc", this);
        emptyExpLbl->setStyleSheet("color: #6C757D; font-size: 11px; font-style: italic;");
        expBox->addWidget(emptyExpLbl);
    }
    profileContentLayout->addLayout(expBox);
}

void CandidatePage::buildRecentApplicationsCard() {
    safeClearLayout(recentAppsLayout);

    QHBoxLayout *header = new QHBoxLayout();
    QLabel *title = new QLabel("📄 Đơn ứng tuyển gần đây", this);
    title->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");
    QPushButton *viewAllBtn = new QPushButton("Xem tất cả →", this);
    viewAllBtn->setCursor(Qt::PointingHandCursor);
    viewAllBtn->setStyleSheet("color: #B95334; font-size: 12px; font-weight: 600; background: transparent; border: none;");
    connect(viewAllBtn, &QPushButton::clicked, this, &CandidatePage::viewAllApplicationsRequested);

    header->addWidget(title);
    header->addStretch();
    header->addWidget(viewAllBtn);
    recentAppsLayout->addLayout(header);

    if (m_applications.empty()) {
        QLabel *emptyAppLabel = new QLabel("Bạn chưa có đơn ứng tuyển nào.", this);
        emptyAppLabel->setStyleSheet("color: #6C757D; font-size: 12px; padding: 18px;");
        emptyAppLabel->setAlignment(Qt::AlignCenter);
        recentAppsLayout->addWidget(emptyAppLabel);
    } else {
        // Hiển thị dữ liệu thực từ m_applications
        int count = 0;
        for (const auto &app : m_applications) {
            if (++count > 4) break;
            Job job;
            Company comp;
            QString jobTitle = "Công việc #" + QString::number(app.getIDjob());
            QString compName = "Doanh nghiệp";

            if (m_data.getJobByID(app.getIDjob(), job)) {
                jobTitle = QString::fromStdString(job.getTitle());
                if (m_data.getCompanyByID(job.getIDcompany(), comp)) {
                    compName = QString::fromStdString(comp.getNameCompany());
                }
            }

            QHBoxLayout *row = new QHBoxLayout();
            row->setSpacing(10);
            row->setAlignment(Qt::AlignVCenter);

            QLabel *logo = new QLabel(this);
            logo->setAlignment(Qt::AlignCenter);
            logo->setFixedSize(44, 44);
            QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
            QString initials = StyleHelper::getInitials(compName, "DN");
            logo->setPixmap(StyleHelper::getRoundedBadge(logoPath, initials, 44, 10));

            QVBoxLayout *txt = new QVBoxLayout();
            txt->setSpacing(2);
            txt->setAlignment(Qt::AlignVCenter);
            QLabel *roleLbl = new QLabel(jobTitle, this);
            roleLbl->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
            QLabel *compLbl = new QLabel(compName, this);
            compLbl->setStyleSheet("font-size: 11px; color: #6C757D;");
            txt->addWidget(roleLbl);
            txt->addWidget(compLbl);

            QLabel *dateLbl = new QLabel(QString::fromStdString(app.getAppliedAt()), this);
            dateLbl->setStyleSheet("font-size: 11px; color: #6C757D;");

            QString st = QString::fromStdString(app.getStatus());
            QLabel *badge = new QLabel(st, this);
            if (st == "ACCEPTED") {
                badge->setText("Đã chấp nhận");
                badge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 3px 8px;");
            } else if (st == "PENDING") {
                badge->setText("Đang xem xét");
                badge->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 3px 8px;");
            } else if (st == "REJECTED") {
                badge->setText("Đã từ chối");
                badge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 3px 8px;");
            } else {
                badge->setText(st);
                badge->setStyleSheet("background-color: #F8F9FA; color: #6C757D; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 3px 8px;");
            }

            row->addWidget(logo, 0, Qt::AlignVCenter);
            row->addLayout(txt);
            row->addStretch();
            row->addWidget(dateLbl);
            row->addWidget(badge);
            recentAppsLayout->addLayout(row);
        }
    }
}

void CandidatePage::buildJobList() {
    safeClearLayout(jobsListLayout);

    JobSearchFilter filter;
    filter.keyword = m_filterKeyword.trimmed().toStdString();
    if (inpageSearchEdit && !inpageSearchEdit->text().trimmed().isEmpty()) {
        filter.keyword = inpageSearchEdit->text().trimmed().toStdString();
    }
    if (inpageLocCombo && !inpageLocCombo->currentData().toString().isEmpty()) {
        filter.location = inpageLocCombo->currentData().toString().toStdString();
    }
    if (inpageMajorCombo && !inpageMajorCombo->currentData().toString().isEmpty()) {
        filter.major = inpageMajorCombo->currentData().toString().toStdString();
    }
    filter.onlyOpenJobs = true;

    // Toàn bộ logic tìm kiếm, Inverted Index, lọc từ khóa, lọc hạn nộp,
    // tính điểm matching/calculatorTotal và sắp xếp kết quả được xử lý trong JobSearchEngine
    std::vector<SearchResultItem> results = m_searchEngine.searchForCandidate(filter, m_candidate);

    if (m_viewMode == ViewMode::MatchedJobs) {
        // TRANG VIỆC LÀM PHÙ HỢP (Yêu cầu 4 & 11.B): Chỉ hiển thị việc đạt matching >= 70% và đủ điều kiện
        for (auto it = results.begin(); it != results.end(); ) {
            if (!it->isEligible || it->matchScore < 70.0) {
                it = results.erase(it);
            } else {
                ++it;
            }
        }
    }
    // TRANG CHỦ (Yêu cầu 11.A): Hiển thị tất cả công việc còn hiệu lực và đang tuyển dụng

    if (results.empty()) {
        QString emptyMsg = (m_viewMode == ViewMode::MatchedJobs)
            ? "Hiện chưa có tin tuyển dụng nào đạt ngưỡng phù hợp (≥ 70%)."
            : "Không tìm thấy tin tuyển dụng nào phù hợp với bộ lọc.";
        QLabel *emptyLabel = new QLabel(emptyMsg, this);
        emptyLabel->setStyleSheet("color: #6C757D; font-size: 14px; padding: 30px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        jobsListLayout->addWidget(emptyLabel);
        return;
    }

    for (const auto &item : results) {
        const Job &job = item.job;
        Company comp = item.company;
        if (comp.getIDcompany() <= 0 && job.getIDcompany() > 0) {
            m_data.getCompanyByID(job.getIDcompany(), comp);
        }

        QFrame *card = new QFrame(this);
        card->setObjectName("cardBox");
        card->setStyleSheet(
            "#cardBox {"
            "   background-color: #FFFFFF;"
            "   border: 1px solid #ECE5DE;"
            "   border-radius: 12px;"
            "   padding: 20px;"
            "}"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(12);

        // Hàng 1: Logo, Tên Job + Tên Công ty, và Khối Điểm phù hợp
        QHBoxLayout *topRow = new QHBoxLayout();
        topRow->setSpacing(14);
        topRow->setAlignment(Qt::AlignVCenter);

        QLabel *logoLabel = new QLabel(card);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setFixedSize(54, 54);
        QString qCompName = QString::fromStdString(comp.getNameCompany().empty() ? "Doanh nghiệp" : comp.getNameCompany());
        QString compInitials = StyleHelper::getInitials(qCompName, "DN");
        QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
        logoLabel->setPixmap(StyleHelper::getRoundedBadge(logoPath, compInitials, 54, 12));

        QVBoxLayout *jobTitleLayout = new QVBoxLayout();
        jobTitleLayout->setSpacing(3);
        QLabel *titleLabel = new QLabel(QString::fromStdString(job.getTitle()), card);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
        QLabel *compLabel = new QLabel(qCompName, card);
        compLabel->setStyleSheet("font-size: 13px; color: #B95334; font-weight: 600;");
        jobTitleLayout->addWidget(titleLabel);
        jobTitleLayout->addWidget(compLabel);

        // Match Score Box
        QFrame *scoreBox = new QFrame(card);
        QVBoxLayout *scoreLayout = new QVBoxLayout(scoreBox);
        scoreLayout->setContentsMargins(8, 4, 8, 4);
        scoreLayout->setSpacing(0);

        if (item.isEligible && item.matchScore >= 0.0) {
            int score = static_cast<int>(std::round(item.matchScore));
            bool isHigh = score >= 70;
            scoreBox->setStyleSheet(isHigh ? "background-color: #E3F7EB; border-radius: 8px; padding: 6px 12px;"
                                           : "background-color: #FEF3E2; border-radius: 8px; padding: 6px 12px;");
            QLabel *scoreVal = new QLabel(QString::number(score) + "%", scoreBox);
            scoreVal->setAlignment(Qt::AlignCenter);
            scoreVal->setStyleSheet(isHigh ? "color: #1B7F49; font-size: 17px; font-weight: 800; background: transparent;"
                                           : "color: #B76E00; font-size: 17px; font-weight: 800; background: transparent;");
            QLabel *scoreTxt = new QLabel(isHigh ? "Phù hợp" : "Chưa đạt gợi ý", scoreBox);
            scoreTxt->setAlignment(Qt::AlignCenter);
            scoreTxt->setStyleSheet(isHigh ? "color: #1B7F49; font-size: 10px; font-weight: 600; background: transparent;"
                                           : "color: #B76E00; font-size: 10px; font-weight: 600; background: transparent;");
            scoreLayout->addWidget(scoreVal);
            scoreLayout->addWidget(scoreTxt);
        } else {
            scoreBox->setStyleSheet("background-color: #FEF3E2; border-radius: 8px; padding: 6px 10px;");
            QLabel *scoreVal = new QLabel("Chưa đủ điều kiện", scoreBox);
            scoreVal->setAlignment(Qt::AlignCenter);
            scoreVal->setStyleSheet("color: #B76E00; font-size: 12px; font-weight: 700; background: transparent;");
            QLabel *scoreTxt = new QLabel("Không đạt tiên quyết", scoreBox);
            scoreTxt->setAlignment(Qt::AlignCenter);
            scoreTxt->setStyleSheet("color: #B76E00; font-size: 10px; font-weight: 500; background: transparent;");
            scoreLayout->addWidget(scoreVal);
            scoreLayout->addWidget(scoreTxt);
        }

        topRow->addWidget(logoLabel, 0, Qt::AlignVCenter);
        topRow->addLayout(jobTitleLayout, 1);
        topRow->addWidget(scoreBox, 0, Qt::AlignVCenter);
        cardLayout->addLayout(topRow);

        // Hàng 2: Metadata (Địa điểm, Loại hình, Kinh nghiệm, Mức lương)
        QHBoxLayout *metaRow = new QHBoxLayout();
        metaRow->setSpacing(16);

        QString loc = QString::fromStdString(job.getLocation().empty() ? "Chưa cập nhật" : job.getLocation());
        QLabel *locLabel = new QLabel("📍 " + loc, card);
        locLabel->setStyleSheet("color: #6C757D; font-size: 12px; font-weight: 500;");

        QLabel *typeLabel = new QLabel("💼 Toàn thời gian", card);
        typeLabel->setStyleSheet("color: #6C757D; font-size: 12px; font-weight: 500;");

        int exp = job.getMinExperience();
        QString expStr = exp > 0 ? QString("%1 tháng kinh nghiệm").arg(exp) : "Không yêu cầu kinh nghiệm";
        QLabel *expLabel = new QLabel("📊 " + expStr, card);
        expLabel->setStyleSheet("color: #6C757D; font-size: 12px; font-weight: 500;");

        metaRow->addWidget(locLabel);
        metaRow->addWidget(typeLabel);
        metaRow->addWidget(expLabel);
        metaRow->addStretch();
        cardLayout->addLayout(metaRow);

        // Hàng 3: Mô tả công việc
        QString desc = QString::fromStdString(job.getDescription());
        if (desc.isEmpty()) {
            desc = "Chưa cập nhật mô tả chi tiết công việc.";
        }
        QLabel *descLabel = new QLabel(desc, card);
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("color: #4A4A4A; font-size: 13px; line-height: 1.4;");
        cardLayout->addWidget(descLabel);

        // Hàng 4: Tag kỹ năng & Nút Xem chi tiết →
        QHBoxLayout *bottomRow = new QHBoxLayout();
        bottomRow->setSpacing(8);

        // Danh sách tag kỹ năng thực tế (chỉ hiển thị kỹ năng thực tế được yêu cầu, không gán mặc định giả)
        const auto &reqSkills = job.getRequiredSkill();
        for (const auto &sk : reqSkills) {
            if (!sk.name.empty()) {
                QLabel *tagLabel = new QLabel(QString::fromStdString(sk.name), card);
                tagLabel->setStyleSheet("background-color: #F5EBE1; color: #754533; font-size: 11px; font-weight: 600; border-radius: 12px; padding: 4px 10px;");
                bottomRow->addWidget(tagLabel);
            }
        }
        bottomRow->addStretch();

        QPushButton *detailBtn = new QPushButton("Xem chi tiết →", card);
        detailBtn->setCursor(Qt::PointingHandCursor);
        detailBtn->setStyleSheet(
            "background-color: transparent; color: #B95334; border: 1px solid #B95334; "
            "border-radius: 8px; padding: 6px 16px; font-size: 13px; font-weight: 600;"
        );
        int jobId = job.getIDjob();
        connect(detailBtn, &QPushButton::clicked, this, [this, jobId]() {
            emit viewJobRequested(jobId);
        });
        bottomRow->addWidget(detailBtn);

        cardLayout->addLayout(bottomRow);
        jobsListLayout->addWidget(card);
    }
}

void CandidatePage::filterJobs(const QString &keyword) {
    m_filterKeyword = keyword;
    if (inpageSearchEdit && inpageSearchEdit->text() != keyword) {
        inpageSearchEdit->setText(keyword);
    } else {
        buildJobList();
    }
}

void CandidatePage::setViewMode(ViewMode mode) {
    m_viewMode = mode;
    if (m_viewMode == ViewMode::AllJobs) {
        if (secTitle) secTitle->setText("Cơ hội việc làm đang tuyển dụng");
        if (secSub) secSub->setText("Khám phá tất cả các vị trí việc làm còn hiệu lực trong hệ thống");
        if (viewAllBtn) viewAllBtn->setVisible(false);
    } else {
        if (secTitle) secTitle->setText("Việc làm phù hợp với bạn");
        if (secSub) secSub->setText("Được chọn lọc dựa trên chuyên ngành, kỹ năng và kinh nghiệm đạt chuẩn (Điểm phù hợp ≥ 70%)");
        if (viewAllBtn) viewAllBtn->setVisible(false); // Bỏ mục "Xem tất cả" theo Yêu cầu 4
    }
    buildJobList();
}

void CandidatePage::onRefreshData() {
    m_searchEngine.reload(m_data);
    setSession(m_userId);
}

void CandidatePage::onChangeAvatarClicked() {
    QString file = QFileDialog::getOpenFileName(this, "Chọn ảnh đại diện", "", "Hình ảnh (*.png *.jpg *.jpeg *.bmp *.webp)");
    if (file.isEmpty()) return;

    QString savedPath;
    if (StyleHelper::saveCandidateAvatar(m_candidate.getIDcandidate(), file, savedPath)) {
        populateDashboard();
        CustomMessageBox::success(this, "Cập nhật thành công", "Đã cập nhật ảnh đại diện mới thành công!");
    } else {
        CustomMessageBox::warning(this, "Lỗi cập nhật", "Không thể nạp tệp hình ảnh đã chọn. Vui lòng chọn tệp định dạng JPG hoặc PNG hợp lệ.");
    }
}

bool CandidatePage::eventFilter(QObject *obj, QEvent *event) {
    if (obj == avatarCircleLabel && event->type() == QEvent::MouseButtonPress) {
        onChangeAvatarClicked();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void CandidatePage::onEditProfileClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Chỉnh sửa thông tin cá nhân & học vấn");
    dialog.resize(600, 560);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    // Header
    QVBoxLayout *hBox = new QVBoxLayout();
    hBox->setSpacing(2);
    QLabel *hTitle = new QLabel("✏ Chỉnh sửa thông tin cá nhân & học vấn", &dialog);
    hTitle->setStyleSheet("font-size: 17px; font-weight: 700; color: #1E1E1E;");
    QLabel *hSub = new QLabel("Cập nhật thông tin chính xác giúp hệ thống tính điểm phù hợp việc làm tối ưu", &dialog);
    hSub->setStyleSheet("font-size: 12px; color: #6C757D;");
    hBox->addWidget(hTitle);
    hBox->addWidget(hSub);
    mainLayout->addLayout(hBox);

    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *formContainer = new QWidget(scrollArea);
    formContainer->setStyleSheet("background-color: transparent;");
    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(14);

    const auto &curP = m_candidate.getPersonalInfo();
    const auto &curProf = m_candidate.getProfessionalInfo();

    // 1. Nhóm Cá nhân & Liên hệ
    QFrame *pCard = new QFrame(formContainer);
    pCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 14px;");
    QVBoxLayout *pCardLay = new QVBoxLayout(pCard);
    pCardLay->setSpacing(10);
    QLabel *pSecTitle = new QLabel("👤 Thông tin cá nhân & Liên hệ", pCard);
    pSecTitle->setStyleSheet("font-size: 13px; font-weight: 700; color: #B95334;");
    pCardLay->addWidget(pSecTitle);

    QFormLayout *pForm = new QFormLayout();
    pForm->setSpacing(8);

    QLineEdit *nameEdit = new QLineEdit(QString::fromStdString(curP.fullName), pCard);
    nameEdit->setPlaceholderText("VD: Nguyễn Văn An");
    nameEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *bdayEdit = new QLineEdit(QString::fromStdString(formatDateDisplay(curP.birthday)), pCard);
    bdayEdit->setPlaceholderText("dd/MM/yyyy (VD: 18/05/2002)");
    bdayEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QComboBox *genderCombo = new QComboBox(pCard);
    genderCombo->addItem("Nam");
    genderCombo->addItem("Nữ");
    genderCombo->addItem("Khác");
    if (!curP.gender.empty()) {
        int idx = genderCombo->findText(QString::fromStdString(curP.gender));
        if (idx >= 0) genderCombo->setCurrentIndex(idx);
    }
    genderCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    QLineEdit *phoneEdit = new QLineEdit(QString::fromStdString(curP.phoneCandidate), pCard);
    phoneEdit->setPlaceholderText("VD: 0905123456");
    phoneEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]{0,11}$"), phoneEdit));
    phoneEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *addrEdit = new QLineEdit(QString::fromStdString(curP.addressCandidate), pCard);
    addrEdit->setPlaceholderText("VD: 54 Nguyễn Lương Bằng, Đà Nẵng");
    addrEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    pForm->addRow("Họ và tên *:", nameEdit);
    pForm->addRow("Ngày sinh (dd/MM/yyyy):", bdayEdit);
    pForm->addRow("Giới tính:", genderCombo);
    pForm->addRow("Số điện thoại:", phoneEdit);
    pForm->addRow("Địa chỉ cư trú:", addrEdit);
    pCardLay->addLayout(pForm);
    formLayout->addWidget(pCard);

    // 2. Nhóm Học vấn & Chuyên ngành
    QFrame *eduCard = new QFrame(formContainer);
    eduCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 14px;");
    QVBoxLayout *eduCardLay = new QVBoxLayout(eduCard);
    eduCardLay->setSpacing(10);
    QLabel *eduSecTitle = new QLabel("🎓 Học vấn & Chuyên ngành đào tạo", eduCard);
    eduSecTitle->setStyleSheet("font-size: 13px; font-weight: 700; color: #B95334;");
    eduCardLay->addWidget(eduSecTitle);

    QFormLayout *eduForm = new QFormLayout();
    eduForm->setSpacing(8);

    QLineEdit *schoolEdit = new QLineEdit(QString::fromStdString(curProf.school), eduCard);
    schoolEdit->setPlaceholderText("VD: ĐH Bách Khoa - ĐH Đà Nẵng");
    schoolEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *majorEdit = new QLineEdit(QString::fromStdString(curProf.major), eduCard);
    majorEdit->setPlaceholderText("VD: Công nghệ thông tin, Kỹ thuật phần mềm...");
    majorEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QComboBox *eduCombo = new QComboBox(eduCard);
    eduCombo->addItem("Chưa chọn", "");
    eduCombo->addItem("Đại học", "Đại học");
    eduCombo->addItem("Cao đẳng", "Cao đẳng");
    eduCombo->addItem("Thạc sĩ", "Thạc sĩ");
    eduCombo->addItem("Kỹ sư", "Kỹ sư");
    eduCombo->addItem("Cử nhân", "Cử nhân");
    if (!curProf.educationLevel.empty()) {
        int idx = eduCombo->findText(QString::fromStdString(curProf.educationLevel));
        if (idx >= 0) eduCombo->setCurrentIndex(idx);
    }
    eduCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    QDoubleSpinBox *gpaSpin = new QDoubleSpinBox(eduCard);
    gpaSpin->setRange(0.00, 4.00);
    gpaSpin->setSingleStep(0.05);
    gpaSpin->setDecimals(2);
    gpaSpin->setValue(curProf.GPA > 0.0f ? curProf.GPA : 0.0);
    gpaSpin->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    eduForm->addRow("Trường đào tạo:", schoolEdit);
    eduForm->addRow("Chuyên ngành:", majorEdit);
    eduForm->addRow("Trình độ đào tạo:", eduCombo);
    eduForm->addRow("Điểm GPA (hệ 4.0):", gpaSpin);
    eduCardLay->addLayout(eduForm);
    formLayout->addWidget(eduCard);

    scrollArea->setWidget(formContainer);
    mainLayout->addWidget(scrollArea, 1);

    // Footer buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 7px 18px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton("✓ Lưu thay đổi", &dialog);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 7px 22px; font-weight: 700;");

    connect(saveBtn, &QPushButton::clicked, [&]() {
        QString newName = nameEdit->text().trimmed();
        if (newName.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu thông tin", "Họ và tên ứng viên không được để trống.");
            return;
        }

        QString bdayText = bdayEdit->text().trimmed();
        if (!bdayText.isEmpty()) {
            if (!isValidDateOfBirth(bdayText.toStdString())) {
                CustomMessageBox::warning(&dialog, "Ngày sinh không hợp lệ",
                    "Ngày sinh phải đúng định dạng dd/MM/yyyy (ví dụ: 18/05/2002) và phải là ngày có thực trong lịch (đúng năm nhuận, không quá ngày hiện tại).");
                return;
            }
        }

        QString phoneText = phoneEdit->text().trimmed();
        if (!phoneText.isEmpty()) {
            if (!isValidPhoneNumber(phoneText.toStdString())) {
                CustomMessageBox::warning(&dialog, "Số điện thoại không hợp lệ",
                    "Số điện thoại chỉ được chứa chữ số, bắt đầu bằng số 0 và có độ dài từ 10 đến 11 số (ví dụ: 0905123456).");
                return;
            }
        }

        PersonalInfo p = m_candidate.getPersonalInfo();
        p.fullName = newName.toStdString();
        p.birthday = bdayText.toStdString();
        p.gender = genderCombo->currentText().toStdString();
        p.phoneCandidate = phoneText.toStdString();
        p.addressCandidate = addrEdit->text().trimmed().toStdString();
        m_candidate.setPersonalInfo(p);

        ProfessionalInfo prof = m_candidate.getProfessionalInfo();
        prof.school = schoolEdit->text().trimmed().toStdString();
        prof.major = formatMajorDisplay(majorEdit->text().trimmed().toStdString());
        prof.educationLevel = eduCombo->currentData().toString().toStdString();
        prof.GPA = static_cast<float>(gpaSpin->value());
        m_candidate.setProfessionalInfo(prof);

        if (m_candidate.updateCandidate(p, prof, m_data)) {
            m_searchEngine.reload(m_data);
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Cập nhật thành công", "Thông tin hồ sơ cá nhân và học vấn đã được lưu chính xác vào candidates.txt!");
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi dữ liệu hồ sơ vào candidates.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onAddSkillClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thêm kỹ năng chuyên môn");
    dialog.resize(460, 260);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("⚡ Thêm kỹ năng chuyên môn mới", &dialog);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Kỹ năng càng khớp với yêu cầu tin tuyển dụng sẽ gia tăng điểm matching", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(10);

    QLineEdit *skillEdit = new QLineEdit(formCard);
    skillEdit->setPlaceholderText("VD: C++, Qt, Python, OOP, STL, Docker, SQL...");
    skillEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QComboBox *levelCombo = new QComboBox(formCard);
    levelCombo->addItem("1 - Mới bắt đầu (Beginner)", 1);
    levelCombo->addItem("2 - Cơ bản (Elementary)", 2);
    levelCombo->addItem("3 - Trung cấp (Intermediate)", 3);
    levelCombo->addItem("4 - Khá tốt (Advanced)", 4);
    levelCombo->addItem("5 - Chuyên gia (Expert)", 5);
    levelCombo->setCurrentIndex(2); // Mặc định level 3
    levelCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    form->addRow("Tên kỹ năng *:", skillEdit);
    form->addRow("Mức độ thành thạo:", levelCombo);
    mainLayout->addWidget(formCard);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *addBtn = new QPushButton("✓ Thêm kỹ năng", &dialog);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 20px; font-weight: 700;");

    connect(addBtn, &QPushButton::clicked, [&]() {
        QString sName = skillEdit->text().trimmed();
        if (sName.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Vui lòng nhập tên kỹ năng chuyên môn.");
            return;
        }

        Skill s;
        s.IDskill = 0;
        s.name = sName.toStdString();
        s.level = levelCombo->currentData().toInt();

        if (m_candidate.addSkill(s, m_data)) {
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", QString("Đã thêm kỹ năng \"%1\" vào hồ sơ thành công!").arg(sName));
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi kỹ năng vào candidate_skill.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(addBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onAddCertificateClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thêm chứng chỉ chuyên môn & ngoại ngữ");
    dialog.resize(480, 300);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("📜 Thêm chứng chỉ đạt được", &dialog);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Chứng chỉ đạt chuẩn đáp ứng các yêu cầu tiên quyết của doanh nghiệp", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(10);

    QComboBox *typeCombo = new QComboBox(formCard);
    typeCombo->addItem("IELTS (Tiếng Anh)", 1);
    typeCombo->addItem("TOEIC (Tiếng Anh)", 2);
    typeCombo->addItem("TOEFL (Tiếng Anh)", 3);
    typeCombo->addItem("JLPT (Tiếng Nhật)", 4);
    typeCombo->addItem("AWS / Cloud Certification", 5);
    typeCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    QLineEdit *orgEdit = new QLineEdit(formCard);
    orgEdit->setPlaceholderText("VD: IDP, British Council, ETS, Amazon...");
    orgEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QDoubleSpinBox *scoreSpin = new QDoubleSpinBox(formCard);
    scoreSpin->setRange(1.0, 9.0);
    scoreSpin->setSingleStep(0.5);
    scoreSpin->setDecimals(1);
    scoreSpin->setValue(7.0);
    scoreSpin->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    auto configSpinForCertType = [scoreSpin](int typeId, bool setDefault) {
        if (typeId == 1) { // IELTS: 7.0
            scoreSpin->setRange(1.0, 9.0);
            scoreSpin->setDecimals(1);
            scoreSpin->setSingleStep(0.5);
            if (setDefault) scoreSpin->setValue(7.0);
        } else if (typeId == 2) { // TOEIC: 700
            scoreSpin->setRange(100.0, 990.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(50.0);
            if (setDefault) scoreSpin->setValue(700.0);
        } else if (typeId == 3) { // TOEFL / HSK
            scoreSpin->setRange(1.0, 120.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(5.0);
            if (setDefault) scoreSpin->setValue(80.0);
        } else if (typeId == 4) { // JLPT
            scoreSpin->setRange(1.0, 5.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(1.0);
            if (setDefault) scoreSpin->setValue(3.0);
        } else if (typeId == 5) { // Cloud / AWS
            scoreSpin->setRange(1.0, 5.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(1.0);
            if (setDefault) scoreSpin->setValue(1.0);
        } else {
            scoreSpin->setRange(0.0, 1000.0);
            scoreSpin->setDecimals(1);
            scoreSpin->setSingleStep(0.5);
            if (setDefault) scoreSpin->setValue(100.0);
        }
    };

    QObject::connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [typeCombo, configSpinForCertType](int) {
        configSpinForCertType(typeCombo->currentData().toInt(), true);
    });

    form->addRow("Loại chứng chỉ *:", typeCombo);
    form->addRow("Đơn vị / Tổ chức cấp *:", orgEdit);
    form->addRow("Điểm số / Cấp độ *:", scoreSpin);
    mainLayout->addWidget(formCard);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *addBtn = new QPushButton("✓ Thêm chứng chỉ", &dialog);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 20px; font-weight: 700;");

    connect(addBtn, &QPushButton::clicked, [&]() {
        QString org = orgEdit->text().trimmed();
        if (org.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Vui lòng nhập tên đơn vị hoặc tổ chức cấp chứng chỉ.");
            return;
        }

        Certificate cert;
        cert.IDcertificate = 0;
        cert.certificateTypeID = typeCombo->currentData().toInt();
        cert.organization = org.toStdString();
        cert.value = scoreSpin->value();

        if (m_candidate.addCertificate(cert, m_data)) {
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", "Đã thêm chứng chỉ mới vào hồ sơ thành công!");
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi chứng chỉ vào candidate_certificate.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(addBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onAddExperienceClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thêm kinh nghiệm làm việc");
    dialog.resize(520, 420);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("💼 Thêm kinh nghiệm làm việc mới", &dialog);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Kinh nghiệm thực tế tại doanh nghiệp giúp tăng điểm đánh giá tổng hợp", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(8);

    QLineEdit *posEdit = new QLineEdit(formCard);
    posEdit->setPlaceholderText("VD: C++ Software Engineer, Thực tập sinh Backend...");
    posEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *compEdit = new QLineEdit(formCard);
    compEdit->setPlaceholderText("VD: FPT Software, Viettel, VNG...");
    compEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QDateEdit *startEdit = new QDateEdit(formCard);
    startEdit->setCalendarPopup(true);
    startEdit->setDisplayFormat("dd/MM/yyyy");
    startEdit->setMaximumDate(QDate::currentDate());
    startEdit->setDate(QDate::currentDate().addYears(-1));

    QDateEdit *endEdit = new QDateEdit(formCard);
    endEdit->setCalendarPopup(true);
    endEdit->setDisplayFormat("dd/MM/yyyy");
    endEdit->setMaximumDate(QDate::currentDate());
    endEdit->setDate(QDate::currentDate());

    QCheckBox *currentCheck = new QCheckBox(
        "Đang làm việc tại đây", formCard
    );

    connect(currentCheck, &QCheckBox::toggled, endEdit, [endEdit](bool checked) {
        endEdit->setEnabled(!checked);
    });

    QTextEdit *descEdit = new QTextEdit(formCard);
    descEdit->setPlaceholderText("Mô tả trách nhiệm công việc, kỹ nghệ sử dụng và thành tích...");
    descEdit->setMaximumHeight(75);
    descEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    form->addRow("Vị trí đảm nhiệm *:", posEdit);
    form->addRow("Tên công ty *:", compEdit);
    form->addRow("Thời gian bắt đầu *:", startEdit);
    form->addRow("Thời gian kết thúc:", endEdit);
    form->addRow("", currentCheck); 
    form->addRow("Mô tả chi tiết:", descEdit);
    mainLayout->addWidget(formCard);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *addBtn = new QPushButton("✓ Thêm kinh nghiệm", &dialog);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 20px; font-weight: 700;");

    connect(addBtn, &QPushButton::clicked, [&]() {
        QString pos = posEdit->text().trimmed();
        QString comp = compEdit->text().trimmed();
        if (pos.isEmpty() || comp.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Vui lòng nhập vị trí đảm nhiệm và tên công ty.");
            return;
        }

        QDate startDate = startEdit->date();

        QDate endDate = currentCheck->isChecked() ? QDate::currentDate() : endEdit->date();

        if (startDate > endDate) {
            CustomMessageBox::warning( &dialog, "Ngày không hợp lệ", "Ngày bắt đầu không được sau ngày kết thúc.");
            return;
        }

        string startText = startDate.toString("yyyy-MM-dd").toStdString();

        string endText = currentCheck->isChecked() ? string("Hiện tại") : endDate.toString("yyyy-MM-dd").toStdString();

        int newId = static_cast<int>(m_candidate.getExperiences().size()) + 1;
        Experience exp(
            newId,
            comp.toStdString(),
            pos.toStdString(),
            descEdit->toPlainText().trimmed().toStdString(),
            startText,
            endText
        );

        if (m_candidate.addExperience(exp, m_data)) {
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", "Đã thêm kinh nghiệm làm việc mới vào hồ sơ thành công!");
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi kinh nghiệm vào candidate_experience.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(addBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onEditSkillClicked(int index) {
    ProfessionalInfo prof = m_candidate.getProfessionalInfo();
    if (index < 0 || index >= static_cast<int>(prof.skills.size())) {
        return;
    }
    const Skill &currentSkill = prof.skills[index];

    QDialog dialog(this);
    dialog.setWindowTitle("Chỉnh sửa kỹ năng chuyên môn");
    dialog.resize(460, 260);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("✏ Chỉnh sửa kỹ năng chuyên môn", &dialog);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Cập nhật lại tên kỹ năng và mức độ thành thạo tương ứng", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(10);

    QLineEdit *skillEdit = new QLineEdit(formCard);
    skillEdit->setText(QString::fromStdString(currentSkill.name));
    skillEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QComboBox *levelCombo = new QComboBox(formCard);
    levelCombo->addItem("1 - Mới bắt đầu (Beginner)", 1);
    levelCombo->addItem("2 - Cơ bản (Elementary)", 2);
    levelCombo->addItem("3 - Trung cấp (Intermediate)", 3);
    levelCombo->addItem("4 - Khá tốt (Advanced)", 4);
    levelCombo->addItem("5 - Chuyên gia (Expert)", 5);
    int targetIdx = currentSkill.level - 1;
    if (targetIdx >= 0 && targetIdx < levelCombo->count()) {
        levelCombo->setCurrentIndex(targetIdx);
    }
    levelCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    form->addRow("Tên kỹ năng *:", skillEdit);
    form->addRow("Mức độ thành thạo:", levelCombo);
    mainLayout->addWidget(formCard);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton("✓ Lưu thay đổi", &dialog);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 20px; font-weight: 700;");

    connect(saveBtn, &QPushButton::clicked, [&]() {
        QString sName = skillEdit->text().trimmed();
        if (sName.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Vui lòng nhập tên kỹ năng chuyên môn.");
            return;
        }

        Skill s;
        s.IDskill = 0;
        s.name = sName.toStdString();
        s.level = levelCombo->currentData().toInt();

        if (m_candidate.updateSkill(index, s, m_data)) {
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", QString("Đã cập nhật kỹ năng \"%1\" thành công!").arg(sName));
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi kỹ năng vào candidate_skill.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onDeleteSkillClicked(int index) {
    const auto &skills = m_candidate.getSkill();
    if (index < 0 || index >= static_cast<int>(skills.size())) {
        return;
    }
    QString skillName = QString::fromStdString(skills[index].name);

    if (CustomMessageBox::question(this, "Xác nhận xóa kỹ năng",
        QString("Bạn có chắc chắn muốn xóa kỹ năng \"%1\" khỏi hồ sơ năng lực?").arg(skillName),
        "Xóa kỹ năng", "Hủy bỏ")) {

        if (m_candidate.deleteSkill(index, m_data)) {
            populateDashboard();
            CustomMessageBox::success(this, "Thành công", QString("Đã xóa kỹ năng \"%1\" khỏi hồ sơ.").arg(skillName));
        } else {
            CustomMessageBox::critical(this, "Lỗi lưu trữ", "Không thể cập nhật candidate_skill.txt.");
        }
    }
}

void CandidatePage::onEditCertificateClicked(int index) {
    ProfessionalInfo prof = m_candidate.getProfessionalInfo();
    if (index < 0 || index >= static_cast<int>(prof.certificates.size())) {
        return;
    }
    const Certificate &curCert = prof.certificates[index];

    QDialog dialog(this);
    dialog.setWindowTitle("Chỉnh sửa chứng chỉ");
    dialog.resize(480, 300);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("✏ Chỉnh sửa chứng chỉ", &dialog);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Cập nhật lại loại chứng chỉ, tổ chức cấp và điểm số", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(10);

    QComboBox *typeCombo = new QComboBox(formCard);
    typeCombo->addItem("IELTS (Tiếng Anh)", 1);
    typeCombo->addItem("TOEIC (Tiếng Anh)", 2);
    typeCombo->addItem("TOEFL (Tiếng Anh)", 3);
    typeCombo->addItem("JLPT (Tiếng Nhật)", 4);
    typeCombo->addItem("AWS / Cloud Certification", 5);
    if (curCert.certificateTypeID == 6) {
        typeCombo->addItem("Chứng chỉ chuyên môn khác", 6);
    }
    int typeIdx = typeCombo->findData(curCert.certificateTypeID);
    if (typeIdx >= 0) typeCombo->setCurrentIndex(typeIdx);
    typeCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    QLineEdit *orgEdit = new QLineEdit(formCard);
    orgEdit->setText(QString::fromStdString(curCert.organization));
    orgEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QDoubleSpinBox *scoreSpin = new QDoubleSpinBox(formCard);
    scoreSpin->setRange(0.0, 1000.0);
    scoreSpin->setSingleStep(0.5);
    scoreSpin->setDecimals(curCert.certificateTypeID == 1 ? 1 : 0);
    scoreSpin->setValue(curCert.value);
    scoreSpin->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px;");

    auto configSpinForEdit = [scoreSpin](int typeId) {
        if (typeId == 1) { // IELTS: 7.0
            scoreSpin->setRange(1.0, 9.0);
            scoreSpin->setDecimals(1);
            scoreSpin->setSingleStep(0.5);
        } else if (typeId == 2) { // TOEIC: 700
            scoreSpin->setRange(100.0, 990.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(50.0);
        } else if (typeId == 3) { // TOEFL / HSK
            scoreSpin->setRange(1.0, 120.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(5.0);
        } else if (typeId == 4 || typeId == 5) {
            scoreSpin->setRange(1.0, 5.0);
            scoreSpin->setDecimals(0);
            scoreSpin->setSingleStep(1.0);
        } else {
            scoreSpin->setRange(0.0, 1000.0);
            scoreSpin->setDecimals(1);
            scoreSpin->setSingleStep(0.5);
        }
    };
    configSpinForEdit(curCert.certificateTypeID);
    QObject::connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [typeCombo, configSpinForEdit](int) {
        configSpinForEdit(typeCombo->currentData().toInt());
    });

    form->addRow("Loại chứng chỉ *:", typeCombo);
    form->addRow("Đơn vị / Tổ chức cấp *:", orgEdit);
    form->addRow("Điểm số / Cấp độ *:", scoreSpin);
    mainLayout->addWidget(formCard);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton("✓ Lưu thay đổi", &dialog);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 20px; font-weight: 700;");

    connect(saveBtn, &QPushButton::clicked, [&]() {
        QString org = orgEdit->text().trimmed();
        if (org.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Vui lòng nhập tên đơn vị hoặc tổ chức cấp chứng chỉ.");
            return;
        }

        Certificate cert = curCert;
        cert.certificateTypeID = typeCombo->currentData().toInt();
        cert.organization = org.toStdString();
        cert.value = scoreSpin->value();

        if (m_candidate.updateCertificate(index, cert, m_data)) {
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", "Đã cập nhật chứng chỉ thành công!");
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi chứng chỉ vào candidate_certificate.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onDeleteCertificateClicked(int index) {
    const auto &certs = m_candidate.getCertificate();
    if (index < 0 || index >= static_cast<int>(certs.size())) {
        return;
    }
    QString certName = QString::fromStdString(certs[index].organization);

    if (CustomMessageBox::question(this, "Xác nhận xóa chứng chỉ",
        QString("Bạn có chắc chắn muốn xóa chứng chỉ tại \"%1\" khỏi hồ sơ năng lực?").arg(certName),
        "Xóa chứng chỉ", "Hủy bỏ")) {

        if (m_candidate.deleteCertificate(index, m_data)) {
            populateDashboard();
            CustomMessageBox::success(this, "Thành công", QString("Đã xóa chứng chỉ khỏi hồ sơ."));
        } else {
            CustomMessageBox::critical(this, "Lỗi lưu trữ", "Không thể cập nhật candidate_certificate.txt.");
        }
    }
}

void CandidatePage::onEditExperienceClicked(int index) {
    std::vector<Experience> exps = m_candidate.getExperiences();
    if (index < 0 || index >= static_cast<int>(exps.size())) {
        return;
    }
    const Experience &currentExp = exps[index];

    QDialog dialog(this);
    dialog.setWindowTitle("Chỉnh sửa kinh nghiệm làm việc");
    dialog.resize(520, 420);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("💼 Chỉnh sửa kinh nghiệm làm việc", &dialog);
    title->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Cập nhật lại vị trí, công ty và khoảng thời gian làm việc", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 8px; padding: 12px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(8);

    QLineEdit *posEdit = new QLineEdit(formCard);
    posEdit->setText(QString::fromStdString(currentExp.getPosition()));
    posEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *compEdit = new QLineEdit(formCard);
    compEdit->setText(QString::fromStdString(currentExp.getCompanyName()));
    compEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QDate savedStart = QDate::fromString(
        QString::fromStdString(currentExp.getStartDate()),
        "yyyy-MM-dd"
    );

    QString savedEndText =
        QString::fromStdString(currentExp.getEndDate());

    bool isCurrent = (savedEndText == "Hiện tại");

    QDate savedEnd = isCurrent
        ? QDate::currentDate()
        : QDate::fromString(savedEndText, "yyyy-MM-dd");

    if (!savedStart.isValid() || !savedEnd.isValid()
        || savedStart > savedEnd
        || savedEnd > QDate::currentDate()) {
        CustomMessageBox::warning(
            this,
            "Dữ liệu ngày không hợp lệ",
            "Ngày kinh nghiệm đã lưu không hợp lệ.\n"
            "Hãy kiểm tra bản ghi tương ứng trong "
            "candidate_experience.txt.\n"
            "Định dạng ngày trong file phải là YYYY-MM-DD."
        );
        return;
    }

    QDateEdit *startEdit = new QDateEdit(formCard);
    startEdit->setCalendarPopup(true);
    startEdit->setDisplayFormat("dd/MM/yyyy");
    startEdit->setMaximumDate(QDate::currentDate());
    startEdit->setDate(savedStart);

    QDateEdit *endEdit = new QDateEdit(formCard);
    endEdit->setCalendarPopup(true);
    endEdit->setDisplayFormat("dd/MM/yyyy");
    endEdit->setMaximumDate(QDate::currentDate());
    endEdit->setDate(savedEnd);

    QCheckBox *currentCheck = new QCheckBox(
        "Đang làm việc tại đây", formCard
    );

    currentCheck->setChecked(isCurrent);
    endEdit->setEnabled(!isCurrent);

    connect(currentCheck, &QCheckBox::toggled,
            endEdit, [endEdit](bool checked) {
        endEdit->setEnabled(!checked);
    });

    QTextEdit *descEdit = new QTextEdit(formCard);
    descEdit->setText(QString::fromStdString(currentExp.getDescription()));
    descEdit->setMaximumHeight(75);
    descEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    form->addRow("Vị trí đảm nhiệm *:", posEdit);
    form->addRow("Tên công ty *:", compEdit);
    form->addRow("Thời gian bắt đầu *:", startEdit);
    form->addRow("Thời gian kết thúc:", endEdit);
    form->addRow("", currentCheck);
    form->addRow("Mô tả chi tiết:", descEdit);
    mainLayout->addWidget(formCard);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 600;");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    QPushButton *saveBtn = new QPushButton("✓ Lưu thay đổi", &dialog);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 20px; font-weight: 700;");

    connect(saveBtn, &QPushButton::clicked, [&]() {
        QString pos = posEdit->text().trimmed();
        QString comp = compEdit->text().trimmed();
        if (pos.isEmpty() || comp.isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Vui lòng nhập vị trí đảm nhiệm và tên công ty.");
            return;
        }
        QDate startDate = startEdit->date();

        QDate endDate = currentCheck->isChecked()
            ? QDate::currentDate()
            : endEdit->date();

        if (startDate > endDate) {
            CustomMessageBox::warning(
                &dialog,
                "Ngày không hợp lệ",
                "Ngày bắt đầu không được sau ngày kết thúc."
            );
            return;
        }

        string startText =
            startDate.toString("yyyy-MM-dd").toStdString();

        string endText = currentCheck->isChecked()
            ? string("Hiện tại")
            : endDate.toString("yyyy-MM-dd").toStdString();

        Experience updatedExp(
            currentExp.getIDexperience(),
            comp.toStdString(),
            pos.toStdString(),
            descEdit->toPlainText().trimmed().toStdString(),
            startText,
            endText
        );

        if (m_candidate.updateExperience(index, updatedExp, m_data)) {
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", "Đã cập nhật thông tin kinh nghiệm làm việc thành công!");
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi kinh nghiệm vào candidate_experience.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CandidatePage::onDeleteExperienceClicked(int index) {
    const auto &exps = m_candidate.getExperiences();
    if (index < 0 || index >= static_cast<int>(exps.size())) {
        return;
    }
    QString compName = QString::fromStdString(exps[index].getCompanyName());
    QString posName = QString::fromStdString(exps[index].getPosition());

    if (CustomMessageBox::question(this, "Xác nhận xóa kinh nghiệm",
        QString("Bạn có chắc chắn muốn xóa kinh nghiệm \"%1\" tại \"%2\" khỏi hồ sơ năng lực?").arg(posName).arg(compName),
        "Xóa kinh nghiệm", "Hủy bỏ")) {

        if (m_candidate.deleteExperience(index, m_data)) {
            populateDashboard();
            CustomMessageBox::success(this, "Thành công", "Đã xóa kinh nghiệm làm việc khỏi hồ sơ.");
        } else {
            CustomMessageBox::critical(this, "Lỗi lưu trữ", "Không thể cập nhật candidate_experience.txt.");
        }
    }
}

void CandidatePage::showAllJobsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Tất cả cơ hội việc làm — JobMatch");
    dialog.resize(880, 620);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    // Header
    QHBoxLayout *header = new QHBoxLayout();
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    QLabel *headerTitle = new QLabel("💼 Khám phá cơ hội việc làm", &dialog);
    headerTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    QLabel *headerSub = new QLabel("Tìm kiếm và lọc tin tuyển dụng theo vị trí, chuyên ngành và mức độ phù hợp", &dialog);
    headerSub->setStyleSheet("font-size: 12px; color: #6C757D;");
    titleLayout->addWidget(headerTitle);
    titleLayout->addWidget(headerSub);
    header->addLayout(titleLayout);
    header->addStretch();
    mainLayout->addLayout(header);

    // Filter Bar Card
    QFrame *filterCard = new QFrame(&dialog);
    filterCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 10px;");
    QHBoxLayout *filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setSpacing(10);
    filterLayout->setContentsMargins(8, 6, 8, 6);

    QLineEdit *searchEdit = new QLineEdit(filterCard);
    searchEdit->setPlaceholderText("🔍 Tìm theo vị trí, kỹ năng, công ty...");
    searchEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QComboBox *locCombo = new QComboBox(filterCard);
    locCombo->addItem("Tất cả địa điểm", "");
    locCombo->addItem("Đà Nẵng", "Đà Nẵng");
    locCombo->addItem("Hà Nội", "Hà Nội");
    locCombo->addItem("TP. Hồ Chí Minh", "Hồ Chí Minh");
    locCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px; font-size: 12px;");

    QComboBox *majorCombo = new QComboBox(filterCard);
    majorCombo->addItem("Tất cả chuyên ngành", "");
    majorCombo->addItem("Công nghệ thông tin", "Công nghệ thông tin");
    majorCombo->addItem("Kỹ thuật phần mềm", "Kỹ thuật phần mềm");
    majorCombo->addItem("Khoa học máy tính", "Khoa học máy tính");
    majorCombo->addItem("Hệ thống thông tin", "Hệ thống thông tin");
    majorCombo->addItem("Trí tuệ nhân tạo", "Trí tuệ nhân tạo");
    majorCombo->addItem("Mạng máy tính", "Mạng máy tính");
    majorCombo->addItem("An toàn thông tin", "An toàn thông tin");
    majorCombo->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 8px; font-size: 12px;");

    QCheckBox *onlyEligibleCheck = new QCheckBox("Chỉ việc làm đủ điều kiện", filterCard);
    onlyEligibleCheck->setStyleSheet("font-size: 12px; color: #4A4A4A; font-weight: 500;");

    filterLayout->addWidget(searchEdit, 2);
    filterLayout->addWidget(locCombo, 1);
    filterLayout->addWidget(majorCombo, 1);
    filterLayout->addWidget(onlyEligibleCheck);
    mainLayout->addWidget(filterCard);

    // Scroll Area for job cards
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *listContainer = new QWidget(scrollArea);
    listContainer->setStyleSheet("background-color: transparent;");
    QVBoxLayout *itemsLayout = new QVBoxLayout(listContainer);
    itemsLayout->setContentsMargins(0, 0, 0, 0);
    itemsLayout->setSpacing(12);

    auto renderList = [&]() {
        safeClearLayout(itemsLayout);

        JobSearchFilter filter;
        filter.keyword = searchEdit->text().trimmed().toStdString();
        filter.location = locCombo->currentData().toString().toStdString();
        filter.major = majorCombo->currentData().toString().toStdString();
        filter.onlyOpenJobs = true;

        std::vector<SearchResultItem> results = m_searchEngine.searchForCandidate(filter, m_candidate);

        int count = 0;
        for (const auto &item : results) {
            if (onlyEligibleCheck->isChecked() && !item.isEligible) {
                continue;
            }
            count++;
            const Job &job = item.job;
            Company comp = item.company;
            if (comp.getIDcompany() <= 0 && job.getIDcompany() > 0) {
                m_data.getCompanyByID(job.getIDcompany(), comp);
            }

            QFrame *card = new QFrame(listContainer);
            card->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 14px 18px;");
            QVBoxLayout *cLay = new QVBoxLayout(card);
            cLay->setSpacing(8);

            QHBoxLayout *top = new QHBoxLayout();
            top->setSpacing(12);
            top->setAlignment(Qt::AlignVCenter);
            QLabel *logo = new QLabel(card);
            logo->setAlignment(Qt::AlignCenter);
            logo->setFixedSize(44, 44);
            QString qCompName = QString::fromStdString(comp.getNameCompany());
            QString compInitials = StyleHelper::getInitials(qCompName, "DN");
            QString logoPath = StyleHelper::getCompanyLogoPath(comp.getIDcompany());
            logo->setPixmap(StyleHelper::getRoundedBadge(logoPath, compInitials, 44, 10));

            QVBoxLayout *tBox = new QVBoxLayout();
            tBox->setSpacing(2);
            QLabel *jTitle = new QLabel(QString::fromStdString(job.getTitle()), card);
            jTitle->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");
            QLabel *cName = new QLabel(QString::fromStdString(comp.getNameCompany()), card);
            cName->setStyleSheet("font-size: 12px; color: #B95334; font-weight: 600;");
            tBox->addWidget(jTitle);
            tBox->addWidget(cName);

            // Score badge
            QFrame *scBox = new QFrame(card);
            QVBoxLayout *scLay = new QVBoxLayout(scBox);
            scLay->setContentsMargins(6, 3, 6, 3);
            scLay->setSpacing(0);
            if (item.isEligible && item.matchScore >= 0.0) {
                bool meetsThreshold = item.matchScore >= 70.0;
            
                QString background = meetsThreshold ? "#E3F7EB" : "#FEF3E2";
                QString color = meetsThreshold ? "#1B7F49" : "#B76E00";
            
                scBox->setStyleSheet(
                    QString(
                        "background-color: %1;"
                        "border-radius: 6px;"
                        "padding: 4px 10px;"
                    ).arg(background)
                );
            
                QLabel *v = new QLabel(
                    QString::number(item.matchScore, 'f', 2) + "%",
                    scBox
                );
            
                v->setAlignment(Qt::AlignCenter);
                v->setStyleSheet(
                    QString(
                        "color: %1;"
                        "font-size: 14px;"
                        "font-weight: 800;"
                        "background: transparent;"
                    ).arg(color)
                );
            
                QLabel *t = new QLabel(
                    meetsThreshold ? "Phù hợp" : "Chưa đạt gợi ý",
                    scBox
                );
            
                t->setAlignment(Qt::AlignCenter);
                t->setStyleSheet(
                    QString(
                        "color: %1;"
                        "font-size: 9px;"
                        "font-weight: 600;"
                        "background: transparent;"
                    ).arg(color)
                );
            
                scLay->addWidget(v);
                scLay->addWidget(t);
            } else {
                scBox->setStyleSheet(
                    "background-color: #FEF3E2;"
                    "border-radius: 6px;"
                    "padding: 4px 8px;"
                );
            
                QLabel *v = new QLabel("Không đạt tiên quyết", scBox);
                v->setAlignment(Qt::AlignCenter);
                v->setStyleSheet(
                    "color: #B76E00;"
                    "font-size: 11px;"
                    "font-weight: 700;"
                    "background: transparent;"
                );
            
                scLay->addWidget(v);
            }

            top->addWidget(logo, 0, Qt::AlignVCenter);
            top->addLayout(tBox, 1);
            top->addWidget(scBox, 0, Qt::AlignVCenter);
            cLay->addLayout(top);

            if (!job.getDescription().empty()) {
                QLabel *jDesc = new QLabel(QString::fromStdString(job.getDescription()), card);
                jDesc->setStyleSheet("color: #4A4A4A; font-size: 12px; line-height: 1.3;");
                jDesc->setWordWrap(true);
                cLay->addWidget(jDesc);
            }

            // Info row
            QHBoxLayout *infoRow = new QHBoxLayout();
            QLabel *loc = new QLabel("📍 " + QString::fromStdString(job.getLocation()), card);
            loc->setStyleSheet("color: #6C757D; font-size: 11px;");
            QLabel *exp = new QLabel(job.getMinExperience() > 0 ? QString("📊 %1 tháng KN").arg(job.getMinExperience()) : "📊 Không yêu cầu KN", card);
            exp->setStyleSheet("color: #6C757D; font-size: 11px;");
            infoRow->addWidget(loc);
            infoRow->addWidget(exp);
            infoRow->addStretch();

            QPushButton *viewBtn = new QPushButton("Xem chi tiết →", card);
            viewBtn->setCursor(Qt::PointingHandCursor);
            viewBtn->setStyleSheet("background-color: transparent; color: #B95334; border: 1px solid #B95334; border-radius: 6px; padding: 4px 12px; font-size: 11px; font-weight: 600;");
            int jId = job.getIDjob();
            connect(viewBtn, &QPushButton::clicked, [this, jId, &dialog]() {
                dialog.accept();
                emit viewJobRequested(jId);
            });
            infoRow->addWidget(viewBtn);
            cLay->addLayout(infoRow);

            itemsLayout->addWidget(card);
        }

        if (count == 0) {
            QLabel *empty = new QLabel("Không tìm thấy tin tuyển dụng nào phù hợp với bộ lọc.", listContainer);
            empty->setStyleSheet("color: #6C757D; font-size: 13px; padding: 40px;");
            empty->setAlignment(Qt::AlignCenter);
            itemsLayout->addWidget(empty);
        }
        itemsLayout->addStretch();
    };

    connect(searchEdit, &QLineEdit::textChanged, [&]() { renderList(); });
    connect(locCombo, &QComboBox::currentIndexChanged, [&]() { renderList(); });
    connect(majorCombo, &QComboBox::currentIndexChanged, [&]() { renderList(); });
    connect(onlyEligibleCheck, &QCheckBox::toggled, [&]() { renderList(); });

    renderList();

    scrollArea->setWidget(listContainer);
    mainLayout->addWidget(scrollArea, 1);

    // Footer
    QHBoxLayout *footer = new QHBoxLayout();
    footer->addStretch();
    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 7px 22px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    footer->addWidget(closeBtn);
    mainLayout->addLayout(footer);

    dialog.exec();
}
