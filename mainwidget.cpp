#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , serialport()
    , unlocked(true)
    , mm_cam(new Camera_preview)
{
    ui->setupUi(this);
    for(auto i : QSerialPortInfo::availablePorts()){
        ui->serialPorts->addItem(i.portName());
    }

    /*ui->zero->setFocusPolicy(Qt::NoFocus);
    ui->deux->setFocusPolicy(Qt::NoFocus);
    ui->trois->setFocusPolicy(Qt::NoFocus);
    ui->quatre->setFocusPolicy(Qt::NoFocus);
    ui->zero->setFocusPolicy(Qt::NoFocus);
    ui->zero->setFocusPolicy(Qt::NoFocus);
    ui->zero->setFocusPolicy(Qt::NoFocus);
    ui->zero->setFocusPolicy(Qt::NoFocus);
    ui->zero->setFocusPolicy(Qt::NoFocus);
    ui->zero->setFocusPolicy(Qt::NoFocus);*/

    serialport.setBaudRate(115200);
    serialport.setDataBits(QSerialPort::Data8);
    serialport.setParity(QSerialPort::NoParity);
    serialport.setStopBits(QSerialPort::OneStop);
    serialport.setFlowControl(QSerialPort::NoFlowControl);
    serialport.open(QIODevice::ReadWrite);
    connect(&serialport, SIGNAL(readyRead()), this, SLOT(serialEvent()));

    // Auto connect
    serialport.setPortName("ttyUSB0");
    logln("Tentative de connection sur le port: ttyUSB0"/* + '\n'*/);

    if(serialport.open(QIODevice::ReadWrite)){
        logln("Connection réussi sur le port: ttyUSB0"/* + '\n'*/);
        serialport.write("bbb\n");
        logln("Demande des parametres..");
    }
    else{
        logln("Connection échouer sur le port: ttyUSB0"/* + '\n'*/);
    }

}

MainWidget::~MainWidget()
{
    serialport.close();
    delete ui;
}

void MainWidget::log(QString text)
{
    //ui->textEdit->append(text);
    //ui->textEdit->setText(ui->textEdit->toPlainText() + text);

    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertPlainText (text);

    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void MainWidget::logln(QString text)
{
    //ui->textEdit->append(text);
    //ui->textEdit->setText(ui->textEdit->toPlainText() + text + '\n');
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertPlainText (text + '\n');


    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void MainWidget::on_refresh_buttons_clicked()
{
    ui->serialPorts->clear();
    for(auto i : QSerialPortInfo::availablePorts()){
        ui->serialPorts->addItem(i.portName());
    }
}

void MainWidget::on_connect_button_clicked()
{
    serialport.setPortName(ui->serialPorts->currentText());
    logln("Tentative de connection sur le port: " + ui->serialPorts->currentText()/* + '\n'*/);

    if(serialport.open(QIODevice::ReadWrite)){
        logln("Connection réussi sur le port: " + ui->serialPorts->currentText()/* + '\n'*/);
    }
    else{
        logln("Connection échouer sur le port: " + ui->serialPorts->currentText()/* + '\n'*/);
    }
}

void MainWidget::on_ping_button_clicked()
{
    serialport.write("pin\n");
}


void MainWidget::serialEvent(){

    while (serialport.canReadLine()){
        auto data = serialport.readLine();
        qDebug() << data;
        if(data.startsWith("pong!")){
            logln("L'Arduino a bien repondu a la requete de ping");
        }
        else if(data.startsWith("inf01")){
            logln("Initialisation de la carte SD réussi");
        }
        else if(data.startsWith("inf02")){
            logln("Lecture des parametres depuis la carte SD réussi");
        }
        else if(data.startsWith("inf03")){
            logln("Enregistrement des parametres sur la carte SD réussi");
        }
        else if(data.startsWith("inf04")){
            logln("Programme démarer");
        }
        else if(data.startsWith("inf05")){
            logln("Programme terminé");
            mm_cam->stop_rec();
        }
        else if(data.startsWith("inf06")){
            logln("Moteurs arrivés a la position de départ");
        }
        else if(data.startsWith("inf07")){
            logln("Est en train de continuer");
        }
        else if(data.startsWith("inf08")){
            logln("Peut continuer");
            if(ui->checkBox->isChecked()){
                serialport.write("ctn\n");
            }
        }
        //Erreurs
        else if(data.startsWith("err01")){
            logln("Aucun carte SD détécté");
        }
        else if(data.startsWith("err02")){
            logln("Erreur de chargement des parametres");
        }
        else if(data.startsWith("err03")){
            logln("Erreur d'ecriture des parametres sur la carte sd");
        }
        //mouvement manuels
        else if(data.startsWith("mvd1")){
            logln("Deplacement alimentation poudre terminé");
        }
        else if(data.startsWith("mvd2")){
            logln("Deplacement du moteur chariot horizontal terminé");
        }
        else if(data.startsWith("mvd3")){
            logln("Deplacement du moteur chariot vertical terminé");
        }
        else if(data.startsWith("rst")){
            logln("Arduino est reset");
        }
        else if(data.startsWith("pas")){
            std::string str(data);
            str = str.substr(3);
            QStringList settings;
            std::string setting;

            settings = QString::fromStdString(str).split(' ');

            ui->etat_label->setText(settings[0] + "/" + settings[1]);
            //mm_cam->stop_rec();
            //mm_cam->start_rec(settings[0].toInt());
            mm_cam->split_rec(settings[0].toInt());
        }
        else if(data.startsWith("pos")){
            std::string str(data);
            str = str.substr(3);
            QStringList settings;
            std::string setting;

            settings = QString::fromStdString(str).split(' ');

            /*log(QString::fromStdString(str));
            for(auto i : settings){
                log(i);
            }*/

            ui->poudre_absolute->setText(settings[0]);
            ui->chariot_absolute->setText(settings[1]);
            ui->lit_absolute->setText(settings[2]);
        }

        // Ecriture des parametres dans toutes les cases
        else if(data.startsWith("b")){
            std::string str(data);
            int value(std::stoi(str.substr(3)));
            //log(QString::number(value));
            //if(data.startsWith("b01"))
            if(data.startsWith("b01")){
                ui->poudre_step->setText(QString::number(value));
            }
            else if(data.startsWith("b02")){
                ui->chariot_step->setText(QString::number(value));
            }
            if(data.startsWith("b03")){
                ui->lit_step->setText(QString::number(value));
            }
            else if(data.startsWith("b04")){
                ui->poudre_speed->setText(QString::number(value));
            }
            if(data.startsWith("b05")){
                ui->chariot_speed->setText(QString::number(value));
            }
            else if(data.startsWith("b06")){
                ui->lit_speed->setText(QString::number(value));
            }
            if(data.startsWith("b07")){
                ui->poudre_acceleration->setText(QString::number(value));
            }
            else if(data.startsWith("b08")){
                ui->chariot_acceleration->setText(QString::number(value));
            }
            if(data.startsWith("b09")){
                ui->lit_acceleration->setText(QString::number(value));
            }
            else if(data.startsWith("b10")){
                ui->poudre_position->setText(QString::number(value));
            }
            if(data.startsWith("b11")){
                ui->chariot_position->setText(QString::number(value));
            }
            else if(data.startsWith("b12")){
                ui->lit_position->setText(QString::number(value));
            }
            if(data.startsWith("b13")){
                ui->first_layer_multi->setText(QString::number(value));
            }
            else if(data.startsWith("b14")){
                ui->layer_affected_by_multi->setText(QString::number(value));
            }
            else if(data.startsWith("b15")){
                ui->total_layers->setText(QString::number(value));
            }
            else if(data.startsWith("b16")){
                ui->delay->setText(QString::number(value));
            }
            else if(data.startsWith("b17")){
                ui->delay_all->setText(QString::number(value));
            }
        }
    }

}

void MainWidget::on_poudre_up_clicked()
{
    serialport.write("mov1 ");
    serialport.write(ui->poudre_text->text().toStdString().c_str());
    serialport.write("\n");

    log("Demande alimentation poudre un deplacement de: ");
    log(std::to_string(ui->poudre_text->text().toInt() * 1).c_str());
    logln("mm");
    qDebug() << ui->poudre_text->text();
}

void MainWidget::on_poudre_down_clicked()
{
    serialport.write("mov1 ");
    serialport.write(std::to_string(ui->poudre_text->text().toInt() * -1).c_str());
    serialport.write("\n");
    log("Demande alimentation poudre un deplacement de: ");
    log(std::to_string(ui->poudre_text->text().toInt() * -1).c_str());
    logln("mm");
    qDebug() << ui->poudre_text->text();
}


void MainWidget::on_racleur_up_clicked()
{
    serialport.write("mov3 ");
    serialport.write(ui->racleur_text->text().toStdString().c_str());
    serialport.write("\n");
    log("Demande au moteur racleur de monter de: ");
    log(std::to_string(ui->racleur_text->text().toInt() * 1).c_str());
    logln("mm");
    qDebug() << ui->racleur_text->text();
}

void MainWidget::on_racleur_down_clicked()
{
    serialport.write("mov3 -");
    serialport.write(ui->racleur_text->text().toStdString().c_str());
    serialport.write("\n");
    log("Demande au moteur racleur de descendre de: ");
    log(std::to_string(ui->racleur_text->text().toInt() * 1).c_str());
    logln("mm");
    qDebug() << ui->racleur_text->text();
}

void MainWidget::on_racleur_back_clicked()
{
    serialport.write("mov2 -");
    serialport.write(ui->racleur_text->text().toStdString().c_str());
    serialport.write("\n");
    log("Demande au moteur racleur de reculer de: ");
    log(std::to_string(ui->racleur_text->text().toInt() * 1).c_str());
    logln("mm");
    qDebug() << ui->racleur_text->text();
}

void MainWidget::on_racleur_forward_clicked()
{
    serialport.write("mov2 ");
    serialport.write(ui->racleur_text->text().toStdString().c_str());
    serialport.write("\n");
    log("Demande au moteur racleur d'avancer de: ");
    log(std::to_string(ui->racleur_text->text().toInt() * 1).c_str());
    logln("mm");
    qDebug() << ui->racleur_text->text();
}

void MainWidget::on_pushButton_clicked()
{
    serialport.write("rst\n");
    logln("L'arduino est en train de se reset");
    qDebug() << "Reset en cours";
}

void MainWidget::on_pushButton_2_clicked()
{
    serialport.close();
    logln("Déconnecté");
    qDebug() << "Fermeture serialport";
}

void MainWidget::on_pushButton_8_clicked()
{
    serialport.write("hme\n");
    logln("Home de tout les axes");
    qDebug() << "Homing all axes";
}

void MainWidget::on_pushButton_5_clicked()
{
    serialport.write("hm1\n");
    logln("Home du bac alimentation poudre");
    qDebug() << "Homing alimentation poudre";
}

void MainWidget::on_pushButton_6_clicked()
{
    serialport.write("hm2\n");
    logln("Home du racleur horizontal");
    qDebug() << "Homing chariot";
}

void MainWidget::on_pushButton_7_clicked()
{
    serialport.write("hm3\n");
    logln("Home du racleur vertical");
    qDebug() << "Homing lit";
}

void MainWidget::on_pushButton_4_clicked()
{
    serialport.write("bbb\n");
    logln("Demande des parametres..");
}

void MainWidget::on_pushButton_9_clicked()
{
    serialport.write("pos\n");
    logln("Demande de la positions des moteurs...");
}

void MainWidget::on_pushButton_10_clicked()
{
    serialport.write("sep");
    serialport.write(ui->poudre_absolute->text().toStdString().c_str());
    serialport.write(" ");
    serialport.write(ui->chariot_absolute->text().toStdString().c_str());
    serialport.write(" ");
    serialport.write(ui->lit_absolute->text().toStdString().c_str());
    serialport.write("\n");

    logln("Demandre deplacement:");
    log("Bac a poudre: ");
    logln(ui->poudre_absolute->text().toStdString().c_str());
    log("Racleur horizontal: ");
    logln(ui->chariot_absolute->text().toStdString().c_str());
    log("Racleur vertical: ");
    logln(ui->lit_absolute->text().toStdString().c_str());

}

void MainWidget::on_pushButton_3_clicked()
{
    serialport.write("par");

    serialport.write(ui->poudre_step->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->chariot_step->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->lit_step->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->poudre_speed->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->chariot_speed->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->lit_speed->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->poudre_acceleration->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->chariot_acceleration->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->lit_acceleration->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->poudre_position->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->chariot_position->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->lit_position->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->first_layer_multi->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->layer_affected_by_multi->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->total_layers->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->delay->text().toStdString().c_str());
    serialport.write(" ");

    serialport.write(ui->delay_all->text().toStdString().c_str());
    serialport.write(" ");
    serialport.write("\n");
}

void MainWidget::on_unlock_button_clicked()
{
    if(!unlocked){
        ui->unlock_button->setText("Deverouiller");
        unlocked = true;
        ui->groupBox_6->setEnabled(false);
    }
    else{
        ui->unlock_button->setText("Verouiller");
        unlocked = false;
        ui->groupBox_6->setEnabled(true);
    }
}

void MainWidget::on_pushButton_12_clicked()
{
    serialport.write("stp\n");
    logln("Arret du programme...");
}


void MainWidget::on_pushButton_16_clicked()
{
    serialport.write("ctn\n");
    logln("Demande de continuation");
}


void MainWidget::on_pushButton_11_clicked()
{

    ui->etat_label->clear();
    mm_cam->start_rec(1);
    serialport.write("str\n");
    logln("Demarage du programe...");
}

void MainWidget::on_pushButton_14_clicked()
{
    system("shutdown -s -f -t 0");
    system("sudo shutdown now");

}

void MainWidget::appendline(QString str){
    QList<QWidget*> mylineEdits = this->findChildren<QWidget*>();
    QListIterator<QWidget*> it(mylineEdits); // iterate through the list of widgets
    QWidget *lineEditField;
    while (it.hasNext()) {
        lineEditField = it.next(); // take each widget in the list
        if(QLineEdit *lineE = qobject_cast<QLineEdit*>(lineEditField)) {  // check if iterated widget is of type QLineEdit
            //
            if (lineE->hasFocus())
              {
                lineE->setText(lineE->text() + str);
              }

        }
    }
}

void MainWidget::deleteline(){
    QList<QWidget*> mylineEdits = this->findChildren<QWidget*>();
    QListIterator<QWidget*> it(mylineEdits); // iterate through the list of widgets
    QWidget *lineEditField;
    while (it.hasNext()) {
        lineEditField = it.next(); // take each widget in the list
        if(QLineEdit *lineE = qobject_cast<QLineEdit*>(lineEditField)) {  // check if iterated widget is of type QLineEdit
            //
            if (lineE->hasFocus())
              {
                std::string str = lineE->text().toStdString();
                if(str == ""){
                    return;
                }
                str.pop_back();
                lineE->setText(QString::fromStdString(str));
              }

        }
    }
}

void MainWidget::on_zero_clicked()
{
     appendline("0");
}

void MainWidget::on_un_clicked()
{
    appendline("1");
}


void MainWidget::on_deux_clicked()
{
    appendline("2");
}


void MainWidget::on_trois_clicked()
{
    appendline("3");
}

void MainWidget::on_quatre_clicked()
{
    appendline("4");
}

void MainWidget::on_cinq_clicked()
{
    appendline("5");
}

void MainWidget::on_six_clicked()
{
    appendline("6");
}

void MainWidget::on_sept_clicked()
{
    appendline("7");
}

void MainWidget::on_huit_clicked()
{
    appendline("8");
}

void MainWidget::on_neuf_clicked()
{
    appendline("9");
}


void MainWidget::on_pushButton_15_clicked()
{
    appendline(".");
}


void MainWidget::on_back_clicked()
{
    deleteline();
}


void MainWidget::on_pushButton_17_clicked()
{
    mm_cam->show_preview();
}

void MainWidget::on_pushButton_18_clicked()
{
    QString filename(QFileDialog::getExistingDirectory(this, "Exporter les vidéos", "/media/pi/"));
    ui->label_24->setText(filename);
    //qDebug() << filename;
}

void MainWidget::on_tabWidget_3_tabBarClicked(int index)
{
    QDir dir("/home/pi/Desktop/videos/");
    dir.setNameFilters(QStringList() << "*.h264");
    //QDir dir("C:/Users/GalacsG/Desktop/temp");
    if(index == 4){
        ui->label_22->setText("Nombre de videos: " + QString::number(dir.count()));
        ui->progressBar->setMaximum(dir.count());

        int total(0);
        for(auto i : dir.entryInfoList()){
            total += i.size();
        }

        float num = total;
        QStringList list;
        list << "KB" << "MB" << "GB" << "TB";

        QStringListIterator i(list);
        QString unit("bytes");

        while(num >= 1024.0 && i.hasNext())
         {
            unit = i.next();
            num /= 1024.0;
        }
        ui->label_23->setText("Taille: " + QString().setNum(num,'f',2)+" "+unit);
        //qDebug() << QString().setNum(num,'f',2)+" "+unit;

    }
}

void MainWidget::on_pushButton_19_clicked()
{
    QDir dir("/home/pi/Desktop/videos/");
    //QDir dir("C:/Users/GalacsG/Desktop/temp");
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(dir.count());
    for(auto i : dir.entryInfoList()){
        //qDebug() << i.absoluteFilePath();
        //qDebug() << ui->label_24->text() + '/' + i.fileName();

        QFile::copy(i.absoluteFilePath(), (ui->label_24->text() + '/' + i.fileName()));

        ui->progressBar->setValue(ui->progressBar->value() + 1);
        //qDebug() << ui->label_24->text() + i.fileName();
    }
    ui->progressBar->setValue(0);
}

void MainWidget::on_pushButton_20_clicked()
{
    QDir dir("/home/pi/Desktop/videos/");
    //QDir dir("C:/Users/GalacsG/Desktop/temp");
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(dir.count());
    QProcess proc;
    std::string str;
    for(auto i : dir.entryInfoList()){
        //qDebug() << i.absoluteFilePath();
        //qDebug() << ui->label_24->text() + '/' + i.fileName();

        //QFile::copy(i.absoluteFilePath(), ui->label_24->text() + '/' + i.fileName());
        str = "ffmpeg -i \"" + i.absoluteFilePath().toStdString() + "\" -vcodec copy \"" + ui->label_24->text().toStdString() + '/' + i.fileName().toStdString();
        for (int var = 0; var < 5; ++var) {
            str.pop_back();
        }
        str += ".mp4\"";
        proc.start(QString::fromStdString(str));
        proc.waitForFinished();
        proc.close();
        //qDebug() << QString::fromStdString(str);

        ui->progressBar->setValue(ui->progressBar->value() + 1);
        //qDebug() << ui->label_24->text() + i.fileName();
    }
}

void MainWidget::on_pushButton_21_clicked()
{
    mm_cam->start_manual_rec();
    ui->rec->setText("En cours d'enregistrement");
}

void MainWidget::on_pushButton_22_clicked()
{
    mm_cam->stop_rec();
    ui->rec->setText("");
}

void MainWidget::on_pushButton_23_clicked()
{
    QDir dir("/home/pi/Desktop/videos");
    dir.setNameFilters(QStringList() << "*.h264");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }

    this->on_tabWidget_3_tabBarClicked(4);
}
