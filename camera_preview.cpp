#include "camera_preview.h"
#include "ui_camera_preview.h"

Camera_preview::Camera_preview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Camera_preview),
    m_cam(new QProcess)
{
    ui->setupUi(this);
    //this->setWindowState(Qt::WindowFullScreen);
    m_cam.start("python3 cam.py");
}


void Camera_preview::show_preview(){
    this->showFullScreen();
    m_cam.write("pre\n");
    // If you know it you know it
    QEventLoop loop;
    connect(this, SIGNAL(destroyed()), & loop, SLOT(quit()));
    loop.exec();
}

void Camera_preview::stop_preview(){
    m_cam.write("spr\n");
}

void Camera_preview::start_rec(int a){
    m_cam.write("rec");
    m_cam.write(std::to_string(a).c_str());
    m_cam.write("\n");
}

void Camera_preview::split_rec(int a){
    m_cam.write("spl");
    qDebug() << "Split " << std::to_string(a).c_str();
    m_cam.write(std::to_string(a).c_str());
    m_cam.write("\n");
}


void Camera_preview::start_manual_rec(){               // Attention !!!

    m_cam.write("rmc");
    m_cam.write("\n");

    /*m_cam.write("rec");
    m_cam.write(std::to_string(a).c_str());
    m_cam.write("\n");*/
}

void Camera_preview::stop_rec(){
    m_cam.write("stp\n");
}

Camera_preview::~Camera_preview()
{
    delete ui;
}

void Camera_preview::on_pushButton_clicked()
{
    this->stop_preview();
}
