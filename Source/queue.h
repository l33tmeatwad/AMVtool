#ifndef QUEUE_H
#define QUEUE_H
#include <QList>
#include <QFileInfo>

class queue
{
public:
    QString InputFiles(QString inputFile);
    QList<QStringList> getInputDetails(QString mediafile);
    int findPosition();
    QList<QStringList> checkInput(int position);
    void setupRecontainer();

};

#endif // QUEUE_H
