#include "amvtool.h"
#include <QApplication>

QList<QStringList> mainQueueInfo;
QList<QStringList> outputConfig;

QStringList defaultConfiguration = { "Original File Location", "MP4", "0", "YUV420", "8","BT.709", "x264", "Constant Rate Factor", "Slow", "Animation", "18", "Original Audio", "All", "AAC", "Quality", "5", "0"};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AMVtool w;
    w.show();

    return a.exec();
}
