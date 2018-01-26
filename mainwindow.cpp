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
#include <QDragEnterEvent>
#include <QMimeData>

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
    icon = style->standardIcon(QStyle::SP_BrowserReload);
    ui->actionReload->setIcon(icon);
    icon = style->standardIcon(QStyle::SP_ArrowUp);
    ui->pushButtonMoveUp->setIcon(icon);
    icon = style->standardIcon(QStyle::SP_ArrowDown);
    ui->pushButtonMoveDown->setIcon(icon);

    filename="";
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
    GS = new QGraphicsScene;
    ui->graphicsView->setScene(GS);
    SVGW = new QSvgWidget;
    ui->verticalLayout_SVG->addWidget(SVGW);
    ui->verticalLayout_SVG->addStretch();
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(drawItem(QListWidgetItem*)));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(on_pushButtonModify_clicked()));

    QStringList Largs = QApplication::arguments();
    qDebug() << Largs;
    if(Largs.length()>1){
        filename = Largs.at(1);
        on_actionReload_triggered();
    }
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
    QMessageBox MBHelp(QMessageBox::NoIcon,"关于","海天鹰SVG编辑器 1.1\n\nLinux 平台基于 Qt 的最简单的 SVG 分行查看、编辑、保存程序。\n作者：黄颖\n邮箱：sonichy@163.com\n主页：http://sonichy.gitee.io");
    MBHelp.setWindowIcon(QIcon(":/icon.png"));
    MBHelp.setIconPixmap(QPixmap(":/icon.png"));
    MBHelp.exec();
}

void MainWindow::on_action_open_triggered()
{
    filename = QFileDialog::getOpenFileName(this,"打开",filename,"SVG(*.svg)");
    on_actionReload_triggered();
}

void MainWindow::drawItem(QListWidgetItem *item)
{
    ui->lineEdit->setText(item->text());
    QString element = tagxml + tagsvg;
    element += item->text();
    element += "</svg>";
    //qDebug() << element;
    GS->clear();
    QXmlStreamReader *XMLSR = new QXmlStreamReader(element);
    QSvgRenderer *SVGR = new QSvgRenderer(XMLSR);
    QGraphicsSvgItem *GSVGI = new QGraphicsSvgItem;
    GSVGI->setSharedRenderer(SVGR);
    GS->addItem(GSVGI);
}

void MainWindow::on_pushButtonMoveUp_clicked()
{
    if(ui->listWidget->currentRow()>1){
        QString s = ui->listWidget->item(ui->listWidget->currentRow()-1)->text();
        ui->listWidget->item(ui->listWidget->currentRow()-1)->setText(ui->listWidget->currentItem()->text());
        ui->listWidget->currentItem()->setText(s);
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()-1);
        preview();
    }
}

void MainWindow::on_pushButtonMoveDown_clicked()
{
    if(ui->listWidget->currentRow()<ui->listWidget->count()-2){
        QString s = ui->listWidget->item(ui->listWidget->currentRow()+1)->text();
        ui->listWidget->item(ui->listWidget->currentRow()+1)->setText(ui->listWidget->currentItem()->text());
        ui->listWidget->currentItem()->setText(s);
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
        preview();
    }
}

void MainWindow::on_pushButtonModify_clicked()
{
    if(ui->listWidget->count()>0){
        ui->listWidget->currentItem()->setText(ui->lineEdit->text());
        drawItem(ui->listWidget->item(ui->listWidget->currentRow()));
        preview();
    }
}

void MainWindow::on_pushButtonInsert_clicked()
{
    if(ui->lineEdit->text()!="" && ui->listWidget->currentRow()<ui->listWidget->count()-1){
        ui->listWidget->insertItem(ui->listWidget->currentRow()+1,ui->lineEdit->text());
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
        preview();
    }
}

void MainWindow::on_pushButtonDelete_clicked()
{
    if(ui->listWidget->count()>0){
        QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
        delete item;
        preview();
    }
}

void MainWindow::preview()
{
    QString s="";
    for(int i=0;i<ui->listWidget->count();i++){
        s += ui->listWidget->item(i)->text() + "\n";
    }
    //qDebug() << s.toUtf8();
    SVGW->load(s.toUtf8());
}

void MainWindow::on_action_new_triggered()
{
    ui->lineEdit->setText("");
    ui->listWidget->clear();
    QString s="";
    SVGW->load(s.toUtf8());
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
        ui->statusBar->showMessage("保存 " + filename);
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

void MainWindow::on_actionReload_triggered()
{
    if(!filename.isEmpty()){
        ui->label->setPixmap(QPixmap(filename));
        QFile *file = new QFile(filename);
        if(file->open(QIODevice::ReadOnly)){
            QTextStream TS(file);
            QString s = TS.readAll();
            file->close();
            ui->statusBar->showMessage("打开 " + filename);
            lines = s.split("\n");
            if(lines.filter("<?xml").count()>0)
                tagxml = lines.filter("<?xml").at(0);
            //qDebug() << tagxml;
            if(lines.filter("<svg").count()>0)
                tagsvg = lines.filter("<svg").at(0);
            //qDebug() << tagsvg;
            ui->listWidget->clear();
            ui->listWidget->addItems(lines);
            //}
        }else{
            ui->statusBar->showMessage("打开 " + filename + file->errorString());
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << e->mimeData()->formats();
    //if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
        e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
}

void MainWindow::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return;

    QString fileName = urls.first().toLocalFile();

    foreach (QUrl u, urls) {
        qDebug() << u.toString();
    }
    qDebug() << urls.size();

    if(fileName.isEmpty())
        return;

    filename = fileName;
    on_actionReload_triggered();
}

void MainWindow::on_actionLine_triggered()
{
    ui->lineEdit->setText("<line x1=\"3\" y1=\"3\" x2=\"30\" y2=\"30\" stroke=\"rgb(0,0,0)\" stroke-width=\"1\" />");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionRect_triggered()
{
    ui->lineEdit->setText("<rect x=\"3\" y=\"6\" width=\"26\" height=\"20\" fill=\"transparent\" stroke-width=\"1\" stroke=\"rgb(0,0,0)\"/>");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionCircle_triggered()
{
    ui->lineEdit->setText("<circle cx=\"16\" cy=\"16\" r=\"13\" stroke=\"black\" stroke-width=\"1\" fill=\"red\" />");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionEllipse_triggered()
{
    ui->lineEdit->setText("<ellipse cx=\"16\" cy=\"16\" rx=\"14\" ry=\"8\" fill=\"rgb(200,100,50)\" stroke=\"rgb(0,0,100)\" stroke-width=\"1\" />");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionPolygon_triggered()
{
    ui->lineEdit->setText("<polygon points=\"16,6 6,21 26,26\" fill=\"#cccccc\" stroke=\"#000000\" stroke-width=\"1\" />");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionPolyline_triggered()
{
    ui->lineEdit->setText("<polyline points=\"5,3 5,10 15,10 15,20 25,20 25,30\" fill=\"white\" stroke=\"red\" stroke-width=\"1\" />");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionPath_triggered()
{
    ui->lineEdit->setText("<path d=\"M10 5 L5 30 C30 20 30 15 15 5\" fill=\"none\" stroke=\"red\" stroke-width=\"1\" />");
    on_pushButtonInsert_clicked();
}

void MainWindow::on_actionText_triggered()
{
    ui->lineEdit->setText("<text x=\"4\" y=\"27\" fill=\"red\" font-size=\"34\">A</text>");
    on_pushButtonInsert_clicked();
}
