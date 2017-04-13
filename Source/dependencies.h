#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#ifdef WIN32
#define _UNICODE
#define OS "Windows"
#else
#define OS "Unix"
#endif

#include <QList>
#include <QObject>
#include <QProcess>

class dependencies : public QObject
{
    Q_OBJECT
public:
    dependencies(QObject *parent = 0);
    QStringList checkDependencies();

private slots:
    void readErrors();
private:
    QProcess *exec;
    bool execfail;
    QString findDir(QString executable);
    bool checkEXEC(QString execDir);
};

#endif // DEPENDENCIES_H
