#ifndef AMVTOOL_H
#define AMVTOOL_H
#include "setupencode.h"
#include <QMainWindow>
#include <QProcess>
#include <QDragEnterEvent>
#include <QFileDialog>

#ifdef WIN32
#define _UNICODE
#define ffmpegexec qApp->applicationDirPath() + "/" + "ffmpeg.exe"
#define vspipeexec qApp->applicationDirPath() + "/" + "vspipe.exe"
#elif __linux__
#define ffmpegexec "/usr/bin/ffmpeg"
#define vspipeexec "/usr/local/bin/vspipe"
#else
#define ffmpegexec qApp->applicationDirPath() + "/" + "ffmpeg"
#define vspipeexec "/usr/local/bin/vspipe"
#endif

extern QList<QStringList> mainQueueInfo;

namespace Ui {
class AMVtool;
}

class AMVtool : public QMainWindow
{
    Q_OBJECT

public:
    explicit AMVtool(QWidget *parent = 0);
    ~AMVtool();




private slots:


    void on_addFiles_clicked();

    void encodeFinished(int exitcode, QProcess::ExitStatus);

    void on_removeFiles_clicked();

    void readyReadStandardOutput();

    void on_configEncSettings_clicked();

    void on_fileList_doubleClicked(const QModelIndex &index);

    void on_convertFiles_clicked();

    void on_fileList_clicked();

    void on_configAll_clicked();

    void on_showDetails_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    Ui::AMVtool *ui;
    void addFilesToQueue(QStringList inputFiles);
    void openConfigBox(int selectedfile);
    float parseTimecode(QString timecode);
    int currentProcess(QString currentstatus);
    void changeEnabled(bool status, QString button);

    void CheckQueue();
    void Encode(int queue, QList<QStringList> inputDetails, QStringList configList);
    QString mediatypes;
    QString outputfile;
    QString mOutputString;

    QFileDialog dialog;

    QProcess *encode;
    QProcess *pipe;
    int queue;
    float progress;
    bool stopprocess;
    bool outputcreated;


    int inputDuration;

    QString timecode = "time=+([0-9][0-9]):([0-9][0-9]):([0-9][0-9](\\.[0-9][0-9]?)?)";
};

#endif // AMVTOOL_H
