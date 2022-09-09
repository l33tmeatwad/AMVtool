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
extern bool RecontainerSettings;

class filesettings
{
public:
    bool checkFolder(QString folder);
    void addSettings(QString originalLocation, QString lumarange, QString colormatrix, bool isVPY);
    void changeSettings(int ql, QString bitdepth, QStringList configurationList);
    void removeSettings(int ql);
    void recontainerSettings(QList<QStringList> mediaInfo, int vstream, int queue);
    QStringList findContainers(QString codec);
private:
    QStringList AVI = { "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2", "XviD"};
    QStringList MOV = { "AVC", "HEVC", "MPEG-4", "ULRG", "ULRA", "ULY0", "ULH0", "ULY2", "ULH2", "YQY2"};
    QStringList MP4 = { "AVC", "HEVC", "MPEG-4"};
};

#endif // FILESETTINGS_H
