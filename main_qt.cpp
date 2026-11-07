#include <QApplication>
#include <QDir>
#include <QFile>
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
        mainWindow.show();
        loginWindow.hide();
    });

    // Connect Logout -> Return to LoginWindow
    QObject::connect(&mainWindow, &MainWindow::logoutRequested,
                     [&loginWindow, &mainWindow]() {
        mainWindow.hide();
        loginWindow.show();
    });

    // Initial view: Show Login Window
    loginWindow.show();

    return app.exec();
}
