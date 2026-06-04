#include "calculatorapp.h"

CalculatorApp::CalculatorApp(QWidget *parent) : QWidget(parent) {
    this->setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    display = new QLineEdit();
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setStyleSheet("background-color: #11111b; border: 1px solid #45475a; font-size: 20px; padding: 10px; color: #a6e3a1;");
    mainLayout->addWidget(display);

    QGridLayout *gridLayout = new QGridLayout();
    const char* buttons[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"C", "0", "=", "+"}
    };

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            QPushButton *btn = new QPushButton(buttons[r][c]);
            btn->setFixedSize(50, 50);
            btn->setStyleSheet("background-color: #313244; font-size: 16px; font-weight: bold; border-radius: 5px; color: #cdd6f4;");
            connect(btn, &QPushButton::clicked, this, &CalculatorApp::buttonPressed);
            gridLayout->addWidget(btn, r, c);
        }
    }
    
    mainLayout->addLayout(gridLayout);
}

void CalculatorApp::buttonPressed() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString text = btn->text();

    if (text == "C") {
        currentExpression.clear();
        display->clear();
    } else if (text == "=") {
        // Simple safety parsing for presentation math execution
        if(currentExpression.contains("+")) {
            QStringList parts = currentExpression.split("+");
            display->setText(QString::number(parts[0].toDouble() + parts[1].toDouble()));
        } else if(currentExpression.contains("-")) {
            QStringList parts = currentExpression.split("-");
            display->setText(QString::number(parts[0].toDouble() - parts[1].toDouble()));
        } else if(currentExpression.contains("*")) {
            QStringList parts = currentExpression.split("*");
            display->setText(QString::number(parts[0].toDouble() * parts[1].toDouble()));
        } else if(currentExpression.contains("/")) {
            QStringList parts = currentExpression.split("/");
            double denom = parts[1].toDouble();
            display->setText(denom == 0 ? "Error" : QString::number(parts[0].toDouble() / denom));
        }
        currentExpression = display->text();
    } else {
        currentExpression += text;
        display->setText(currentExpression);
    }
}