/*
 * reference:FFmpeg\doc\examples\remuxing.c
 * */
#include "commonHeader.h"

#include "avccReader.h"

#include "avcDecoder.h"


int main(int argn, char** argv) {
    AVFormatContext *ifmt_ctx{nullptr};
    ::std::string file_name = ::std::string(RESOURCE_DIR) + "/sync.flv";
    int ret = 0;

    ret = (avformat_open_input(&ifmt_ctx, file_name.c_str(), 0, 0));
    if (ret < 0) {
        fprintf(stderr, "Could not open input file '%s'", file_name.c_str());
        return -1;
    }

    ret = (avformat_find_stream_info(ifmt_ctx, 0));
    if (ret < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        return -1;
    }

    int stream_index = -1;
    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            stream_index = i;
            break;
        }

    }

    if (stream_index == -1) {
        fprintf(stderr, "Failed to find stream info");
        return -1;
    }

    av_dump_format(ifmt_ctx, stream_index, nullptr, 0);

    AVCodecContext *video_dec_ctx = NULL;
    AVCodec *video_dec = avcodec_find_decoder(ifmt_ctx->streams[stream_index]->codecpar->codec_id);
    video_dec_ctx = avcodec_alloc_context3(video_dec);
    int codec_params_result = avcodec_parameters_to_context(video_dec_ctx, ifmt_ctx->streams[stream_index]->codecpar);
    avcodec_open2(video_dec_ctx, video_dec, nullptr);

    AvcDecode decoder;
    decoder.Init(ifmt_ctx->streams[stream_index]->codecpar->extradata, ifmt_ctx->streams[stream_index]->codecpar->extradata_size);
    
    
    AVPacket pkt;
    int packet_total = 0;
    while (true) {
        AVStream *in_stream;

        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;

        if (pkt.stream_index == stream_index) {
            decoder.SendPacket(&pkt);
            decoder.ReceiveFrame();
            packet_total++;
            
            avcodec_send_packet(video_dec_ctx, &pkt);
            AVFrame* frame = av_frame_alloc();
            avcodec_receive_frame(video_dec_ctx, frame);
            av_frame_free(&frame);
            frame = nullptr;  
        }
        av_packet_unref(&pkt);
    }
    fprintf(stderr, "Total packet %d", packet_total);
    avformat_close_input(&ifmt_ctx);
    return 0;
}
