#ifndef FILESETTINGS_H
#define FILESETTINGS_H
#include <QStringList>

#ifdef __APPLE__
#define lcontainer "MOV"
#define lastreams "ALL"
#define lacodec "ALAC"
#else
#define lcontainer "AVI"
#define lastreams "1"
#define lacodec "PCM"
#endif

extern QList<QStringList> mainQueueInfo;
extern QStringList defaultConfiguration;
extern QList<QStringList> outputConfig;

class filesettings
{


public:
    filesettings();
    QString InputFiles(QString inputFile);
    void chnageSettings(int ql, QStringList configurationList);
    void removeSettings(int ql);
    QList<QStringList> getInputDetails(QString mediafile);
    void setupLossless();
    void setupRecontainer();
    QString pickContainer(QString container, QString codec);

private:
    void addSettings(QString originalLocation, bool isVPY);
};

#endif // FILESETTINGS_H
