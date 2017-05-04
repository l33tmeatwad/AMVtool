#ifndef QUEUE_H
#define QUEUE_H
#include <QList>
#include <QFileInfo>

extern QList<QStringList> mainQueueInfo;
extern QList<QStringList> outputConfig;

class queue
{
public:
    QString InputFiles(QString inputFile);
    QList<QStringList> getInputDetails(QString mediafile);
    int findPosition();
    QList<QStringList> checkInput(int position);
    void setupRecontainer(bool incaudio);

};

#endif // QUEUE_H
