#ifndef CAMERA_PREVIEW_H
#define CAMERA_PREVIEW_H

#include <QDialog>
#include <QProcess>

#include <QtDebug>

namespace Ui {
class Camera_preview;
}

class Camera_preview : public QDialog
{
    Q_OBJECT

public:
    explicit Camera_preview(QWidget *parent = nullptr);
    ~Camera_preview();
    void show_preview();
    void stop_preview();

    void start_rec(int a);
    void split_rec(int a);
    void start_manual_rec();
    void stop_rec();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Camera_preview *ui;
    QProcess m_cam;
};

#endif // CAMERA_PREVIEW_H
