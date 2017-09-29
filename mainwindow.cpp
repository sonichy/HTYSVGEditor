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
    ui->graphicsView->setScene(GS);
    SVGW = new QSvgWidget;
    ui->verticalLayout_SVG->addWidget(SVGW);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(drawItem(QListWidgetItem*)));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(on_pushButtonModify_clicked()));
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
            lines = s.split("\n");
            //for(int i=0;i<line.size();i++){
            tagxml = lines.filter("<?xml").at(0);
            qDebug() << tagxml;
            tagsvg = lines.filter("<svg").at(0);
            qDebug() << tagsvg;
            ui->listWidget->clear();
            ui->listWidget->addItems(lines);
            //}
        }else{
            ui->statusBar->showMessage("打开 " + filename + file->errorString());
        }
    }
}

void MainWindow::drawItem(QListWidgetItem *item)
{
    ui->lineEdit->setText(item->text());
    //QString element = "<?xml version='1.0' ?><svg width='128' height='128' version='1.1' xmlns='http://www.w3.org/2000/svg'>";
    QString element = tagxml + tagsvg;
    element += item->text();
    element += "</svg>";
    qDebug() << element;
    GS->clear();
    QXmlStreamReader *XMLSR = new QXmlStreamReader(element);
    QSvgRenderer *SVGR = new QSvgRenderer(XMLSR);
    QGraphicsSvgItem *GSVGI = new QGraphicsSvgItem;
    GSVGI->setSharedRenderer(SVGR);
    GS->addItem(GSVGI);
}

void MainWindow::on_pushButtonModify_clicked()
{
    ui->listWidget->currentItem()->setText(ui->lineEdit->text());
    drawItem(ui->listWidget->item(ui->listWidget->currentRow()));
    preview();
}

void MainWindow::on_pushButtonDelete_clicked()
{
    QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
    delete item;
    preview();
}

void MainWindow::preview()
{
    QString s="";
    for(int i=0;i<ui->listWidget->count();i++){
        s += ui->listWidget->item(i)->text() + "\n";
    }
    qDebug() << s.toLatin1();
    SVGW->load(s.toLatin1());
}
