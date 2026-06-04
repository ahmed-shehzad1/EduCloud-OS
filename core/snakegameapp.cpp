#include "snakegameapp.h"
#include <QPainter>
#include <QRandomGenerator>

SnakeGameApp::SnakeGameApp(QWidget *parent) : QWidget(parent) {
    this->setFixedSize(400, 400);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setStyleSheet("background-color: #11111b;");
    
    direction = 'R';
    gameOver = false;
    score = 0;

    snake.append(QPoint(5, 5));
    snake.append(QPoint(4, 5));
    
    spawnFood();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SnakeGameApp::gameLoop);
    timer->start(150); // Speed of game tick
}

void SnakeGameApp::spawnFood() {
    int x = QRandomGenerator::global()->bounded(20);
    int y = QRandomGenerator::global()->bounded(20);
    food = QPoint(x, y);
}

void SnakeGameApp::gameLoop() {
    if (gameOver) return;

    QPoint head = snake.first();

    if (direction == 'U') head.setY(head.y() - 1);
    else if (direction == 'D') head.setY(head.y() + 1);
    else if (direction == 'L') head.setX(head.x() - 1);
    else if (direction == 'R') head.setX(head.x() + 1);

    snake.prepend(head);

    if (head == food) {
        score += 10;
        spawnFood();
    } else {
        snake.removeLast();
    }

    checkCollision();
    update();
}

void SnakeGameApp::checkCollision() {
    QPoint head = snake.first();

    // Check Wall Collisions
    if (head.x() < 0 || head.x() >= 20 || head.y() < 0 || head.y() >= 20) {
        gameOver = true;
    }

    // Check Self Collisions
    for (int i = 1; i < snake.size(); ++i) {
        if (head == snake[i]) {
            gameOver = true;
        }
    }
}

void SnakeGameApp::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    if (gameOver) {
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER\nScore: " + QString::number(score));
        return;
    }

    // Draw Food
    painter.setBrush(QBrush(QColor("#f38ba8")));
    painter.drawRect(food.x() * 20, food.y() * 20, 18, 18);

    // Draw Snake
    painter.setBrush(QBrush(QColor("#a6e3a1")));
    for (const auto &p : snake) {
        painter.drawRect(p.x() * 20, p.y() * 20, 18, 18);
    }
}

void SnakeGameApp::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Qt::Key_Up && direction != 'D') direction = 'U';
    else if (key == Qt::Key_Down && direction != 'U') direction = 'D';
    else if (key == Qt::Key_Left && direction != 'R') direction = 'L';
    else if (key == Qt::Key_Right && direction != 'L') direction = 'R';
}