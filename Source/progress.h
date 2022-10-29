#ifndef PROGRESS_H
#define PROGRESS_H
#include <QList>

class progress
{
public:
    QList<QString> currentProcess(QString currentstatus, float duration, float framerate);

private:
    QString generateTimeLeft(float spd, float dur, float prog);
    QString buildTimecode(float time);
    float parseTimecode(QString timecode);

    QString timecode = "time=+([0-9][0-9]):([0-9][0-9]):([0-9][0-9](\\.[0-9][0-9]?)?)";

};

#endif // PROGRESS_H
