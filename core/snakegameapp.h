#ifndef SNAKEGAMEAPP_H
#define SNAKEGAMEAPP_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QList>
#include <QPoint>

class SnakeGameApp : public QWidget {
    Q_OBJECT

public:
    explicit SnakeGameApp(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    void spawnFood();
    void checkCollision();

    QTimer *timer;
    QList<QPoint> snake;
    QPoint food;
    char direction; // 'U', 'D', 'L', 'R'
    bool gameOver;
    int score;
};

#endif // SNAKEGAMEAPP_H