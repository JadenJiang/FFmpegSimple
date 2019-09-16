#pragma once
extern "C"
{
#include <stdio.h>
#include <errno.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}
#undef av_err2str;
const char* av_err2str(int errnum) {
    static char errbuf[1024] = { 0 };
    memset(errbuf, 0x00, sizeof(errbuf));
    av_strerror(errnum, errbuf, sizeof(errbuf));
    return errbuf;
}


char input_url[] = R"(F:\videoFile\bbb.mp4)";
char output_url[] = R"(F:\videoFile\test.h264)";
char encode_name[] = "h264_qsv";
char hwdevice_name[] = "qsv";