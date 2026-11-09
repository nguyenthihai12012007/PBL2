#include "CustomMessageBox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QApplication>

CustomMessageBox::CustomMessageBox(QWidget *parent,
                                   const QString &title,
                                   const QString &message,
                                   IconType icon,
                                   bool isQuestion,
                                   const QString &yesText,
                                   const QString &noText)
    : QDialog(parent) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI(title, message, icon, isQuestion, yesText, noText);
}

void CustomMessageBox::setupUI(const QString &title,
                               const QString &message,
                               IconType icon,
                               bool isQuestion,
                               const QString &yesText,
                               const QString &noText) {
    // Outer layout with padding for shadow
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(16, 16, 16, 16);
    outerLayout->setSizeConstraint(QLayout::SetFixedSize);

    // Card container
    QFrame *card = new QFrame(this);
    card->setObjectName("dialogCard");
    card->setStyleSheet(
        "#dialogCard {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #ECE5DE;"
        "   border-radius: 14px;"
        "}"
    );

    // Soft drop shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(22);
    shadow->setColor(QColor(0, 0, 0, 40));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 22, 24, 20);
    cardLayout->setSpacing(12);

    // 1. Icon Badge
    QLabel *iconBadge = new QLabel(card);
    iconBadge->setFixedSize(48, 48);
    iconBadge->setAlignment(Qt::AlignCenter);

    QString badgeStyle = "font-size: 20px; font-weight: 800; border-radius: 24px;";
    QString iconChar;

    switch (icon) {
    case Success:
        iconChar = "✓";
        badgeStyle += "background-color: #E3F7EB; color: #1B7F49;";
        break;
    case Warning:
        iconChar = "!";
        badgeStyle += "background-color: #FEF3E2; color: #B76E00;";
        break;
    case Critical:
        iconChar = "✕";
        badgeStyle += "background-color: #FEE2E2; color: #DC2626;";
        break;
    case Question:
        iconChar = "?";
        badgeStyle += "background-color: #FCE8E2; color: #B95334;";
        break;
    case Information:
    default:
        iconChar = "ℹ";
        badgeStyle += "background-color: #E0F2FE; color: #0284C7;";
        break;
    }

    iconBadge->setText(iconChar);
    iconBadge->setStyleSheet(badgeStyle);

    QHBoxLayout *iconCenterLayout = new QHBoxLayout();
    iconCenterLayout->addStretch();
    iconCenterLayout->addWidget(iconBadge);
    iconCenterLayout->addStretch();
    cardLayout->addLayout(iconCenterLayout);

    // 2. Title
    QLabel *titleLabel = new QLabel(title, card);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #1E1E1E;");
    cardLayout->addWidget(titleLabel);

    // 3. Message Text
    QLabel *msgLabel = new QLabel(message, card);
    msgLabel->setWordWrap(true);
    msgLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    if (message.contains('\n') || message.contains("•")) {
        msgLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        msgLabel->setStyleSheet("font-size: 13px; color: #374151; padding: 4px 8px; line-height: 1.4;");
    } else {
        msgLabel->setAlignment(Qt::AlignCenter);
        msgLabel->setStyleSheet("font-size: 13px; color: #4A4A4A; padding: 2px 4px;");
    }
    cardLayout->addWidget(msgLabel);

    cardLayout->addSpacing(6);

    // 4. Buttons Layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(10);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    if (isQuestion) {
        QPushButton *noBtn = new QPushButton(noText, card);
        noBtn->setCursor(Qt::PointingHandCursor);
        noBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: transparent;"
            "   color: #6C757D;"
            "   border: 1px solid #ECE5DE;"
            "   border-radius: 8px;"
            "   font-size: 13px;"
            "   font-weight: 600;"
            "   padding: 8px 20px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #F4EBE2;"
            "   color: #1E1E1E;"
            "}"
        );
        connect(noBtn, &QPushButton::clicked, this, &QDialog::reject);

        QPushButton *yesBtn = new QPushButton(yesText, card);
        yesBtn->setCursor(Qt::PointingHandCursor);
        yesBtn->setDefault(true);
        yesBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #B95334;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 8px;"
            "   font-size: 13px;"
            "   font-weight: 600;"
            "   padding: 8px 24px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #A3452B;"
            "}"
        );
        connect(yesBtn, &QPushButton::clicked, this, &QDialog::accept);

        buttonsLayout->addWidget(noBtn);
        buttonsLayout->addWidget(yesBtn);
    } else {
        QPushButton *okBtn = new QPushButton("Đã hiểu", card);
        okBtn->setCursor(Qt::PointingHandCursor);
        okBtn->setDefault(true);
        okBtn->setFixedWidth(140);
        okBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #B95334;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 8px;"
            "   font-size: 13px;"
            "   font-weight: 600;"
            "   padding: 8px 24px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #A3452B;"
            "}"
        );
        connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
        buttonsLayout->addWidget(okBtn);
    }

    cardLayout->addLayout(buttonsLayout);
    outerLayout->addWidget(card);

    setFixedWidth(480);
}

void CustomMessageBox::information(QWidget *parent, const QString &title, const QString &message) {
    CustomMessageBox box(parent, title, message, Information, false);
    box.exec();
}

void CustomMessageBox::success(QWidget *parent, const QString &title, const QString &message) {
    CustomMessageBox box(parent, title, message, Success, false);
    box.exec();
}

void CustomMessageBox::warning(QWidget *parent, const QString &title, const QString &message) {
    CustomMessageBox box(parent, title, message, Warning, false);
    box.exec();
}

void CustomMessageBox::critical(QWidget *parent, const QString &title, const QString &message) {
    CustomMessageBox box(parent, title, message, Critical, false);
    box.exec();
}

bool CustomMessageBox::question(QWidget *parent, const QString &title, const QString &message,
                                const QString &yesText, const QString &noText) {
    CustomMessageBox box(parent, title, message, Question, true, yesText, noText);
    return box.exec() == QDialog::Accepted;
}
