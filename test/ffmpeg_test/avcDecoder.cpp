#include "avcDecoder.h"
#include "avccReader.h"

int BitOneRead(BitReader& reader) {
    if (reader.bit_offset_ == 8) {
        reader.pos_++;
        reader.bit_offset_ = 0;
    }
    
    int data = reader.buffer_[reader.pos_];
    int result = (data >> (7 - reader.bit_offset_)) & 0x1;
    reader.bit_offset_++;
    return result;
}

int BitRead(BitReader& reader, int length) {
    int result = 0;
    for (int i = 0; i < length; i++) {
        result <<= 1;
        result |= (BitOneRead(reader));
    }
    return result;
}

int BitUeRead(BitReader& reader) {
    int count = 0;
    while (BitRead(reader, 1) == 0) {
        count++;
    }
    
    int value = BitRead(reader, count);
    int result = (1 << (count)) - 1 + value;
    return result;
}

int BitSeRead(BitReader& reader) {
    int ue = BitUeRead(reader);
    if (ue & 1) {
        ue = (ue + 1) / 2;
    } else {
        ue = - (ue / 2);
    }
    return ue;
}

void BitReadReserve(BitReader& reader) {
    if (reader.bit_offset_ == 8) {
        reader.pos_++;
        reader.bit_offset_ = 0;
    }
    
    if (reader.bit_offset_ != 0) {
        BitRead(reader, 8 - reader.bit_offset_);
    }
}


void AvcDecode::AddSps(BitReader& reader) {
    SeqParameterSet sps;
    sps.size_ = BitRead(reader, 16);
    sps.reserved_alpha_ = BitRead(reader, 1);
    sps.ref_idc_ = BitRead(reader, 2);
    sps.unit_type_ = BitRead(reader, 5);

    sps.nal_data_ = nullptr;
    SpsEntity* entity = new SpsEntity();
    sps.nal_data_ = entity;
    entity->profile_idc_ = BitRead(reader, 8);
    entity->constraint_set_flag_.constraint_set0_flag_ = BitRead(reader, 1);
    entity->constraint_set_flag_.constraint_set1_flag_ = BitRead(reader, 1);
    entity->constraint_set_flag_.constraint_set2_flag_ = BitRead(reader, 1);
    entity->constraint_set_flag_.constraint_set3_flag_ = BitRead(reader, 1);
    entity->constraint_set_flag_.constraint_set4_flag_ = BitRead(reader, 1);
    entity->constraint_set_flag_.constraint_set5_flag_ = BitRead(reader, 1);
    entity->constraint_set_flag_.reserved_zero_2bits_ = BitRead(reader, 2);
 
    entity->level_idc_ = BitRead(reader, 8);
    entity->seq_parameter_set_id_ = BitUeRead(reader);
    
    entity->high_profile_specific_.chroma_format_idc_ = BitUeRead(reader);
    entity->high_profile_specific_.bit_depth_luma_minus8_ = BitUeRead(reader);
    entity->high_profile_specific_.bit_depth_chroma_minus8 = BitUeRead(reader);
    entity->high_profile_specific_.qpprime_y_zero_transform_bypass_flag_ = BitRead(reader, 1);
    entity->high_profile_specific_.seq_scaling_matrix_present_flag_ = BitRead(reader, 1);
    
    entity->log2_max_frame_num_minus4_ = BitUeRead(reader);
    entity->pic_order_cnt_type_ = BitUeRead(reader);
    entity->log2_max_pic_order_cnt_lsb_minus4_ = BitUeRead(reader);
    entity->num_ref_frames_ = BitUeRead(reader);
    entity->gaps_in_frame_num_value_allowed_flag_ = BitRead(reader, 1);
    entity->pic_width_in_mbs_minus1_ = BitUeRead(reader);
    entity->pic_height_in_map_units_minus1_ = BitUeRead(reader);
    entity->frame_mbs_only_flag_ = BitRead(reader, 1);
    entity->direct_8x8_inference_flag_ = BitRead(reader, 1);
    entity->frame_cropping_flag_ = BitRead(reader, 1);
    
    entity->frame_cropping_.frame_crop_left_offset_  = BitUeRead(reader);
    entity->frame_cropping_.frame_crop_right_offset_  = BitUeRead(reader);
    entity->frame_cropping_.frame_crop_top_offset_  = BitUeRead(reader);
    entity->frame_cropping_.frame_crop_bottom_offset_  = BitUeRead(reader);
   
    entity->vui_parameters_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.aspect_ratio_info_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.aspect_ratio_info_.aspect_ratio_idc_  = BitRead(reader, 8);
    entity->vui_parameters_.overscan_info_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.video_signal_type_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.chroma_loc_info_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.timing_info_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.time_info_.num_units_in_tick_ = BitRead(reader, 32);
    entity->vui_parameters_.time_info_.time_scale_ = BitRead(reader, 32);
    entity->vui_parameters_.time_info_.fixed_frame_rate_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.nal_hrd_parameters_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.vcl_hrd_parameters_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.pic_struct_present_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.bitstream_restriction_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.bitstream_restriction_.motion_vectors_over_pic_boundaries_flag_ = BitRead(reader, 1);
    entity->vui_parameters_.bitstream_restriction_.max_bytes_per_pic_denom_ = BitUeRead(reader);
    entity->vui_parameters_.bitstream_restriction_.max_bits_per_mb_denom_ = BitUeRead(reader);
    entity->vui_parameters_.bitstream_restriction_.log2_max_mv_length_horizontal_ = BitUeRead(reader);
    entity->vui_parameters_.bitstream_restriction_.log2_max_mv_length_vertical_ = BitUeRead(reader);
    entity->vui_parameters_.bitstream_restriction_.max_num_reorder_frames_ = BitUeRead(reader);
    entity->vui_parameters_.bitstream_restriction_.max_dec_frame_buffering_ = BitUeRead(reader);

    sps_.push_back(sps);
}

void AvcDecode::AddPps(BitReader& reader) {
    PicParameterSet pps;
    pps.size_ = BitRead(reader, 16);
    pps.reserved_alpha_ = BitRead(reader, 1);
    pps.ref_idc_ = BitRead(reader, 2);
    pps.unit_type_ = BitRead(reader, 5);
    pps.nal_data_ = nullptr;
    PpsEntity* entity = new PpsEntity();
    pps.nal_data_ = entity;
    
    entity->pic_parameter_set_id_ = BitUeRead(reader);
    entity->seq_parameter_set_id_ = BitUeRead(reader);
    entity->entropy_coding_mode_flag_ = BitRead(reader, 1);
    entity->bottom_field_pic_order_in_frame_present_flag_ = BitRead(reader, 1);
    entity->num_slice_groups_minus1_ = BitUeRead(reader);
    entity->num_ref_idx_l0_default_active_minus1_ = BitUeRead(reader);
    entity->num_ref_idx_l1_default_active_minus1_ = BitUeRead(reader);
    entity->weighted_pred_flag_ = BitRead(reader, 1);
    entity->weighted_bipred_idc_ = BitRead(reader, 2);
    entity->pic_init_qp_minus26_ = BitRead(reader, 1);
    entity->pic_init_qp_minus26_ = BitUeRead(reader);
    entity->chroma_qp_index_offset_ = BitUeRead(reader);
    entity->deblocking_filter_control_present_flag_ = BitRead(reader, 1);
    entity->constrained_intra_pred_flag_ = BitRead(reader, 1);
    entity->redundant_pic_cnt_present_flag_ = BitRead(reader, 1);
    entity->transform_8x8_mode_flag_ = BitRead(reader, 1);
    entity->pic_scaling_matrix_present_flag_ = BitRead(reader, 1);
    entity->second_chroma_qp_index_offset_ = BitRead(reader, 1);
    pps_.push_back(pps);
}

void AvcDecode::Init(uint8_t* extra_data, int size) {
    BitReader reader;
    reader.buffer_ = extra_data;
    reader.bit_offset_ = 0;
    reader.pos_ = 0;
    reader.size_ = size;
    
    specific_.version_ = BitRead(reader, 8);
    specific_.profile_ = BitRead(reader, 8);
    specific_.compatible_profile_ = BitRead(reader, 8);
    specific_.level_ = BitRead(reader, 8);
    specific_.reserved_alpha_ = BitRead(reader, 6);
    specific_.nalu_length_minus_one_ = BitRead(reader, 2);
    specific_.reserved_beta_ = BitRead(reader, 3);
    specific_.seq_parameter_sec_count_ = BitRead(reader, 5);

    for (int i = 0; i < specific_.seq_parameter_sec_count_; i++) {
        AddSps(reader);
        BitReadReserve(reader);
    }
    
    specific_.pic_parameter_sec_count_ = BitRead(reader, 8);
    for (int i = 0; i < specific_.pic_parameter_sec_count_; i++) {
        AddPps(reader);
        BitReadReserve(reader);
    }
}

SliceData*  AvcDecode::ReadSliceData(BitReader& reader) {
    
}



void AvcDecode::DecodeIdr(BitReader& reader, CommonNaluData& data) {
    SliceEntity* entity = new SliceEntity();
    data.nal_data_ = entity;
    
    entity->header_.first_mb_in_slice_ = BitUeRead(reader);
    entity->header_.slice_type_ = BitUeRead(reader);
    entity->header_.pic_parameter_set_id_ = BitUeRead(reader);
    
    SpsEntity* sps = (SpsEntity*)sps_[0].nal_data_;
    int frame_num_bits = sps->log2_max_frame_num_minus4_ + 4;
    entity->header_.frame_num_ = BitRead(reader, frame_num_bits);
 
    entity->header_.idr_pic_id_ = BitUeRead(reader);
    
    int pic_order_bits = sps->log2_max_pic_order_cnt_lsb_minus4_ + 4;
    entity->header_.pic_order_cnt_lsb_ = BitRead(reader, pic_order_bits);
    
    entity->header_.dec_ref_pic_marking_.no_output_of_prior_pics_flag_ = BitRead(reader, 1);
    entity->header_.dec_ref_pic_marking_.long_term_reference_flag_ = BitRead(reader, 1);
    entity->header_.slice_qp_delta_ = BitSeRead(reader);
    entity->header_.disable_deblocking_filter_idc_ = BitUeRead(reader);
    entity->header_.slice_alpha_c0_offset_div2_ = BitUeRead(reader);
    entity->header_.slice_beta_offset_div2_ = BitUeRead(reader);
    
    BitReadReserve(reader);
    entity->slice_data_ = ReadSliceData(reader);
    
    
    
    
    
    
}




void AvcDecode::SendPacket(AVPacket* pkt) {
    AvccReader avcc_reader(pkt->buf->data, pkt->buf->size);
    Nalu* nalu{nullptr};
    while ((nalu = avcc_reader.GetNalu()) != nullptr)
    {
        BitReader reader;
        reader.buffer_ = nalu->GetBuffer();
        reader.pos_ = 0;
        reader.bit_offset_ = 0;
        reader.size_ = nalu->GetBufferSize();
        
        CommonNaluData data;
        data.size_ = BitRead(reader, 32);
        data.reserved_alpha_ = BitRead(reader, 1);
        data.ref_idc_ = BitRead(reader, 2);
        data.unit_type_ = BitRead(reader, 5);
        
        switch (data.unit_type_) {
            case 6:
                // sei直接忽略
                break;
            case 5:
                // idr 帧
                DecodeIdr(reader, data);
                break;
            default:
                break;
        }
        fprintf(stderr, "Total packet %d\n", nalu->GetBufferSize());
    }
    
    
    int s = 0;
}

void AvcDecode::ReceiveFrame() {
    
}


/*
int ff_h264_decode_extradata(const uint8_t *data, int size, H264ParamSets *ps,
                             int *is_avc, int *nal_length_size,
                             int err_recognition, void *logctx)
{
    int ret;

    if (!data || size <= 0)
        return -1;

    if (data[0] == 1) {
        int i, cnt, nalsize;
        const uint8_t *p = data;

        *is_avc = 1;

        if (size < 7) {
            av_log(logctx, AV_LOG_ERROR, "avcC %d too short\n", size);
            return AVERROR_INVALIDDATA;
        }

        // Decode sps from avcC
        cnt = *(p + 5) & 0x1f; // Number of sps
        p  += 6;
        for (i = 0; i < cnt; i++) {
            nalsize = AV_RB16(p) + 2;
            if (nalsize > size - (p - data))
                return AVERROR_INVALIDDATA;
            ret = decode_extradata_ps_mp4(p, nalsize, ps, err_recognition, logctx);
            if (ret < 0) {
                av_log(logctx, AV_LOG_ERROR,
                       "Decoding sps %d from avcC failed\n", i);
                return ret;
            }
            p += nalsize;
        }
        // Decode pps from avcC
        cnt = *(p++); // Number of pps
        for (i = 0; i < cnt; i++) {
            nalsize = AV_RB16(p) + 2;
            if (nalsize > size - (p - data))
                return AVERROR_INVALIDDATA;
            ret = decode_extradata_ps_mp4(p, nalsize, ps, err_recognition, logctx);
            if (ret < 0) {
                av_log(logctx, AV_LOG_ERROR,
                       "Decoding pps %d from avcC failed\n", i);
                return ret;
            }
            p += nalsize;
        }
        // Store right nal length size that will be used to parse all other nals
        *nal_length_size = (data[4] & 0x03) + 1;
    } else {
        *is_avc = 0;
        ret = decode_extradata_ps(data, size, ps, 0, logctx);
        if (ret < 0)
            return ret;
    }
    return size;
}*/
