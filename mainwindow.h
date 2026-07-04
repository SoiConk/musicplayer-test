#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum LoopMode {
    NoLoop,
    LoopOne,
    LoopAll
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnOpen_clicked();

    void on_btnPlay_clicked();
    void updatePlayStateUI(QMediaPlayer::PlaybackState state);

    QString formatTime(qint64 ms);
    void updateTimeLabel(qint64 current, qint64 total);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void on_sliderProgress_sliderMoved(int position);

    void on_sliderVolume_sliderMoved(int value);

    void on_btnOpenFolder_clicked();

    void handleMediaStatus(QMediaPlayer::MediaStatus status);
    void on_listPlaylist_itemDoubleClicked(QListWidgetItem *item);

    void playNext();
    void playPrevious();
    void on_btnLoop_clicked();
private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QString filePath;

    bool heldSlider = false;

    QStringList playlist;
    int currentIndex = -1;
    void playCurrent();
    LoopMode loopMode = NoLoop;
};
#endif // MAINWINDOW_H
