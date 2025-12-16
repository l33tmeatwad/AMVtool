#include "setupencode.h"

setupencode::setupencode(QObject *parent) : QObject(parent)
{

}

QStringList setupencode::SetupEncode(int queue, QStringList fileInfo, QList<QStringList> inputMediaInfo, QStringList configList)
{




    QString mediafile = fileInfo[0];
    QString outputdir = configList[0];
    QString container = configList[1];
    int vstream = configList[2].toInt();
    QString colorspace = getColorSpace(configList[3],configList[4]);
    QString colormatrix = getColorMatrix(configList[5]);
    bool convertHDR = configList[6].toInt();
    QString codecname = getCodecName(configList[7]);
    QString vmode = configList[8];
    QString preset = configList[9];
    QString tune = configList[10];
    QString bitrate = configList[11];
    QString asource = configList[12];
    QString astream = configList[13];
    QString acodec = getAudioCodecName(configList[14]);
    QString amode = configList[15];
    QString abitrate = configList[16];
    int acopy = configList[17].toInt();
    QString modifymc = configList[18];
    QString deinterlace = configList[19];
    int cthresh = configList[20].toInt();
    QString fieldorder = configList[21];
    QString resize = configList[22];
    QString aspectratio = configList[23];
    int MaxMuxing = configList[24].toInt();
    bool FastStart = configList[25].toInt();
    bool DisableBFrames = configList[26].toInt();
    bool Experimental = configList[27].toInt();

    QString inputVideoStreamID = inputMediaInfo[1][vstream];
    QString inputColorSpace = inputMediaInfo[6][vstream];
    QString inputWidth = inputMediaInfo[8][vstream];
    QString inputHeight = inputMediaInfo[9][vstream];
    QString inputFPS = inputMediaInfo[10][vstream];
    QStringList inputAudioStreams = inputMediaInfo[11];
    QStringList inputAudioCodecs = inputMediaInfo[12];
    QStringList inputAudioChannels = inputMediaInfo[13];
    QStringList inputAudioLayout = inputMediaInfo[14];

    QStringList containerCompatibility;
    QStringList tilesThreads = getTilesAndThreads(inputHeight.toInt());

    QString vfilters = "";
    if (codecname != "copy")
        vfilters = SetupFilters(fileInfo[2].contains("HDR"), convertHDR, inputColorSpace, colorspace, colormatrix, deinterlace, cthresh, fieldorder, resize, aspectratio);

    QStringList ffmpegcommand;
    outputfile = OutputFile(fileInfo[1], inputMediaInfo[0][2], container.toLower());

    if (vmode.contains("2 Pass") && mainQueueInfo[queue][4] =="1")
        astream = "None";

    QString mapID;

    if (inputMediaInfo[0][2] == "VapourSynth")
    {
        if (inputColorSpace.contains("RGB"))
        {
            ffmpegcommand.append({ "-f", "rawvideo", "-pix_fmt", "gbrp", "-s", inputWidth + "x" + inputHeight, "-r", inputFPS});
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

    if (inputMediaInfo[0][0].toInt() > 1 || inputMediaInfo[0][1].toInt() > 0 || asource != "Original Audio")
    {
        if (inputVideoStreamID.contains("x"))
            mapID = "i";
        else
            mapID = "0";

        ffmpegcommand.append({"-map", mapID + ":" +  inputVideoStreamID});
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
                for (int i = 0; i < inputMediaInfo[0][1].toInt(); i++)
                {
                    bool usestream = true;
                    if (acopy == 1 && container != "MKV")
                    {
                        usestream = canCopyAudio(container, inputAudioCodecs[i]);
                    }
                    if (usestream)
                    {
                        if (inputAudioStreams[i].contains("x"))
                            mapID = source + ":i";
                        else
                            mapID = source;

                        ffmpegcommand.append({"-map", mapID + ":" + inputAudioStreams[i]});
                    }

                }
            }
            if (astream.toLower() != "all" && astream.toLower() != "none")
            {
                bool usestream = true;
                if (acopy == 1 && container != "MKV")
                {
                    int stream = astream.toInt()-1;
                    usestream = canCopyAudio(container, inputAudioCodecs[stream]);

                }
                if (usestream)
                {
                    int stream = astream.toInt()-1;
                    if (inputAudioStreams[stream].contains("x"))
                        mapID = source + ":i";
                    else
                        mapID = source;

                    ffmpegcommand.append({"-map", mapID + ":" + inputAudioStreams[stream]});
                }
            }
        }
    }
    if (MaxMuxing > 0)
    {
        ffmpegcommand.append({"-max_muxing_queue_size",QString::number(MaxMuxing)});
    }
    if (Experimental)
    {
        ffmpegcommand.append({"-strict","-2"});
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

    if (container == "MOV" || container == "MP4")
    {
        if (FastStart)
            ffmpegcommand.append({"-movflags","+faststart"});
    }

    ffmpegcommand.append({ "-c:v", codecname });


    if (codecname == "libx264" || codecname == "hevc" || codecname == "libvpx-vp9")
    {

        if (vmode.contains("Constant Rate"))
        {
            ffmpegcommand.append({ "-crf",bitrate });
            if (codecname == "libvpx-vp9")
                ffmpegcommand.append({"-qcomp","0.7"});
            if (vmode.contains("2 Pass"))
            {
                ffmpegcommand.append({ "-pass",mainQueueInfo[queue][4] });
            }
        }

        if (vmode.contains("Bitrate"))
        {
            ffmpegcommand.append({ "-b:v",bitrate + "k" });
            if (codecname == "libvpx-vp9")
                ffmpegcommand.append({"-qcomp","0.3"});

            if (vmode.contains("2 Pass"))
            {
                ffmpegcommand.append({ "-pass",mainQueueInfo[queue][4] });
            }
        }
        if (vmode.contains("Lossless"))
            ffmpegcommand.append({ "-x265-params","lossless=1" });
        if (codecname == "libvpx-vp9")
        {
            ffmpegcommand.append({ "-tile-columns",tilesThreads[0],"-threads",tilesThreads[1],"-auto-alt-ref", "1", "-row-mt", "1", "-lag-in-frames", "25" });
        }
        else
        {
            ffmpegcommand.append({ "-preset", preset.toLower().replace(" ","") });
            if (tune != "(None)" && !vmode.contains("Lossless"))
                ffmpegcommand.append({ "-tune", tune.toLower().replace(" ","") });
        }
        if (codecname != "libvpx-vp9" && DisableBFrames)
            ffmpegcommand.append({ "-bf", "0" });
    }
    if (codecname == "dnxhd" || codecname == "prores")
    {

        ffmpegcommand.append({"-profile:v",getProfile(codecname,colorspace,vmode)});
    }

    if (codecname != "copy")
    {
        ffmpegcommand.append({"-pix_fmt", colorspace });
        if (vfilters != "")
        {
            ffmpegcommand.append({"-vf",vfilters});
        }
        if (colorspace.contains("yuv") && vmode != "Finishing Quality")
        {
            if (codecname == "prores")
            {
                QString colorprimaries = colormatrix;
                ffmpegcommand.append({"-bsf:v","prores_metadata=color_primaries="+colorprimaries.replace("nc","")+":colorspace="+colormatrix});
            }
            else
                ffmpegcommand.append({"-colorspace", colormatrix });
        }
        if (deinterlace == "VFR to CFR")
        {
            if (fieldorder != "From Source")
                inputFPS = fieldorder;
            ffmpegcommand.append({"-vsync","1","-r",inputFPS});
        }
    }
    else
    {
        ffmpegcommand.append({"-r",inputFPS});
        if (aspectratio != "No")
            ffmpegcommand.append({"-aspect",aspectratio.replace(":","/").replace("/1","")});
    }

    if (astream != "None")
    {
        if (astream == "All")
        {
            int skipped = 0;
            for (int i = 0; i < inputMediaInfo[0][1].toInt(); i++)
            {
                bool skip = false;

                bool copystream = canCopyAudio(container, inputAudioCodecs[i]);
                QString audiocodec;
                if (copystream && acopy > 0)
                {
                    audiocodec = "copy";
                }
                else
                {
                    audiocodec = acodec;
                    if (acopy == 1 && audiocodec != "copy")
                    {
                        skip = true;
                        skipped++;
                    }
                }
                if (!skip)
                {
                    ffmpegcommand.append({"-c:a:" + QString::number(i-skipped), audiocodec });
                    if (audiocodec == "aac" || audiocodec == "libmp3lame" || audiocodec == "libopus")
                    {
                        if (amode == "Quality")
                            ffmpegcommand.append({"-q:a:" + QString::number(i-skipped), abitrate });
                        else
                            ffmpegcommand.append({"-b:a:" + QString::number(i-skipped), abitrate + "k" });
                    }
                    if (audiocodec == "libopus" && inputAudioChannels[i].toInt() == 6 && inputAudioLayout[i].contains("Ls Rs") && modifymc == "No")
                        ffmpegcommand.append({"-filter:a:" + QString::number(i-skipped),"pan=5.1|FL=FL|FC=FC|FR=FR|BL=SL|BR=SR|LFE=LFE"});
                    if (audiocodec != "copy" && inputAudioChannels[i].toInt() > 2 && inputAudioLayout[i].contains("C") && modifymc == "Output Center Only")
                        ffmpegcommand.append({"-filter:a:" + QString::number(i-skipped),"pan=mono|FC=FC"});
                    if (audiocodec != "copy" && inputAudioChannels[i].toInt() > 2 && modifymc == "Downmix to Stereo")
                        ffmpegcommand.append({"-ac","2"});

                }
            }
        }
        else
        {
            if (acopy > 0 && canCopyAudio(container, inputAudioCodecs[astream.toInt()-1]))
            {
                acodec = "copy";
            }
            bool skip = false;

            if (acopy == 1 && acodec != "copy")
                skip = true;
            if (!skip)
            {
                ffmpegcommand.append({"-c:a", acodec });
                if (acodec == "aac" || acodec == "libmp3lame" || acodec == "libopus")
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
                if (acodec == "libopus" && inputAudioChannels[astream.toInt()-1].toInt() == 6 && inputAudioLayout[astream.toInt()-1].contains("Ls Rs") && modifymc == "No")
                    ffmpegcommand.append({"-filter:a","pan=5.1|FL=FL|FC=FC|FR=FR|BL=SL|BR=SR|LFE=LFE"});
                if (acodec != "copy" && inputAudioChannels[astream.toInt()-1].toInt() > 2 && inputAudioLayout[astream.toInt()-1].contains("C") && modifymc == "Output Center Only")
                    ffmpegcommand.append({"-filter:a","pan=mono|FC=FC"});
                if (acodec != "copy" && inputAudioChannels[astream.toInt()-1].toInt() > 2 && modifymc == "Downmix to Stereo")
                    ffmpegcommand.append({"-ac","2"});
            }
        }
    }
    else
    {
        ffmpegcommand.append("-an");
    }
    ffmpegcommand.append("-y");

    if (mainQueueInfo[queue][4] == "1" && vmode.contains("2 Pass"))
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


QString setupencode::OutputFile(QString originalfile, QString originalType, QString newtype)
{
    int extension = 4;
    if (!originalfile.right(4).contains("."))
        extension = 5;
    if (originalType.contains("MPEG-TS"))
        extension = 3;
    originalfile = originalfile.left(originalfile.length()-extension) + "-AMVtool." + newtype;
    return originalfile;
}

QString setupencode::getCodecName(QString codecname)
{
    codecname = codecname.toLower().replace(" ","");
    if (codecname == "dnxhr")
    {
        codecname = "dnxhd";
    }
    if (codecname == "x264")
    {
        codecname = "lib" + codecname;
    }
    if (codecname == "x265")
    {
        codecname = "hevc";
    }
    if (codecname == "vp9")
    {
        codecname = "libvpx-vp9";
    }
    return codecname;
}

QString setupencode::getColorSpace(QString colorspace, QString bitdepth)
{
    if (colorspace == "RGB24")
    {
        colorspace = "gbrp";
    }
    if (colorspace == "RGBA")
    {
        colorspace = "gbrap";
    }
    if (colorspace.contains("YUV"))
    {
        colorspace = colorspace + "P";
    }
    if (bitdepth != "8")
        colorspace.append(bitdepth+"le");

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

QString setupencode::getProfile(QString codec, QString colorspace, QString profile)
{
    if (codec == "dnxhd")
    {
        if (profile == "Low Bandwidth")
            profile = "dnxhr_lb";
        if (profile == "Standard Quality")
            profile = "dnxhr_sq";
        if (profile == "High Quality" && !colorspace.contains("10"))
            profile = "dnxhr_hq";
        if (profile == "High Quality" && colorspace.contains("10"))
            profile = "dnxhr_hqx";
        if (profile == "Finishing Quality")
            profile = "dnxhr_444";
    }
    if (codec == "prores")
    {
        codec = "prores_ks";
        if (profile == "Auto")
            profile = "-1";
        if (profile == "Proxy")
            profile = "0";
        if (profile == "LT")
            profile = "1";
        if (profile == "Standard")
            profile = "2";
        if (profile == "High Quality" && colorspace.contains("422"))
            profile = "3";
        if (profile == "High Quality" && colorspace.contains("444"))
            profile = "4";
        if (profile == "Highest Quality")
            profile = "5";

    }

    return profile;
}

QString setupencode::getAudioCodecName(QString codecname)
{
    codecname = codecname.toLower().replace(" ","");
    if (codecname == "mp3")
    {
        codecname = "lib" + codecname + "lame";
    }
    if (codecname == "opus")
    {
        codecname = "lib" + codecname;
    }
    if (codecname == "pcm")
    {
        codecname = codecname + "_s24le";
    }
    return codecname;
}

QStringList setupencode::getTilesAndThreads(int height)
{
    int tiles = 0;
    int threads = 2;
    if (height >= 360 && height < 720)
    {
        tiles = 1;
        threads = 4;
    }
    if (height >= 720 && height < 1080)
    {
        tiles = 2;
        threads = 8;
    }
    if (height >= 1080 && height < 1440)
    {
        tiles = 3;
        threads = 16;
    }
    return {QString::number(tiles),QString::number(threads)};
}

QString setupencode::SetupFilters(bool isHDR, bool convertHDR, QString inputcolorspace, QString colorspace, QString colormatrix, QString deinterlace, int cthresh, QString fieldorder, QString resize, QString aspectratio)
{

    if (!isHDR)
        convertHDR = false;
    QString outputfilters = "";
    if (deinterlace != "None" && deinterlace != "VFR to CFR")
    {
        if (deinterlace == "Deinterlace")
        {
            outputfilters.append("yadif");
            if (fieldorder == "Top")
                outputfilters.append("=parity=tff");
            if (fieldorder == "Bottom")
                outputfilters.append("=parity=bff");
        }
        else
        {
            outputfilters.append("fieldmatch=cthresh="+QString::number(cthresh));
            if (fieldorder == "Top")
                outputfilters.append(":order=tff");
            if (fieldorder == "Bottom")
                outputfilters.append(":order=bff");
            if (deinterlace == "Both")
                outputfilters.append(",yadif");
            if (fieldorder == "Top")
                outputfilters.append("=parity=tff");
            if (fieldorder == "Bottom")
                outputfilters.append("=parity=bff");
            outputfilters.append(",decimate");
        }
    }


    if (convertHDR)
    {
        if (outputfilters != "")
            outputfilters.append(",");
        outputfilters.append("zscale=t=linear:npl=100,format=gbrpf32le,zscale=p="+colormatrix+",tonemap=tonemap=hable:desat=0,zscale=t="+colormatrix+":m="+colormatrix+":r=tv,format="+colorspace);
    }
    if (resize != "No")
    {
        if (outputfilters != "")
            outputfilters.append(",");
        if (inputcolorspace.contains("RGB") && colorspace.contains("yuv"))
            outputfilters.append("colorspace=iall="+colormatrix+":all="+colormatrix+":format="+colorspace+",");
        if (resize.contains("x"))
        {
            resize = resize;
            resize.replace("x",":");
        }
        else
        {
            if (WidthResolutions.contains(resize))
                resize = resize+":-2";
            if (HeightResolutions.contains(resize))
                resize = "-2:"+resize;
        }


        outputfilters.append("zscale="+resize+":filter=spline36");
    }
    if (aspectratio != "No")
    {
        if (outputfilters != "")
            outputfilters.append(",");
        outputfilters.append("setdar=dar="+aspectratio.replace(":","/").replace("/1",""));
    }
    else
    {
        if (resize != "No")
        {
            outputfilters.append(",setsar=sar=1:1");
        }
    }

    return outputfilters;
}

bool setupencode::canCopyAudio(QString container, QString format)
{
    bool cancopyaudio = false;
    QStringList containerlist;

    if (container == "AVI")
        containerlist.append( {"MPEG Audio", "PCM"} );
    if (container == "MOV")
        containerlist.append( {"AAC", "AC-3", "ALAC", "PCM"} );
    if (container == "MP4")
        containerlist.append( {"AAC", "AC-3", "MPEG Audio"} );
    if (container == "WEBM")
        containerlist.append( {"Opus", "Vorbis"} );

    if (containerlist.contains(format) || container == "MKV")
    {
        cancopyaudio = true;
    }
    return cancopyaudio;
}
