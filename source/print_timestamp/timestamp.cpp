#include <stdio.h>
#include <map>
#include <chrono>
#include <ctime>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/mathematics.h"
};


std::map<int, std::string> media_type_map;
AVRational MilliTimeBase = { 1, 1000 };
AVRational MicroTimeBase = { 1, 1000000 };


inline static int64_t getSystemTime() {
    using namespace std::chrono;
    microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
    auto len = ms.count();
    return len;
}

inline static void print_video(int64_t pts, int64_t dts, int64_t now, bool key_frame) {
    static int64_t previous = -1;
    int64_t delay = now - pts;
    int64_t diff = 0;
    if (previous < 0) {
        diff = 0;
    }
    else {
        diff = dts - previous;
    }
    fprintf(stderr, "video key_frame[%" PRId8 "] pts[%" PRId64 "] dts[%" PRId64 "] " \
                    "now[%" PRId64 "] now-pts[%" PRId64 "] diff[%" PRId64 "]\n",
            key_frame, pts, dts, now, 
            delay, diff
        );
    previous = dts;
}

inline static void print_audio(int64_t pts, int64_t dts, int64_t now) {
    static int64_t previous = -1;
    int64_t delay = now - pts;
    int64_t diff = 0;
    if (previous < 0) {
        diff = 0;
    }
    else {
        diff = pts - previous;
    }
    fprintf(stderr, "audio key_frame[%" PRId8 "] pts[%" PRId64 "] dts[%" PRId64 "] " \
        "now[%" PRId64 "] now-pts[%" PRId64 "] diff[%" PRId64 "]\n",
        true, pts, dts, now,
        delay, diff
    );
    previous = pts;
}

inline static void printTimestamp(enum AVMediaType type, const AVPacket *pkt)
{
    //fprintf(stderr, "type[%" PRIi32 "]  pts[%" PRId64 "] dts[%" PRId64 "]\n", type, pkt->pts, pkt->dts);
    int64_t now = getSystemTime() / 1000;
    static int64_t first_video = now - pkt->pts;
    int64_t pts = pkt->pts + first_video;
    int64_t dts = pkt->dts + first_video;
    int64_t duration = pkt->duration;
    if (type == AVMEDIA_TYPE_VIDEO) {
        print_video(pts, dts, now, pkt->flags & AV_PKT_FLAG_KEY);
    }
    else if (type == AVMEDIA_TYPE_AUDIO) {
        print_audio(pts, dts, now);
    }
}


int main(int argc, char* argv[])
{
    media_type_map.insert(std::make_pair(AVMEDIA_TYPE_VIDEO, "video"));
    media_type_map.insert(std::make_pair(AVMEDIA_TYPE_AUDIO, "audio"));
    media_type_map.insert(std::make_pair(AVMEDIA_TYPE_SUBTITLE, "subtitle"));

    int ret;
    AVFormatContext *ifmt_ctx = NULL;
    AVInputFormat *ifmt = NULL;
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
    
    const char *in_filename = argv[1];
    if (in_filename == NULL) {
        //in_filename = R"(http://zhibo.hkstv.tv/livestream/mutfysrq/playlist.m3u8)";
        in_filename = R"(D:\videoFile\nba_720p_29fps.mp4)";
    }


    ret = avformat_open_input(&ifmt_ctx, in_filename, NULL, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open sourec %s failed\n", in_filename);
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_find_stream_info error\n");
        goto end;
    }
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    
    
    AVPacket pkt;
    for (;;){
        AVStream *in_stream;
        av_init_packet(&pkt);
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0) {
            break;
        }
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, MilliTimeBase, (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, MilliTimeBase, (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, MilliTimeBase);
        printTimestamp(in_stream->codecpar->codec_type, &pkt);
        av_packet_unref(&pkt);
    }
    fprintf(stderr, "read end\n");

end:
    if(ifmt_ctx)
        avformat_close_input(&ifmt_ctx);
    return ret;

}