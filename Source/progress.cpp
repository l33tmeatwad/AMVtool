#include "progress.h"
#include <cmath>
#include <QRegularExpression>

QList<QString> progress::currentProcess(QString currentstatus, float duration)
{
    int currentpercent = 0;
    float progress = 0;

    QRegularExpression findtime(timecode);
    QRegularExpressionMatch findstatus = findtime.match(currentstatus);
    if (findstatus.captured(0).length() != -1)
    {
        progress = parseTimecode(findstatus.captured(0).right(11));
    }

    float encspeed = 0.01f;
    QString encodespeed;
    if (currentstatus.contains("speed="))
    {
        int speedspot = currentstatus.indexOf("speed=");
        encodespeed = currentstatus.replace(currentstatus.left(speedspot+6), "").replace("x","").replace(" ", "");
        encodespeed = encodespeed.left(encodespeed.length()-1);
        encspeed = encodespeed.toFloat();
    }
    currentpercent = (progress/duration) *100;

    QString timeleft = generateTimeLeft(encspeed,duration,progress);
    return { QString::number(currentpercent), "Speed=" + QString::number(encspeed) + "x" + " | Time Left=" + timeleft };
}

QString progress::generateTimeLeft(float spd, float dur, float prog)
{
    QString remainingTime;
    if (prog > dur)
    {
        remainingTime = "Unknown";
    }
    else
    {
        float milli = 1000;
        float timeleft = ((dur-prog)/milli)/spd;
        float time = 60;
        float hours = (timeleft/time)/time;
        hours = floor(hours);
        timeleft = timeleft-((hours*time)*time);
        float minutes = (timeleft/time);
        minutes = floor(minutes);
        timeleft = timeleft-(minutes*time);
        if (QString::number(timeleft).length() < 6)
        {
            timeleft = timeleft + 0.0001;
        }

        remainingTime = buildTimecode(hours) + ":" + buildTimecode(minutes) + ":" + buildTimecode(timeleft);
    }
    return remainingTime;
}

QString progress::buildTimecode(float time)
{
    QString timemod;
    if (time < 10)
    {
        timemod = "0" + QString::number(time);
    }
    else
    {
        timemod = QString::number(time);
    }
    if (timemod.contains(".") && timemod.length() > 6)
    {
        timemod = timemod.left(6);
    }

    return timemod;
}

float progress::parseTimecode(QString timecode)
{
    float hours = ((timecode.left(2).toInt() * 60) * 60) * 1000;
    float minutes = (timecode.left(5).right(2).toInt() * 60) * 1000;
    float seconds = (timecode.left(8).right(2).toInt()) * 1000;
    float milliseconds = (timecode.left(11).right(2).toInt()) * 10;
    float mstime = hours+minutes+seconds+milliseconds;
    return mstime;
}
