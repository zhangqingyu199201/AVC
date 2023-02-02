#pragma once

#include "commonHeader.h"

class NalEntity {
public:
    NalEntity() {}
    virtual ~NalEntity() {}
};

class BitReader {
public:
    uint8_t* buffer_;
    int pos_;
    int bit_offset_;
    int size_;
};



class SpsEntity : public NalEntity {
public:
    int profile_idc_; // 1 bytes;

    struct {
        int constraint_set0_flag_; // 1 bits
        int constraint_set1_flag_; // 1 bits
        int constraint_set2_flag_; // 1 bits
        int constraint_set3_flag_; // 1 bits
        int constraint_set4_flag_; // 1 bits
        int constraint_set5_flag_; // 1 bits
        int reserved_zero_2bits_; // 2 bits
    } constraint_set_flag_;
    
    int level_idc_; // 1 bytes
    uint64_t seq_parameter_set_id_; // 1 bytes
    
    struct {
        int chroma_format_idc_; // 1 bytes
        int bit_depth_luma_minus8_; // 1 bytes
        int bit_depth_chroma_minus8; // 1 bytes
        int qpprime_y_zero_transform_bypass_flag_; // 1 bits
        int seq_scaling_matrix_present_flag_; // 1 bits
    } high_profile_specific_;
    
    int log2_max_frame_num_minus4_;
    int pic_order_cnt_type_;
    int log2_max_pic_order_cnt_lsb_minus4_;
    int num_ref_frames_;
    int gaps_in_frame_num_value_allowed_flag_;
    int pic_width_in_mbs_minus1_;
    int pic_height_in_map_units_minus1_;
    int frame_mbs_only_flag_;
    int direct_8x8_inference_flag_;
    int frame_cropping_flag_;
    struct {
        int frame_crop_left_offset_;
        int frame_crop_right_offset_;
        int frame_crop_top_offset_;
        int frame_crop_bottom_offset_;
    } frame_cropping_;
    
    int vui_parameters_present_flag_;
    struct {
        int aspect_ratio_info_present_flag_;
        struct {
            int aspect_ratio_idc_;
        } aspect_ratio_info_;
        
        int overscan_info_present_flag_;
        int video_signal_type_present_flag_;
        int chroma_loc_info_present_flag_;
        int timing_info_present_flag_;
        struct {
            int num_units_in_tick_;
            int time_scale_;
            int fixed_frame_rate_flag_;
        } time_info_;
        
        int nal_hrd_parameters_present_flag_;
        int vcl_hrd_parameters_present_flag_;
        int pic_struct_present_flag_;
        int bitstream_restriction_flag_;
        struct {
            int motion_vectors_over_pic_boundaries_flag_;
            int max_bytes_per_pic_denom_;
            int max_bits_per_mb_denom_;
            int log2_max_mv_length_horizontal_;
            int log2_max_mv_length_vertical_;
            int max_num_reorder_frames_;
            int max_dec_frame_buffering_;
        } bitstream_restriction_;
    } vui_parameters_;
};

class PpsEntity : public NalEntity {
public:
    int pic_parameter_set_id_;
    int seq_parameter_set_id_;
    int entropy_coding_mode_flag_;
    int bottom_field_pic_order_in_frame_present_flag_;
    int num_slice_groups_minus1_;
    int num_ref_idx_l0_default_active_minus1_;
    int num_ref_idx_l1_default_active_minus1_;
    int weighted_pred_flag_;
    int weighted_bipred_idc_;
    int pic_init_qp_minus26_;
    int pic_init_qs_minus26_;
    int chroma_qp_index_offset_;
    int deblocking_filter_control_present_flag_;
    int constrained_intra_pred_flag_;
    int redundant_pic_cnt_present_flag_;
    int transform_8x8_mode_flag_;
    int pic_scaling_matrix_present_flag_;
    int second_chroma_qp_index_offset_;
};


class SliceData {
public:
    int cabac_alignment_one_bit_;
};



class SliceEntity : public NalEntity {
public:
    struct {
        int first_mb_in_slice_;
        int slice_type_;
        int pic_parameter_set_id_;
        int frame_num_;
        int idr_pic_id_;
        int pic_order_cnt_lsb_;
        
        struct {
            int no_output_of_prior_pics_flag_;
            int long_term_reference_flag_;
        } dec_ref_pic_marking_;

        int slice_qp_delta_;
        int disable_deblocking_filter_idc_;
        int slice_alpha_c0_offset_div2_;
        int slice_beta_offset_div2_;
    } header_;
    
    SliceData* slice_data_;
};

class CommonNaluData {
public:
    int size_;  // 2 bytes
    int reserved_alpha_; // 1 bits
    int ref_idc_; // 2 bits
    int unit_type_; // 5 bits
    NalEntity* nal_data_ = nullptr;
};

using SeqParameterSet = CommonNaluData;
using PicParameterSet = CommonNaluData;

class AvcSpecific {
public:
    int version_;
    int profile_; // 1 bytes
    int compatible_profile_; // 1 bytes
    int level_; // 1 bytes
    int reserved_alpha_; // 6 bits
    int nalu_length_minus_one_; // 2 bits
    int reserved_beta_; // 3 bits
    int seq_parameter_sec_count_; // 5 bits
    int pic_parameter_sec_count_;
};

class AvcDecode {
public:

    void Init(uint8_t* extra_data, int size);

    void AddSps(BitReader& reader);
    
    void AddPps(BitReader& reader);
    
    SliceData* ReadSliceData(BitReader& reader);
    
    void DecodeIdr(BitReader& reader, CommonNaluData& data);

    void SendPacket(AVPacket* pkt);

    void ReceiveFrame();
    
    AvcSpecific specific_;
    ::std::vector<SeqParameterSet> sps_;
    ::std::vector<PicParameterSet> pps_;
};
