#include "MainWindow.h"
#include <QHBoxLayout>
#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QAbstractItemView>
#include <QListView>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. Initialize Backend
    cairoMetro.buildSystem("metro_data.txt");

    stack = new QStackedWidget();

    // --- PAGE 1: START SCREEN ---
    auto *startPage = new QWidget();
    startPage->setObjectName("startPage");

    auto *startPageLayout = new QVBoxLayout(startPage);
    startPageLayout->setContentsMargins(0, 0, 0, 0);

    auto *overlay = new QWidget();
    overlay->setObjectName("startOverlay");
    auto *overlayLayout = new QVBoxLayout(overlay);

    // --- LOGO SETUP ---
    QLabel *logoLabel = new QLabel();
    QPixmap pix(":/logo3.png");

    if (!pix.isNull()) {
        // Scaled to width of 600px for full-screen feel
        logoLabel->setPixmap(pix.scaledToWidth(600, Qt::SmoothTransformation));
    }
    logoLabel->setAlignment(Qt::AlignCenter);

    QPushButton *startBtn = new QPushButton("Start Journey");
    startBtn->setFixedWidth(280);
    // UPDATED: Changed object name to differentiate this button
    startBtn->setObjectName("startJourneyButton");

    // --- POSITIONING (PUSHES CONTENT HIGHER) ---
    overlayLayout->addStretch(1);
    overlayLayout->addWidget(logoLabel);
    overlayLayout->addSpacing(10);
    overlayLayout->addWidget(startBtn, 0, Qt::AlignCenter);
    overlayLayout->addStretch(1);

    startPageLayout->addWidget(overlay);

    // --- PAGE 2: NAVIGATION SCREEN ---
    auto *navPage = new QWidget();
    auto *mainLayout = new QVBoxLayout(navPage);
    mainLayout->setContentsMargins(25, 30, 25, 25);
    mainLayout->setSpacing(0);

    auto *inputContainer = new QWidget();
    auto *inputLayout = new QVBoxLayout(inputContainer);
    inputLayout->setContentsMargins(0, 0, 0, 10);
    inputLayout->setSpacing(5);

    QLabel *lblFrom = new QLabel("From"); lblFrom->setObjectName("fieldLabel");
    fromCombo = new QComboBox(); fromCombo->setEditable(true); fromCombo->setPlaceholderText("Departure Station");

    auto *swapLayout = new QHBoxLayout();
    QPushButton *swapBtn = new QPushButton("⇅"); swapBtn->setFixedSize(45, 45); swapBtn->setObjectName("swapButton");

    auto *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15); shadow->setOffset(0, 4); shadow->setColor(QColor(0, 0, 0, 150));
    swapBtn->setGraphicsEffect(shadow);

    swapLayout->addStretch();
    swapLayout->addWidget(swapBtn);
    swapLayout->setContentsMargins(0, -20, 20, -20);

    QLabel *lblTo = new QLabel("To"); lblTo->setObjectName("fieldLabel");
    toCombo = new QComboBox(); toCombo->setEditable(true); toCombo->setPlaceholderText("Destination Station");

    auto stations = cairoMetro.getAllStationNames();
    for(const auto& s : stations) {
        fromCombo->addItem(QString::fromStdString(s));
        toCombo->addItem(QString::fromStdString(s));
    }

    QString listStyle = "QListView { background-color: #1A2F29; color: white; border: 1px solid #22D978; selection-background-color: #376754; selection-color: #22D978; }";

    fromCombo->completer()->setCompletionMode(QCompleter::PopupCompletion);
    fromCombo->completer()->popup()->setStyleSheet(listStyle);
    fromCombo->view()->setStyleSheet(listStyle);

    toCombo->completer()->setCompletionMode(QCompleter::PopupCompletion);
    toCombo->completer()->popup()->setStyleSheet(listStyle);
    toCombo->view()->setStyleSheet(listStyle);

    inputLayout->addWidget(lblFrom); inputLayout->addWidget(fromCombo);
    inputLayout->addLayout(swapLayout); inputLayout->addWidget(lblTo); inputLayout->addWidget(toCombo);

    QPushButton *btn = new QPushButton("Get details");
    // UPDATED: Changed object name to differentiate this button
    btn->setObjectName("getDetailsButton");

    resultsArea = new QWidget(); resultsArea->setObjectName("resultsArea");
    auto *resLayout = new QHBoxLayout(resultsArea);
    resLayout->setContentsMargins(15, 25, 15, 25);

    timeLabel = new QLabel("<b>--</b><br>Min");
    stationLabel = new QLabel("<b>--</b><br>Stations");
    priceLabel = new QLabel("<b>--</b><br>Price");

    resLayout->addWidget(timeLabel); resLayout->addWidget(stationLabel); resLayout->addWidget(priceLabel);

    descLabel = new QLabel(""); descLabel->setObjectName("descLabel");

    auto *scroll = new QScrollArea();
    auto *content = new QWidget(); content->setObjectName("scrollContent");
    routeLayout = new QVBoxLayout(content); routeLayout->setAlignment(Qt::AlignTop);
    scroll->setWidget(content); scroll->setWidgetResizable(true); scroll->setFrameShape(QFrame::NoFrame);

    mainLayout->addWidget(inputContainer);
    mainLayout->addWidget(btn);
    mainLayout->addWidget(resultsArea);
    mainLayout->addWidget(descLabel);
    mainLayout->addWidget(scroll);

    stack->addWidget(startPage);
    stack->addWidget(navPage);
    setCentralWidget(stack);

    resize(450, 850);
    setupStyles();

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::nextPage);
    connect(btn, &QPushButton::clicked, this, &MainWindow::handleGetDetails);
    connect(swapBtn, &QPushButton::clicked, [this]() {
        QString temp = fromCombo->currentText();
        fromCombo->setCurrentText(toCombo->currentText());
        toCombo->setCurrentText(temp);
    });
}

void MainWindow::nextPage() {
    int nextIndex = (stack->currentIndex() + 1) % stack->count();
    QWidget *nextWidget = stack->widget(nextIndex);

    auto *effect = new QGraphicsOpacityEffect(this);
    nextWidget->setGraphicsEffect(effect);

    auto *animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(400);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    stack->setCurrentIndex(nextIndex);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, [nextWidget]() {
        nextWidget->setGraphicsEffect(nullptr);
    });
}

QWidget* MainWindow::createDirectionHeader(const QString& text) {
    auto *lbl = new QLabel(text);
    lbl->setStyleSheet("color: #22D978; font-weight: bold; font-size: 14px; padding: 12px 15px; background: #142621; border: 1px solid #22D978; border-radius: 8px; margin: 5px 0;");
    return lbl;
}

void MainWindow::handleGetDetails() {
    RouteInfo info = cairoMetro.getRouteDetails(fromCombo->currentText().toStdString(), toCombo->currentText().toStdString());
    if(info.pathNames.empty()) return;

    timeLabel->setText("<span style='font-size:24px; color:#22D978;'><b>" + QString::number(info.timeEstimate) + "</b></span><br><span style='color:#A0B3AB;'>Min</span>");
    stationLabel->setText("<span style='font-size:24px; color:#22D978;'><b>" + QString::number(info.stationCount) + "</b></span><br><span style='color:#A0B3AB;'>Stations</span>");
    priceLabel->setText("<span style='font-size:24px; color:#22D978;'><b>" + QString::number(info.price) + "</b></span><br><span style='color:#A0B3AB;'>EGP</span>");

    QLayoutItem *item;
    while((item = routeLayout->takeAt(0)) != nullptr) {
        if(item->widget()) delete item->widget();
        delete item;
    }

    routeLayout->addWidget(createDirectionHeader("Start: " + QString::fromStdString(info.initialDirection)));

    QString description = QString::fromStdString(info.directionDescription);
    QString transferStation = "";
    if(description.contains("Transfer at ")) {
        int start = description.indexOf("at ") + 3;
        int end = description.indexOf(" towards");
        if(end != -1 && start > 2) transferStation = description.mid(start, end - start).trimmed();
        descLabel->hide();
    } else {
        descLabel->setText(description); descLabel->show();
    }

    for(int i = 0; i < (int)info.pathNames.size(); ++i) {
        QString currentName = QString::fromStdString(info.pathNames[i]);
        bool isTransfer = (currentName == transferStation);
        QString rowNote = isTransfer ? description : "";
        routeLayout->addWidget(createStationRow(currentName, i==0, i==(int)info.pathNames.size()-1, rowNote));
        if(isTransfer && !info.afterTransferDirection.empty()) {
            routeLayout->addWidget(createDirectionHeader("Switch to: " + QString::fromStdString(info.afterTransferDirection)));
        }
    }
    routeLayout->addStretch();
}

QWidget* MainWindow::createStationRow(const QString& name, bool isFirst, bool isLast, const QString& note) {
    auto *w = new QWidget(); auto *lay = new QHBoxLayout(w);
    lay->setContentsMargins(15, 8, 15, 8);
    auto *dot = new QLabel(isFirst || isLast ? "●" : "○");
    dot->setStyleSheet(QString("color: %1; font-size: 20px;").arg(isFirst || isLast ? "#22D978" : "#376754"));
    auto *nameLbl = new QLabel(name); nameLbl->setStyleSheet("font-size: 15px; color: #EAEAEA; font-weight: 600;");
    lay->addWidget(dot); lay->addWidget(nameLbl);

    if(!note.isEmpty()) {
        auto *noteLbl = new QLabel(" " + note + " ");
        noteLbl->setStyleSheet("font-size: 12px; color: #22D978; font-weight: bold; border: 2px solid #22D978; border-radius: 10px; background-color: #142621; padding: 6px; margin-left: 10px;");
        lay->addWidget(noteLbl);
    }
    lay->addStretch();
    return w;
}

void MainWindow::setupStyles() {
    // UPDATED Stylesheet to handle distinct button colors
    this->setStyleSheet(
        "QMainWindow { background-color: #0D1F1A; }"
        "QWidget#startPage { background-color: #0D1F1A; }"
        "QWidget#scrollContent { background-color: #0D1F1A; }"
        "QLabel#fieldLabel { color: #A0B3AB; font-weight: bold; font-size: 13px; margin-left: 5px; }"
        "QLabel#descLabel { color: #22D978; font-weight: bold; margin: 10px 0; qproperty-alignment: AlignCenter; }"
        "QComboBox { background-color: #1A2F29; border: 1px solid #376754; border-radius: 12px; padding: 12px; font-size: 14px; color: #FFFFFF; }"
        "QComboBox::drop-down { border: 0px; width: 0px; }"
        "QPushButton#swapButton { background-color: #22D978; color: #0D1F1A; border-radius: 22px; border: 2px solid #0D1F1A; font-size: 18px; font-weight: bold; }"

        // Shared styles for both big action buttons
        "QPushButton#startJourneyButton, QPushButton#getDetailsButton { color: #0D1F1A; border-radius: 15px; padding: 18px; font-weight: bold; font-size: 18px; margin: 10px 0; }"

        // Specific color for Start Journey (Page 1) - #2DB180
        "QPushButton#startJourneyButton { background-color: #2DB180; }"

        // Specific color for Get Details (Page 2) - #22D978
        "QPushButton#getDetailsButton { background-color: #22D978; }"

        "QWidget#resultsArea { background-color: #1A2F29; border-radius: 20px; margin: 20px 0; border: 1px solid #22D97833; }"
        "QWidget#resultsArea QLabel { color: #EAEAEA; font-size: 14px; qproperty-alignment: AlignCenter; }"
    );
}