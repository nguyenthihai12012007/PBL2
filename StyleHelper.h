#ifndef STYLE_HELPER_H
#define STYLE_HELPER_H

#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QFont>
#include <QRect>
#include <QImage>
#include <QCoreApplication>
#include <algorithm>

class StyleHelper {
public:
    static QString getProjectRootDir() {
        static QString cachedRoot;
        if (!cachedRoot.isEmpty() && QFile::exists(QDir(cachedRoot).filePath("users.txt"))) {
            return cachedRoot;
        }

        QStringList searchDirs = {
            QDir::currentPath(),
            QCoreApplication::applicationDirPath(),
            QDir(QCoreApplication::applicationDirPath()).filePath(".."),
            QDir(QCoreApplication::applicationDirPath()).filePath("../.."),
            QDir(QCoreApplication::applicationDirPath()).filePath("../../..")
        };

        for (const QString &dir : searchDirs) {
            if (QFile::exists(QDir(dir).filePath("users.txt"))) {
                cachedRoot = QDir(dir).canonicalPath();
                return cachedRoot;
            }
        }
        cachedRoot = QDir::currentPath();
        return cachedRoot;
    }

    static QString getCandidateAvatarPath(int candidateId) {
        if (candidateId <= 0) return "";
        QString root = getProjectRootDir();
        QStringList exts = {".png", ".jpg", ".jpeg"};
        for (const QString &ext : exts) {
            QString path = QDir(root).filePath(QString("avatars/candidate_%1%2").arg(candidateId).arg(ext));
            if (QFile::exists(path)) return path;
        }
        return "";
    }

    static QString getCompanyLogoPath(int companyId) {
        if (companyId <= 0) return "";
        QString root = getProjectRootDir();
        QStringList exts = {".png", ".jpg", ".jpeg"};
        for (const QString &ext : exts) {
            QString path = QDir(root).filePath(QString("avatars/company_%1%2").arg(companyId).arg(ext));
            if (QFile::exists(path)) return path;
        }
        return "";
    }

    static QString getInitials(const QString &name, const QString &defaultInitials = "JM") {
        QString trimmed = name.trimmed();
        if (trimmed.isEmpty()) return defaultInitials;
        QStringList parts = trimmed.split(' ', Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            return parts.first().left(1).toUpper() + parts.last().left(1).toUpper();
        } else if (!parts.isEmpty()) {
            return parts.first().left(2).toUpper();
        }
        return defaultInitials;
    }

    static bool saveCandidateAvatar(int candidateId, const QString &sourceImagePath, QString &outSavedPath) {
        if (candidateId <= 0 || sourceImagePath.isEmpty()) return false;
        QImage img;
        if (!img.load(sourceImagePath)) return false;

        QString root = getProjectRootDir();
        QDir rootDir(root);
        rootDir.mkpath("avatars");

        // Đảm bảo không bị lạc thư mục làm việc sau khi mở file dialog
        QDir::setCurrent(root);

        // Scale & Center Crop vuông 400x400
        QImage scaled = img.scaled(400, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int x = (scaled.width() - 400) / 2;
        int y = (scaled.height() - 400) / 2;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        int w = std::min(400, scaled.width());
        int h = std::min(400, scaled.height());
        QImage cropped = scaled.copy(x, y, w, h);

        QString targetPath = rootDir.filePath(QString("avatars/candidate_%1.png").arg(candidateId));
        bool ok = cropped.save(targetPath, "PNG");
        if (ok) {
            // Xóa các file đuôi khác nếu có
            QStringList exts = {".jpg", ".jpeg"};
            for (const QString &ext : exts) {
                QString oldPath = rootDir.filePath(QString("avatars/candidate_%1%2").arg(candidateId).arg(ext));
                if (QFile::exists(oldPath)) QFile::remove(oldPath);
            }
            outSavedPath = targetPath;
        }
        return ok;
    }

    static bool saveCompanyLogo(int companyId, const QString &sourceImagePath, QString &outSavedPath) {
        if (companyId <= 0 || sourceImagePath.isEmpty()) return false;
        QImage img;
        if (!img.load(sourceImagePath)) return false;

        QString root = getProjectRootDir();
        QDir rootDir(root);
        rootDir.mkpath("avatars");

        QDir::setCurrent(root);

        QImage scaled = img.scaled(400, 400, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int x = (scaled.width() - 400) / 2;
        int y = (scaled.height() - 400) / 2;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        int w = std::min(400, scaled.width());
        int h = std::min(400, scaled.height());
        QImage cropped = scaled.copy(x, y, w, h);

        QString targetPath = rootDir.filePath(QString("avatars/company_%1.png").arg(companyId));
        bool ok = cropped.save(targetPath, "PNG");
        if (ok) {
            QStringList exts = {".jpg", ".jpeg"};
            for (const QString &ext : exts) {
                QString oldPath = rootDir.filePath(QString("avatars/company_%1%2").arg(companyId).arg(ext));
                if (QFile::exists(oldPath)) QFile::remove(oldPath);
            }
            outSavedPath = targetPath;
        }
        return ok;
    }

    static QPixmap getCircularAvatar(const QString &imagePath, const QString &initials, int size,
                                     const QString &bgColorHex = "#EFE4DA",
                                     const QString &textColorHex = "#8A4E36") {
        int renderSize = size * 2; // Vẽ độ phân giải 2x cho màn hình Retina macOS siêu sắc nét
        QPixmap out(renderSize, renderSize);
        out.fill(Qt::transparent);
        {
            QPainter p(&out);
            p.setRenderHint(QPainter::Antialiasing, true);
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            p.setRenderHint(QPainter::TextAntialiasing, true);

            QPainterPath path;
            path.addEllipse(0, 0, renderSize, renderSize);
            p.setClipPath(path);

            // 1. Luôn đổ màu nền đặc trước để avatar luôn phủ kín 100% khung hình
            p.fillPath(path, QColor(bgColorHex));

            bool imageDrawn = false;
            if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
                QPixmap src(imagePath);
                if (!src.isNull() && src.width() > 0 && src.height() > 0) {
                    QPixmap scaled = src.scaled(renderSize, renderSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                    int x = (scaled.width() - renderSize) / 2;
                    int y = (scaled.height() - renderSize) / 2;
                    p.drawPixmap(0, 0, scaled.copy(x, y, renderSize, renderSize));
                    imageDrawn = true;
                }
            }

            if (!imageDrawn) {
                p.setPen(QColor(textColorHex));
                QFont f("Helvetica", renderSize * (initials.length() > 2 ? 0.32 : 0.38), QFont::Bold);
                p.setFont(f);
                p.drawText(QRect(0, 0, renderSize, renderSize), Qt::AlignCenter, initials);
            }
        }
        out.setDevicePixelRatio(2.0);
        return out;
    }

    static QPixmap getRoundedBadge(const QString &imagePath, const QString &initials, int size, int radius = 10,
                                   const QString &bgColorHex = "#B95334",
                                   const QString &textColorHex = "#FFFFFF") {
        int renderSize = size * 2; // Vẽ độ phân giải 2x cho màn hình Retina macOS siêu sắc nét
        int renderRadius = radius * 2;
        QPixmap out(renderSize, renderSize);
        out.fill(Qt::transparent);
        {
            QPainter p(&out);
            p.setRenderHint(QPainter::Antialiasing, true);
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            p.setRenderHint(QPainter::TextAntialiasing, true);

            QPainterPath path;
            path.addRoundedRect(0, 0, renderSize, renderSize, renderRadius, renderRadius);
            p.setClipPath(path);

            bool imageDrawn = false;
            if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
                QPixmap src(imagePath);
                if (!src.isNull() && src.width() > 0 && src.height() > 0) {
                    // Đổ nền trắng tinh khôi cho logo công ty
                    p.fillPath(path, QColor("#FFFFFF"));

                    // Tạo khoảng đệm (padding) êm ái để logo không bị chạm mép hoặc bị cắt góc bo tròn
                    int pad = qMax(4, renderSize / 14);
                    QRect targetRect(pad, pad, renderSize - 2 * pad, renderSize - 2 * pad);
                    QPixmap scaled = src.scaled(targetRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

                    int drawX = targetRect.x() + (targetRect.width() - scaled.width()) / 2;
                    int drawY = targetRect.y() + (targetRect.height() - scaled.height()) / 2;
                    p.drawPixmap(drawX, drawY, scaled);
                    imageDrawn = true;
                }
            }

            if (!imageDrawn) {
                // Đổ màu nền thương hiệu cho avatar dạng chữ viết tắt (initials)
                p.fillPath(path, QColor(bgColorHex.isEmpty() ? "#B95334" : bgColorHex));
                p.setPen(QColor(textColorHex.isEmpty() ? "#FFFFFF" : textColorHex));
                QFont f("Helvetica", renderSize * (initials.length() > 2 ? 0.32 : 0.38), QFont::Bold);
                p.setFont(f);
                p.drawText(QRect(0, 0, renderSize, renderSize), Qt::AlignCenter, initials);
            }

            // Vẽ viền bo góc sắc nét bao quanh card badge
            p.setClipping(false);
            p.setPen(QPen(QColor("#ECE5DE"), 2));
            p.drawRoundedRect(QRectF(0.5, 0.5, renderSize - 1.0, renderSize - 1.0), renderRadius, renderRadius);
        }
        out.setDevicePixelRatio(2.0);
        return out;
    }
    // Color Palette Definitions (Bám sát design_reference.png)
    // 1. Cam đất / Terracotta chủ đạo (Brand, Primary Button, Sidebar)
    inline static const QString COLOR_PRIMARY          = "#B95334";
    inline static const QString COLOR_PRIMARY_HOVER    = "#A3462A";
    inline static const QString COLOR_PRIMARY_PRESSED  = "#8C3920";

    // 2. Nền ấm & Card
    inline static const QString COLOR_BG_MAIN          = "#FBF8F5"; // Trắng kem ngà sáng
    inline static const QString COLOR_CARD_BG          = "#FFFFFF"; // Nền card trắng tinh
    inline static const QString COLOR_SIDEBAR_ACTIVE   = "#FFF4ED"; // Nền active menu cam kem
    inline static const QString COLOR_SIDEBAR_ACTIVE_TEXT = "#B95334";

    // 3. Chữ & Phụ đề
    inline static const QString COLOR_TEXT_MAIN        = "#1E1E1E";
    inline static const QString COLOR_TEXT_MUTED       = "#6C757D";

    // 4. Đường viền mỏng
    inline static const QString COLOR_BORDER           = "#ECE5DE";
    inline static const QString COLOR_BORDER_LIGHT     = "#F4EFEA";

    // 5. Trạng thái & Tag
    inline static const QString COLOR_SUCCESS_BG       = "#E3F7EB"; // Xanh lá nhạt
    inline static const QString COLOR_SUCCESS_TEXT     = "#1B7F49";
    inline static const QString COLOR_WARNING_BG       = "#FEF3E2"; // Vàng cam nhạt (đang xem xét)
    inline static const QString COLOR_WARNING_TEXT     = "#B76E00";
    inline static const QString COLOR_TAG_BG           = "#F5EBE1"; // Be nhạt tag kỹ năng
    inline static const QString COLOR_TAG_TEXT         = "#754533";

    static QString getStyleSheet() {
        return R"(
            /* Thiết lập toàn cục */
            QWidget {
                font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
                font-size: 13px;
                color: #1E1E1E;
            }

            /* Nền chính của ứng dụng: Màu kem ngà ấm */
            QMainWindow, QDialog {
                background-color: #FBF8F5;
            }

            QScrollArea {
                border: none;
                background-color: transparent;
            }

            /* ========================================================= */
            /* SIDEBAR BÊN TRÁI: Màu cam đất Terracotta (#B95334)        */
            /* ========================================================= */
            #sidebarFrame {
                background-color: #B95334;
                border: none;
            }

            #sidebarTitle {
                color: #FFFFFF;
                font-size: 18px;
                font-weight: 800;
                letter-spacing: 0.5px;
            }

            #sidebarSubtitle {
                color: #FCE8E2;
                font-size: 11px;
                font-weight: 400;
            }

            /* Nút điều hướng trong Sidebar */
            QPushButton.navButton {
                text-align: left;
                padding: 10px 14px;
                font-size: 13px;
                font-weight: 500;
                color: #FFFFFF;
                background-color: transparent;
                border: none;
                border-radius: 8px;
            }

            QPushButton.navButton:hover {
                background-color: rgba(255, 255, 255, 0.15);
                color: #FFFFFF;
            }

            /* Mục Active: Nền kem sáng bo góc, chữ cam đất nổi bật */
            QPushButton.navButton:checked, 
            QPushButton.navButton.active,
            QPushButton.navButton[active="true"] {
                color: #B95334;
                background-color: #FFF4ED;
                font-weight: 700;
            }

            /* Nút Đăng xuất ở chân Sidebar */
            QPushButton#logoutButton {
                text-align: left;
                padding: 9px 14px;
                font-size: 12px;
                font-weight: 500;
                color: #FFE6DF;
                background-color: rgba(0, 0, 0, 0.12);
                border: 1px solid rgba(255, 255, 255, 0.2);
                border-radius: 6px;
            }

            QPushButton#logoutButton:hover {
                color: #FFFFFF;
                background-color: rgba(0, 0, 0, 0.25);
            }

            /* Badge số lượng thông báo trên sidebar */
            QLabel.navBadge {
                background-color: #DE6E4B;
                color: #FFFFFF;
                font-size: 11px;
                font-weight: 700;
                border-radius: 9px;
                padding: 1px 7px;
            }

            /* ========================================================= */
            /* TOP BAR TRÊN CÙNG: Thanh tìm kiếm & Cụm User Avatar       */
            /* ========================================================= */
            #topBarFrame {
                background-color: #FBF8F5;
                border-bottom: 1px solid #ECE5DE;
            }

            #topSearchBar {
                background-color: #FFFFFF;
                border: 1px solid #ECE5DE;
                border-radius: 18px;
                padding: 7px 16px 7px 16px;
                font-size: 13px;
                color: #1E1E1E;
            }

            #topSearchBar:focus {
                border: 1.5px solid #B95334;
            }

            #userAvatarCircle {
                background-color: transparent;
                color: #FFFFFF;
                font-size: 13px;
                font-weight: 700;
                border: none;
                border-radius: 18px;
                min-width: 36px;
                max-width: 36px;
                min-height: 36px;
                max-height: 36px;
                qproperty-alignment: 'AlignCenter';
            }

            #userNameTopLabel {
                font-size: 13px;
                font-weight: 700;
                color: #1E1E1E;
            }

            #userRoleTopLabel {
                font-size: 11px;
                color: #6C757D;
            }

            /* ========================================================= */
            /* THẺ CARD TRẮNG BO TRÒN: (border-radius: 12px)              */
            /* ========================================================= */
            QFrame.cardBox, #cardBox {
                background-color: #FFFFFF;
                border: 1px solid #ECE5DE;
                border-radius: 12px;
            }

            /* ========================================================= */
            /* NÚT BẤM (BUTTONS)                                         */
            /* ========================================================= */
            /* Primary Button (Khối cam đất) */
            QPushButton.primaryBtn, QPushButton.primaryButton {
                background-color: #B95334;
                color: #FFFFFF;
                border: none;
                border-radius: 8px;
                padding: 8px 18px;
                font-size: 13px;
                font-weight: 600;
            }

            QPushButton.primaryBtn:hover, QPushButton.primaryButton:hover {
                background-color: #A3462A;
            }

            QPushButton.primaryBtn:pressed, QPushButton.primaryButton:pressed {
                background-color: #8C3920;
            }

            /* Outline Button (Viền cam đất mảnh) */
            QPushButton.outlineBtn, QPushButton.secondaryButton {
                background-color: transparent;
                color: #B95334;
                border: 1px solid #B95334;
                border-radius: 8px;
                padding: 7px 15px;
                font-size: 13px;
                font-weight: 600;
            }

            QPushButton.outlineBtn:hover, QPushButton.secondaryButton:hover {
                background-color: #FFF4ED;
                color: #B95334;
                border-color: #B95334;
            }

            /* Tag Pill Button / Label (Kỹ năng) */
            .tagPill {
                background-color: #F5EBE1;
                color: #754533;
                border: none;
                border-radius: 12px;
                padding: 4px 10px;
                font-size: 11px;
                font-weight: 600;
            }

            /* ========================================================= */
            /* HUY HIỆU TRẠNG THÁI (STATUS BADGES)                       */
            /* ========================================================= */
            /* Huy hiệu Match Score xanh lá */
            #matchScoreBox {
                background-color: #E3F7EB;
                border-radius: 8px;
                padding: 6px 12px;
            }

            #matchScoreNum {
                color: #1B7F49;
                font-size: 18px;
                font-weight: 800;
            }

            #matchScoreText {
                color: #1B7F49;
                font-size: 10px;
                font-weight: 600;
            }

            /* Status: Đang tuyển / Đã gửi / Thành công */
            .badgeSuccess {
                background-color: #E3F7EB;
                color: #1B7F49;
                font-size: 11px;
                font-weight: 600;
                border-radius: 6px;
                padding: 3px 8px;
            }

            /* Status: Đang xem xét / Chờ duyệt */
            .badgeWarning {
                background-color: #FEF3E2;
                color: #B76E00;
                font-size: 11px;
                font-weight: 600;
                border-radius: 6px;
                padding: 3px 8px;
            }

            /* Status: Đã đóng / Từ chối */
            .badgeDanger {
                background-color: #FEE2E2;
                color: #DC2626;
                font-size: 11px;
                font-weight: 600;
                border-radius: 6px;
                padding: 3px 8px;
            }

            /* ========================================================= */
            /* THANH TIẾN ĐỘ (PROGRESS BAR)                              */
            /* ========================================================= */
            QProgressBar {
                border: none;
                border-radius: 4px;
                background-color: #ECE5DE;
                text-align: right;
                min-height: 8px;
                max-height: 8px;
            }

            QProgressBar::chunk {
                background-color: #B95334;
                border-radius: 4px;
            }

            /* ========================================================= */
            /* INPUTS & FORMS                                            */
            /* ========================================================= */
            QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit, QTextEdit {
                background-color: #FFFFFF;
                border: 1px solid #ECE5DE;
                border-radius: 6px;
                padding: 7px 10px;
                color: #1E1E1E;
            }

            QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDateEdit:focus, QTextEdit:focus {
                border: 1.5px solid #B95334;
                outline: none;
            }

            /* Menu xổ xuống của ComboBox: Khóa nền trắng và chữ đen rõ nét */
            QComboBox::drop-down {
                border: none;
                width: 22px;
            }

            QComboBox QAbstractItemView {
                background-color: #FFFFFF;
                color: #1E1E1E;
                border: 1px solid #ECE5DE;
                border-radius: 6px;
                padding: 4px;
                outline: none;
                selection-background-color: #FFF4ED;
                selection-color: #B95334;
            }

            QComboBox QAbstractItemView::item {
                color: #1E1E1E;
                padding: 6px 10px;
                border-radius: 4px;
                min-height: 24px;
            }

            QComboBox QAbstractItemView::item:hover,
            QComboBox QAbstractItemView::item:selected {
                background-color: #FFF4ED;
                color: #B95334;
            }

            /* ========================================================= */
            /* BẢNG BIỂU & DANH SÁCH (TABLE & LIST WIDGETS)              */
            /* ========================================================= */
            QTableWidget, QTableView {
                background-color: #FFFFFF;
                color: #1E1E1E;
                gridline-color: #ECE5DE;
                border: 1px solid #ECE5DE;
                border-radius: 8px;
                outline: none;
            }

            QTableWidget::item, QTableView::item {
                color: #1E1E1E;
                padding: 6px 10px;
                background-color: #FFFFFF;
            }

            QTableWidget::item:selected, QTableView::item:selected {
                background-color: #FFF4ED;
                color: #B95334;
                font-weight: 600;
            }

            QHeaderView::section {
                background-color: #F8F4F0;
                color: #1E1E1E;
                font-weight: 700;
                font-size: 12px;
                border: none;
                border-bottom: 1.5px solid #ECE5DE;
                padding: 6px 10px;
            }

            QListWidget, QListView {
                background-color: #FFFFFF;
                color: #1E1E1E;
                border: 1px solid #ECE5DE;
                border-radius: 8px;
                padding: 4px;
                outline: none;
            }

            QListWidget::item, QListView::item {
                color: #1E1E1E;
                background-color: #FFFFFF;
                padding: 6px 10px;
                border-radius: 6px;
                margin-bottom: 2px;
            }

            QListWidget::item:hover, QListView::item:hover {
                background-color: #F8F4F0;
                color: #1E1E1E;
            }

            QListWidget::item:selected, QListView::item:selected {
                background-color: #FFF4ED;
                color: #B95334;
                font-weight: 600;
            }

            /* ========================================================= */
            /* THẺ TAB (TAB WIDGET & TAB BAR)                            */
            /* ========================================================= */
            QTabWidget::pane {
                border: 1px solid #ECE5DE;
                border-radius: 8px;
                background-color: #FFFFFF;
                top: -1px;
            }

            QTabBar::tab {
                background-color: #F5EBE1;
                color: #6C757D;
                font-weight: 600;
                font-size: 13px;
                padding: 9px 20px;
                min-width: 140px;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                margin-right: 6px;
                border: 1px solid #ECE5DE;
                border-bottom: none;
            }

            QTabBar::tab:selected {
                background-color: #B95334;
                color: #FFFFFF;
                font-weight: 700;
            }

            QTabBar::tab:hover:!selected {
                background-color: #EFE4DA;
                color: #1E1E1E;
            }

            /* ========================================================= */
            /* MENU NGỮ CẢNH (CONTEXT MENU / 3 CHẤM)                     */
            /* ========================================================= */
            QMenu {
                background-color: #FFFFFF;
                color: #1E1E1E;
                border: 1px solid #ECE5DE;
                border-radius: 8px;
                padding: 6px 0px;
            }

            QMenu::item {
                background-color: transparent;
                color: #1E1E1E;
                padding: 8px 24px 8px 16px;
                font-size: 13px;
            }

            QMenu::item:selected {
                background-color: #FFF4ED;
                color: #B95334;
                font-weight: 600;
            }

            QMenu::separator {
                height: 1px;
                background-color: #ECE5DE;
                margin: 4px 8px;
            }

            /* ========================================================= */
            /* SCROLLBAR TINH TẾ                                         */
            /* ========================================================= */
            QScrollBar:vertical {
                border: none;
                background: transparent;
                width: 8px;
                margin: 0px;
            }

            QScrollBar::handle:vertical {
                background: #D8CECA;
                min-height: 30px;
                border-radius: 4px;
            }

            QScrollBar::handle:vertical:hover {
                background: #B95334;
            }

            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
        )";
    }
};

#endif 
