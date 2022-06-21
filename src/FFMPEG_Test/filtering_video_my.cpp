//
//#define _XOPEN_SOURCE 600 /* for usleep */
////#include <unistd.h>				// #include <io.h>로 대체가 된다고..?
//#include <io.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//extern "C" {
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/buffersink.h>
//#include <libavfilter/buffersrc.h>
//#include <libavutil/opt.h>
//}
//
//
//const char *filter_descr = "scale=78:24,transpose=cclock";
///* other way:
//   scale=78:24 [scl]; [scl] transpose=cclock // assumes "[in]" and "[out]" to be input output pads respectively
// */
//
//static AVFormatContext *fmt_ctx;
//static AVCodecContext *dec_ctx;
//AVFilterContext *buffersink_ctx;
//AVFilterContext *buffersrc_ctx;
//AVFilterGraph *filter_graph;
//static int video_stream_index = -1;
//static int64_t last_pts = AV_NOPTS_VALUE;
//
//
//#ifdef av_err2str
//#undef av_err2str
//#include <string>
//av_always_inline std::string av_err2string(int errnum) {
//	char str[AV_ERROR_MAX_STRING_SIZE];
//	return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
//}
//#define av_err2str(err) av_err2string(err).c_str()
//#endif  // av_err2str
//
//
//static int open_input_file(const char *filename)
//{
//	const AVCodec *dec;
//	int ret;
//
//	// Open AVFormatContext
//	if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
//		return ret;
//	}
//
//	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
//		return ret;
//	}
//
//	/* select the video stream */
//	ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
//	if (ret < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
//		return ret;
//	}
//	video_stream_index = ret;
//
//	/* create decoding context */
//	dec_ctx = avcodec_alloc_context3(dec);
//	if (!dec_ctx)
//		return AVERROR(ENOMEM);
//
//
//	avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
//
//	/* init the video decoder */
//	if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
//		return ret;
//	}
//
//	return 0;
//}
//
//static int init_filters(const char *filters_descr)
//{
//	char args[512];
//	int ret = 0;
//	const AVFilter *buffersrc = avfilter_get_by_name("buffer");
//	const AVFilter *buffersink = avfilter_get_by_name("buffersink");
//	AVFilterInOut *outputs = avfilter_inout_alloc();
//	AVFilterInOut *inputs = avfilter_inout_alloc();
//	AVRational time_base = fmt_ctx->streams[video_stream_index]->time_base;
//	enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };
//
//	filter_graph = avfilter_graph_alloc();
//	if (!outputs || !inputs || !filter_graph) {
//		ret = AVERROR(ENOMEM);
//		goto end;
//	}
//
//	/* buffer video source: the decoded frames from the decoder will be inserted here. */
//	snprintf(args, sizeof(args),
//		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
//		dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
//		time_base.num, time_base.den,
//		dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);
//
//	ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
//		args, NULL, filter_graph);
//	if (ret < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
//		goto end;
//	}
//
//	/* buffer video sink: to terminate the filter chain. */
//	ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
//		NULL, NULL, filter_graph);
//	if (ret < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
//		goto end;
//	}
//
//	ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
//		AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
//	if (ret < 0) {
//		av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
//		goto end;
//	}
//
//	/*
//	 * Set the endpoints for the filter graph. The filter_graph will
//	 * be linked to the graph described by filters_descr.
//	 */
//
//	 /*
//	  * The buffer source output must be connected to the input pad of
//	  * the first filter described by filters_descr; since the first
//	  * filter input label is not specified, it is set to "in" by
//	  * default.
//	  */
//	outputs->name = av_strdup("in");
//	outputs->filter_ctx = buffersrc_ctx;
//	outputs->pad_idx = 0;
//	outputs->next = NULL;
//
//	/*
//	 * The buffer sink input must be connected to the output pad of
//	 * the last filter described by filters_descr; since the last
//	 * filter output label is not specified, it is set to "out" by
//	 * default.
//	 */
//	inputs->name = av_strdup("out");
//	inputs->filter_ctx = buffersink_ctx;
//	inputs->pad_idx = 0;
//	inputs->next = NULL;
//
//	if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
//		&inputs, &outputs, NULL)) < 0)
//		goto end;
//
//	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
//		goto end;
//
//end:
//	avfilter_inout_free(&inputs);
//	avfilter_inout_free(&outputs);
//
//	return ret;
//}
//
//#include <Windows.h>
//void usleep(__int64 usec) {
//	HANDLE timer;
//	LARGE_INTEGER ft;
//
//	ft.QuadPart = -(10 * usec);
//
//	timer = CreateWaitableTimer(NULL, TRUE, NULL);
//	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
//	WaitForSingleObject(timer, INFINITE);
//	CloseHandle(timer);
//}
//
//static void display_frame(const AVFrame *frame, AVRational time_base)
//{
//	int x, y;
//	uint8_t *p0, *p;
//	int64_t delay;
//
//	if (frame->pts != AV_NOPTS_VALUE) {
//		if (last_pts != AV_NOPTS_VALUE) {
//			/* sleep roughly the right amount of time;
//			 * usleep is in microseconds, just like AV_TIME_BASE. */
//			 //delay = av_rescale_q(frame->pts - last_pts,
//			 //	time_base, AV_TIME_BASE_Q);
//			delay = av_rescale_q(frame->pts - last_pts,
//				time_base, av_get_time_base_q());
//
//			if (delay > 0 && delay < 1000000)
//				usleep(delay);
//		}
//		last_pts = frame->pts;
//	}
//
//	/* Trivial ASCII grayscale display. */
//	p0 = frame->data[0];
//	puts("\033c");
//	for (y = 0; y < frame->height; y++) {
//		p = p0;
//		for (x = 0; x < frame->width; x++)
//			putchar(" .-+#"[*(p++) / 52]);
//		putchar('\n');
//		p0 += frame->linesize[0];
//	}
//	fflush(stdout);
//}
//
//int main(/*int argc, char **argv*/)
//{
//	int ret;
//	AVPacket *packet;
//	AVFrame *frame;
//	AVFrame *filt_frame;
//
//	const char* file_path = "rtsp://root:asdf12345!@192.168.10.244:554/AVStream1_2"; //"C:/Users/lhu/source/repos/FFMPEG_Test/test.mp4";
//
//	frame = av_frame_alloc();
//	filt_frame = av_frame_alloc();
//	packet = av_packet_alloc();
//	if (!frame || !filt_frame || !packet) {
//		fprintf(stderr, "Could not allocate frame or packet\n");
//		exit(1);
//	}
//
//	// 1. file_path를 통해 AVFormatContext, AVCodecContext 초기화.
//	if ((ret = open_input_file(file_path/*argv[1]*/)) < 0)
//		goto end;
//
//	// 2. AVFilterContext* buffersink_ctx, buffersrc_ctx 초기화, AVFilterGraph* filter_graph 초기화.
//	if ((ret = init_filters(filter_descr)) < 0)
//		goto end;
//
//	while (1) {
//		// 3. Read 패킷.
//		if ((ret = av_read_frame(fmt_ctx, packet)) < 0)
//			break;
//
//		if (packet->stream_index == video_stream_index) {
//			// 4. 패킷 데이터를 디코더에 입력.
//			ret = avcodec_send_packet(dec_ctx, packet);
//			if (ret < 0) {
//				av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
//				break;
//			}
//
//			while (ret >= 0) {
//				// 5. 디코더에서 디코딩 된 frame을 반환받는다.
//				ret = avcodec_receive_frame(dec_ctx, frame);
//				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//					break;
//				}
//				else if (ret < 0) {
//					av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
//					goto end;
//				}
//
//				frame->pts = frame->best_effort_timestamp;
//
//				// 버퍼 소스(buffersrc_ctx)에 frame을 추가.
//				if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
//					av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
//					break;
//				}
//
//				while (1) {
//					// 싱크(buffersink_ctx)에서 필터링된 데이터가 있는 frame을 가져와 frame에 넣는다.
//					ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
//					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//						break;
//					if (ret < 0)
//						goto end;
//					display_frame(filt_frame, buffersink_ctx->inputs[0]->time_base);
//					av_frame_unref(filt_frame);
//				}
//				av_frame_unref(frame);
//			}
//		}
//		av_packet_unref(packet);
//	}
//end:
//	avfilter_graph_free(&filter_graph);
//	avcodec_free_context(&dec_ctx);
//	avformat_close_input(&fmt_ctx);
//	av_frame_free(&frame);
//	av_frame_free(&filt_frame);
//	av_packet_free(&packet);
//
//	if (ret < 0 && ret != AVERROR_EOF) {
//		fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
//		exit(1);
//	}
//
//	exit(0);
//}
