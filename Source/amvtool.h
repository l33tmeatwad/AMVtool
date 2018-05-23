#ifndef AMVTOOL_H
#define AMVTOOL_H
#include <QMainWindow>
#include <QProcess>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QLabel>

#ifdef WIN32
#define _UNICODE
#define vspipeexec qApp->applicationDirPath() + "/" + "vspipe.exe"
#else
#define vspipeexec "/usr/local/bin/vspipe"
#endif

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
    void readErrors();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    Ui::AMVtool *ui;
    QLabel *timeRemaining;
    QString checkDependencies();
    void addFilesToQueue(QStringList inputFiles);
    void openConfigBox(int selectedfile);
    void changeEnabled(bool status, QString button);
    void updateQueue(int pos, QString status);
    void CheckQueue();
    void ProcessFile(int pos);
    QString selectNewFolder();
    void Encode(int pos, QList<QStringList> inputDetails, QStringList configList);

    QString ffmpegexec;
    QString mediatypes;
    QString mOutputString;
    QList<int> hibitdepth;

    QFileDialog dialog;

    QProcess *encode;
    QProcess *pipe;
    int position;
    bool stopprocess;
    bool outputcreated;


    float inputDuration;

};

#endif // AMVTOOL_H
