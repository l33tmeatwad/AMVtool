#include "amvtool.h"
#include <QApplication>

QList<QStringList> mainQueueInfo;
QList<QStringList> outputConfig;

QStringList defaultConfiguration = { "Original File Location", "MP4", "0", "YUV420P8", "BT.709", "x264", "Constant Rate Factor", "Slower", "Animation", "18", "Original Audio", "All", "AAC", "Quality", "4", "0"};

QString encodepass = "Single";



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AMVtool w;
    w.show();

    return a.exec();
}
