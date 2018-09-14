#pragma once

#ifdef __cplusplus
extern "C"
#endif
{
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/audio_fifo.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
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


extern std::function<void(AVAudioFifo*)> FiFo_DeleteFun;
extern std::function<void(SwrContext*)> Swr_DeleteFun;
extern std::function<void(AVFrame*)> AVFrame_DeleteFun;

class AudioFiFO {
public:
    AudioFiFO();
    ~AudioFiFO();

    int initFiFO(int32_t channels, enum AVSampleFormat format);

    int getFifoSize();

    void refreshFifo();
    int pushAudioData(uint8_t **input_samples, const int frame_size);

    int popAudioData(uint8_t **output_samples, const int nb_samples);

private:
    std::unique_ptr<AVAudioFifo, decltype(FiFo_DeleteFun)> m_audioFifo;
    int32_t m_channels;
    enum AVSampleFormat m_format;

};



class AudioResample {
public:
    AudioResample();

    ~AudioResample();
    void setInputAudioParm(int32_t sample_rate, int32_t channels, enum AVSampleFormat format);

    void setOutputAudioParm(int32_t sample_rate, int32_t channels, enum AVSampleFormat format);

    int createEngine();

    int pushAudioData(const uint8_t **input_samples, const int frame_size);

    /**
    * Read data from Fifo.
    * @return       number of samples actually read, or negative AVERROR code
    *               on failure. The number of samples actually read will not
    *               be greater than nb_samples, and will only be less than
    *               nb_samples fifo_size is less than nb_samples.
    */
    int popAudioData(uint8_t **output_samples, const int nb_samples);
    int getFifoSize();

    void refreshFifo();
private:
    void reallocFrame(int frame_size);


private:
    std::unique_ptr<SwrContext, decltype(Swr_DeleteFun)> m_swr_ctx;
    std::unique_ptr<AudioFiFO> m_audiofifo;
    std::shared_ptr<AVFrame> m_temp_buffer;

    int32_t m_src_samplerate;
    int32_t m_src_channels;
    enum AVSampleFormat m_src_format;

    int32_t m_dst_samplerate;
    int32_t m_dst_channels;
    enum AVSampleFormat m_dst_format;
};


