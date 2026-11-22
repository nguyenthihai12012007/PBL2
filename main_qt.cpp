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

    app.setApplicationName("JobMatch");
    app.setApplicationDisplayName("JobMatch — Quản lý việc làm");
    app.setOrganizationName("DUT_PBL2");

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

    app.setStyleSheet(StyleHelper::getStyleSheet());

    Data dt;

    LoginWindow loginWindow(dt);
    MainWindow mainWindow(dt);

    QObject::connect(&loginWindow, &LoginWindow::loginSuccess,
                     [&loginWindow, &mainWindow](int userId, const QString &email, const QString &role) {
        mainWindow.setSession(userId, email, role);
        mainWindow.showMaximized();
        loginWindow.hide();
    });

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

    if (app.arguments().contains("--screenshot")) {
        loginWindow.resize(1080, 630);
        loginWindow.show();
        app.processEvents();
        QPixmap pix = loginWindow.grab();
        pix.save("login_preview.png");

        if (app.arguments().contains("--reg")) {
            if (QPushButton *btn = loginWindow.findChild<QPushButton*>("switchToRegBtn")) {
                btn->click();
            }
            app.processEvents();
            QPixmap pixReg = loginWindow.grab();
            pixReg.save("register_preview.png");

            if (QComboBox *combo = loginWindow.findChild<QComboBox*>("regRoleCombo")) {
                combo->setCurrentIndex(1);
            }
            app.processEvents();
            QPixmap pixReg2 = loginWindow.grab();
            pixReg2.save("register_company_preview.png");

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
