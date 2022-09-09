#include "amvtool.h"
#include <QApplication>

QList<QStringList> mainQueueInfo;
QList<QStringList> outputConfig;

QStringList defaultConfiguration = { "Original File Location", "MP4", "0", "YUV420", "8","BT.709", "0", "x264", "Constant Rate Factor", "Medium", "Animation", "18", "Original Audio", "All", "AAC", "Quality", "5", "0", "None", "9", "Auto", "No", "No", "0", "0"};
bool RecontainerSettings = false;
QString vspipeexec;

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    AMVtool w;
    w.show();

    return a.exec();
}
