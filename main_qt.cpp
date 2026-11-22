#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractItemView>
#include <QDir>
#include <QFile>
#include <QFrame>
#include "LoginWindow.h"
#include "MainWindow.h"
#include "StyleHelper.h"
#include "class.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Application metadata
    app.setApplicationName("JobMatch");
    app.setApplicationDisplayName("JobMatch — Quản lý việc làm");
    app.setOrganizationName("DUT_PBL2");

    // Ensure working directory is the project root containing data files (.txt)
    QStringList searchDirs = {
        QDir::currentPath(),
        app.applicationDirPath(),
        QDir(app.applicationDirPath()).filePath(".."),
        QDir(app.applicationDirPath()).filePath("../.."),
        QDir(app.applicationDirPath()).filePath("../../..")
    };

    for (const QString &dirPath : searchDirs) {
        if (QFile::exists(QDir(dirPath).filePath("users.txt"))) {
            QDir::setCurrent(dirPath);
            break;
        }
    }

    // Apply global modern stylesheet
    app.setStyleSheet(StyleHelper::getStyleSheet());

    // Initialize core business data repository
    Data dt;

    // Create main windows with shared Data repository
    LoginWindow loginWindow(dt);
    MainWindow mainWindow(dt);

    // Connect Login Success -> Show MainWindow with authenticated user session
    QObject::connect(&loginWindow, &LoginWindow::loginSuccess,
                     [&loginWindow, &mainWindow](int userId, const QString &email, const QString &role) {
        mainWindow.setSession(userId, email, role);
        mainWindow.showMaximized();
        loginWindow.hide();
    });

    // Connect Logout -> Return to LoginWindow
    QObject::connect(&mainWindow, &MainWindow::logoutRequested,
                     [&loginWindow, &mainWindow]() {
        mainWindow.hide();
        loginWindow.show();
    });

    if (app.arguments().contains("--dump-main")) {
        mainWindow.setSession(13, "hr@abctechnology.com", "COMPANY");
        mainWindow.resize(1280, 800);
        mainWindow.show();
        app.processEvents();
        QPixmap pixMain = mainWindow.grab();
        pixMain.save("/tmp/mainwindow_preview.png");
        return 0;
    }

    // Initial view: Show Login Window in optimized desktop window mode
    if (app.arguments().contains("--screenshot")) {
        loginWindow.resize(1080, 630);
        loginWindow.show();
        app.processEvents();
        QPixmap pix = loginWindow.grab();
        pix.save("login_preview.png");

        if (app.arguments().contains("--reg")) {
            // Test register screen
            if (QPushButton *btn = loginWindow.findChild<QPushButton*>("switchToRegBtn")) {
                btn->click();
            }
            app.processEvents();
            QPixmap pixReg = loginWindow.grab();
            pixReg.save("register_preview.png");

            // Also test with "Doanh nghiệp tuyển dụng" selected
            if (QComboBox *combo = loginWindow.findChild<QComboBox*>("regRoleCombo")) {
                combo->setCurrentIndex(1);
            }
            app.processEvents();
            QPixmap pixReg2 = loginWindow.grab();
            pixReg2.save("register_company_preview.png");

            // Also capture popup dropdown list view for testing
            if (QComboBox *combo = loginWindow.findChild<QComboBox*>("regRoleCombo")) {
                combo->showPopup();
                app.processEvents();
                if (QWidget *top = QApplication::activePopupWidget()) {
                    top->grab().save("combobox_dropdown_preview.png");
                } else if (combo->view()) {
                    combo->view()->grab().save("combobox_dropdown_preview.png");
                }
                combo->hidePopup();
            }
        }
        return 0;
    }

    loginWindow.show();

    return app.exec();
}
