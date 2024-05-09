#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QStyle>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //          Buttons style:
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    //          Init player
    m_player = new QMediaPlayer();
    m_player->setVolume(70);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontalSliderVolume->setValue(m_player->volume());


    connect(ui->pushButtonPlay, &QPushButton::clicked, this->m_player, &QMediaPlayer::play);
    connect(ui->pushButtonPause, &QPushButton::clicked, this->m_player, &QMediaPlayer::pause);
    connect(ui->pushButtonStop, &QPushButton::clicked, this->m_player, &QMediaPlayer::stop);

    connect(m_player, &QMediaPlayer::durationChanged, this, &Widget::on_durationChanged);
    connect(ui->pushButtonMute, &QPushButton::clicked, this, &Widget::on_pushButtonMute_clicked);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Widget::updateHorizontalSlider_Progress);
    connect(ui->horizontalSliderProgress, &QSlider::sliderMoved, this, &Widget::on_horizontalSliderProgress_valueChanged);

}

Widget::~Widget()
{
    delete m_player;
    delete ui;
}


void Widget::on_pushButtonOpen_clicked()
{
    QString file = QFileDialog::getOpenFileName
                (
                this,
                "Open file",
                NULL,
                "Audio files (*mp3 *.flac)"
                );    
    QFileInfo fileInfo(file);
    QString fileName = fileInfo.fileName();
    ui->labelFile->setText(fileName);
    m_player->setMedia(QUrl::fromLocalFile(file));
    Widget::setWindowTitle("MediaPlayer_P_21 | " + fileName);
}

void Widget::on_pushButtonMute_clicked()
{
    if (!m_player->isMuted())
    {
        m_player->setMuted(true);
        ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else
    {
        m_player->setMuted(false);
        ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

void Widget::updateHorizontalSlider_Progress()
{
    ui->horizontalSliderProgress->setValue(m_player->position());
    QTime qt_position = QTime::fromMSecsSinceStartOfDay(m_player->position());
    ui->labelProgress->setText(QString("Progress: ").append(qt_position.toString(m_player->position() < 3600000 ?"mm:ss" : "hh:mm:ss")));
}

void Widget::on_horizontalSliderProgress_valueChanged(int value)
{
    if (ui->horizontalSliderProgress->isSliderDown())
    {
        m_player->setPosition(value);
    }
}




/*void Widget::on_pushButtonPlay_clicked()
{
        m_player->play();
}
void Widget::on_pushButtonPause_clicked()
{
       m_player->pause();
}
void Widget::on_pushButtonStop_clicked()
{
        m_player->stop();
}*/


void Widget::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
}

void Widget::on_durationChanged(qint64 duration)
{
    ui->horizontalSliderProgress->setMaximum(duration);
    QTime qt_duration = QTime::fromMSecsSinceStartOfDay(duration);
    ui->labelDuration->setText(QString("Duration: ").append(qt_duration.toString(duration < 3600000 ?"mm:ss" : "hh:mm:ss")));
    ui->horizontalSliderProgress->setMaximum(duration);
}

