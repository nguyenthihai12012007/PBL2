#include "CompanyPage.h"
#include "CustomMessageBox.h"
#include "StyleHelper.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QDateEdit>
#include <QTabWidget>
#include <QListWidget>
#include <QComboBox>
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

CompanyPage::CompanyPage(Data &data, JobSearchEngine &searchEngine, QWidget *parent)
    : QWidget(parent), m_data(data), m_searchEngine(searchEngine) {
    setupUI();
}

void CompanyPage::setupUI() {
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background-color: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(28, 20, 28, 28);
    contentLayout->setSpacing(20);

    // Profile Banner Header Doanh nghiệp
    companyHeaderLayout = new QVBoxLayout();
    companyHeaderLayout->setSpacing(0);
    contentLayout->addLayout(companyHeaderLayout);

    // Main 2-Column Dashboard Layout
    QHBoxLayout *dashboardLayout = new QHBoxLayout();
    dashboardLayout->setSpacing(24);
    dashboardLayout->setAlignment(Qt::AlignTop);

    // =========================================================================
    // LEFT COLUMN (~65%): Danh sách việc làm đang tuyển
    // =========================================================================
    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->setSpacing(16);

    // Header
    QHBoxLayout *secHeaderLayout = new QHBoxLayout();
    secHeaderLayout->setSpacing(10);

    QLabel *iconBadge = new QLabel("💼", scrollContent);
    iconBadge->setStyleSheet("background-color: #FCE8E2; font-size: 14px; border-radius: 6px; padding: 6px;");

    QVBoxLayout *titleTextLayout = new QVBoxLayout();
    titleTextLayout->setSpacing(2);
    QLabel *secTitle = new QLabel("Danh sách việc làm đang tuyển", scrollContent);
    secTitle->setStyleSheet("font-size: 17px; font-weight: 700; color: #1E1E1E;");
    QLabel *secSub = new QLabel("Quản lý các tin tuyển dụng của công ty", scrollContent);
    secSub->setStyleSheet("font-size: 12px; color: #6C757D;");
    titleTextLayout->addWidget(secTitle);
    titleTextLayout->addWidget(secSub);

    QPushButton *createJobBtn = new QPushButton("+ Đăng tin tuyển dụng", scrollContent);
    createJobBtn->setCursor(Qt::PointingHandCursor);
    createJobBtn->setStyleSheet(
        "background-color: #B95334; color: #FFFFFF; font-size: 13px; font-weight: 600; "
        "border: none; border-radius: 8px; padding: 8px 18px;"
    );
    connect(createJobBtn, &QPushButton::clicked, this, &CompanyPage::onCreateJobClicked);

    secHeaderLayout->addWidget(iconBadge);
    secHeaderLayout->addLayout(titleTextLayout);
    secHeaderLayout->addStretch();
    secHeaderLayout->addWidget(createJobBtn);
    leftCol->addLayout(secHeaderLayout);

    // Jobs container
    jobsListLayout = new QVBoxLayout();
    jobsListLayout->setSpacing(14);
    leftCol->addLayout(jobsListLayout);
    leftCol->addStretch();

    dashboardLayout->addLayout(leftCol, 65);

    // =========================================================================
    // RIGHT COLUMN (~35%): Hồ sơ ứng tuyển mới nhất
    // =========================================================================
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(16);

    QFrame *applicantsCard = new QFrame(scrollContent);
    applicantsCard->setObjectName("cardBox");
    applicantsCard->setStyleSheet(
        "#cardBox {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 12px;"
        "   padding: 18px;"
        "}"
    );
    latestApplicantsLayout = new QVBoxLayout(applicantsCard);
    latestApplicantsLayout->setSpacing(14);
    rightCol->addWidget(applicantsCard);
    rightCol->addStretch();

    dashboardLayout->addLayout(rightCol, 35);

    contentLayout->addLayout(dashboardLayout);
    scrollArea->setWidget(scrollContent);
    rootLayout->addWidget(scrollArea);
}

void CompanyPage::setSession(int userId) {
    m_userId = userId;

    // 1. Tải thông tin doanh nghiệp từ cơ sở dữ liệu thật
    if (!m_data.getCompanyByUserID(userId, m_company)) {
        // Fallback tạo đối tượng mới với ID tự sinh và liên kết IDuser chính xác
        m_company = Company();
        int newId = m_data.getNextID("companies.txt");
        m_company.setIDcompany(newId);
        m_company.setIDuser(userId);
        m_company.setRole("COMPANY");
    }

    // 2. Tải danh sách công việc của công ty
    m_jobs.clear();
    m_data.getJobsByCompanyID(m_company.getIDcompany(), m_jobs);

    // 3. Tải các đơn ứng tuyển vào các công việc của công ty từ applications.txt
    m_applications.clear();
    m_data.getApplicationsByCompanyID(m_company.getIDcompany(), m_applications);

    populateDashboard();
}

void CompanyPage::populateDashboard() {
    buildCompanyProfileHeader();
    buildJobsList();
    buildLatestApplicantsCard();
}

void CompanyPage::buildCompanyProfileHeader() {
    safeClearLayout(companyHeaderLayout);

    QFrame *card = new QFrame(this);
    card->setObjectName("companyProfileCard");
    card->setStyleSheet(
        "#companyProfileCard {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 12px;"
        "   padding: 16px 20px;"
        "}"
    );

    QHBoxLayout *mainLay = new QHBoxLayout(card);
    mainLay->setSpacing(18);
    mainLay->setAlignment(Qt::AlignVCenter);

    // 1. Logo
    std::string compName = m_company.getNameCompany();
    if (compName.empty()) compName = "Công ty";
    QString qName = QString::fromStdString(compName);
    QString initials = StyleHelper::getInitials(qName, "CT");

    companyLogoLabel = new QLabel(card);
    companyLogoLabel->setAlignment(Qt::AlignCenter);
    companyLogoLabel->setFixedSize(54, 54);
    companyLogoLabel->setCursor(Qt::PointingHandCursor);
    companyLogoLabel->setToolTip("Nhấn để đổi logo công ty");
    companyLogoLabel->installEventFilter(this);
    QString logoPath = StyleHelper::getCompanyLogoPath(m_company.getIDcompany());
    companyLogoLabel->setPixmap(StyleHelper::getRoundedBadge(logoPath, initials, 54, 12));

    QVBoxLayout *logoCol = new QVBoxLayout();
    logoCol->setSpacing(4);
    logoCol->setAlignment(Qt::AlignCenter);
    logoCol->addWidget(companyLogoLabel, 0, Qt::AlignCenter);

    QPushButton *changeLogoBtn = new QPushButton("📷 Đổi logo", card);
    changeLogoBtn->setCursor(Qt::PointingHandCursor);
    changeLogoBtn->setStyleSheet("background-color: transparent; color: #B95334; font-size: 11px; font-weight: 600; border: none; padding: 2px;");
    connect(changeLogoBtn, &QPushButton::clicked, this, &CompanyPage::onChangeLogoClicked);
    logoCol->addWidget(changeLogoBtn, 0, Qt::AlignCenter);

    // 2. Info column
    QVBoxLayout *infoCol = new QVBoxLayout();
    infoCol->setSpacing(6);

    QLabel *nameLbl = new QLabel(qName, card);
    nameLbl->setStyleSheet("font-size: 18px; font-weight: 800; color: #1E1E1E;");

    QHBoxLayout *metaRow = new QHBoxLayout();
    metaRow->setSpacing(16);

    QString web = QString::fromStdString(m_company.getWebsite().empty() ? "Chưa có website" : m_company.getWebsite());
    QLabel *webLbl = new QLabel("🌐 " + web, card);
    webLbl->setStyleSheet("font-size: 12px; color: #B95334; font-weight: 500;");

    QString addr = QString::fromStdString(m_company.getAddressCompany().empty() ? "Chưa có địa chỉ" : m_company.getAddressCompany());
    QLabel *addrLbl = new QLabel("📍 " + addr, card);
    addrLbl->setStyleSheet("font-size: 12px; color: #6C757D;");

    QString phone = QString::fromStdString(m_company.getPhoneCompany().empty() ? "Chưa có SĐT" : m_company.getPhoneCompany());
    QLabel *phoneLbl = new QLabel("📞 " + phone, card);
    phoneLbl->setStyleSheet("font-size: 12px; color: #6C757D;");

    metaRow->addWidget(webLbl);
    metaRow->addWidget(addrLbl);
    metaRow->addWidget(phoneLbl);
    metaRow->addStretch();

    QString desc = QString::fromStdString(m_company.getDescription());
    if (desc.isEmpty()) desc = "Doanh nghiệp công nghệ phát triển giải pháp phần mềm.";
    QLabel *descLbl = new QLabel(desc, card);
    descLbl->setWordWrap(true);
    descLbl->setStyleSheet("font-size: 13px; color: #555555; line-height: 1.4;");

    infoCol->addWidget(nameLbl);
    infoCol->addLayout(metaRow);
    infoCol->addWidget(descLbl);

    // 3. Stats & Action Buttons
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(8);
    rightCol->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QPushButton *editCompBtn = new QPushButton("✏ Chỉnh sửa thông tin", card);
    editCompBtn->setCursor(Qt::PointingHandCursor);
    editCompBtn->setStyleSheet(
        "background-color: transparent; color: #B95334; border: 1px solid #B95334; "
        "border-radius: 6px; padding: 6px 14px; font-size: 12px; font-weight: 600;"
    );
    connect(editCompBtn, &QPushButton::clicked, this, &CompanyPage::onEditCompanyProfileClicked);

    QHBoxLayout *statBadges = new QHBoxLayout();
    statBadges->setSpacing(8);

    QLabel *jobCountBadge = new QLabel(QString("💼 %1 tin đang tuyển").arg(m_jobs.size()), card);
    jobCountBadge->setStyleSheet("background-color: #F8F9FA; color: #4A4A4A; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 8px; border: 1px solid #ECE5DE;");

    QLabel *appCountBadge = new QLabel(QString("📄 %1 đơn ứng tuyển").arg(m_applications.size()), card);
    appCountBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 8px;");

    statBadges->addWidget(jobCountBadge);
    statBadges->addWidget(appCountBadge);

    rightCol->addLayout(statBadges);
    rightCol->addWidget(editCompBtn, 0, Qt::AlignRight);

    mainLay->addLayout(logoCol);
    mainLay->addLayout(infoCol, 1);
    mainLay->addLayout(rightCol);

    companyHeaderLayout->addWidget(card);
}

void CompanyPage::onEditCompanyProfileClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Chỉnh sửa thông tin doanh nghiệp");
    dialog.resize(560, 480);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    QLabel *title = new QLabel("🏢 Chỉnh sửa thông tin doanh nghiệp", &dialog);
    title->setStyleSheet("font-size: 17px; font-weight: 700; color: #1E1E1E;");
    QLabel *sub = new QLabel("Thông tin công ty sẽ được hiển thị công khai trên các tin tuyển dụng", &dialog);
    sub->setStyleSheet("font-size: 12px; color: #6C757D;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);

    QFrame *formCard = new QFrame(&dialog);
    formCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 14px;");
    QFormLayout *form = new QFormLayout(formCard);
    form->setSpacing(10);

    QLineEdit *nameEdit = new QLineEdit(QString::fromStdString(m_company.getNameCompany()), formCard);
    nameEdit->setPlaceholderText("VD: Tập đoàn Công nghệ VietTech");
    nameEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *phoneEdit = new QLineEdit(QString::fromStdString(m_company.getPhoneCompany()), formCard);
    phoneEdit->setPlaceholderText("VD: 0243.888.9999");
    phoneEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *addrEdit = new QLineEdit(QString::fromStdString(m_company.getAddressCompany()), formCard);
    addrEdit->setPlaceholderText("VD: Tòa nhà FPT, Cầu Giấy, Hà Nội");
    addrEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QLineEdit *webEdit = new QLineEdit(QString::fromStdString(m_company.getWebsite()), formCard);
    webEdit->setPlaceholderText("VD: https://viettech.vn");
    webEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    QTextEdit *descEdit = new QTextEdit(formCard);
    descEdit->setPlaceholderText("Mô tả tổng quan về doanh nghiệp, môi trường làm việc, quy mô và văn hóa công ty...");
    descEdit->setText(QString::fromStdString(m_company.getDescription()));
    descEdit->setMinimumHeight(100);
    descEdit->setStyleSheet("background-color: #FBF8F5; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 10px;");

    form->addRow("Tên doanh nghiệp *:", nameEdit);
    form->addRow("Số điện thoại liên hệ *:", phoneEdit);
    form->addRow("Địa chỉ trụ sở *:", addrEdit);
    form->addRow("Website chính thức:", webEdit);
    form->addRow("Giới thiệu doanh nghiệp:", descEdit);
    mainLayout->addWidget(formCard);

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
            CustomMessageBox::warning(&dialog, "Thiếu dữ liệu", "Tên doanh nghiệp không được để trống.");
            return;
        }

        if (m_company.updateCompany(newName.toStdString(),
                                    phoneEdit->text().trimmed().toStdString(),
                                    addrEdit->text().trimmed().toStdString(),
                                    webEdit->text().trimmed().toStdString(),
                                    descEdit->toPlainText().trimmed().toStdString(),
                                    m_data)) {
            m_searchEngine.reload(m_data);
            populateDashboard();
            dialog.accept();
            CustomMessageBox::success(this, "Cập nhật thành công", "Thông tin doanh nghiệp đã được cập nhật chính xác vào companies.txt!");
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi lưu trữ", "Không thể ghi thông tin doanh nghiệp vào companies.txt.");
        }
    });

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}

void CompanyPage::buildJobsList() {
    safeClearLayout(jobsListLayout);

    // Tìm kiếm và lọc tin tuyển dụng bằng module C++ nghiệp vụ JobSearchEngine
    std::vector<Job> companyJobs = m_searchEngine.searchForCompany(
        m_company.getIDcompany(),
        m_filterKeyword.trimmed().toStdString()
    );

    if (companyJobs.empty()) {
        QLabel *emptyLabel = new QLabel("Không có tin tuyển dụng nào phù hợp.", this);
        emptyLabel->setStyleSheet("color: #6C757D; font-size: 13px; padding: 30px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        jobsListLayout->addWidget(emptyLabel);
        return;
    }

    for (const auto &job : companyJobs) {
        // Đếm số lượng ứng viên qua bảng băm trong JobSearchEngine (O(1))
        int applicantCount = m_searchEngine.countApplicantsForJob(job.getIDjob());

        QFrame *card = new QFrame(this);
        card->setObjectName("cardBox");
        card->setStyleSheet(
            "#cardBox {"
            "   background-color: #FFFFFF;"
            "   border: 1px solid #ECE5DE;"
            "   border-radius: 12px;"
            "   padding: 16px 20px;"
            "}"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(10);

        // Hàng 1: Logo + Thông tin vị trí + Khối ứng viên / Thao tác
        QHBoxLayout *topRow = new QHBoxLayout();
        topRow->setSpacing(16);
        topRow->setAlignment(Qt::AlignVCenter);

        // Logo thương hiệu
        QLabel *logoLabel = new QLabel(card);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setFixedSize(46, 46);
        QString logoPath = StyleHelper::getCompanyLogoPath(m_company.getIDcompany());
        QString compInitials = StyleHelper::getInitials(QString::fromStdString(m_company.getNameCompany()), "DN");
        logoLabel->setPixmap(StyleHelper::getRoundedBadge(logoPath, compInitials, 46, 10));
        topRow->addWidget(logoLabel, 0, Qt::AlignVCenter);

        // Thông tin công việc (Giữa)
        QVBoxLayout *infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(4);

        QHBoxLayout *titleLine = new QHBoxLayout();
        titleLine->setSpacing(10);

        QLabel *titleLabel = new QLabel(QString::fromStdString(job.getTitle()), card);
        titleLabel->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");

        QLabel *statusBadge = new QLabel(job.getStatus() ? "Đang tuyển" : "Đã đóng", card);
        if (job.getStatus()) {
            statusBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-size: 11px; font-weight: 600; border-radius: 4px; padding: 2px 7px;");
        } else {
            statusBadge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 600; border-radius: 4px; padding: 2px 7px;");
        }

        titleLine->addWidget(titleLabel);
        titleLine->addWidget(statusBadge);
        titleLine->addStretch();
        infoLayout->addLayout(titleLine);

        // Metadata
        QHBoxLayout *metaLine = new QHBoxLayout();
        metaLine->setSpacing(14);
        QString loc = QString::fromStdString(job.getLocation().empty() ? "Hà Nội" : job.getLocation());
        QLabel *locLbl = new QLabel("📍 " + loc, card);
        locLbl->setStyleSheet("color: #6C757D; font-size: 12px;");

        QLabel *typeLbl = new QLabel("💼 Toàn thời gian", card);
        typeLbl->setStyleSheet("color: #6C757D; font-size: 12px;");

        int exp = job.getMinExperience();
        QString expStr = exp > 0 ? QString("%1 - %2 năm kinh nghiệm").arg(exp).arg(exp + 2) : "Mới tốt nghiệp";
        QLabel *expLbl = new QLabel("📊 " + expStr, card);
        expLbl->setStyleSheet("color: #6C757D; font-size: 12px;");

        metaLine->addWidget(locLbl);
        metaLine->addWidget(typeLbl);
        metaLine->addWidget(expLbl);
        metaLine->addStretch();
        infoLayout->addLayout(metaLine);

        topRow->addLayout(infoLayout, 1);

        // Khối Ứng viên & Nút thao tác (Bên phải)
        QHBoxLayout *actionLayout = new QHBoxLayout();
        actionLayout->setSpacing(16);

        // Số lượng ứng viên
        QVBoxLayout *countLayout = new QVBoxLayout();
        countLayout->setSpacing(0);
        countLayout->setAlignment(Qt::AlignCenter);

        QLabel *countNum = new QLabel(QString::number(applicantCount), card);
        countNum->setAlignment(Qt::AlignCenter);
        countNum->setStyleSheet("font-size: 18px; font-weight: 800; color: #B95334;");
        QLabel *countText = new QLabel("ứng viên", card);
        countText->setAlignment(Qt::AlignCenter);
        countText->setStyleSheet("font-size: 11px; color: #6C757D;");
        countLayout->addWidget(countNum);
        countLayout->addWidget(countText);
        actionLayout->addLayout(countLayout);

        // Nút Xem ứng viên
        QPushButton *viewCandidatesBtn = new QPushButton("👥 Xem ứng viên", card);
        viewCandidatesBtn->setCursor(Qt::PointingHandCursor);
        viewCandidatesBtn->setStyleSheet(
            "background-color: transparent; color: #B95334; border: 1px solid #B95334; "
            "border-radius: 8px; padding: 6px 14px; font-size: 12px; font-weight: 600;"
        );
        int jobId = job.getIDjob();
        QString jobTitle = QString::fromStdString(job.getTitle());
        connect(viewCandidatesBtn, &QPushButton::clicked, this, [this, jobId, jobTitle]() {
            showApplicantsDialog(jobId, jobTitle);
        });
        actionLayout->addWidget(viewCandidatesBtn);

        // Nút menu 3 chấm ⋮
        QPushButton *menuBtn = new QPushButton("⋮", card);
        menuBtn->setCursor(Qt::PointingHandCursor);
        menuBtn->setFixedSize(28, 28);
        menuBtn->setStyleSheet("color: #6C757D; font-size: 16px; font-weight: 700; background: transparent; border: none;");

        connect(menuBtn, &QPushButton::clicked, this, [this, jobId, menuBtn]() {
            QMenu contextMenu(this);
            QAction *viewAction = contextMenu.addAction("Xem chi tiết việc làm");
            QAction *editAction = contextMenu.addAction("✏ Chỉnh sửa tin tuyển dụng");
            QAction *closeAction = contextMenu.addAction("Đóng tuyển dụng");
            QAction *deleteAction = contextMenu.addAction("Xóa tin này");

            QAction *selected = contextMenu.exec(menuBtn->mapToGlobal(QPoint(0, menuBtn->height())));
            if (selected == viewAction) {
                emit viewJobRequested(jobId);
            } else if (selected == editAction) {
                onEditJobClicked(jobId);
            } else if (selected == closeAction) {
                m_company.closeJob(jobId, m_data);
                m_searchEngine.reload(m_data);
                setSession(m_userId);
                CustomMessageBox::success(this, "Thành công", "Đã đóng tin tuyển dụng thành công!");
            } else if (selected == deleteAction) {
                if (CustomMessageBox::question(this, "Xác nhận xóa", "Bạn có chắc chắn muốn xóa tin tuyển dụng này khỏi cơ sở dữ liệu?", "Xóa tin", "Hủy bỏ")) {
                    m_company.deleteJob(jobId, m_data);
                    m_searchEngine.reload(m_data);
                    setSession(m_userId);
                }
            }
        });
        actionLayout->addWidget(menuBtn);

        topRow->addLayout(actionLayout);
        cardLayout->addLayout(topRow);

        // Hàng 2: Ghi chú / Mô tả công việc (Toàn chiều rộng thẻ, không bị che mất hay cắt chữ)
        QString desc = QString::fromStdString(job.getDescription());
        if (!desc.isEmpty()) {
            QLabel *descLbl = new QLabel(desc, card);
            descLbl->setStyleSheet("color: #555555; font-size: 12px; line-height: 1.4;");
            descLbl->setWordWrap(true);
            cardLayout->addWidget(descLbl);
        }

        jobsListLayout->addWidget(card);
    }
}

void CompanyPage::buildLatestApplicantsCard() {
    safeClearLayout(latestApplicantsLayout);

    QHBoxLayout *header = new QHBoxLayout();
    QLabel *title = new QLabel("📄 Hồ sơ ứng tuyển mới nhất", this);
    title->setStyleSheet("font-size: 15px; font-weight: 700; color: #1E1E1E;");

    QPushButton *viewAllBtn = new QPushButton("Xem tất cả →", this);
    viewAllBtn->setCursor(Qt::PointingHandCursor);
    viewAllBtn->setStyleSheet("color: #B95334; font-size: 12px; font-weight: 600; background: transparent; border: none;");
    connect(viewAllBtn, &QPushButton::clicked, this, [this]() {
        showApplicantsDialog(0, "Tất cả vị trí tuyển dụng");
    });

    header->addWidget(title);
    header->addStretch();
    header->addWidget(viewAllBtn);
    latestApplicantsLayout->addLayout(header);

    if (m_applications.empty()) {
        QLabel *emptyLabel = new QLabel("Chưa có hồ sơ ứng tuyển nào.", this);
        emptyLabel->setStyleSheet("color: #6C757D; font-size: 12px; padding: 20px;");
        emptyLabel->setAlignment(Qt::AlignCenter);
        latestApplicantsLayout->addWidget(emptyLabel);
        return;
    }

    // Hiển thị danh sách ứng viên thực tế từ applications.txt
    int count = 0;
    for (const auto &app : m_applications) {
        if (++count > 6) break;

        Candidate cand;
        std::string candName = "Ứng viên #" + std::to_string(app.getIDcandidate());
        if (m_data.getCandidateByID(app.getIDcandidate(), cand)) {
            candName = cand.getPersonalInfo().fullName;
        }

        Job job;
        std::string jobTitle = "Vị trí tuyển dụng";
        if (m_data.getJobByID(app.getIDjob(), job)) {
            jobTitle = job.getTitle();
        }

        QHBoxLayout *row = new QHBoxLayout();
        row->setSpacing(10);
        row->setAlignment(Qt::AlignVCenter);

        // Avatar viết tắt
        QString qCandName = QString::fromStdString(candName).trimmed();
        QString initials = StyleHelper::getInitials(qCandName, "UV");

        QLabel *avatar = new QLabel(this);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setFixedSize(36, 36);
        int candId = cand.getIDcandidate() > 0 ? cand.getIDcandidate() : app.getIDcandidate();
        QString avPath = StyleHelper::getCandidateAvatarPath(candId);
        avatar->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 36));

        QVBoxLayout *textLayout = new QVBoxLayout();
        textLayout->setSpacing(2);
        textLayout->setAlignment(Qt::AlignVCenter);
        QLabel *nameLabel = new QLabel(qCandName, this);
        nameLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
        QLabel *subLabel = new QLabel("Ứng tuyển: " + QString::fromStdString(jobTitle), this);
        subLabel->setStyleSheet("font-size: 11px; color: #6C757D;");
        textLayout->addWidget(nameLabel);
        textLayout->addWidget(subLabel);

        QLabel *timeLabel = new QLabel(QString::fromStdString(app.getAppliedAt()), this);
        timeLabel->setStyleSheet("font-size: 11px; color: #6C757D;");

        QPushButton *viewProfileBtn = new QPushButton("Xem hồ sơ", this);
        viewProfileBtn->setCursor(Qt::PointingHandCursor);
        viewProfileBtn->setStyleSheet(
            "background-color: transparent; color: #B95334; border: 1px solid #B95334; "
            "border-radius: 6px; padding: 4px 10px; font-size: 11px; font-weight: 600;"
        );

        int cId = app.getIDcandidate();
        float score = app.getAppropriateLevel();
        QString qJobTitle = QString::fromStdString(jobTitle);
        int appId = app.getIDappli();
        QString currentStatus = QString::fromStdString(app.getStatus());
        int currJobId = app.getIDjob();
        connect(viewProfileBtn, &QPushButton::clicked, this, [this, cId, qJobTitle, score, appId, currentStatus, currJobId]() {
            viewCandidateDetails(cId, qJobTitle, score, appId, currentStatus, currJobId);
        });

        row->addWidget(avatar, 0, Qt::AlignVCenter);
        row->addLayout(textLayout, 1);
        row->addWidget(timeLabel, 0, Qt::AlignVCenter);
        row->addWidget(viewProfileBtn, 0, Qt::AlignVCenter);

        latestApplicantsLayout->addLayout(row);
    }
}

bool CompanyPage::processApplicationDecision(int appId, int jobId, const QString &candidateName, const QString &newStatus, QWidget *parentDialog) {
    // 1. Kiểm tra quyền sở hữu: Tin tuyển dụng phải thuộc doanh nghiệp hiện tại
    Job job;
    if (!m_data.getJobByID(jobId, job) || job.getIDcompany() != m_company.getIDcompany()) {
        CustomMessageBox::warning(parentDialog ? parentDialog : this, "Từ chối quyền truy cập",
            "Bạn không có quyền quản lý đơn ứng tuyển cho vị trí này.");
        return false;
    }

    // 2. Xác nhận hành động từ nhà tuyển dụng
    QString actionName = (newStatus == "ACCEPTED") ? "CHẤP NHẬN" : "TỪ CHỐI";
    QString confirmMsg = QString("Bạn có chắc chắn muốn %1 đơn ứng tuyển của ứng viên [%2] cho vị trí [%3]?")
        .arg(actionName)
        .arg(candidateName)
        .arg(QString::fromStdString(job.getTitle()));

    if (!CustomMessageBox::question(parentDialog ? parentDialog : this, "Xác nhận thao tác", confirmMsg, actionName, "Hủy bỏ")) {
        return false;
    }

    // 3. Cập nhật trạng thái thông qua API nghiệp vụ
    if (m_data.updateApplicationStatus(appId, newStatus.toStdString())) {
        m_searchEngine.updateApplicationStatusInIndex(appId, newStatus.toStdString());
        QString successMsg = (newStatus == "ACCEPTED")
            ? QString("Đã chấp nhận đơn ứng tuyển của [%1] thành công!").arg(candidateName)
            : QString("Đã từ chối đơn ứng tuyển của [%1].").arg(candidateName);
        CustomMessageBox::success(parentDialog ? parentDialog : this, "Thành công", successMsg);

        // 4. Tải lại toàn bộ dữ liệu trang công ty từ cơ sở dữ liệu
        setSession(m_userId);
        return true;
    } else {
        CustomMessageBox::critical(parentDialog ? parentDialog : this, "Lỗi cập nhật",
            "Không thể cập nhật trạng thái đơn ứng tuyển trong cơ sở dữ liệu applications.txt.");
        return false;
    }
}

void CompanyPage::showApplicantsDialog(int jobId, const QString &jobTitle) {
    QDialog dialog(this);
    QString dlgTitle = (jobId > 0) ? QString("Hồ sơ ứng tuyển — %1").arg(jobTitle) : "Tất cả hồ sơ ứng tuyển";
    dialog.setWindowTitle(dlgTitle);
    dialog.resize(840, 600);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(14);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    QLabel *headerTitle = new QLabel(dlgTitle, &dialog);
    headerTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #1E1E1E;");
    QLabel *headerSub = new QLabel("Xem xét và phê duyệt hoặc từ chối hồ sơ ứng viên", &dialog);
    headerSub->setStyleSheet("font-size: 12px; color: #6C757D;");
    titleLayout->addWidget(headerTitle);
    titleLayout->addWidget(headerSub);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    std::vector<Application> targetApps;
    for (const auto &app : m_applications) {
        if (jobId == 0 || app.getIDjob() == jobId) {
            targetApps.push_back(app);
        }
    }

    int totalCount = static_cast<int>(targetApps.size());
    int pendingCount = 0;
    int acceptedCount = 0;
    int rejectedCount = 0;
    for (const auto &a : targetApps) {
        if (a.getStatus() == "PENDING") pendingCount++;
        else if (a.getStatus() == "ACCEPTED") acceptedCount++;
        else if (a.getStatus() == "REJECTED") rejectedCount++;
    }

    // Filter Bar with Status Tabs
    QHBoxLayout *filterBar = new QHBoxLayout();
    filterBar->setSpacing(8);

    auto makeTabBtn = [&](const QString &text, bool active) -> QPushButton* {
        QPushButton *btn = new QPushButton(text, &dialog);
        btn->setCursor(Qt::PointingHandCursor);
        if (active) {
            btn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 700; border: none; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
        } else {
            btn->setStyleSheet("background-color: #FFFFFF; color: #6C757D; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
        }
        return btn;
    };

    QPushButton *btnAll = makeTabBtn(QString("Tất cả (%1)").arg(totalCount), true);
    QPushButton *btnPending = makeTabBtn(QString("⏳ Chờ duyệt (%1)").arg(pendingCount), false);
    QPushButton *btnAccepted = makeTabBtn(QString("✓ Đã nhận (%1)").arg(acceptedCount), false);
    QPushButton *btnRejected = makeTabBtn(QString("✕ Từ chối (%1)").arg(rejectedCount), false);

    filterBar->addWidget(btnAll);
    filterBar->addWidget(btnPending);
    filterBar->addWidget(btnAccepted);
    filterBar->addWidget(btnRejected);
    filterBar->addStretch();
    mainLayout->addLayout(filterBar);

    // Scroll area for applicants list
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *listContainer = new QWidget(scrollArea);
    listContainer->setStyleSheet("background-color: transparent;");
    QVBoxLayout *listLayout = new QVBoxLayout(listContainer);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(12);

    QString currentStatusFilter = "ALL";

    auto setTabActive = [&](QPushButton *activeBtn) {
        for (QPushButton *b : {btnAll, btnPending, btnAccepted, btnRejected}) {
            if (b == activeBtn) {
                b->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 700; border: none; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
            } else {
                b->setStyleSheet("background-color: #FFFFFF; color: #6C757D; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-size: 12px;");
            }
        }
    };

    auto renderApplicants = [&]() {
        safeClearLayout(listLayout);

        int displayedCount = 0;
        for (const auto &app : targetApps) {
            if (currentStatusFilter != "ALL" && app.getStatus() != currentStatusFilter.toStdString()) {
                continue;
            }
            displayedCount++;

            Candidate cand;
            std::string cname = "Ứng viên #" + std::to_string(app.getIDcandidate());
            if (m_data.getCandidateByID(app.getIDcandidate(), cand)) {
                cname = cand.getPersonalInfo().fullName;
            }
            QString qCandName = QString::fromStdString(cname);

            Job job;
            std::string jTitle = "Vị trí tuyển dụng";
            if (m_data.getJobByID(app.getIDjob(), job)) {
                jTitle = job.getTitle();
            }

            QFrame *itemCard = new QFrame(listContainer);
            itemCard->setStyleSheet(
                "background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 12px 16px;"
            );
            QHBoxLayout *itemLayout = new QHBoxLayout(itemCard);
            itemLayout->setSpacing(14);
            itemLayout->setAlignment(Qt::AlignVCenter);

            QString initials = StyleHelper::getInitials(qCandName, "UV");

            QLabel *avatar = new QLabel(itemCard);
            avatar->setAlignment(Qt::AlignCenter);
            avatar->setFixedSize(40, 40);
            int candId = cand.getIDcandidate() > 0 ? cand.getIDcandidate() : app.getIDcandidate();
            QString avPath = StyleHelper::getCandidateAvatarPath(candId);
            avatar->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 40));

            // Thông tin ứng viên
            QVBoxLayout *candInfo = new QVBoxLayout();
            candInfo->setSpacing(2);
            candInfo->setAlignment(Qt::AlignVCenter);
            QLabel *nameLbl = new QLabel(qCandName, itemCard);
            nameLbl->setStyleSheet("font-size: 14px; font-weight: 700; color: #1E1E1E;");

            QString metaText = QString("Vị trí: %1  |  Ngày nộp: %2").arg(QString::fromStdString(jTitle)).arg(QString::fromStdString(app.getAppliedAt()));
            QLabel *metaLbl = new QLabel(metaText, itemCard);
            metaLbl->setStyleSheet("font-size: 11px; color: #6C757D;");

            candInfo->addWidget(nameLbl);
            candInfo->addWidget(metaLbl);

            // Điểm phù hợp
            QVBoxLayout *scoreLayout = new QVBoxLayout();
            scoreLayout->setSpacing(0);
            scoreLayout->setAlignment(Qt::AlignCenter);
            QLabel *scoreVal = new QLabel(QString::number(app.getAppropriateLevel(), 'f', 1) + "%", itemCard);
            scoreVal->setAlignment(Qt::AlignCenter);
            scoreVal->setStyleSheet("font-size: 14px; font-weight: 800; color: #B95334;");
            QLabel *scoreLbl = new QLabel("Phù hợp", itemCard);
            scoreLbl->setAlignment(Qt::AlignCenter);
            scoreLbl->setStyleSheet("font-size: 10px; color: #6C757D;");
            scoreLayout->addWidget(scoreVal);
            scoreLayout->addWidget(scoreLbl);

            // Badge trạng thái
            QLabel *statusBadge = new QLabel(itemCard);
            statusBadge->setAlignment(Qt::AlignCenter);
            std::string st = app.getStatus();
            if (st == "ACCEPTED") {
                statusBadge->setText("Đã chấp nhận");
                statusBadge->setStyleSheet("background-color: #E3F7EB; color: #1B7F49; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 10px;");
            } else if (st == "REJECTED") {
                statusBadge->setText("Đã từ chối");
                statusBadge->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 10px;");
            } else {
                statusBadge->setText("Đang xem xét");
                statusBadge->setStyleSheet("background-color: #FEF3E2; color: #B76E00; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 10px;");
            }

            // Nút Thao tác
            QHBoxLayout *btnsLayout = new QHBoxLayout();
            btnsLayout->setSpacing(8);

            QPushButton *detailBtn = new QPushButton("Chi tiết", itemCard);
            detailBtn->setCursor(Qt::PointingHandCursor);
            detailBtn->setStyleSheet(
                "background-color: transparent; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 5px 12px; font-size: 11px; font-weight: 600;"
            );
            int cId = app.getIDcandidate();
            float cScore = app.getAppropriateLevel();
            QString qTitle = QString::fromStdString(jTitle);
            int appId = app.getIDappli();
            int currJobId = app.getIDjob();
            QString qSt = QString::fromStdString(st);

            connect(detailBtn, &QPushButton::clicked, this, [this, cId, qTitle, cScore, appId, qSt, currJobId]() {
                viewCandidateDetails(cId, qTitle, cScore, appId, qSt, currJobId);
            });

            btnsLayout->addWidget(detailBtn);

            if (st == "PENDING") {
                QPushButton *acceptBtn = new QPushButton("✓ Duyệt", itemCard);
                acceptBtn->setCursor(Qt::PointingHandCursor);
                acceptBtn->setStyleSheet(
                    "background-color: #E3F7EB; color: #1B7F49; border: 1px solid #C4EDD4; border-radius: 6px; padding: 5px 12px; font-size: 11px; font-weight: 600;"
                );

                QPushButton *rejectBtn = new QPushButton("✕ Từ chối", itemCard);
                rejectBtn->setCursor(Qt::PointingHandCursor);
                rejectBtn->setStyleSheet(
                    "background-color: #FEE2E2; color: #DC2626; border: 1px solid #FCA5A5; border-radius: 6px; padding: 5px 12px; font-size: 11px; font-weight: 600;"
                );

                connect(acceptBtn, &QPushButton::clicked, this, [this, appId, currJobId, qCandName, &dialog]() {
                    if (processApplicationDecision(appId, currJobId, qCandName, "ACCEPTED", &dialog)) {
                        dialog.accept();
                    }
                });

                connect(rejectBtn, &QPushButton::clicked, this, [this, appId, currJobId, qCandName, &dialog]() {
                    if (processApplicationDecision(appId, currJobId, qCandName, "REJECTED", &dialog)) {
                        dialog.accept();
                    }
                });

                btnsLayout->addWidget(acceptBtn);
                btnsLayout->addWidget(rejectBtn);
            }

            itemLayout->addWidget(avatar, 0, Qt::AlignVCenter);
            itemLayout->addLayout(candInfo, 1);
            itemLayout->addLayout(scoreLayout);
            itemLayout->addWidget(statusBadge, 0, Qt::AlignVCenter);
            itemLayout->addLayout(btnsLayout);

            listLayout->addWidget(itemCard);
        }

        if (displayedCount == 0) {
            QLabel *emptyLabel = new QLabel("Không có hồ sơ ứng tuyển nào trong mục này.", listContainer);
            emptyLabel->setStyleSheet("color: #6C757D; font-size: 13px; padding: 40px;");
            emptyLabel->setAlignment(Qt::AlignCenter);
            listLayout->addWidget(emptyLabel);
        }
        listLayout->addStretch();
    };

    connect(btnAll, &QPushButton::clicked, [&]() {
        currentStatusFilter = "ALL";
        setTabActive(btnAll);
        renderApplicants();
    });

    connect(btnPending, &QPushButton::clicked, [&]() {
        currentStatusFilter = "PENDING";
        setTabActive(btnPending);
        renderApplicants();
    });

    connect(btnAccepted, &QPushButton::clicked, [&]() {
        currentStatusFilter = "ACCEPTED";
        setTabActive(btnAccepted);
        renderApplicants();
    });

    connect(btnRejected, &QPushButton::clicked, [&]() {
        currentStatusFilter = "REJECTED";
        setTabActive(btnRejected);
        renderApplicants();
    });

    renderApplicants();

    scrollArea->setWidget(listContainer);
    mainLayout->addWidget(scrollArea);

    // Footer
    QHBoxLayout *footerLayout = new QHBoxLayout();
    footerLayout->addStretch();
    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 6px 18px; font-weight: 600;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    footerLayout->addWidget(closeBtn);
    mainLayout->addLayout(footerLayout);

    dialog.exec();
}

void CompanyPage::viewCandidateDetails(int candidateId, const QString &jobTitle, float score, int appId, const QString &currentStatus, int jobId) {
    Candidate cand;
    if (!m_data.getCandidateByID(candidateId, cand)) {
        CustomMessageBox::warning(this, "Thông báo", "Không tìm thấy thông tin chi tiết ứng viên trong cơ sở dữ liệu.");
        return;
    }
    m_data.loadCandidateSkill(cand);
    m_data.loadCandidateCertificate(cand);
    m_data.loadCandidateExperience(cand);

    const auto &p = cand.getPersonalInfo();
    const auto &prof = cand.getProfessionalInfo();
    QString candFullName = QString::fromStdString(p.fullName.empty() ? ("Ứng viên #" + std::to_string(candidateId)) : p.fullName);

    QDialog dialog(this);
    dialog.setWindowTitle("Hồ sơ ứng viên: " + candFullName);
    dialog.resize(680, 580);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Header card
    QFrame *headerCard = new QFrame(&dialog);
    headerCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setSpacing(16);
    headerLayout->setAlignment(Qt::AlignVCenter);

    QString initials = "UV";
    QStringList parts = candFullName.split(' ', Qt::SkipEmptyParts);
    if (parts.size() >= 2) initials = parts.first().left(1).toUpper() + parts.last().left(1).toUpper();
    else if (!parts.isEmpty()) initials = parts.first().left(2).toUpper();

    QLabel *avatar = new QLabel(headerCard);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(54, 54);
    QString avPath = StyleHelper::getCandidateAvatarPath(cand.getIDcandidate());
    avatar->setPixmap(StyleHelper::getCircularAvatar(avPath, initials, 54, "#B95334", "#FFFFFF"));

    QVBoxLayout *headInfo = new QVBoxLayout();
    headInfo->setSpacing(4);
    headInfo->setAlignment(Qt::AlignVCenter);
    QLabel *nameLabel = new QLabel(candFullName, headerCard);
    nameLabel->setStyleSheet("font-size: 18px; font-weight: 800; color: #1E1E1E;");
    QLabel *jobLabel = new QLabel("Vị trí ứng tuyển: " + jobTitle, headerCard);
    jobLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #B95334;");

    headInfo->addWidget(nameLabel);
    headInfo->addWidget(jobLabel);

    QVBoxLayout *scoreBox = new QVBoxLayout();
    scoreBox->setSpacing(2);
    scoreBox->setAlignment(Qt::AlignCenter);
    QLabel *scoreNum = new QLabel(QString::number(score, 'f', 1) + "%", headerCard);
    scoreNum->setAlignment(Qt::AlignCenter);
    scoreNum->setStyleSheet("font-size: 20px; font-weight: 800; color: #B95334;");
    QLabel *scoreLbl = new QLabel("Độ phù hợp", headerCard);
    scoreLbl->setAlignment(Qt::AlignCenter);
    scoreLbl->setStyleSheet("font-size: 11px; color: #6C757D;");
    scoreBox->addWidget(scoreNum);
    scoreBox->addWidget(scoreLbl);

    headerLayout->addWidget(avatar, 0, Qt::AlignVCenter);
    headerLayout->addLayout(headInfo, 1);
    headerLayout->addLayout(scoreBox);
    mainLayout->addWidget(headerCard);

    // Scrollable content for details
    QScrollArea *scrollArea = new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background-color: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(14);

    // Thẻ thông tin hợp nhất, loại bỏ các ô trắng phân mục rời rạc chiếm diện tích
    QFrame *detailCard = new QFrame(scrollContent);
    detailCard->setStyleSheet("background-color: #FFFFFF; border: 1px solid #ECE5DE; border-radius: 10px; padding: 16px;");
    QVBoxLayout *dLayout = new QVBoxLayout(detailCard);
    dLayout->setSpacing(12);

    auto makeSection = [detailCard](QVBoxLayout *parentLayout, const QString &secTitle, const QString &icon, bool addDivider) -> QVBoxLayout* {
        if (addDivider) {
            QFrame *line = new QFrame(detailCard);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("background-color: #F0EAE4; max-height: 1px; border: none; margin: 4px 0px;");
            parentLayout->addWidget(line);
        }
        QVBoxLayout *secLayout = new QVBoxLayout();
        secLayout->setSpacing(6);
        QLabel *titleLbl = new QLabel(icon + " " + secTitle, detailCard);
        titleLbl->setStyleSheet("font-size: 13px; font-weight: 700; color: #1E1E1E;");
        secLayout->addWidget(titleLbl);
        parentLayout->addLayout(secLayout);
        return secLayout;
    };

    // 1. Thông tin cá nhân & Liên hệ
    QVBoxLayout *pLayout = makeSection(dLayout, "Thông tin cá nhân & Liên hệ", "👤", false);
    QFormLayout *pForm = new QFormLayout();
    pForm->setSpacing(6);
    pForm->addRow("Email:", new QLabel(QString::fromStdString(cand.getEmail().empty() ? "Chưa cập nhật" : cand.getEmail()), detailCard));
    pForm->addRow("Điện thoại:", new QLabel(QString::fromStdString(p.phoneCandidate.empty() ? "Chưa cập nhật" : p.phoneCandidate), detailCard));
    pForm->addRow("Ngày sinh:", new QLabel(QString::fromStdString(p.birthday.empty() ? "Chưa cập nhật" : p.birthday) + " (" + QString::fromStdString(p.gender) + ")", detailCard));
    pForm->addRow("Địa chỉ:", new QLabel(QString::fromStdString(p.addressCandidate.empty() ? "Chưa cập nhật" : p.addressCandidate), detailCard));
    pLayout->addLayout(pForm);

    // 2. Học vấn & Bằng cấp
    QVBoxLayout *eduLayout = makeSection(dLayout, "Học vấn & Trình độ", "🎓", true);
    QFormLayout *eduForm = new QFormLayout();
    eduForm->setSpacing(6);
    eduForm->addRow("Ngành học:", new QLabel(QString::fromStdString(prof.major.empty() ? "Chưa cập nhật" : prof.major), detailCard));
    eduForm->addRow("Trường đào tạo:", new QLabel(QString::fromStdString(prof.school.empty() ? "Chưa cập nhật" : prof.school), detailCard));
    eduForm->addRow("Trình độ:", new QLabel(QString::fromStdString(prof.educationLevel.empty() ? "Đại học" : prof.educationLevel), detailCard));
    eduForm->addRow("Điểm GPA:", new QLabel(QString::number(prof.GPA, 'f', 2), detailCard));
    eduLayout->addLayout(eduForm);

    // 3. Kỹ năng chuyên môn
    const auto &skills = cand.getSkill();
    if (!skills.empty()) {
        QVBoxLayout *skLayout = makeSection(dLayout, "Kỹ năng chuyên môn", "⚡", true);
        QHBoxLayout *badgesLayout = new QHBoxLayout();
        badgesLayout->setSpacing(8);
        for (const auto &sk : skills) {
            QLabel *b = new QLabel(QString::fromStdString(sk.name) + " (Lv " + QString::number(sk.level) + ")", detailCard);
            b->setStyleSheet("background-color: #FCE8E2; color: #B95334; font-size: 11px; font-weight: 600; border-radius: 6px; padding: 4px 8px;");
            badgesLayout->addWidget(b);
        }
        badgesLayout->addStretch();
        skLayout->addLayout(badgesLayout);
    }

    // 4. Chứng chỉ
    const auto &certs = cand.getCertificate();
    if (!certs.empty()) {
        QVBoxLayout *certLayout = makeSection(dLayout, "Chứng chỉ chuyên ngành & Ngoại ngữ", "📜", true);
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
            QLabel *cLbl = new QLabel(cText, detailCard);
            cLbl->setStyleSheet("font-size: 12px; color: #333;");
            certLayout->addWidget(cLbl);
        }
    }

    // 5. Kinh nghiệm làm việc
    const auto &exps = cand.getExperiences();
    if (!exps.empty()) {
        QVBoxLayout *expLayout = makeSection(dLayout, "Kinh nghiệm làm việc", "💼", true);
        for (const auto &e : exps) {
            QVBoxLayout *eBox = new QVBoxLayout();
            eBox->setSpacing(2);
            QLabel *posLbl = new QLabel(QString::fromStdString(e.getPosition()) + " @ " + QString::fromStdString(e.getCompanyName()), detailCard);
            posLbl->setStyleSheet("font-size: 13px; font-weight: 600; color: #1E1E1E;");
            QLabel *dateLbl = new QLabel(QString::fromStdString(e.getStartDate()) + " → " + QString::fromStdString(e.getEndDate()), detailCard);
            dateLbl->setStyleSheet("font-size: 11px; color: #6C757D;");
            eBox->addWidget(posLbl);
            eBox->addWidget(dateLbl);
            if (!e.getDescription().empty()) {
                QLabel *descLbl = new QLabel(QString::fromStdString(e.getDescription()), detailCard);
                descLbl->setStyleSheet("font-size: 12px; color: #555; padding-left: 12px;");
                descLbl->setWordWrap(true);
                eBox->addWidget(descLbl);
            }
            expLayout->addLayout(eBox);
        }
    }

    contentLayout->addWidget(detailCard);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);

    // Footer with decisions & close
    QHBoxLayout *footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(10);

    QLabel *stBadge = new QLabel(&dialog);
    if (currentStatus == "ACCEPTED") {
        stBadge->setText("✓ Trạng thái: Đã chấp nhận");
        stBadge->setStyleSheet("color: #1B7F49; font-weight: 700; font-size: 13px;");
    } else if (currentStatus == "REJECTED") {
        stBadge->setText("✕ Trạng thái: Đã từ chối");
        stBadge->setStyleSheet("color: #DC2626; font-weight: 700; font-size: 13px;");
    } else {
        stBadge->setText("⏳ Trạng thái: Đang xem xét");
        stBadge->setStyleSheet("color: #B76E00; font-weight: 700; font-size: 13px;");
    }
    footerLayout->addWidget(stBadge);
    footerLayout->addStretch();

    if (currentStatus == "PENDING" && appId > 0) {
        QPushButton *acceptBtn = new QPushButton("✓ Chấp nhận hồ sơ", &dialog);
        acceptBtn->setCursor(Qt::PointingHandCursor);
        acceptBtn->setStyleSheet("background-color: #1B7F49; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 7px 16px;");

        QPushButton *rejectBtn = new QPushButton("✕ Từ chối", &dialog);
        rejectBtn->setCursor(Qt::PointingHandCursor);
        rejectBtn->setStyleSheet("background-color: #FEE2E2; color: #DC2626; font-weight: 600; border: 1px solid #FCA5A5; border-radius: 6px; padding: 7px 16px;");

        connect(acceptBtn, &QPushButton::clicked, this, [this, appId, jobId, candFullName, &dialog]() {
            if (processApplicationDecision(appId, jobId, candFullName, "ACCEPTED", &dialog)) {
                dialog.accept();
            }
        });

        connect(rejectBtn, &QPushButton::clicked, this, [this, appId, jobId, candFullName, &dialog]() {
            if (processApplicationDecision(appId, jobId, candFullName, "REJECTED", &dialog)) {
                dialog.accept();
            }
        });

        footerLayout->addWidget(acceptBtn);
        footerLayout->addWidget(rejectBtn);
    }

    QPushButton *closeBtn = new QPushButton("Đóng", &dialog);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: #ECE5DE; color: #1E1E1E; border: none; border-radius: 6px; padding: 7px 18px; font-weight: 600;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    footerLayout->addWidget(closeBtn);

    mainLayout->addLayout(footerLayout);
    dialog.exec();
}

void CompanyPage::filterContent(const QString &keyword) {
    m_filterKeyword = keyword;
    buildJobsList();
}

void CompanyPage::onRefreshData() {
    m_searchEngine.reload(m_data);
    setSession(m_userId);
}

void CompanyPage::onChangeLogoClicked() {
    QString file = QFileDialog::getOpenFileName(this, "Chọn logo công ty", "", "Hình ảnh (*.png *.jpg *.jpeg *.bmp *.webp)");
    if (file.isEmpty()) return;

    QString savedPath;
    if (StyleHelper::saveCompanyLogo(m_company.getIDcompany(), file, savedPath)) {
        populateDashboard();
        CustomMessageBox::success(this, "Cập nhật thành công", "Đã cập nhật logo công ty mới thành công!");
    } else {
        CustomMessageBox::warning(this, "Lỗi cập nhật", "Không thể nạp tệp hình ảnh logo đã chọn. Vui lòng chọn tệp định dạng JPG hoặc PNG hợp lệ.");
    }
}

bool CompanyPage::eventFilter(QObject *obj, QEvent *event) {
    if (obj == companyLogoLabel && event->type() == QEvent::MouseButtonPress) {
        onChangeLogoClicked();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void CompanyPage::onCreateJobClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Đăng tin tuyển dụng mới");
    dialog.setFixedSize(580, 580);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    QTabWidget *tabWidget = new QTabWidget(&dialog);

    // =========================================================================
    // TAB 1: THÔNG TIN CHUNG
    // =========================================================================
    QWidget *generalTab = new QWidget(tabWidget);
    QFormLayout *form = new QFormLayout(generalTab);
    form->setContentsMargins(14, 14, 14, 14);
    form->setSpacing(10);

    QLineEdit *titleEdit = new QLineEdit(generalTab);
    titleEdit->setPlaceholderText("VD: Lập trình viên C++ hiệu năng cao");

    QLineEdit *locEdit = new QLineEdit(generalTab);
    locEdit->setText("Đà Nẵng");

    QLineEdit *majorEdit = new QLineEdit(generalTab);
    majorEdit->setText("Công nghệ thông tin");

    QSpinBox *salarySpin = new QSpinBox(generalTab);
    salarySpin->setRange(1000000, 100000000);
    salarySpin->setSingleStep(1000000);
    salarySpin->setValue(15000000);

    QSpinBox *expSpin = new QSpinBox(generalTab);
    expSpin->setRange(0, 10);
    expSpin->setValue(1);

    QDoubleSpinBox *gpaSpin = new QDoubleSpinBox(generalTab);
    gpaSpin->setRange(0.0, 4.0);
    gpaSpin->setSingleStep(0.1);
    gpaSpin->setValue(2.5);

    QDateEdit *deadlineEdit = new QDateEdit(generalTab);
    deadlineEdit->setCalendarPopup(true);
    deadlineEdit->setDate(QDate::currentDate().addMonths(3));
    deadlineEdit->setDisplayFormat("yyyy-MM-dd");

    QTextEdit *descEdit = new QTextEdit(generalTab);
    descEdit->setPlaceholderText("Mô tả tóm tắt yêu cầu công việc");
    descEdit->setMaximumHeight(70);

    form->addRow("Tiêu đề vị trí (*):", titleEdit);
    form->addRow("Địa điểm làm việc:", locEdit);
    form->addRow("Ngành yêu cầu:", majorEdit);
    form->addRow("Mức lương (VNĐ):", salarySpin);
    form->addRow("Kinh nghiệm (năm):", expSpin);
    form->addRow("GPA tối thiểu:", gpaSpin);
    form->addRow("Hạn nộp hồ sơ:", deadlineEdit);
    form->addRow("Mô tả công việc:", descEdit);

    tabWidget->addTab(generalTab, "Thông tin chung");

    // =========================================================================
    // TAB 2: KỸ NĂNG & CHỨNG CHỈ YÊU CẦU
    // =========================================================================
    QWidget *reqTab = new QWidget(tabWidget);
    QVBoxLayout *reqLayout = new QVBoxLayout(reqTab);
    reqLayout->setContentsMargins(14, 14, 14, 14);
    reqLayout->setSpacing(10);

    std::vector<Skill> addedSkills;
    std::vector<CertificateRequirementGroup> addedCertGroups;

    // 1. Nhóm kỹ năng
    QLabel *skillsHeader = new QLabel("Kỹ năng chuyên môn yêu cầu:", reqTab);
    skillsHeader->setStyleSheet("font-weight: 700; color: #1E1E1E;");
    reqLayout->addWidget(skillsHeader);

    QHBoxLayout *skillInputRow = new QHBoxLayout();
    QLineEdit *skillNameEdit = new QLineEdit(reqTab);
    skillNameEdit->setPlaceholderText("Tên kỹ năng (VD: C++, Qt, SQL)");

    QSpinBox *skillLevelSpin = new QSpinBox(reqTab);
    skillLevelSpin->setRange(1, 5);
    skillLevelSpin->setValue(3);
    skillLevelSpin->setPrefix("Level ");

    QPushButton *addSkillBtn = new QPushButton("+ Thêm", reqTab);
    addSkillBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border-radius: 4px; padding: 5px 12px;");

    skillInputRow->addWidget(skillNameEdit, 2);
    skillInputRow->addWidget(skillLevelSpin, 1);
    skillInputRow->addWidget(addSkillBtn);
    reqLayout->addLayout(skillInputRow);

    QListWidget *skillsListWidget = new QListWidget(reqTab);
    skillsListWidget->setMaximumHeight(80);
    skillsListWidget->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 4px;");
    reqLayout->addWidget(skillsListWidget);

    QPushButton *removeSkillBtn = new QPushButton("✕ Xóa kỹ năng đã chọn", reqTab);
    removeSkillBtn->setStyleSheet("background: transparent; color: #DC2626; border: none; font-size: 11px; text-align: left;");
    reqLayout->addWidget(removeSkillBtn);

    // 2. Nhóm chứng chỉ
    QLabel *certsHeader = new QLabel("Chứng chỉ ngoại ngữ / chuyên môn:", reqTab);
    certsHeader->setStyleSheet("font-weight: 700; color: #1E1E1E;");
    reqLayout->addWidget(certsHeader);

    QHBoxLayout *certInputRow = new QHBoxLayout();
    QComboBox *certTypeCombo = new QComboBox(reqTab);
    certTypeCombo->addItem("IELTS (Tiếng Anh)", 1);
    certTypeCombo->addItem("TOEIC (Tiếng Anh)", 2);
    certTypeCombo->addItem("TOEFL / HSK", 3);
    certTypeCombo->addItem("JLPT (Tiếng Nhật)", 4);
    certTypeCombo->addItem("Cloud / AWS", 5);

    QDoubleSpinBox *certScoreSpin = new QDoubleSpinBox(reqTab);
    certScoreSpin->setRange(0.0, 990.0);
    certScoreSpin->setSingleStep(0.5);
    certScoreSpin->setValue(6.5);

    auto updateSpinForCert = [certScoreSpin](int typeId) {
        if (typeId == 1) { // IELTS
            certScoreSpin->setRange(1.0, 9.0);
            certScoreSpin->setSingleStep(0.5);
            certScoreSpin->setValue(6.5);
        } else if (typeId == 2) { // TOEIC
            certScoreSpin->setRange(100.0, 990.0);
            certScoreSpin->setSingleStep(50.0);
            certScoreSpin->setValue(650.0);
        } else if (typeId == 3) { // TOEFL / HSK
            certScoreSpin->setRange(1.0, 120.0);
            certScoreSpin->setSingleStep(5.0);
            certScoreSpin->setValue(80.0);
        } else if (typeId == 4) { // JLPT
            certScoreSpin->setRange(1.0, 5.0);
            certScoreSpin->setSingleStep(1.0);
            certScoreSpin->setValue(3.0);
        } else if (typeId == 5) { // Cloud / AWS
            certScoreSpin->setRange(1.0, 5.0);
            certScoreSpin->setSingleStep(1.0);
            certScoreSpin->setValue(1.0);
        }
    };
    QObject::connect(certTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [certTypeCombo, updateSpinForCert](int) {
        updateSpinForCert(certTypeCombo->currentData().toInt());
    });

    QComboBox *certReqCombo = new QComboBox(reqTab);
    certReqCombo->addItem("Bắt buộc", 1);
    certReqCombo->addItem("Ưu tiên", 0);

    QPushButton *addCertBtn = new QPushButton("+ Thêm", reqTab);
    addCertBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border-radius: 4px; padding: 5px 12px;");

    certInputRow->addWidget(certTypeCombo);
    certInputRow->addWidget(certScoreSpin);
    certInputRow->addWidget(certReqCombo);
    certInputRow->addWidget(addCertBtn);
    reqLayout->addLayout(certInputRow);

    QListWidget *certsListWidget = new QListWidget(reqTab);
    certsListWidget->setMaximumHeight(80);
    certsListWidget->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 4px;");
    reqLayout->addWidget(certsListWidget);

    QPushButton *removeCertBtn = new QPushButton("✕ Xóa chứng chỉ đã chọn", reqTab);
    removeCertBtn->setStyleSheet("background: transparent; color: #DC2626; border: none; font-size: 11px; text-align: left;");
    reqLayout->addWidget(removeCertBtn);

    tabWidget->addTab(reqTab, "Kỹ năng & Chứng chỉ");
    layout->addWidget(tabWidget);

    // Xử lý thêm / xóa kỹ năng
    connect(addSkillBtn, &QPushButton::clicked, [&]() {
        QString sName = skillNameEdit->text().trimmed();
        if (sName.isEmpty()) return;
        Skill s;
        s.IDskill = 0;
        s.name = sName.toStdString();
        s.level = skillLevelSpin->value();
        addedSkills.push_back(s);
        skillsListWidget->addItem(QString("• %1 — Level tối thiểu: %2/5").arg(sName).arg(s.level));
        skillNameEdit->clear();
    });

    connect(removeSkillBtn, &QPushButton::clicked, [&]() {
        int row = skillsListWidget->currentRow();
        if (row >= 0 && row < static_cast<int>(addedSkills.size())) {
            addedSkills.erase(addedSkills.begin() + row);
            delete skillsListWidget->takeItem(row);
        }
    });

    // Xử lý thêm / xóa chứng chỉ
    connect(addCertBtn, &QPushButton::clicked, [&]() {
        int typeId = certTypeCombo->currentData().toInt();
        double minVal = certScoreSpin->value();
        bool isReq = (certReqCombo->currentData().toInt() == 1);
        QString typeName = certTypeCombo->currentText();

        CertificateRequirement req;
        req.certificateTypeID = typeId;
        req.minValue = minVal;

        CertificateRequirementGroup group;
        group.isRequired = isReq;
        group.alternatives.push_back(req);
        addedCertGroups.push_back(group);

        QString reqBadge = isReq ? "[BẮT BUỘC]" : "[ƯU TIÊN]";
        certsListWidget->addItem(QString("%1 %2 >= %3").arg(reqBadge).arg(typeName).arg(minVal));
    });

    connect(removeCertBtn, &QPushButton::clicked, [&]() {
        int row = certsListWidget->currentRow();
        if (row >= 0 && row < static_cast<int>(addedCertGroups.size())) {
            addedCertGroups.erase(addedCertGroups.begin() + row);
            delete certsListWidget->takeItem(row);
        }
    });

    // Buttons chân dialog
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setStyleSheet("background-color: transparent; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-weight: 500;");
    QPushButton *saveBtn = new QPushButton("Đăng tin ngay", &dialog);
    saveBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 6px 18px;");

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    layout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, [&]() {
        if (titleEdit->text().trimmed().isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thông báo", "Vui lòng nhập tiêu đề vị trí!");
            return;
        }

        int newId = m_data.getNextID("jobs.txt");
        Job newJob;
        newJob.setIDjob(newId);
        newJob.setIDcompany(m_company.getIDcompany());
        newJob.setTitle(titleEdit->text().trimmed().toStdString());
        newJob.setLocation(locEdit->text().trimmed().toStdString());
        newJob.setNameMajor(majorEdit->text().trimmed().toStdString());
        newJob.setSalary(salarySpin->value());
        newJob.setMinExperience(expSpin->value());
        newJob.setMinGPA(static_cast<float>(gpaSpin->value()));
        newJob.setDescription(descEdit->toPlainText().trimmed().toStdString());
        newJob.setDeadline(deadlineEdit->date().toString("yyyy-MM-dd").toStdString());
        newJob.setStatus(true);

        // Gán kỹ năng và chứng chỉ vào newJob
        newJob.setRequiredSkills(addedSkills);
        newJob.setCertificateGroups(addedCertGroups);

        if (m_company.addJob(newJob, m_data)) {
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", "Đã lưu tin tuyển dụng cùng danh sách kỹ năng & chứng chỉ thành công vào hệ thống!");
            m_searchEngine.reload(m_data);
            setSession(m_userId);
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi", "Không thể lưu tin tuyển dụng vào jobs.txt.");
        }
    });

    dialog.exec();
}

void CompanyPage::onEditJobClicked(int jobId) {
    auto it = std::find_if(m_jobs.begin(), m_jobs.end(), [jobId](const Job& j) {
        return j.getIDjob() == jobId;
    });
    if (it == m_jobs.end()) {
        CustomMessageBox::warning(this, "Thông báo", "Không tìm thấy thông tin tin tuyển dụng này.");
        return;
    }
    Job targetJob = *it;

    QDialog dialog(this);
    dialog.setWindowTitle("Chỉnh sửa tin tuyển dụng");
    dialog.setFixedSize(580, 580);
    dialog.setStyleSheet("background-color: #FBF8F5;");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    QTabWidget *tabWidget = new QTabWidget(&dialog);

    // =========================================================================
    // TAB 1: THÔNG TIN CHUNG
    // =========================================================================
    QWidget *generalTab = new QWidget(tabWidget);
    QFormLayout *form = new QFormLayout(generalTab);
    form->setContentsMargins(14, 14, 14, 14);
    form->setSpacing(10);

    QLineEdit *titleEdit = new QLineEdit(generalTab);
    titleEdit->setText(QString::fromStdString(targetJob.getTitle()));

    QLineEdit *locEdit = new QLineEdit(generalTab);
    locEdit->setText(QString::fromStdString(targetJob.getLocation()));

    QLineEdit *majorEdit = new QLineEdit(generalTab);
    majorEdit->setText(QString::fromStdString(targetJob.getNameMajor()));

    QSpinBox *salarySpin = new QSpinBox(generalTab);
    salarySpin->setRange(1000000, 100000000);
    salarySpin->setSingleStep(1000000);
    salarySpin->setValue(targetJob.getSalary() > 0 ? targetJob.getSalary() : 15000000);

    QSpinBox *expSpin = new QSpinBox(generalTab);
    expSpin->setRange(0, 10);
    expSpin->setValue(targetJob.getMinExperience());

    QDoubleSpinBox *gpaSpin = new QDoubleSpinBox(generalTab);
    gpaSpin->setRange(0.0, 4.0);
    gpaSpin->setSingleStep(0.1);
    gpaSpin->setValue(targetJob.getMinGPA() > 0 ? targetJob.getMinGPA() : 2.5);

    QDateEdit *deadlineEdit = new QDateEdit(generalTab);
    deadlineEdit->setCalendarPopup(true);
    QDate d = QDate::fromString(QString::fromStdString(targetJob.getDeadline()), "yyyy-MM-dd");
    if (!d.isValid()) d = QDate::currentDate().addMonths(3);
    deadlineEdit->setDate(d);
    deadlineEdit->setDisplayFormat("yyyy-MM-dd");

    QTextEdit *descEdit = new QTextEdit(generalTab);
    descEdit->setText(QString::fromStdString(targetJob.getDescription()));
    descEdit->setMaximumHeight(70);

    form->addRow("Tiêu đề vị trí (*):", titleEdit);
    form->addRow("Địa điểm làm việc:", locEdit);
    form->addRow("Ngành yêu cầu:", majorEdit);
    form->addRow("Mức lương (VNĐ):", salarySpin);
    form->addRow("Kinh nghiệm (năm):", expSpin);
    form->addRow("GPA tối thiểu:", gpaSpin);
    form->addRow("Hạn nộp hồ sơ:", deadlineEdit);
    form->addRow("Mô tả công việc:", descEdit);

    tabWidget->addTab(generalTab, "Thông tin chung");

    // =========================================================================
    // TAB 2: KỸ NĂNG & CHỨNG CHỈ YÊU CẦU
    // =========================================================================
    QWidget *reqTab = new QWidget(tabWidget);
    QVBoxLayout *reqLayout = new QVBoxLayout(reqTab);
    reqLayout->setContentsMargins(14, 14, 14, 14);
    reqLayout->setSpacing(10);

    std::vector<Skill> addedSkills = targetJob.getRequiredSkill();
    std::vector<CertificateRequirementGroup> addedCertGroups = targetJob.getCertificateGroups();

    // 1. Nhóm kỹ năng
    QLabel *skillsHeader = new QLabel("Kỹ năng chuyên môn yêu cầu:", reqTab);
    skillsHeader->setStyleSheet("font-weight: 700; color: #1E1E1E;");
    reqLayout->addWidget(skillsHeader);

    QHBoxLayout *skillInputRow = new QHBoxLayout();
    QLineEdit *skillNameEdit = new QLineEdit(reqTab);
    skillNameEdit->setPlaceholderText("Tên kỹ năng (VD: C++, Qt, SQL)");

    QSpinBox *skillLevelSpin = new QSpinBox(reqTab);
    skillLevelSpin->setRange(1, 5);
    skillLevelSpin->setValue(3);
    skillLevelSpin->setPrefix("Level ");

    QPushButton *addSkillBtn = new QPushButton("+ Thêm", reqTab);
    addSkillBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border-radius: 4px; padding: 5px 12px;");

    skillInputRow->addWidget(skillNameEdit, 2);
    skillInputRow->addWidget(skillLevelSpin, 1);
    skillInputRow->addWidget(addSkillBtn);
    reqLayout->addLayout(skillInputRow);

    QListWidget *skillsListWidget = new QListWidget(reqTab);
    skillsListWidget->setMaximumHeight(80);
    skillsListWidget->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 4px;");
    for (const auto& s : addedSkills) {
        skillsListWidget->addItem(QString("• %1 — Level tối thiểu: %2/5").arg(QString::fromStdString(s.name)).arg(s.level));
    }
    reqLayout->addWidget(skillsListWidget);

    QPushButton *removeSkillBtn = new QPushButton("✕ Xóa kỹ năng đã chọn", reqTab);
    removeSkillBtn->setStyleSheet("background: transparent; color: #DC2626; border: none; font-size: 11px; text-align: left;");
    reqLayout->addWidget(removeSkillBtn);

    // 2. Nhóm chứng chỉ
    QLabel *certsHeader = new QLabel("Chứng chỉ ngoại ngữ / chuyên môn:", reqTab);
    certsHeader->setStyleSheet("font-weight: 700; color: #1E1E1E;");
    reqLayout->addWidget(certsHeader);

    QHBoxLayout *certInputRow = new QHBoxLayout();
    QComboBox *certTypeCombo = new QComboBox(reqTab);
    certTypeCombo->addItem("IELTS (Tiếng Anh)", 1);
    certTypeCombo->addItem("TOEIC (Tiếng Anh)", 2);
    certTypeCombo->addItem("TOEFL / HSK", 3);
    certTypeCombo->addItem("JLPT (Tiếng Nhật)", 4);
    certTypeCombo->addItem("Cloud / AWS", 5);

    QDoubleSpinBox *certScoreSpin = new QDoubleSpinBox(reqTab);
    certScoreSpin->setRange(0.0, 990.0);
    certScoreSpin->setSingleStep(0.5);
    certScoreSpin->setValue(6.5);

    auto updateSpinForCert = [certScoreSpin](int typeId) {
        if (typeId == 1) { // IELTS
            certScoreSpin->setRange(1.0, 9.0);
            certScoreSpin->setSingleStep(0.5);
            certScoreSpin->setValue(6.5);
        } else if (typeId == 2) { // TOEIC
            certScoreSpin->setRange(100.0, 990.0);
            certScoreSpin->setSingleStep(50.0);
            certScoreSpin->setValue(650.0);
        } else if (typeId == 3) { // TOEFL / HSK
            certScoreSpin->setRange(1.0, 120.0);
            certScoreSpin->setSingleStep(5.0);
            certScoreSpin->setValue(80.0);
        } else if (typeId == 4) { // JLPT
            certScoreSpin->setRange(1.0, 5.0);
            certScoreSpin->setSingleStep(1.0);
            certScoreSpin->setValue(3.0);
        } else if (typeId == 5) { // Cloud / AWS
            certScoreSpin->setRange(1.0, 5.0);
            certScoreSpin->setSingleStep(1.0);
            certScoreSpin->setValue(1.0);
        }
    };
    QObject::connect(certTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [certTypeCombo, updateSpinForCert](int) {
        updateSpinForCert(certTypeCombo->currentData().toInt());
    });

    QComboBox *certReqCombo = new QComboBox(reqTab);
    certReqCombo->addItem("Bắt buộc", 1);
    certReqCombo->addItem("Ưu tiên", 0);

    QPushButton *addCertBtn = new QPushButton("+ Thêm", reqTab);
    addCertBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border-radius: 4px; padding: 5px 12px;");

    certInputRow->addWidget(certTypeCombo);
    certInputRow->addWidget(certScoreSpin);
    certInputRow->addWidget(certReqCombo);
    certInputRow->addWidget(addCertBtn);
    reqLayout->addLayout(certInputRow);

    QListWidget *certsListWidget = new QListWidget(reqTab);
    certsListWidget->setMaximumHeight(80);
    certsListWidget->setStyleSheet("background-color: #FFFFFF; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 4px;");
    for (const auto& grp : addedCertGroups) {
        for (const auto& alt : grp.alternatives) {
            QString typeName = "Chứng chỉ";
            if (alt.certificateTypeID == 1) typeName = "IELTS";
            else if (alt.certificateTypeID == 2) typeName = "TOEIC";
            else if (alt.certificateTypeID == 3) typeName = "TOEFL / HSK";
            else if (alt.certificateTypeID == 4) typeName = "JLPT";
            else if (alt.certificateTypeID == 5) typeName = "Cloud / AWS";
            else typeName = "Chuyên môn";

            QString reqBadge = grp.isRequired ? "[BẮT BUỘC]" : "[ƯU TIÊN]";
            certsListWidget->addItem(QString("%1 %2 >= %3").arg(reqBadge).arg(typeName).arg(alt.minValue));
        }
    }
    reqLayout->addWidget(certsListWidget);

    QPushButton *removeCertBtn = new QPushButton("✕ Xóa chứng chỉ đã chọn", reqTab);
    removeCertBtn->setStyleSheet("background: transparent; color: #DC2626; border: none; font-size: 11px; text-align: left;");
    reqLayout->addWidget(removeCertBtn);

    tabWidget->addTab(reqTab, "Kỹ năng & Chứng chỉ");
    layout->addWidget(tabWidget);

    // Xử lý thêm / xóa kỹ năng
    connect(addSkillBtn, &QPushButton::clicked, [&]() {
        QString sName = skillNameEdit->text().trimmed();
        if (sName.isEmpty()) return;
        Skill s;
        s.IDskill = 0;
        s.name = sName.toStdString();
        s.level = skillLevelSpin->value();
        addedSkills.push_back(s);
        skillsListWidget->addItem(QString("• %1 — Level tối thiểu: %2/5").arg(sName).arg(s.level));
        skillNameEdit->clear();
    });

    connect(removeSkillBtn, &QPushButton::clicked, [&]() {
        int row = skillsListWidget->currentRow();
        if (row >= 0 && row < static_cast<int>(addedSkills.size())) {
            addedSkills.erase(addedSkills.begin() + row);
            delete skillsListWidget->takeItem(row);
        }
    });

    // Xử lý thêm / xóa chứng chỉ
    connect(addCertBtn, &QPushButton::clicked, [&]() {
        int typeId = certTypeCombo->currentData().toInt();
        double minVal = certScoreSpin->value();
        bool isReq = (certReqCombo->currentData().toInt() == 1);
        QString typeName = certTypeCombo->currentText();

        CertificateRequirement req;
        req.certificateTypeID = typeId;
        req.minValue = minVal;

        CertificateRequirementGroup group;
        group.isRequired = isReq;
        group.alternatives.push_back(req);
        addedCertGroups.push_back(group);

        QString reqBadge = isReq ? "[BẮT BUỘC]" : "[ƯU TIÊN]";
        certsListWidget->addItem(QString("%1 %2 >= %3").arg(reqBadge).arg(typeName).arg(minVal));
    });

    connect(removeCertBtn, &QPushButton::clicked, [&]() {
        int row = certsListWidget->currentRow();
        if (row >= 0 && row < static_cast<int>(addedCertGroups.size())) {
            addedCertGroups.erase(addedCertGroups.begin() + row);
            delete certsListWidget->takeItem(row);
        }
    });

    // Buttons chân dialog
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton *cancelBtn = new QPushButton("Hủy bỏ", &dialog);
    cancelBtn->setStyleSheet("background-color: transparent; color: #1E1E1E; border: 1px solid #ECE5DE; border-radius: 6px; padding: 6px 14px; font-weight: 500;");
    QPushButton *saveBtn = new QPushButton("✓ Lưu thay đổi", &dialog);
    saveBtn->setStyleSheet("background-color: #B95334; color: #FFFFFF; font-weight: 600; border: none; border-radius: 6px; padding: 6px 18px;");

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    layout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, [&]() {
        if (titleEdit->text().trimmed().isEmpty()) {
            CustomMessageBox::warning(&dialog, "Thông báo", "Vui lòng nhập tiêu đề vị trí!");
            return;
        }

        targetJob.setTitle(titleEdit->text().trimmed().toStdString());
        targetJob.setLocation(locEdit->text().trimmed().toStdString());
        targetJob.setNameMajor(majorEdit->text().trimmed().toStdString());
        targetJob.setSalary(salarySpin->value());
        targetJob.setMinExperience(expSpin->value());
        targetJob.setMinGPA(static_cast<float>(gpaSpin->value()));
        targetJob.setDescription(descEdit->toPlainText().trimmed().toStdString());
        targetJob.setDeadline(deadlineEdit->date().toString("yyyy-MM-dd").toStdString());

        targetJob.setRequiredSkills(addedSkills);
        targetJob.setCertificateGroups(addedCertGroups);

        if (m_company.updateJob(targetJob, m_data)) {
            dialog.accept();
            CustomMessageBox::success(this, "Thành công", "Đã cập nhật tin tuyển dụng thành công!");
            m_searchEngine.reload(m_data);
            setSession(m_userId);
        } else {
            CustomMessageBox::critical(&dialog, "Lỗi", "Không thể cập nhật tin tuyển dụng vào jobs.txt.");
        }
    });

    dialog.exec();
}
