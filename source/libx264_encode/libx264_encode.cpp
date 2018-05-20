#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/timeb.h>
 
 
#if defined ( __cplusplus)
extern "C"
{
#include "x264/x264.h"
};
#else
#include "x264/x264.h"
#endif


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

 
 
int main(int argc, char** argv)
{
 
    int ret;
    int y_size;

    int64_t start_time, end_time;
    int  i_frame_output = 0;
    start_time = x264_mdate();
 
    //FILE* fp_src  = fopen("../cuc_ieschool_640x360_yuv444p.yuv", "rb");
    FILE* fp_src  = fopen(R"(F:\videoFile\720p.yuv420p)", "rb");
 
    FILE* fp_dst = fopen(R"(F:\videoFile\1_.h264)", "wb");
        
	//Encode 50 frame
	//if set 0, encode all frame
	int frame_num=500;
	int csp=X264_CSP_I420;
	int width=1280,height=720;

	int iNal   = 0;
	x264_nal_t* pNals = NULL;
	x264_t* pHandle   = NULL;
	x264_picture_t* pPic_in = (x264_picture_t*)malloc(sizeof(x264_picture_t));
	x264_picture_t* pPic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));
	x264_param_t* pParam = (x264_param_t*)malloc(sizeof(x264_param_t));
	
	//Check
	if(fp_src==NULL||fp_dst==NULL){
		printf("Error open files.\n");
		return -1;
	}

	x264_param_default(pParam);
	x264_param_apply_profile(pParam, x264_profile_names[5]);
	x264_param_default_preset(pParam, x264_preset_names[5], x264_tune_names[3]);


	///////////////////////////X264_RC_CRF///////////////////////////////////////////////
 	pParam->rc.i_rc_method = X264_RC_CRF;
 	pParam->rc.f_rf_constant = 23;

	///////////////////////////X264_RC_CQP///////////////////////////////////////////////
// 	pParam->rc.i_rc_method = X264_RC_CQP;
// 	pParam->rc.i_qp_constant = 23;
// 	pParam->rc.i_qp_min = 20;
// 	pParam->rc.i_qp_max = 25;

	//////////////////////////////X264_RC_ABR////////////////////////////////////////////
	//pParam->rc.i_rc_method = X264_RC_ABR;
	//pParam->rc.i_vbv_max_bitrate = 5000;
	//pParam->rc.i_vbv_buffer_size = 5000;
	//pParam->rc.i_bitrate   = 5000;
	//pParam->b_tff = 1;

    ret = x264_param_parse(pParam, "nal-hrd", x264_nal_hrd_names[2]);
	pParam->i_width   = width; 
	pParam->i_height  = height;
	pParam->i_csp=csp;
//	pParam->b_interlaced = 1;
// 	pParam->i_log_level  = X264_LOG_DEBUG;
// 	pParam->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;
// 	pParam->i_frame_total = 0;
// 	pParam->i_keyint_max = 25;
// 	pParam->i_bframe  = 5;
// 	pParam->b_open_gop  = 0;
// 	pParam->i_bframe_pyramid = 0;
// 	pParam->rc.i_qp_constant=0;
// 	pParam->rc.i_qp_max=0;
// 	pParam->rc.i_qp_min=0;
// 	pParam->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;
// 	pParam->i_fps_den  = 1; 
// 	pParam->i_fps_num  = 25;
// 	pParam->i_timebase_den = pParam->i_fps_num;
// 	pParam->i_timebase_num = pParam->i_fps_den;
	
	
	
	
	pHandle = x264_encoder_open(pParam);

	if(pHandle == NULL)
		exit(-1);
    
	x264_picture_init(pPic_out);
	x264_picture_alloc(pPic_in, csp, pParam->i_width, pParam->i_height);

	//ret = x264_encoder_headers(pHandle, &pNals, &iNal);

	y_size = pParam->i_width * pParam->i_height;
	//detect frame number
	if(frame_num==0){
		fseek(fp_src,0,SEEK_END);
		switch(csp){
		case X264_CSP_I444:frame_num=ftell(fp_src)/(y_size*3);break;
		case X264_CSP_I420:frame_num=ftell(fp_src)/(y_size*3/2);break;
		default:printf("Colorspace Not Support.\n");return -1;
		}
		fseek(fp_src,0,SEEK_SET);
	}
	
	//Loop to Encode
	for(i_frame_output =0; i_frame_output<frame_num; i_frame_output++){
		switch(csp){
		case X264_CSP_I444:{
			fread(pPic_in->img.plane[0],y_size,1,fp_src);	//Y
			fread(pPic_in->img.plane[1],y_size,1,fp_src);	//U
			fread(pPic_in->img.plane[2],y_size,1,fp_src);	//V
			break;}
		case X264_CSP_I420:{
			fread(pPic_in->img.plane[0],y_size,1,fp_src);	//Y
			fread(pPic_in->img.plane[1],y_size/4,1,fp_src);	//U
			fread(pPic_in->img.plane[2],y_size/4,1,fp_src);	//V
			break;}
		default:{
			printf("Colorspace Not Support.\n");
			return -1;}
		}
		pPic_in->i_pts = i_frame_output;
 		if(i_frame_output == frame_num/2){
// 			pParam->rc.i_vbv_max_bitrate = 500;
// 			pParam->rc.i_vbv_buffer_size = 500;
// 			pParam->rc.i_bitrate   = 500;
// 			pParam->rc.i_rc_method = X264_RC_ABR;
// 			printf("<<<<reset i_bitrate[%d]>>>\n", x264_encoder_reconfig(pHandle, pParam));
		}

		ret = x264_encoder_encode(pHandle, &pNals, &iNal, pPic_in, pPic_out);
		if (ret< 0){
			printf("Error.\n");
			return -1;
		}

		//printf("Succeed encode frame: %5d  %d\n", i_frame_output,iNal);

		for (int j = 0; j < iNal; ++j){
			 fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fp_dst);
		}
	}
	//i=0;
	//flush encoder
	while(1){
		static int index = 0;
		ret = x264_encoder_encode(pHandle, &pNals, &iNal, NULL, pPic_out);
		if(ret==0){
			break;
		}
		//printf("Flush 1 frame %d .\n", ++index);
		for (int j = 0; j < iNal; ++j){
			fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fp_dst);
		}
        i_frame_output++;
	}
	x264_picture_clean(pPic_in);
	x264_encoder_close(pHandle);
	pHandle = NULL;

    end_time = x264_mdate();

    fprintf(stderr, "encoded %d frames, %.2f fps \n", i_frame_output, i_frame_output*1000000.0/(end_time - start_time));

	free(pPic_in);
	free(pPic_out);
	free(pParam);

	fclose(fp_src);
	fclose(fp_dst);

	return 0;
}
