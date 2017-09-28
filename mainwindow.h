#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *GS;
    QString filename;

private slots:
    void on_action_open_triggered();
    void on_action_quit_triggered();
    void on_action_about_triggered();
    void drawItem(QListWidgetItem*);
};

#endif // MAINWINDOW_H
