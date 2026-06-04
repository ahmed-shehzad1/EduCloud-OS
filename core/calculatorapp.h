#ifndef CALCULATORAPP_H
#define CALCULATORAPP_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>

class CalculatorApp : public QWidget {
    Q_OBJECT

public:
    explicit CalculatorApp(QWidget *parent = nullptr);

private slots:
    void buttonPressed();

private:
    QLineEdit *display;
    QString currentExpression;
};

#endif // CALCULATORAPP_H