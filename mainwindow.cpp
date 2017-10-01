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
    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_FileDialogNewFolder);
    ui->action_new->setIcon(icon);
    icon = style->standardIcon(QStyle::SP_DialogOpenButton);
    ui->action_open->setIcon(icon);
    icon = style->standardIcon(QStyle::SP_DialogSaveButton);
    ui->action_save->setIcon(icon);
    //icon = style->standardIcon(QStyle::SP_DialogSaveButton);
    ui->action_saveas->setIcon(icon);
    icon = style->standardIcon(QStyle::SP_DialogCloseButton);
    ui->action_quit->setIcon(icon);
    filename="";
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
    GS = new QGraphicsScene;
    ui->graphicsView->setScene(GS);
    SVGW = new QSvgWidget;
    ui->verticalLayout_SVG->addWidget(SVGW);
    ui->verticalLayout_SVG->addStretch();
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
    QMessageBox MBHelp(QMessageBox::NoIcon,"关于","海天鹰SVG编辑器 1.0\n\nLinux 平台基于 Qt 的最简单的 SVG 分行查看、编辑、保存程序。\n作者：黄颖\n邮箱：sonichy@163.com\n主页：sonichy.96.lt");
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

void MainWindow::on_action_new_triggered()
{
    ui->lineEdit->setText("");
    ui->listWidget->clear();
    QString s="";
    SVGW->load(s.toLatin1());
    GS->clear();
}

void MainWindow::on_action_close_triggered()
{
    on_action_new_triggered();
}

void MainWindow::on_action_save_triggered()
{
    QFile file(filename);
    if(file.open(QFile::WriteOnly)){
        QTextStream ts(&file);
        QString s = "";
        for(int i=0;i<ui->listWidget->count();i++){
            s += ui->listWidget->item(i)->text() + "\n";
        }
        ts << s;
        file.close();
    }else{
        QMessageBox::critical(this,"错误", filename + file.errorString() );
    }
}

void MainWindow::on_action_saveas_triggered()
{
    if(filename==""){
        filename = QFileDialog::getSaveFileName(this, "保存文本", "./未命名");
    }else{
        filename = QFileDialog::getSaveFileName(this, "保存文本", filename);
    }
    if(!filename.isEmpty()){
        on_action_save_triggered();
    }
}
