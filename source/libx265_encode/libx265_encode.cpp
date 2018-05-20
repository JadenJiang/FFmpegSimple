
#include <stdio.h>
#include <stdlib.h>

#if defined ( __cplusplus)
extern "C"
{
#include "x265/x265.h"
};
#else
#include "x265/x265.h"
#endif
#include <sys/types.h>
#include <sys/timeb.h>

int64_t x264_mdate(void)
{
#if _WIN32
    struct timeb tb;
    ftime(&tb);
    return ((int64_t)tb.time * 1000 + (int64_t)tb.millitm) * 1000;
#else
    struct timeval tv_date;
    gettimeofday(&tv_date, NULL);
    return (int64_t)tv_date.tv_sec * 1000000 + (int64_t)tv_date.tv_usec;
#endif
}

int main(int argc, char** argv) {
    FILE *fp_src = NULL;
    FILE *fp_dst = NULL;

    int64_t start_time, end_time;
    int  i_frame_output = 0;
    int y_size;
    int buff_size;
    char *buff = NULL;
    int ret;
    x265_nal *pNals = NULL;
    uint32_t iNal = 0;

    x265_param* pParam = NULL;
    x265_encoder* pHandle = NULL;
    x265_picture *pPic_in = NULL;

    //Encode 50 frame
    //if set 0, encode all frame
    int frame_num = 500;
    int csp = X265_CSP_I420;
    int width = 1280, height = 720;

    start_time = x264_mdate();
    fp_src = fopen(R"(F:\videoFile\720p.yuv420p)", "rb");
    //fp_src=fopen("../cuc_ieschool_640x360_yuv444p.yuv","rb");

    fp_dst = fopen(R"(F:\videoFile\111.h265)", "wb");
    //Check
    if (fp_src == NULL || fp_dst == NULL) {
        return -1;
    }

    pParam = x265_param_alloc();
    x265_param_default(pParam);
    pParam->bRepeatHeaders = 1;//write sps,pps before keyframe
    pParam->internalCsp = csp;
    pParam->sourceWidth = width;
    pParam->sourceHeight = height;
    pParam->fpsNum = 25;
    pParam->fpsDenom = 1;
    //Init
    pHandle = x265_encoder_open(pParam);
    if (pHandle == NULL) {
        printf("x265_encoder_open err\n");
        return 0;
    }
    y_size = pParam->sourceWidth * pParam->sourceHeight;

    pPic_in = x265_picture_alloc();
    x265_picture_init(pParam, pPic_in);
    switch (csp) {
    case X265_CSP_I444: {
        buff = (char *)malloc(y_size * 3);
        pPic_in->planes[0] = buff;
        pPic_in->planes[1] = buff + y_size;
        pPic_in->planes[2] = buff + y_size * 2;
        pPic_in->stride[0] = width;
        pPic_in->stride[1] = width;
        pPic_in->stride[2] = width;
        break;
    }
    case X265_CSP_I420: {
        buff = (char *)malloc(y_size * 3 / 2);
        pPic_in->planes[0] = buff;
        pPic_in->planes[1] = buff + y_size;
        pPic_in->planes[2] = buff + y_size * 5 / 4;
        pPic_in->stride[0] = width;
        pPic_in->stride[1] = width / 2;
        pPic_in->stride[2] = width / 2;
        break;
    }
    default: {
        printf("Colorspace Not Support.\n");
        return -1;
    }
    }

    //detect frame number
    if (frame_num == 0) {
        fseek(fp_src, 0, SEEK_END);
        switch (csp) {
        case X265_CSP_I444:frame_num = ftell(fp_src) / (y_size * 3); break;
        case X265_CSP_I420:frame_num = ftell(fp_src) / (y_size * 3 / 2); break;
        default:printf("Colorspace Not Support.\n"); return -1;
        }
        fseek(fp_src, 0, SEEK_SET);
    }

    //Loop to Encode
    for (i_frame_output = 0; i_frame_output<frame_num; i_frame_output++) {
        switch (csp) {
        case X265_CSP_I444: {
            fread(pPic_in->planes[0], 1, y_size, fp_src);		//Y
            fread(pPic_in->planes[1], 1, y_size, fp_src);		//U
            fread(pPic_in->planes[2], 1, y_size, fp_src);		//V
            break; }
        case X265_CSP_I420: {
            fread(pPic_in->planes[0], 1, y_size, fp_src);		//Y
            fread(pPic_in->planes[1], 1, y_size / 4, fp_src);	//U
            fread(pPic_in->planes[2], 1, y_size / 4, fp_src);	//V
            break; }
        default: {
            printf("Colorspace Not Support.\n");
            return -1; }
        }

        ret = x265_encoder_encode(pHandle, &pNals, &iNal, pPic_in, NULL);
        printf("Succeed encode %5d frames\n", i_frame_output);

        for (int j = 0; j<iNal; j++) {
            fwrite(pNals[j].payload, 1, pNals[j].sizeBytes, fp_dst);
        }
    }
    //Flush Decoder
    while (1) {
        ret = x265_encoder_encode(pHandle, &pNals, &iNal, NULL, NULL);
        if (ret == 0) {
            break;
        }
        printf("Flush 1 frame.\n");

        for (int j = 0; j<iNal; j++) {
            fwrite(pNals[j].payload, 1, pNals[j].sizeBytes, fp_dst);
        }
        i_frame_output++;

    }

    x265_encoder_close(pHandle);
    x265_picture_free(pPic_in);
    x265_param_free(pParam);

    end_time = x264_mdate();

    fprintf(stderr, "encoded %d frames, %.2f fps \n", i_frame_output, i_frame_output*1000000.0 / (end_time - start_time));

    free(buff);
    fclose(fp_src);
    fclose(fp_dst);

    return 0;
}