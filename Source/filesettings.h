#ifndef FILESETTINGS_H
#define FILESETTINGS_H
#include <QList>

#ifdef WIN32
#define _UNICODE
#define OS "Windows"
#else
#define OS "Unix"
#endif

extern QList<QStringList> mainQueueInfo;
extern QStringList defaultConfiguration;
extern QList<QStringList> outputConfig;

class filesettings
{
public:
    bool checkFolder(QString folder);
    void addSettings(QString originalLocation, QString colormatrix, int videoheight, bool isVPY);
    void changeSettings(int ql, QString bitdepth, QStringList configurationList);
    void removeSettings(int ql);
    void recontainerSettings(QList<QStringList> mediaInfo, int vstream, int queue, bool incaudio);
    QString pickContainer(QString container, QString codec);
};

#endif // FILESETTINGS_H
