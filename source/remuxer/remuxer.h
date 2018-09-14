#pragma once
#ifdef __cplusplus
extern "C"
#endif
{
#include "libavutil/audio_fifo.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
#include "libavutil/timestamp.h"
#ifdef __cplusplus
};
#endif

#include <thread>
#include <memory>
#include <functional>


#define ALOGD(fmt, ...)   av_log(nullptr, AV_LOG_DEBUG, "jie=[%s:%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ALOGI(fmt, ...)   av_log(nullptr, AV_LOG_INFO, "jie=[%s:%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ALOGW(fmt, ...)   av_log(nullptr, AV_LOG_WARNING, "jie=[%s:%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ALOGE(fmt, ...)   av_log(nullptr, AV_LOG_ERROR, "jie=[%s:%d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


