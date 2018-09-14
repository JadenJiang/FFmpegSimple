#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
extern "C"
{
#include "libavutil/buffer.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
}


enum {
    OBS_NAL_UNKNOWN = 0,
    OBS_NAL_SLICE = 1,
    OBS_NAL_SLICE_DPA = 2,
    OBS_NAL_SLICE_DPB = 3,
    OBS_NAL_SLICE_DPC = 4,
    OBS_NAL_SLICE_IDR = 5,
    OBS_NAL_SEI = 6,
    OBS_NAL_SPS = 7,
    OBS_NAL_PPS = 8,
    OBS_NAL_AUD = 9,
    OBS_NAL_FILLER = 12,
};


typedef enum {
    H264_SEI_TYPE_BUFFERING_PERIOD = 0,   ///< buffering period (H.264, D.1.1)
    H264_SEI_TYPE_PIC_TIMING = 1,   ///< picture timing
    H264_SEI_TYPE_FILLER_PAYLOAD = 3,   ///< filler data
    H264_SEI_TYPE_USER_DATA_REGISTERED = 4,   ///< registered user data as specified by Rec. ITU-T T.35
    H264_SEI_TYPE_USER_DATA_UNREGISTERED = 5,   ///< unregistered user data
    H264_SEI_TYPE_RECOVERY_POINT = 6,   ///< recovery point (frame # to decoder sync)
    H264_SEI_TYPE_FRAME_PACKING = 45,  ///< frame packing arrangement
    H264_SEI_TYPE_DISPLAY_ORIENTATION = 47,  ///< display orientation
    H264_SEI_TYPE_GREEN_METADATA = 56,  ///< GreenMPEG information
    H264_SEI_TYPE_ALTERNATIVE_TRANSFER = 147, ///< alternative transfer
} H264_SEI_Type;

static inline size_t alignSize(size_t sz, int n)
{
    assert((n & (n - 1)) == 0); // n is a power of 2
    return (sz + n - 1) & -n;
}



static const uint8_t *ff_avc_find_startcode_internal(const uint8_t *p, const uint8_t *end) {
    const uint8_t *a = p + 4 - ((intptr_t)p & 3);

    for (end -= 3; p < a && p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    for (end -= 3; p < end; p += 4) {
        uint32_t x = *(const uint32_t *)p;
        //      if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
        //      if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
        if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
            if (p[1] == 0) {
                if (p[0] == 0 && p[2] == 1)
                    return p;
                if (p[2] == 0 && p[3] == 1)
                    return p + 1;
            }
            if (p[3] == 0) {
                if (p[2] == 0 && p[4] == 1)
                    return p + 2;
                if (p[4] == 0 && p[5] == 1)
                    return p + 3;
            }
        }
    }

    for (end += 3; p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1)
            return p;
    }

    return end + 3;
}


static const uint8_t *avc_find_startcode(const uint8_t *p, const uint8_t *end) {
    const uint8_t *out = ff_avc_find_startcode_internal(p, end);
    if (p < out && out < end && !out[-1])
        out--;
    return out;
}

const uint8_t* decode_unregistered_user_data(const uint8_t *sei_start, const uint8_t *sei_end) {
    const uint8_t *uuid = sei_start;
    const uint8_t *payload = sei_start + 16;
    //std::cout << uuid << std::endl;
   // std::cout << payload << std::endl;
    if (strncmp((const char*)payload, "vp_", 3) == 0) {
        return payload;
    }
    return nullptr;
}



const uint8_t* ff_h264_sei_decode(const uint8_t *sei_start, const uint8_t *sei_end) {
    int offset = 0;
    const uint8_t *offset_ptr = sei_start;

    while ((sei_end - offset_ptr) > 2 &&  *((uint16_t*)offset_ptr))
    {
        int type = 0;
        int32_t size = 0;
        //int32_t next;
        int ret = 0;
        do 
        {
            if ((sei_end - offset_ptr) < 1)
                return nullptr;
            type += *((uint8_t*)offset_ptr);
        } while (*((uint8_t*)offset_ptr++) == 255);


        do
        {
            if ((sei_end - offset_ptr) < 1)
                return nullptr;
            size += *((uint8_t*)offset_ptr);
        } while (*((uint8_t*)offset_ptr++) == 255);

        if (size > (sei_end - offset_ptr)) {
            printf("error SEI type %d size %d truncated at %d\n", type, size, sei_end - offset_ptr);
            return nullptr;
        }
        switch (type)
        {
        case H264_SEI_TYPE_USER_DATA_UNREGISTERED: {
            const uint8_t *vp_sei = decode_unregistered_user_data(offset_ptr, offset_ptr + size);
            if (vp_sei != nullptr)
                return vp_sei;
            break;
        }

        default:
            break;
        }
        offset_ptr += size;
    }

    return nullptr;
}

const uint8_t *vp_sei_user_data(const uint8_t *start, const uint8_t *end) {
    const uint8_t *end_ptr = end;
    const uint8_t *nal_start = start;
    const uint8_t *nal_end;
    int sei_size, type;

    nal_start = avc_find_startcode(nal_start, end_ptr);
    while (true) {
        while (nal_start < end_ptr && !(*(uint8_t*)nal_start++));

        if (nal_start == end_ptr)
            break;

        nal_end = avc_find_startcode(nal_start, end_ptr);
        sei_size = nal_end - nal_start;
        type = *(nal_start++) & 0x1F;
        if (type == OBS_NAL_SEI) {
            return ff_h264_sei_decode(nal_start, nal_end);
            break;
        }
        nal_start = nal_end;
    }
    return nullptr;
}

int simplest_h264SEI_parser(char *url) {

    int ret = 0;
    url = R"(D:\videoFile\test\output.mp4)";
    AVFormatContext *ifmt_ctx = nullptr;
    AVBitStreamFilterContext *bsf = av_bitstream_filter_init("h264_mp4toannexb");

    if((ret = avformat_open_input(&ifmt_ctx, url, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    while (true)
    {
        AVPacket packet;
        ret = av_read_frame(ifmt_ctx, &packet);
        if (ret < 0)
            break;
        int stream_index = packet.stream_index;
        enum AVMediaType media_type = ifmt_ctx->streams[stream_index]->codecpar->codec_type;
        if (media_type != AVMEDIA_TYPE_VIDEO) {
            av_packet_unref(&packet);
            continue;
        }
        av_apply_bitstream_filters(ifmt_ctx->streams[stream_index]->codec, &packet, bsf);

        const uint8_t *vp_sei = vp_sei_user_data(packet.data, packet.data + packet.size);
        if (vp_sei != nullptr) {
            std::cout << "find:  " << vp_sei << std::endl;
        }

        av_packet_unref(&packet);
    }    

 
    return 0;
}



