#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QSvgWidget>

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
    QString filename,tagxml,tagsvg;
    QStringList lines;
    QSvgWidget *SVGW;

private slots:
    void on_action_open_triggered();
    void on_action_quit_triggered();
    void on_action_about_triggered();
    void on_pushButtonModify_clicked();
    void on_pushButtonDelete_clicked();
    void drawItem(QListWidgetItem*);
    void preview();
};

#endif // MAINWINDOW_H
