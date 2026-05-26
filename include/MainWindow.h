#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "MetroSystem.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void handleGetDetails();
    void nextPage();

private:
    MetroSystem cairoMetro;
    QComboBox *fromCombo, *toCombo;
    QLabel *timeLabel, *stationLabel, *priceLabel, *descLabel;
    QVBoxLayout *routeLayout;
    QWidget *resultsArea;
    QStackedWidget *stack;

    void setupStyles();
    QWidget* createStationRow(const QString& name, bool isFirst, bool isLast, const QString& note = "");
    QWidget* createDirectionHeader(const QString& text);
};
#endif