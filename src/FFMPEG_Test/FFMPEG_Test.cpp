#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>

#include <libswscale/swscale.h>	
#include <libavutil/imgutils.h>
}

#include <opencv2/opencv.hpp>

#ifdef _DEBUG
#pragma comment(lib, "opencv_world454d.lib")
#else
#pragma comment(lib, "opencv_world454.lib")
#endif

cv::Mat avframeToCvmat(const AVFrame *frame) {
	int width = frame->width;
	int height = frame->height;
	cv::Mat image(height, width, CV_8UC3);

	int cvLinesizes[1];
	cvLinesizes[0] = image.step1();

	SwsContext *conversion = sws_getContext(
		width, height,
		(AVPixelFormat)frame->format, 
		width, height,
		AVPixelFormat::AV_PIX_FMT_BGR24,
		SWS_FAST_BILINEAR, 
		NULL, NULL, NULL);

	sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
	sws_freeContext(conversion);

	return image;
}

int main() {
	const char* file_path = "C:/Users/lhu/source/repos/FFMPEG_Test/test.mp4";
	int ret = 0;
	int stream_index = 0;

	// 1. Set AVFormatContext
	AVFormatContext* pFormatCtx = NULL;
	ret = avformat_open_input(&pFormatCtx, file_path, NULL, NULL);
	if (ret < 0) return -1;

	// 2. Find Stream Info
	ret = avformat_find_stream_info(pFormatCtx, NULL);
	if (ret < 0) return -1;

	// 3. Select video stream
	const AVCodec* codec = NULL;
	ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if (ret < 0) return -1;

	stream_index = ret;

	// 4. Create Decoding Context
	AVCodecContext* pDecodingContext = NULL;
	pDecodingContext = avcodec_alloc_context3(codec);
	if (!pDecodingContext)
		return AVERROR(ENOMEM);

	avcodec_parameters_to_context(pDecodingContext, pFormatCtx->streams[stream_index]->codecpar);

	// 5. Init Video decoder
	ret = avcodec_open2(pDecodingContext, codec, NULL);
	if (ret < 0) return -1;

	// 6. Create packet, frame
	AVPacket* packet = NULL;
	AVFrame* frame = NULL;
	packet = av_packet_alloc();
	frame = av_frame_alloc();

	if (!packet || !frame)
		return -1;

	cv::Mat frame_image;

	while (1) {
		if (av_read_frame(pFormatCtx, packet) < 0)
			break;

		if (packet->stream_index == stream_index) {
			// 디코딩 명령 수행
			ret = avcodec_send_packet(pDecodingContext, packet);
			if (ret < 0)
				break;
			
			while (ret >= 0) {
				// 디코딩 된 데이터 획득
				ret = avcodec_receive_frame(pDecodingContext, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				}
				else if (ret < 0) {
					return -1;
				}

				printf("saving frame %3d\n", pDecodingContext->frame_number);

				auto data = frame->data[0];
				auto linesize = frame->linesize[0];
				auto width = frame->width;
				auto height = frame->height;

				frame_image = avframeToCvmat(frame);
				if (!frame_image.empty()) {
					cv::imshow("view_window", frame_image);
					cv::waitKey(10);
				}
			}
		}
	}

	return 0;
}