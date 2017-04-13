#include "setupencode.h"

setupencode::setupencode(QObject *parent) : QObject(parent)
{

}

QStringList setupencode::SetupEncode(int queue, QStringList fileInfo, QList<QStringList> inputDetails, QStringList configList)
{
    QString mediafile = fileInfo[0];
    QString outputdir = configList[0];
    QString container = configList[1];
    int vstream = configList[2].toInt();
    QString colorspace = getColorSpace(configList[3]);
    QString colormatrix = getColorMatrix(configList[4]);
    QString codecname = getCodecName(configList[5]);
    QString vmode = configList[6];
    QString preset = configList[7];
    QString tune = configList[8];
    QString bitrate = configList[9];
    QString asource = configList[10];
    QString astream = configList[11];
    QString acodec = getAudioCodecName(configList[12]);
    QString amode = configList[13];
    QString abitrate = configList[14];
    QStringList containerCompatibility;

    QStringList ffmpegcommand;
    outputfile = OutputFile(fileInfo[1], container.toLower());
    if (inputDetails[0][2] == "VapourSynth")
    {
        if (inputDetails[4][vstream].contains("RGB"))
        {
            ffmpegcommand.append({ "-f", "rawvideo", "-pix_fmt", "gbrp", "-s", inputDetails[6][vstream] + "x" + inputDetails[7][vstream], "-r", inputDetails[8][vstream]});
        }
        ffmpegcommand.append({ "-i", "pipe:" });
    }
    else
    {
        ffmpegcommand.append({ "-i", mediafile });
    }

    if (asource != "Original Audio")
    {
        ffmpegcommand.append({"-i", asource});
    }

    if (inputDetails[0][0].toInt() > 1 || inputDetails[0][1].toInt() > 0 || asource != "Original Audio")
    {
        int streamID = inputDetails[1][vstream].toInt();
        if (streamID > 0)
        {
            streamID = streamID-1;
        }

        ffmpegcommand.append({"-map", "0:" +  QString::number(streamID)});
        if (astream != "None")
        {
            QString source;
            if (asource == "Original Audio")
            {
                source = "0";
            }
            else
            {
                source = "1";
            }

            if (astream == "All")
            {
                for (int i = 0; i < inputDetails[0][1].toInt(); i++)
                {
                    streamID = inputDetails[9][i].toInt()-1;
                    if (streamID < 0)
                    {
                        streamID = 0;
                    }
                    bool usestream = true;
                    if (acodec == "copy" && container != "MKV")
                    {
                        usestream = canCopyAudio(container, inputDetails[10][i]);
                    }
                    if (usestream)
                    {
                        ffmpegcommand.append({"-map", source + ":" + QString::number(streamID)});
                    }

                }
            }
            if (astream.toLower() != "all" && astream.toLower() != "none")
            {
                int stream = astream.toInt()-1;
                streamID = inputDetails[9][stream].toInt()-1;
                if (streamID < 0)
                {
                    streamID = 0;
                }
                ffmpegcommand.append({"-map", source + ":" + QString::number(streamID)});
            }
        }
    }


    ffmpegcommand.append("-f");

    if (container == "MKV")
    {
        ffmpegcommand.append("matroska");
    }
    else
    {
        ffmpegcommand.append(container);
    }

    ffmpegcommand.append({ "-c:v", codecname });


    if (codecname == "libx264" || codecname == "hevc")
    {

        if (vmode == "Constant Rate Factor")
        {
            ffmpegcommand.append({ "-crf",bitrate });
        }

        if (vmode.contains("Bitrate"))
        {
            ffmpegcommand.append({ "-b:v",bitrate + "k" });

            if (vmode.contains("2 Pass"))
            {
                ffmpegcommand.append({ "-pass",mainQueueInfo[queue][3] });
            }
        }
    ffmpegcommand.append({ "-preset", preset.toLower().replace(" ",""), "-tune", tune.toLower() });
    }
    if (codecname != "copy")
    {
        ffmpegcommand.append({"-pix_fmt", colorspace });
        if (colorspace.contains("yuv"))
        {
            ffmpegcommand.append({"-colorspace", colormatrix });
        }
    }

    if (astream != "None")
    {
        if (astream == "All" && acodec != "Copy")
        {
            for (int i = 0; i < inputDetails[0][1].toInt(); i++)
            {
                ffmpegcommand.append({"-c:a:" + QString::number(i), acodec });
                if (acodec == "aac" || acodec == "libmp3lame")
                {
                    if (amode == "Quality")
                    {
                        ffmpegcommand.append({"-q:a", abitrate });
                    }
                    else
                    {
                        ffmpegcommand.append({"-b:a", abitrate + "k" });
                    }
                }
            }
        }
        else
        {
            ffmpegcommand.append({"-c:a", acodec });
            if (acodec == "aac" || acodec == "libmp3lame")
            {
                if (amode == "Quality")
                {
                    ffmpegcommand.append({"-q:a", abitrate });
                }
                else
                {
                    ffmpegcommand.append({"-b:a", abitrate + "k" });
                }
            }
        }
    }
    else
    {
        ffmpegcommand.append("-an");
    }
    ffmpegcommand.append("-y");

    if (mainQueueInfo[queue][3] == "1" && vmode.contains("Bitrate"))
    {
        ffmpegcommand.append( pass1null );

    }
    else
    {
        ffmpegcommand.append( outputdir + outputfile );
    }

    return ffmpegcommand;
}


// VapourSynth Stuff //

QStringList setupencode::SetupPipe(QString inputFile, QString colorspace)
{
    QStringList pipecommand;
    if (colorspace.contains("YUV"))
    {
        pipecommand.append("--y4m");
    }

    pipecommand.append({ inputFile,"-" });

    return pipecommand;
}


QString setupencode::OutputFile(QString originalfile, QString newtype)
{
    originalfile = originalfile.left(originalfile.length()-4) + "-AMVtool." + newtype;
    return originalfile;
}

QString setupencode::getCodecName(QString codecname)
{
    codecname = codecname.toLower().replace(" ","");
    if (codecname == "x264")
    {
        codecname = "lib" + codecname;
    }
    if (codecname == "x265")
    {
        codecname = "hevc";
    }
    return codecname;
}

QString setupencode::getColorSpace(QString colorspace)
{
    if (colorspace == "RGBA")
    {
        colorspace = "gbrp";
    }
    if (colorspace == "RGBA")
    {
        colorspace = "rgba";
    }
    if (colorspace.contains("P8"))
    {
        colorspace.replace("P8","P");
    }
    return colorspace.toLower();
}

QString setupencode::getColorMatrix(QString colormatrix)
{
    colormatrix = colormatrix.toLower().replace(".","");
    if (colormatrix == "bt601")
    {
        colormatrix = "smpte170m";
    }
    return colormatrix;
}

QString setupencode::getAudioCodecName(QString codecname)
{
    codecname = codecname.toLower().replace(" ","");
    if (codecname == "mp3")
    {
        codecname = "lib" + codecname + "lame";
    }
    if (codecname == "pcm")
    {
        codecname = codecname + "_s16le";
    }
    return codecname;
}

bool setupencode::canCopyAudio(QString container, QString format)
{
    bool cancopyaudio = false;
    QStringList containerlist;

    if (container == "AVI")
        containerlist.append( {"MP3", "PCM"} );
    if (container == "MOV")
        containerlist.append( {"AAC", "AC-3", "PCM"} );
    if (container == "MP4")
        containerlist.append( {"AAC", "AC-3", "MP3"} );

    if (containerlist.contains(format))
    {
        cancopyaudio = true;
    }
    return cancopyaudio;
}
