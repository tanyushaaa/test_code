#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QDebug>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pictHeight = 370;
    pictWidth = 540;
    step = 0.1;
    leftX = -100; rightX = 100;
    leftY = -100; rightY = 100;
    drawGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::f(double x)
{
    return log(x)*(-1);
}

double MainWindow::f1(double x)
{
    return sin(x)*(-1);
}

double MainWindow::f2(double x)
{
    return cos(x)*(-1);
}

double MainWindow::f3(double x)
{
    return sin(1.0/x)*(-1);
}

void MainWindow::recountPixels()
{
    onePixelX = 540.0/(rightX-leftX);
    onePixelY = 370.0/(rightY-leftY);
    Ox = fabs(leftX); Oy = rightY;
}

void MainWindow::getData()
{
    leftX = ui->inputLeftX->text().toDouble();
    rightX = ui->inputRightX->text().toDouble();
    leftY = ui->inputLeftY->text().toDouble();
    rightY = ui->inputRightY->text().toDouble();
    step = 1.0/ui->inputAccuracy->text().toDouble();
}

void MainWindow::drawGraph(bool notEmpty)
{
    QPixmap graph(540,370);
    QPainter paint;
    paint.begin(&graph);
    paint.eraseRect(0,0,540,370);
    paint.drawLine(Ox*onePixelX,0,Ox*onePixelX,pictHeight);
    paint.drawLine(0,Oy*onePixelY,pictWidth,Oy*onePixelY);

    paint.setPen(QPen(Qt::black,3));
    for(double i = leftX;i<=rightX;i+=10.0)
        paint.drawPoint((i+Ox)*onePixelX,Oy*onePixelY);
    for(double i = leftY;i<=rightY;i+=10.0)
        paint.drawPoint(Ox*onePixelX,(i+Oy)*onePixelY);

    if(!notEmpty) {
        paint.end();
        ui->outputGraph->setPixmap(graph);
        return;
    }

    paint.setPen(QPen(Qt::green,1,Qt::SolidLine));
    paint.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path,p[3];
    bool first[4] = {1,1,1,1};

    for(double i = (double)leftX+step;i<=(double)rightX;i+=step) {
        if(!isnan(f(i))) {
            if(first[0]) {
                path.moveTo((i+Ox)*onePixelX,(f(i)+Oy)*onePixelY);
                first[0] = false;
            }
            else
                path.lineTo((i+Ox)*onePixelX,(f(i)+Oy)*onePixelY);
        }
        if(!isnan(f1(i))) {
            if(first[1]) {
                p[0].moveTo((i+Ox)*onePixelX,(f1(i)+Oy)*onePixelY);
                first[1] = false;
            }
            else
                p[0].lineTo((i+Ox)*onePixelX,(f1(i)+Oy)*onePixelY);
        }
        if(!isnan(f2(i))) {
            if(first[2]) {
                p[1].moveTo((i+Ox)*onePixelX,(f2(i)+Oy)*onePixelY);
                first[2] = false;
            }
            else
                p[1].lineTo((i+Ox)*onePixelX,(f2(i)+Oy)*onePixelY);
        }
        if(!isnan(f3(i))) {
            if(first[3]) {
                p[2].moveTo((i+Ox)*onePixelX,(f3(i)+Oy)*onePixelY);
                first[3] = false;
            }
            else
                p[2].lineTo((i+Ox)*onePixelX,(f3(i)+Oy)*onePixelY);
        }
    }
    if(ui->main->isChecked()) {
        paint.setPen(QPen(Qt::blue,1,Qt::SolidLine));
        paint.drawPath(path);
    }
    if(ui->sin->isChecked()) {
        paint.setPen(QPen(Qt::green,1,Qt::SolidLine));
        paint.drawPath(p[0]);
    }
    if(ui->cos->isChecked()) {
        paint.setPen(QPen(Qt::red,1,Qt::SolidLine));
        paint.drawPath(p[1]);
    }
    if(ui->tg->isChecked()) {
        paint.setPen(QPen(Qt::darkMagenta,1,Qt::SolidLine));
        paint.drawPath(p[2]);
    }
    paint.end();
    ui->outputGraph->setPixmap(graph);
    return;
}

void MainWindow::on_exit_clicked()
{
    this->close();
}

void MainWindow::on_clear_clicked()
{
    recountPixels();
    drawGraph();
}

void MainWindow::on_draw_clicked()
{
    getData();
    recountPixels();
    drawGraph(1);
}

void MainWindow::on_save_clicked()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString name;
   if(date.day()<10)
        name += "0";
    name += QString::number(date.day())+".";
    if(date.month()<10)
        name += "0";
    name += QString::number(date.month())+".";
    name += QString::number(date.year())+"_";
    if(time.hour()<10)
        name += "0";
    name += QString::number(time.hour())+"-";
    if(time.minute()<10)
        name += "0";
    name += QString::number(time.minute())+"-";
    if(time.second()<10)
        name += "0";
    name += QString::number(time.second());
    QFile file(name+".png");
    qDebug() << name;
    file.open(QIODevice::WriteOnly);
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    if(ui->outputGraph->pixmap()->save(&file,"PNG")) {
        msgBox.setText("Saved to program folder with name: "+name+".png");
        msgBox.setWindowTitle("Saved!");
    }
    else {
        msgBox.setText("Error saving.");
        msgBox.setWindowTitle("Error!");
    }
    msgBox.exec();
}
