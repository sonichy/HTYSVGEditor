#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QXmlStreamReader>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filename="";
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
    GS = new QGraphicsScene;
//    QString element = "<?xml version='1.0' ?><svg width='128' height='128' version='1.1' xmlns='http://www.w3.org/2000/svg'>";
//    element += "<polygon points='10,0 10,128 118,128 118,20 98,0' style='fill:#2C53B8'/>";
//    element += "</svg>";
//    QXmlStreamReader *XMLSR = new QXmlStreamReader(element);
//    QSvgRenderer *SVGR = new QSvgRenderer(XMLSR);
//    QGraphicsSvgItem *GSVGI = new QGraphicsSvgItem;
//    GSVGI->setSharedRenderer(SVGR);
//    GS->addItem(GSVGI);
    ui->graphicsView->setScene(GS);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(drawItem(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_quit_triggered()
{
    qApp->quit();
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox MBHelp(QMessageBox::NoIcon,"关于","海天鹰SVG编辑器 1.0\n\n分行查看SVG元素。\n作者：黄颖\n邮箱：sonichy@163.com\n主页：sonichy.96.lt");
    MBHelp.setWindowIcon(QIcon(":/icon.png"));
    MBHelp.setIconPixmap(QPixmap(":/icon.png"));
    MBHelp.exec();
}

void MainWindow::on_action_open_triggered()
{
    filename = QFileDialog::getOpenFileName(this,"打开",filename,"SVG(*.svg)");
    if(!filename.isEmpty()){
        ui->label->setPixmap(QPixmap(filename));
        QFile *file = new QFile(filename);
        if(file->open(QIODevice::ReadOnly)){
            QTextStream TS(file);
            QString s = TS.readAll();
            file->close();
            ui->statusBar->showMessage("打开 " + filename);
            QStringList line = s.split("\n");
            //for(int i=0;i<line.size();i++){
            ui->listWidget->clear();
            ui->listWidget->addItems(line);
            //}
        }else{
            ui->statusBar->showMessage("打开 " + filename + file->errorString());
        }
    }
}

void MainWindow::drawItem(QListWidgetItem *item)
{
    //qDebug() << item->text();
    QString element = "<?xml version='1.0' ?><svg width='128' height='128' version='1.1' xmlns='http://www.w3.org/2000/svg'>";
    element += item->text();
    element += "</svg>";
    GS->clear();
    QXmlStreamReader *XMLSR = new QXmlStreamReader(element);
    QSvgRenderer *SVGR = new QSvgRenderer(XMLSR);
    QGraphicsSvgItem *GSVGI = new QGraphicsSvgItem;
    GSVGI->setSharedRenderer(SVGR);
    GS->addItem(GSVGI);
    //ui->graphicsView->setScene(GS);
}
