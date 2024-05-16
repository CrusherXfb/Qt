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
    //connect(ui->pushButtonMute, &QPushButton::clicked, this, &Widget::on_pushButtonMute_clicked);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Widget::updateHorizontalSlider_Progress);
    connect(ui->horizontalSliderProgress, &QSlider::sliderMoved, this, &Widget::on_horizontalSliderProgress_valueChanged);


    //          Init playlist
    m_playlist_model = new QStandardItemModel(this);
    ui->tableViewPlaylist->setModel(m_playlist_model);
    m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File");
    ui->tableViewPlaylist->hideColumn(1);
    ui->tableViewPlaylist->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);


    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);

    connect(ui->pushButtonNext, &QPushButton::clicked, m_playlist, &QMediaPlaylist::next);
    connect(ui->pushButtonPrev, &QPushButton::clicked, m_playlist, &QMediaPlaylist::previous);

    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, &Widget::on_current_index_changed); 
}

Widget::~Widget()           //Уходя, гасите всех
{
    delete m_playlist_model;
    delete m_playlist;
    delete m_player;
    delete ui;
}



void Widget::on_pushButtonOpen_clicked()
{
    /*QString file = QFileDialog::getOpenFileName
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
    Widget::setWindowTitle(QString("MediaPlayer_P_21 | ").append(file.split('/').last()));*/
    QStringList files = QFileDialog::getOpenFileNames
            (
                this,
                "Open files",
                "D:\\Музыка",
                "Audio files (*mp3 *flac);;mp3 (*mp3);; FLAC (*flac)"
             );
    for(QString file:files)
    {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(file).dirName()));
        items.append(new QStandardItem(file));
        m_playlist_model->appendRow(items);
        m_playlist->addMedia(QUrl(file));
    }
}

void Widget::on_pushButtonMute_clicked()
{
    m_player->setMuted(!m_player->isMuted());
    ui->pushButtonMute->setIcon(style()->standardIcon(
                                    m_player->isMuted()? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));

}

void Widget::updateHorizontalSlider_Progress(qint64 position)
{
    ui->horizontalSliderProgress->setValue(position);
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

void Widget::on_current_index_changed(int position)
{
    ui->tableViewPlaylist->selectRow(position);
    QStandardItem* file = m_playlist_model->item(position, 1);
    QStandardItem* song = m_playlist_model->item(position, 0);
    this->setWindowTitle(QString("MediaPlayerP_21 | ").append(song->text()));
    ui->labelFile->setText(QString("Song file: ").append(file->text()));

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


void Widget::on_pushButtonDEL_clicked() //works incorrectly
{
    QModelIndexList selectedRows = (ui->tableViewPlaylist->selectionModel()->selectedRows());
    for (const QModelIndex &index : selectedRows) {
        m_playlist_model->removeRow(index.row());
        m_playlist->removeMedia(index.row());
    }
}


void Widget::on_pushButtonCLR_clicked()
{
    m_playlist_model->clear();
    m_playlist->clear();
}


void Widget::on_pushButtonLoop_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    ui->pushButtonLoop->setEnabled(false);
    ui->pushButtonShuffle->setEnabled(true);
    ui->pushButtonDefault->setEnabled(true);
}


void Widget::on_pushButtonShuffle_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::Random);
    ui->pushButtonLoop->setEnabled(true);
    ui->pushButtonShuffle->setEnabled(false);
    ui->pushButtonDefault->setEnabled(true);
}


void Widget::on_pushButtonDefault_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    ui->pushButtonLoop->setEnabled(true);
    ui->pushButtonShuffle->setEnabled(true);
    ui->pushButtonDefault->setEnabled(false);
}






