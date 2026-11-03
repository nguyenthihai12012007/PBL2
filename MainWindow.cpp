#include "MainWindow.h"
#include "CandidatePage.h"
#include "CompanyPage.h"
#include "JobPage.h"
#include "StyleHelper.h"
#include "Matching.h"
#include "CustomMessageBox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QDialog>
#include <QScrollArea>
#include <QFormLayout>
#include <QTextEdit>
#include <QDate>
#include <cmath>
#include <cctype>
#include <sstream>

MainWindow::MainWindow(Data &data, QWidget *parent)
    : QMainWindow(parent), m_data(data), currentUserId(0), previousPageIndex(0) {
    m_searchEngine.reload(m_data);
    setWindowTitle("JobMatch C++ — Hệ thống Kết nối Tuyển dụng C++");
    resize(1260, 780);
    setMinimumSize(1024, 640);
    setupUI();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // =========================================================================
    // 1. SIDEBAR BÊN TRÁI: Màu cam đất Terracotta (#B95334)
    // =========================================================================
    QFrame *sidebar = new QFrame(centralWidget);
    sidebar->setObjectName("sidebarFrame");
    sidebar->setFixedWidth(240);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(18, 24, 18, 20);
    sidebarLayout->setSpacing(18);

    // Brand Logo & Header
    QHBoxLayout *brandHeader = new QHBoxLayout();
    brandHeader->setSpacing(10);

    QLabel *codeIcon = new QLabel("</>", sidebar);
    codeIcon->setStyleSheet("background-color: rgba(255, 255, 255, 0.2); color: #FFFFFF; font-size: 15px; font-weight: 800; border-radius: 8px; padding: 6px 8px;");

    QVBoxLayout *brandTextLayout = new QVBoxLayout();
    brandTextLayout->setSpacing(2);
    sidebarBrandTitle = new QLabel("JobMatch C++", sidebar);
    sidebarBrandTitle->setObjectName("sidebarTitle");
    sidebarSubtitle = new QLabel("Kết nối tài năng với cơ hội", sidebar);
    sidebarSubtitle->setObjectName("sidebarSubtitle");
    sidebarSubtitle->setWordWrap(true);
    brandTextLayout->addWidget(sidebarBrandTitle);
    brandTextLayout->addWidget(sidebarSubtitle);

    brandHeader->addWidget(codeIcon);
    brandHeader->addLayout(brandTextLayout);
    sidebarLayout->addLayout(brandHeader);

    // Navigation buttons container
    navButtonsLayout = new QVBoxLayout();
    navButtonsLayout->setSpacing(6);
    sidebarLayout->addLayout(navButtonsLayout);

    sidebarLayout->addStretch();

    // Nút Đăng xuất ở chân Sidebar
    QPushButton *logoutBtn = new QPushButton("🚪 Đăng xuất", sidebar);
    logoutBtn->setObjectName("logoutButton");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    sidebarLayout->addWidget(logoutBtn);

    rootLayout->addWidget(sidebar);

    // =========================================================================
    // 2. VÙNG NỘI DUNG PHẢI (Top Bar + Stacked Pages)
    // =========================================================================
    QWidget *rightContainer = new QWidget(centralWidget);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // TOP BAR
    QFrame *topBar = new QFrame(rightContainer);
    topBar->setObjectName("topBarFrame");
    topBar->setFixedHeight(64);

    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(28, 12, 28, 12);
    topBarLayout->setSpacing(16);

    // Thanh tìm kiếm Search Pill
    searchEdit = new QLineEdit(topBar);
    searchEdit->setObjectName("topSearchBar");
    searchEdit->setPlaceholderText("🔍  Tìm kiếm việc làm, công ty, kỹ năng...");
    searchEdit->setFixedWidth(380);
    topBarLayout->addWidget(searchEdit);

    topBarLayout->addStretch();

    // Icon Chuông thông báo
    QPushButton *bellBtn = new QPushButton("🔔", topBar);
    bellBtn->setFixedSize(36, 36);
    bellBtn->setCursor(Qt::PointingHandCursor);
    bellBtn->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 18px; font-size: 14px;");
    connect(bellBtn, &QPushButton::clicked, this, &MainWindow::showNotificationsDialog);
    topBarLayout->addWidget(bellBtn);

    // Cụm User Profile ở góc phải
    QHBoxLayout *userProfileLayout = new QHBoxLayout();
    userProfileLayout->setSpacing(10);

    avatarCircleLabel = new QLabel("ND", topBar);
    avatarCircleLabel->setObjectName("userAvatarCircle");
    avatarCircleLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *userNameLayout = new QVBoxLayout();
    userNameLayout->setSpacing(1);
    topUserNameLabel = new QLabel("Nguyễn Văn An ▾", topBar);
    topUserNameLabel->setObjectName("userNameTopLabel");
    topUserRoleLabel = new QLabel("Ứng viên", topBar);
    topUserRoleLabel->setObjectName("userRoleTopLabel");
    userNameLayout->addWidget(topUserNameLabel);
    userNameLayout->addWidget(topUserRoleLabel);

    userProfileLayout->addWidget(avatarCircleLabel);
    userProfileLayout->addLayout(userNameLayout);
    topBarLayout->addLayout(userProfileLayout);

    rightLayout->addWidget(topBar);

    // STACKED CONTENT AREA
    stackedWidget = new QStackedWidget(rightContainer);
    candidatePage = new CandidatePage(m_data, m_searchEngine, stackedWidget);
    companyPage = new CompanyPage(m_data, m_searchEngine, stackedWidget);
    jobPage = new JobPage(stackedWidget);

    stackedWidget->addWidget(candidatePage); // Index 0
    stackedWidget->addWidget(companyPage);   // Index 1
    stackedWidget->addWidget(jobPage);       // Index 2

    rightLayout->addWidget(stackedWidget);
    rootLayout->addWidget(rightContainer, 1);

    // Event Connections
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(candidatePage, &CandidatePage::viewJobRequested, this, &MainWindow::onShowJobDetail);
    connect(candidatePage, &CandidatePage::viewAllApplicationsRequested, this, &MainWindow::showCandidateApplicationsDialog);
    connect(companyPage, &CompanyPage::viewJobRequested, this, &MainWindow::onShowJobDetail);
    connect(jobPage, &JobPage::backRequested, this, &MainWindow::onBackFromJobPage);
    connect(jobPage, &JobPage::applyRequested, this, &MainWindow::onApplyJobRequested);
}

void MainWindow::setSession(int userId, const QString &email, const QString &role) {
    currentUserId = userId;
    currentEmail = email;
    currentRole = role;

    QString displayName;
    QString initials = "US";

    if (role == "COMPANY") {
        Company comp;
        if (m_data.getCompanyByUserID(userId, comp)) {
            displayName = QString::fromStdString(comp.getNameCompany());
        }
        if (displayName.trimmed().isEmpty()) displayName = "VietTech Solutions";
        initials = "VT";
        if (displayName.length() >= 2) {
            QStringList p = displayName.split(' ', Qt::SkipEmptyParts);
            if (p.size() >= 2) initials = p.first().left(1).toUpper() + p.last().left(1).toUpper();
            else initials = displayName.left(2).toUpper();
        }

        topUserNameLabel->setText(displayName + " ▾");
        topUserRoleLabel->setText("Nhà tuyển dụng");
        sidebarSubtitle->setText("Kết nối nhà tuyển dụng với nhân tài");
        searchEdit->setPlaceholderText("🔍  Tìm kiếm ứng viên, kỹ năng, vị trí...");

        companyPage->setSession(userId);
        stackedWidget->setCurrentIndex(1);
    } else {
        // CANDIDATE
        Candidate cand;
        if (m_data.getCandidateByUserID(userId, cand)) {
            displayName = QString::fromStdString(cand.getPersonalInfo().fullName);
        }
        if (displayName.trimmed().isEmpty()) displayName = "Nguyễn Văn An";
        initials = "ND";
        if (displayName.length() >= 2) {
            QStringList p = displayName.split(' ', Qt::SkipEmptyParts);
            if (p.size() >= 2) initials = p.first().left(1).toUpper() + p.last().left(1).toUpper();
            else initials = displayName.left(2).toUpper();
        }

        topUserNameLabel->setText(displayName + " ▾");
        topUserRoleLabel->setText("Ứng viên");
        sidebarSubtitle->setText("Kết nối tài năng với cơ hội");
        searchEdit->setPlaceholderText("🔍  Tìm kiếm việc làm, công ty, kỹ năng...");

        candidatePage->setSession(userId);
        stackedWidget->setCurrentIndex(0);
    }

    avatarCircleLabel->setText(initials);
    updateNavigationForRole(role);
}

void MainWindow::updateNavigationForRole(const QString &role) {
    // Xóa các nút cũ
    for (QPushButton *btn : navButtons) {
        navButtonsLayout->removeWidget(btn);
        btn->deleteLater();
    }
    navButtons.clear();

    auto createNavBtn = [this](const QString &text, bool active = false) -> QPushButton* {
        QPushButton *btn = new QPushButton(text, this);
        btn->setProperty("class", active ? "navButton active" : "navButton");
        btn->setCursor(Qt::PointingHandCursor);
        if (active) {
            btn->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700;");
        } else {
            btn->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500;");
        }
        navButtons.append(btn);
        navButtonsLayout->addWidget(btn);
        return btn;
    };

    if (role == "COMPANY") {
        QPushButton *btnHome = createNavBtn("🏠  Trang chủ", true);
        QPushButton *btnProfile = createNavBtn("🏢  Hồ sơ công ty", false);
        QPushButton *btnJobs = createNavBtn("💼  Quản lý việc làm", false);
        QPushButton *btnApps = createNavBtn("📄  Hồ sơ ứng tuyển", false);

        // Divider
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("background-color: rgba(255, 255, 255, 0.2); margin: 6px 0;");
        navButtonsLayout->addWidget(line);

        QPushButton *btnNotif = createNavBtn("🔔  Thông báo (5)", false);
        QPushButton *btnSettings = createNavBtn("⚙️  Cài đặt", false);

        connect(btnHome, &QPushButton::clicked, this, [this, btnHome]() {
            for (auto *b : navButtons) b->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500;");
            btnHome->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700;");
            stackedWidget->setCurrentIndex(1);
        });
        connect(btnProfile, &QPushButton::clicked, this, [this]() {
            showCompanyProfileDialog();
        });
        connect(btnJobs, &QPushButton::clicked, this, [this, btnJobs]() {
            for (auto *b : navButtons) b->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500;");
            btnJobs->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700;");
            stackedWidget->setCurrentIndex(1);
        });
        connect(btnApps, &QPushButton::clicked, this, [this]() {
            companyPage->showApplicantsDialog(0, "Tất cả vị trí tuyển dụng");
        });
        connect(btnNotif, &QPushButton::clicked, this, [this]() {
            showNotificationsDialog();
        });
        connect(btnSettings, &QPushButton::clicked, this, [this]() {
            showSettingsDialog();
        });
    } else {
        // CANDIDATE
        QPushButton *btnHome = createNavBtn("🏠  Trang chủ", true);
        QPushButton *btnProfile = createNavBtn("👤  Hồ sơ của tôi", false);
        QPushButton *btnJobs = createNavBtn("💼  Việc làm phù hợp", false);
        QPushButton *btnApps = createNavBtn("📄  Đơn ứng tuyển", false);

        // Divider
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("background-color: rgba(255, 255, 255, 0.2); margin: 6px 0;");
        navButtonsLayout->addWidget(line);

        QPushButton *btnNotif = createNavBtn("🔔  Thông báo (3)", false);
        QPushButton *btnSettings = createNavBtn("⚙️  Cài đặt", false);

        connect(btnHome, &QPushButton::clicked, this, [this, btnHome]() {
            for (auto *b : navButtons) b->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500;");
            btnHome->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700;");
            stackedWidget->setCurrentIndex(0);
        });
        connect(btnProfile, &QPushButton::clicked, this, [this]() {
            showCandidateProfileDialog();
        });
        connect(btnJobs, &QPushButton::clicked, this, [this, btnJobs]() {
            for (auto *b : navButtons) b->setStyleSheet("background-color: transparent; color: #FFFFFF; font-weight: 500;");
            btnJobs->setStyleSheet("background-color: #FFF4ED; color: #B95334; font-weight: 700;");
            stackedWidget->setCurrentIndex(0);
        });
        connect(btnApps, &QPushButton::clicked, this, [this]() {
            showCandidateApplicationsDialog();
        });
        connect(btnNotif, &QPushButton::clicked, this, [this]() {
            showNotificationsDialog();
        });
        connect(btnSettings, &QPushButton::clicked, this, [this]() {
            showSettingsDialog();
        });
    }
}

void MainWindow::onSearchTextChanged(const QString &text) {
    if (stackedWidget->currentIndex() == 0) {
        candidatePage->filterJobs(text);
    } else if (stackedWidget->currentIndex() == 1) {
        companyPage->filterContent(text);
    }
}

void MainWindow::onShowJobDetail(int jobId) {
    Job job;
    if (!m_data.getJobByID(jobId, job)) {
        CustomMessageBox::warning(this, "Không tìm thấy công việc", "Không thể tải thông tin công việc từ cơ sở dữ liệu.");
        return;
    }

    Company company;
    if (!m_data.getCompanyByID(job.getIDcompany(), company)) {
        company.setNameCompany("Doanh nghiệp");
    }

    previousPageIndex = stackedWidget->currentIndex();

    // Tính điểm phù hợp nếu phiên hiện tại là Candidate
    double matchScore = -1.0;
    Candidate cand;
    bool hasValidCandidate = false;
    if (currentRole == "CANDIDATE") {
        if (m_data.getCandidateByUserID(currentUserId, cand) && cand.getIDcandidate() > 0) {
            m_data.loadCandidateSkill(cand);
            m_data.loadCandidateCertificate(cand);
            m_data.loadCandidateExperience(cand);
            matchScore = calculatorTotal(cand, job);
            hasValidCandidate = true;
        }
    }

    // BƯỚC 1: Gọi loadJob TRƯỚC để nạp thông tin công việc và điểm matching vào giao diện gốc
    jobPage->loadJob(job, company, matchScore);

    // BƯỚC 2: Thiết lập trạng thái nút nộp đơn SAU ĐÓ để không bị loadJob ghi đè
    if (!job.getStatus() || job.isExpired()) {
        jobPage->setApplyState(true, "Đã đóng tuyển dụng");
    } else if (currentRole != "CANDIDATE") {
        jobPage->setApplyState(true, "Chỉ dành cho ứng viên");
    } else {
        if (!hasValidCandidate) {
            // Không tìm thấy hồ sơ Candidate hợp lệ: vô hiệu hóa nút và hiển thị trạng thái phù hợp
            jobPage->setApplyState(true, "Chưa hoàn thiện hồ sơ");
        } else {
            int candId = cand.getIDcandidate();
            Application existingApp;
            if (m_searchEngine.hasCandidateApplied(candId, jobId, existingApp)) {
                QString appliedStatusText = "Đang xem xét";
                std::string st = existingApp.getStatus();
                if (st == "ACCEPTED") appliedStatusText = "Đã chấp nhận";
                else if (st == "REJECTED") appliedStatusText = "Đã từ chối";
                jobPage->setApplyState(true, appliedStatusText);
            } else {
                jobPage->setApplyState(false);
            }
        }
    }

    stackedWidget->setCurrentIndex(2);
}

void MainWindow::onApplyJobRequested(int jobId) {
    if (currentRole != "CANDIDATE") {
        CustomMessageBox::warning(this, "Thông báo", "Chức năng nộp đơn ứng tuyển chỉ dành cho tài khoản Ứng viên.");
        return;
    }

    ApplyResult result = m_searchEngine.applyJob(currentUserId, jobId, m_data);

    switch (result.status) {
    case ApplyStatus::SUCCESS: {
        CustomMessageBox::success(
            this,
            "Ứng tuyển thành công",
            QString("Hồ sơ của bạn đã được ghi nhận và gửi thành công đến nhà tuyển dụng!\n\n"
                    "• Mã đơn ứng tuyển: #%1\n"
                    "• Vị trí: %2\n"
                    "• Điểm phù hợp tính toán: %3%\n"
                    "• Trạng thái: Đang xem xét\n"
                    "• Ngày nộp: %4\n\n"
                    "Dữ liệu đã được lưu chính xác vào applications.txt.")
            .arg(result.application.getIDappli())
            .arg(QString::fromStdString(result.job.getTitle()))
            .arg(QString::number(result.matchScore, 'f', 1))
            .arg(QString::fromStdString(result.application.getAppliedAt()))
        );

        jobPage->setApplyState(true, "Đang xem xét");
        candidatePage->setSession(currentUserId);
        break;
    }
    case ApplyStatus::CANDIDATE_NOT_FOUND:
        CustomMessageBox::critical(
            this,
            "Lỗi hồ sơ ứng viên",
            "Không tìm thấy hồ sơ ứng viên hợp lệ liên kết với tài khoản này.\n"
            "Vui lòng tạo hoặc cập nhật hồ sơ trước khi nộp đơn ứng tuyển."
        );
        break;
    case ApplyStatus::JOB_NOT_FOUND:
        CustomMessageBox::warning(this, "Lỗi dữ liệu", "Không thể xác định thông tin công việc để nộp đơn.");
        break;
    case ApplyStatus::JOB_CLOSED_OR_EXPIRED:
        CustomMessageBox::warning(this, "Thông báo", "Tin tuyển dụng này đã đóng hoặc đã hết hạn nộp hồ sơ.");
        jobPage->setApplyState(true, "Đã đóng tuyển dụng");
        break;
    case ApplyStatus::ALREADY_APPLIED: {
        QString vnStatus = "Đang xem xét";
        std::string st = result.application.getStatus();
        if (st == "ACCEPTED") vnStatus = "Đã chấp nhận";
        else if (st == "REJECTED") vnStatus = "Đã từ chối";

        CustomMessageBox::information(
            this,
            "Thông báo",
            QString("Bạn đã nộp đơn ứng tuyển vào vị trí này trước đó rồi.\nTrạng thái hiện tại: %1").arg(vnStatus)
        );
        jobPage->setApplyState(true, vnStatus);
        break;
    }
    case ApplyStatus::CALCULATION_ERROR:
        CustomMessageBox::warning(
            this,
            "Không thể tính điểm phù hợp",
            "Hệ thống chưa thể tính điểm phù hợp cho vị trí này. "
            "Đơn ứng tuyển chưa được tạo."
        );
        break;
    case ApplyStatus::NOT_ELIGIBLE:
        CustomMessageBox::warning(
            this,
            "Không đủ điều kiện ứng tuyển",
            "Hồ sơ của bạn không đáp ứng điều kiện tiên quyết của vị trí này "
            "(chuyên ngành không phù hợp hoặc thiếu chứng chỉ bắt buộc theo quy định)."
        );
        break;
    case ApplyStatus::SAVE_FAILED:
        CustomMessageBox::critical(this, "Lỗi lưu trữ", "Không thể lưu đơn ứng tuyển vào cơ sở dữ liệu applications.txt.");
        break;
    }
}

void MainWindow::onBackFromJobPage() {
    stackedWidget->setCurrentIndex(previousPageIndex);
}

void MainWindow::onLogoutClicked() {
    if (CustomMessageBox::question(
        this,
        "Đăng xuất",
        "Bạn có chắc chắn muốn đăng xuất khỏi phiên làm việc hiện tại?",
        "Đăng xuất",
        "Hủy bỏ"
    )) {
        emit logoutRequested();
    }
}

void MainWindow::showCandidateProfileDialog() {
    Candidate cand;
    if (!m_data.getCandidateByUserID(currentUserId, cand) || cand.getIDcandidate() <= 0) {
        CustomMessageBox::warning(this, "Thông báo", "Chưa tìm thấy thông tin hồ sơ ứng viên.");
        return;
    }
    m_data.loadCandidateSkill(cand);
    m_data.loadCandidateCertificate(cand);
    m_data.loadCandidateExperience(cand);

    const auto &p = cand.getPersonalInfo();
    const auto &prof = cand.getProfessionalInfo();
    QString fullName = QString::fromStdString(p.fullName.empty() ? "Ứng viên" : p.fullName);

    QDialog dialog(this);
    dialog.setWindowTitle("Hồ sơ năng lực — " + fullName);
    dialog.resize(650, 560);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    // Header Card
    QFrame *hCard = new QFrame(&dialog);
    hCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QHBoxLayout *hLayout = new QHBoxLayout(hCard);
    hLayout->setSpacing(16);

    QString initials = "UV";
    QStringList parts = fullName.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2) initials = parts.first().left(1).toUpper() + parts.last().left(1).toUpper();
    else if (!parts.isEmpty()) initials = parts.first().left(2).toUpper();

    QLabel *avatar = new QLabel(initials, hCard);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(54, 54);
    avatar->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 800; font-size: 18px; border-radius: 27px;");

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(3);
    QLabel *nameLbl = new QLabel(fullName, hCard);
    nameLbl->setStyleSheet("font-size: 18px; font-weight: 800; color: #1E1E1E;");
    QLabel *majorLbl = new QLabel("Chuyên ngành: " + QString::fromStdString(prof.major.empty() ? "Chưa cập nhật" : prof.major), hCard);
    majorLbl->setStyleSheet("font-size: 13px; font-weight: 600; color: #B95334;");
    infoLayout->addWidget(nameLbl);
    infoLayout->addWidget(majorLbl);

    hLayout->addWidget(avatar);
    hLayout->addLayout(infoLayout, 1);
    mainLayout->addWidget(hCard);

    // Scroll details
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *content = new QWidget(scrollArea);
    content->setStyleSheet("background-color: transparent;");
    QVBoxLayout *cLayout = new QVBoxLayout(content);
    cLayout->setContentsMargins(0, 0, 0, 0);
    cLayout->setSpacing(12);

    // Thẻ thông tin hợp nhất, loại bỏ các ô trắng phân mục rời rạc chiếm diện tích
    QFrame *detailsCard = new QFrame(content);
    detailsCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *dLayout = new QVBoxLayout(detailsCard);
    dLayout->setSpacing(12);

    auto makeSection = [detailsCard](QVBoxLayout *parentLayout, const QString &title, const QString &icon, bool addDivider) -> QVBoxLayout* {
        if (addDivider) {
            QFrame *line = new QFrame(detailsCard);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("background-color: #F0EAE4; max-height: 1px; border: none; margin: 4px 0px;");
            parentLayout->addWidget(line);
        }
        QVBoxLayout *secLay = new QVBoxLayout();
        secLay->setSpacing(6);
        QLabel *l = new QLabel(icon + " " + title, detailsCard);
        l->setStyleSheet("font-size: 13px; font-weight: 700; color: #1E1E1E;");
        secLay->addWidget(l);
        parentLayout->addLayout(secLay);
        return secLay;
    };

    // 1. Thông tin cá nhân
    QVBoxLayout *pLay = makeSection(dLayout, "Thông tin cá nhân & Liên hệ", "👤", false);
    QLabel *contactInfo = new QLabel(QString(
        "• Email: %1\n• Điện thoại: %2\n• Ngày sinh: %3 (%4)\n• Địa chỉ: %5"
    ).arg(currentEmail)
     .arg(QString::fromStdString(p.phoneCandidate))
     .arg(QString::fromStdString(p.birthday))
     .arg(QString::fromStdString(p.gender))
     .arg(QString::fromStdString(p.addressCandidate)), detailsCard);
    contactInfo->setStyleSheet("font-size: 12px; line-height: 1.5; color: #333333;");
    pLay->addWidget(contactInfo);

    // 2. Học vấn & GPA
    QVBoxLayout *eduLay = makeSection(dLayout, "Học vấn & Trình độ", "🎓", true);
    QLabel *eduInfo = new QLabel(QString(
        "• Trường đào tạo: %1\n• Chuyên ngành: %2\n• Trình độ: %3\n• Điểm GPA: %4 / 4.00"
    ).arg(QString::fromStdString(prof.school))
     .arg(QString::fromStdString(prof.major))
     .arg(QString::fromStdString(prof.educationLevel))
     .arg(QString::number(prof.GPA, 'f', 2)), detailsCard);
    eduInfo->setStyleSheet("font-size: 12px; line-height: 1.5; color: #333333;");
    eduLay->addWidget(eduInfo);

    // 3. Kỹ năng
    const auto &skills = cand.getSkill();
    if (!skills.empty()) {
        QVBoxLayout *skLay = makeSection(dLayout, "Kỹ năng chuyên môn", "⚡", true);
        QHBoxLayout *tagsLay = new QHBoxLayout();
        tagsLay->setSpacing(8);
        for (const auto &sk : skills) {
            QLabel *tag = new QLabel(QString::fromStdString(sk.name) + " (Lv " + QString::number(sk.level) + ")", detailsCard);
            tag->setStyleSheet("background-color: #FCE8E2; color: #B95334; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 8px;");
            tagsLay->addWidget(tag);
        }
        tagsLay->addStretch();
        skLay->addLayout(tagsLay);
    }

    // 4. Chứng chỉ chuyên ngành & Ngoại ngữ
    const auto &certs = cand.getCertificate();
    if (!certs.empty()) {
        QVBoxLayout *certLay = makeSection(dLayout, "Chứng chỉ chuyên ngành & Ngoại ngữ", "📜", true);
        for (const auto &c : certs) {
            QString typeName = "Chứng chỉ";
            if (c.certificateTypeID == 1) typeName = "IELTS";
            else if (c.certificateTypeID == 2) typeName = "TOEIC";
            else if (c.certificateTypeID == 3) typeName = "TOEFL / HSK";
            else if (c.certificateTypeID == 4) typeName = "JLPT (Tiếng Nhật)";
            else if (c.certificateTypeID == 5) typeName = "Cloud / AWS";
            else typeName = "Chuyên ngành";

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

            QString orgStr = c.organization.empty() ? "Tổ chức uy tín" : QString::fromStdString(c.organization);
            QString cText = QString("• %1: %2 (%3)").arg(typeName).arg(valStr).arg(orgStr);
            QLabel *cLbl = new QLabel(cText, detailsCard);
            cLbl->setStyleSheet("font-size: 12px; color: #333333;");
            certLay->addWidget(cLbl);
        }
    }

    // 5. Kinh nghiệm làm việc
    const auto &exps = cand.getExperiences();
    if (!exps.empty()) {
        QVBoxLayout *expLay = makeSection(dLayout, "Kinh nghiệm làm việc", "💼", true);
        for (const auto &e : exps) {
            QVBoxLayout *eBox = new QVBoxLayout();
            eBox->setSpacing(2);
            QLabel *eLbl = new QLabel(QString("• %1 @ %2 (%3 → %4)")
                .arg(QString::fromStdString(e.getPosition()))
                .arg(QString::fromStdString(e.getCompanyName()))
                .arg(QString::fromStdString(e.getStartDate()))
                .arg(QString::fromStdString(e.getEndDate())), detailsCard);
            eLbl->setStyleSheet("font-size: 12px; font-weight: 600; color: #1E1E1E;");
            eBox->addWidget(eLbl);
            if (!e.getDescription().empty()) {
                QLabel *descLbl = new QLabel(QString::fromStdString(e.getDescription()), detailsCard);
                descLbl->setStyleSheet("font-size: 11px; color: #555555; padding-left: 12px;");
                descLbl->setWordWrap(true);
                eBox->addWidget(descLbl);
            }
            expLay->addLayout(eBox);
        }
    }

    cLayout->addWidget(detailsCard);

    scrollArea->setWidget(content);
    mainLayout->addWidget(scrollArea, 1);

    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->setSpacing(10);
    botLay->addStretch();

    QPushButton *editBtn = new QPushButton("✏ Chỉnh sửa hồ sơ", &dialog);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setStyleSheet("background-color: transparent; color: #B95334; border: 1px solid #B95334; font-weight: 600; border-radius: 6px; padding: 7px 18px;");
    connect(editBtn, &QPushButton::clicked, [&]() {
        dialog.accept();
        if (candidatePage) {
            candidatePage->onEditProfileClicked();
        }
    });

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    botLay->addWidget(editBtn);
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showCandidateApplicationsDialog() {
    Candidate cand;
    if (!m_data.getCandidateByUserID(currentUserId, cand) || cand.getIDcandidate() <= 0) {
        CustomMessageBox::warning(this, "Thông báo", "Chưa tìm thấy thông tin hồ sơ ứng viên.");
        return;
    }
    int candId = cand.getIDcandidate();

    std::vector<Application> apps = m_searchEngine.getApplicationsForCandidate(candId);

    QDialog dialog(this);
    dialog.setWindowTitle("Đơn ứng tuyển của tôi");
    dialog.resize(750, 520);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    QLabel *titleLbl = new QLabel("📄 Danh sách đơn ứng tuyển đã nộp", &dialog);
    titleLbl->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    QLabel *subLbl = new QLabel("Theo dõi tiến độ và kết quả xét duyệt từ các nhà tuyển dụng", &dialog);
    subLbl->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(titleLbl);
    mainLayout->addWidget(subLbl);

    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *content = new QWidget(scrollArea);
    content->setStyleSheet("background-color: transparent;");
    QVBoxLayout *cLayout = new QVBoxLayout(content);
    cLayout->setContentsMargins(0, 0, 0, 0);
    cLayout->setSpacing(10);

    for (const auto &app : apps) {
        int jobId = app.getIDjob();
        Job job;
        std::string jTitle = "Vị trí tuyển dụng #" + std::to_string(jobId);
        std::string compName = "Doanh nghiệp";
        if (m_data.getJobByID(jobId, job)) {
            jTitle = job.getTitle();
            Company comp;
            if (m_data.getCompanyByID(job.getIDcompany(), comp)) {
                compName = comp.getNameCompany();
            }
        }

        QFrame *card = new QFrame(content);
        card->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px 16px;");
        QHBoxLayout *row = new QHBoxLayout(card);
        row->setSpacing(14);

        QVBoxLayout *tBox = new QVBoxLayout();
        tBox->setSpacing(2);
        QLabel *jLbl = new QLabel(QString::fromStdString(jTitle), card);
        jLbl->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");
        QLabel *cNameLbl = new QLabel("🏢 " + QString::fromStdString(compName) + "  |  Ngày nộp: " + QString::fromStdString(app.getAppliedAt()), card);
        cNameLbl->setStyleSheet("font-size: 11px; color: #6C757D;");
        tBox->addWidget(jLbl);
        tBox->addWidget(cNameLbl);

        QLabel *scoreBadge = new QLabel(QString("Điểm: %1%").arg(QString::number(app.getAppropriateLevel(), 'f', 1)), card);
        scoreBadge->setStyleSheet("color: #B95334; font-weight: 700; font-size: 13px;");

        QLabel *stBadge = new QLabel(card);
        std::string statusStr = app.getStatus();
        if (statusStr == "ACCEPTED") {
            stBadge->setText("Đã chấp nhận");
            stBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-weight: 600; font-size: 11px; border-radius: 6px; padding: 4px 10px;");
        } else if (statusStr == "REJECTED") {
            stBadge->setText("Đã từ chối");
            stBadge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-weight: 600; font-size: 11px; border-radius: 6px; padding: 4px 10px;");
        } else {
            stBadge->setText("Đang xem xét");
            stBadge->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-weight: 600; font-size: 11px; border-radius: 6px; padding: 4px 10px;");
        }

        QPushButton *viewJobBtn = new QPushButton("Xem tin", card);
        viewJobBtn->setCursor(Qt::PointingHandCursor);
        viewJobBtn->setStyleSheet("background-color: transparent; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 4px 10px; font-size: 11px; font-weight: 600;");
        connect(viewJobBtn, &QPushButton::clicked, this, [this, jobId, &dialog]() {
            dialog.accept();
            onShowJobDetail(jobId);
        });

        row->addLayout(tBox, 1);
        row->addWidget(scoreBadge);
        row->addWidget(stBadge);
        row->addWidget(viewJobBtn);
        cLayout->addWidget(card);
    }

    if (apps.empty()) {
        QLabel *emptyLbl = new QLabel("Bạn chưa nộp đơn ứng tuyển vào vị trí nào.", content);
        emptyLbl->setStyleSheet("color: #6C757D; font-size: 13px; padding: 40px;");
        emptyLbl->setAlignment(Qt::AlignCenter);
        cLayout->addWidget(emptyLbl);
    }
    cLayout->addStretch();

    scrollArea->setWidget(content);
    mainLayout->addWidget(scrollArea, 1);

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->addStretch();
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showCompanyProfileDialog() {
    Company comp;
    if (!m_data.getCompanyByUserID(currentUserId, comp)) {
        CustomMessageBox::warning(this, "Thông báo", "Chưa tìm thấy thông tin công ty.");
        return;
    }

    std::vector<Job> jobs;
    m_data.getJobsByCompanyID(comp.getIDcompany(), jobs);
    std::vector<Application> apps;
    m_data.getApplicationsByCompanyID(comp.getIDcompany(), apps);

    QDialog dialog(this);
    dialog.setWindowTitle("Thông tin doanh nghiệp — " + QString::fromStdString(comp.getNameCompany()));
    dialog.resize(600, 480);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Header Card
    QFrame *hCard = new QFrame(&dialog);
    hCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QHBoxLayout *hLayout = new QHBoxLayout(hCard);
    hLayout->setSpacing(16);

    QLabel *logo = new QLabel(QString::fromStdString(comp.getNameCompany().empty() ? "VT" : comp.getNameCompany().substr(0, 1)), hCard);
    logo->setAlignment(Qt::AlignCenter);
    logo->setFixedSize(54, 54);
    logo->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 800; font-size: 20px; border-radius: 10px;");

    QVBoxLayout *tBox = new QVBoxLayout();
    tBox->setSpacing(3);
    QLabel *nameLbl = new QLabel(QString::fromStdString(comp.getNameCompany()), hCard);
    nameLbl->setStyleSheet("font-size: 18px; font-weight: 800; color: #1E1E1E;");
    QLabel *webLbl = new QLabel("🌐 " + QString::fromStdString(comp.getWebsite().empty() ? "https://abctech.vn" : comp.getWebsite()), hCard);
    webLbl->setStyleSheet("font-size: 12px; color: #B95334;");
    tBox->addWidget(nameLbl);
    tBox->addWidget(webLbl);

    hLayout->addWidget(logo);
    hLayout->addLayout(tBox, 1);
    mainLayout->addWidget(hCard);

    // Stats
    QHBoxLayout *statsLay = new QHBoxLayout();
    statsLay->setSpacing(12);

    auto makeStatCard = [&dialog](const QString &num, const QString &text) -> QFrame* {
        QFrame *f = new QFrame(&dialog);
        f->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px;");
        QVBoxLayout *l = new QVBoxLayout(f);
        l->setSpacing(2);
        l->setAlignment(Qt::AlignCenter);
        QLabel *n = new QLabel(num, f);
        n->setAlignment(Qt::AlignCenter);
        n->setStyleSheet("font-size: 20px; font-weight: 800; color: #B95334;");
        QLabel *t = new QLabel(text, f);
        t->setAlignment(Qt::AlignCenter);
        t->setStyleSheet("font-size: 11px; color: #6C757D;");
        l->addWidget(n);
        l->addWidget(t);
        return f;
    };

    statsLay->addWidget(makeStatCard(QString::number(jobs.size()), "Tin tuyển dụng đang mở"));
    statsLay->addWidget(makeStatCard(QString::number(apps.size()), "Lượt ứng tuyển đã nhận"));
    mainLayout->addLayout(statsLay);

    // Info details Card
    QFrame *infoCard = new QFrame(&dialog);
    infoCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *infoLay = new QVBoxLayout(infoCard);
    infoLay->setSpacing(8);

    QLabel *detTitle = new QLabel("🏢 Chi tiết doanh nghiệp", infoCard);
    detTitle->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");
    infoLay->addWidget(detTitle);

    QLabel *detContent = new QLabel(QString(
        "• Địa chỉ trụ sở: %1\n• Điện thoại liên hệ: %2\n• Email quản trị: %3\n• Giới thiệu: %4"
    ).arg(QString::fromStdString(comp.getAddressCompany()))
     .arg(QString::fromStdString(comp.getPhoneCompany()))
     .arg(currentEmail)
     .arg(QString::fromStdString(comp.getDescription())), infoCard);
    detContent->setStyleSheet("font-size: 12px; line-height: 1.6; color: #333333;");
    detContent->setWordWrap(true);
    infoLay->addWidget(detContent);
    mainLayout->addWidget(infoCard, 1);

    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->setSpacing(10);
    botLay->addStretch();

    QPushButton *editCompBtn = new QPushButton("✏ Chỉnh sửa thông tin", &dialog);
    editCompBtn->setCursor(Qt::PointingHandCursor);
    editCompBtn->setStyleSheet("background-color: transparent; color: #B95334; border: 1px solid #B95334; font-weight: 600; border-radius: 6px; padding: 7px 18px;");
    connect(editCompBtn, &QPushButton::clicked, [&]() {
        dialog.accept();
        if (companyPage) {
            companyPage->onEditCompanyProfileClicked();
        }
    });

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    botLay->addWidget(editCompBtn);
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showNotificationsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Thông báo hệ thống");
    dialog.resize(520, 380);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    QLabel *titleLbl = new QLabel("🔔 Trung tâm thông báo", &dialog);
    titleLbl->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    mainLayout->addWidget(titleLbl);

    QFrame *nCard1 = new QFrame(&dialog);
    nCard1->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px;");
    QVBoxLayout *l1 = new QVBoxLayout(nCard1);
    QLabel *t1 = new QLabel("⚡ Chào mừng đến với JobMatch C++", nCard1);
    t1->setStyleSheet("font-weight: 700; color: #1E1E1E; font-size: 13px;");
    QLabel *d1 = new QLabel("Hệ thống kết nối tuyển dụng và tìm kiếm việc làm bằng C++ & Qt Widgets đã sẵn sàng.", nCard1);
    d1->setStyleSheet("color: #6C757D; font-size: 12px;");
    l1->addWidget(t1);
    l1->addWidget(d1);
    mainLayout->addWidget(nCard1);

    QFrame *nCard2 = new QFrame(&dialog);
    nCard2->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px;");
    QVBoxLayout *l2 = new QVBoxLayout(nCard2);
    QLabel *t2 = new QLabel("✅ Dữ liệu thời gian thực đồng bộ", nCard2);
    t2->setStyleSheet("font-weight: 700; color: #1B7F49; font-size: 13px;");
    QLabel *d2 = new QLabel(QString("Phiên làm việc hiện tại: %1 (%2)").arg(currentEmail).arg(currentRole == "COMPANY" ? "Doanh nghiệp" : "Ứng viên"), nCard2);
    d2->setStyleSheet("color: #6C757D; font-size: 12px;");
    l2->addWidget(t2);
    l2->addWidget(d2);
    mainLayout->addWidget(nCard2);

    mainLayout->addStretch();

    QPushButton *closeBtn = new QPushButton("Đã hiểu", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 20px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->addStretch();
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}

void MainWindow::showSettingsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Cài đặt & Thông tin hệ thống");
    dialog.resize(520, 420);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    QLabel *titleLbl = new QLabel("⚙️ Cài đặt & Thông tin PBL2", &dialog);
    titleLbl->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    mainLayout->addWidget(titleLbl);

    QFrame *card = new QFrame(&dialog);
    card->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *l = new QVBoxLayout(card);
    l->setSpacing(8);

    QLabel *projTitle = new QLabel("JobMatch C++ — Hệ thống Tuyển dụng & Kết nối Việc làm", card);
    projTitle->setStyleSheet("font-weight: 800; font-size: 14px; color: #B95334;");
    QLabel *info = new QLabel(
        "• Học phần: Đồ án Cơ sở Lập trình (PBL2)\n"
        "• Khoa: Công nghệ Thông tin — Trường ĐH Bách Khoa, ĐHĐN\n"
        "• Ngôn ngữ: C++17 (Kiến trúc chuẩn OOP & Modular)\n"
        "• Giao diện: Qt 6 Widgets (Terracotta Design System)\n"
        "• Phiên bản: 1.0.0 (Release Candidate)\n"
        "• Cơ sở dữ liệu: Hệ thống tệp tin phẳng (.txt Database)", card);
    info->setStyleSheet("font-size: 12px; line-height: 1.6; color: #333333;");
    l->addWidget(projTitle);
    l->addWidget(info);
    mainLayout->addWidget(card);

    mainLayout->addStretch();

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 8px 24px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QHBoxLayout *botLay = new QHBoxLayout();
    botLay->addStretch();
    botLay->addWidget(closeBtn);
    mainLayout->addLayout(botLay);

    dialog.exec();
}
