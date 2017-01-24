#ifndef AMVTOOL_H
#define AMVTOOL_H
#include "setupencode.h"
#include <QMainWindow>
#include <QProcess>

#ifdef WIN32
#define _UNICODE
#define vspipeexec qApp->applicationDirPath() + "/" + "vspipe.exe"
#else
#define vspipeexec "/usr/local/bin/vspipe"
#endif


//extern QList<int> inputVideoStreams;

extern QList<QStringList> mainQueueInfo;
extern QString encodepass;

// extern QStringList inputFPS;

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

    void on_lossless_clicked();

    void on_recontainerAll_clicked();

private:
    Ui::AMVtool *ui;

    void openConfigBox(int selectedfile);
    float parseTimecode(QString timecode);
    int currentProcess(QString currentstatus);
    void changeEnabled(bool status, QString button);

    void CheckQueue();
    void Encode(int queue, QList<QStringList> inputDetails, QStringList configList);

    QString outputfile;
    QString mOutputString;

    QProcess *encode;
    QProcess *pipe;
    int queue;
    float progress;
    bool stopprocess;
    QStringList inputFiles;

    int inputDuration;

    QString timecode = "time=+([0-9][0-9]):([0-9][0-9]):([0-9][0-9](\\.[0-9][0-9]?)?)";
};

#endif // AMVTOOL_H
