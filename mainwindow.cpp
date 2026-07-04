#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QUrl>
#include <QTime>
#include <QDir>
#include <QFileInfoList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    connect(player, &QMediaPlayer::playbackStateChanged,
            this, &MainWindow::updatePlayStateUI);

    connect(player, &QMediaPlayer::positionChanged,
            this, &MainWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged,
            this, &MainWindow::updateDuration);

    connect(ui->sliderProgress, &QSlider::sliderPressed, [this]() {
        heldSlider = true;
    });
    connect(ui->sliderProgress, &QSlider::sliderReleased, [this]() {
        heldSlider = false;
        player->setPosition(ui->sliderProgress->value());
    });

    ui->sliderVolume->setRange(0, 100);
    ui->sliderVolume->setValue(50);
    audioOutput->setVolume(0.5);

    connect(player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::handleMediaStatus);
    connect(ui->btnNext, &QPushButton::clicked,
            this, &MainWindow::playNext);
    connect(ui->btnPrevious, &QPushButton::clicked,
            this, &MainWindow::playPrevious);
}

void MainWindow::on_btnOpen_clicked()
{
    filePath = QFileDialog::getOpenFileName(
        this,
        "Open Audio File",
        "",
        "Audio Files (*.mp3 *.wav)"
    );
    if (!filePath.isEmpty()){
        player->setSource(QUrl::fromLocalFile(filePath));
        QString fileName = filePath.section('/', -1);
        ui->labelFileName->setText(fileName);
        updateTimeLabel(0, 0);
    }
}

void MainWindow::on_btnPlay_clicked()
{
    if (filePath.isEmpty())
        return;
    if (player->playbackState() != QMediaPlayer::PlayingState)
        player->play();
    else
        player->pause();
}

void MainWindow::updatePlayStateUI(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        ui->btnPlay->setText("Pause");
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
        ui->btnPlay->setText("Play");
        break;
    }
}

QString MainWindow::formatTime(qint64 ms)
{
    QTime time(0, 0);
    time = time.addMSecs(ms);
    QString format = "mm:ss";
    if (ms >= 3600000)
        format = "hh:mm:ss";

    return time.toString(format);
}

void MainWindow::updateTimeLabel(qint64 current, qint64 total)
{
    ui->labelTime->setText(
        formatTime(current) + " / " +
        formatTime(total)
        );
}

void MainWindow::updatePosition(qint64 position)
{
    if (heldSlider) {
        return;
    }
    ui->sliderProgress->setValue(position);
    updateTimeLabel(position, player->duration());
}

void MainWindow::updateDuration(qint64 duration)
{
    ui->sliderProgress->setRange(0, duration);
    updateTimeLabel(0, duration);
}

void MainWindow::on_sliderProgress_sliderMoved(int position)
{
    updateTimeLabel(position, player->duration());
}

void MainWindow::on_sliderVolume_sliderMoved(int position)
{
    float volume = position / 100.0f;
    audioOutput->setVolume(volume);
    ui->labelVolume->setText(QString::number(position) + "%");
}

void MainWindow::on_btnOpenFolder_clicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this,
        "Open folder",
        ""
        );

    if (folderPath.isEmpty())
        return;

    QDir dir(folderPath);

    QStringList filters;
    filters << "*.mp3" << "*.wav";

    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    if (files.isEmpty())
        return;

    for (const QFileInfo &fileInfo : std::as_const(files)) {
        QString fullPath = fileInfo.absoluteFilePath();

        if (!playlist.contains(fullPath)) {
            playlist.append(fullPath);
            ui->listPlaylist->addItem(fileInfo.fileName());
        }
    }
    if (currentIndex == -1 && !playlist.isEmpty()) {
        currentIndex = 0;
        playCurrent();
    }
}

void MainWindow::playCurrent()
{
    if (currentIndex < 0 || currentIndex >= playlist.size())
        return;
    QString filePath = playlist[currentIndex];
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
    QString fileName = filePath.section('/', -1);
    ui->labelFileName->setText(fileName);
    ui->listPlaylist->setCurrentRow(currentIndex);
}

void MainWindow::handleMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (status != QMediaPlayer::EndOfMedia)
        return;

    if (playlist.isEmpty())
        return;

    switch (loopMode) {
    case LoopOne:
        playCurrent();
        break;

    case LoopAll:
        currentIndex++;
        if (currentIndex >= playlist.size())
            currentIndex = 0;
        playCurrent();
        break;

    case NoLoop:
        currentIndex++;
        if (currentIndex >= playlist.size()) {
            player->stop();
            return;
        }
        playCurrent();
        break;
    }
}

void MainWindow::on_listPlaylist_itemDoubleClicked(QListWidgetItem *item)
{
    currentIndex = ui->listPlaylist->row(item);
    playCurrent();
    player->play();
}

void MainWindow::playNext()
{
    if (playlist.isEmpty())
        return;

    currentIndex++;

    if (currentIndex >= playlist.size()){
        if (loopMode == LoopAll)
            currentIndex = 0;
        else
            return;
    }

    playCurrent();
}

void MainWindow::playPrevious()
{
    if (playlist.isEmpty())
        return;

    currentIndex--;

    if (currentIndex < 0){
        if (loopMode == LoopAll)
            currentIndex = playlist.size() - 1;
        else
            return;
    }

    playCurrent();
}

void MainWindow::on_btnLoop_clicked()
{
    switch (loopMode) {
    case NoLoop:  loopMode = LoopAll; break;
    case LoopAll: loopMode = LoopOne; break;
    case LoopOne: loopMode = NoLoop;  break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
