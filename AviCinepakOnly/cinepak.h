/*
 * extract from FFmpeg@2023-01-17: https://github.com/FFmpeg/FFmpeg.git
 */

#ifndef _CINEPAK_H_
#define _CINEPAK_H_

/**
 * @defgroup lavc_packet AVPacket
 *
 * Types and functions for working with AVPacket.
 * @{
 */
enum AVPacketSideDataType
{
    /**
     * An AV_PKT_DATA_PALETTE side data packet contains exactly AVPALETTE_SIZE
     * bytes worth of palette. This side data signals that a new palette is
     * present.
     */
    AV_PKT_DATA_PALETTE,

    /**
     * The AV_PKT_DATA_NEW_EXTRADATA is used to notify the codec or the format
     * that the extradata buffer was changed and the receiving side should
     * act upon it appropriately. The new extradata is embedded in the side
     * data buffer and should be immediately used for processing the current
     * frame or packet.
     */
    AV_PKT_DATA_NEW_EXTRADATA,

    /**
     * An AV_PKT_DATA_PARAM_CHANGE side data packet is laid out as follows:
     * @code
     * u32le param_flags
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT)
     *     s32le channel_count
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT)
     *     u64le channel_layout
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE)
     *     s32le sample_rate
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS)
     *     s32le width
     *     s32le height
     * @endcode
     */
    AV_PKT_DATA_PARAM_CHANGE,

    /**
     * An AV_PKT_DATA_H263_MB_INFO side data packet contains a number of
     * structures with info about macroblocks relevant to splitting the
     * packet into smaller packets on macroblock edges (e.g. as for RFC 2190).
     * That is, it does not necessarily contain info about all macroblocks,
     * as long as the distance between macroblocks in the info is smaller
     * than the target payload size.
     * Each MB info structure is 12 bytes, and is laid out as follows:
     * @code
     * u32le bit offset from the start of the packet
     * u8    current quantizer at the start of the macroblock
     * u8    GOB number
     * u16le macroblock address within the GOB
     * u8    horizontal MV predictor
     * u8    vertical MV predictor
     * u8    horizontal MV predictor for block number 3
     * u8    vertical MV predictor for block number 3
     * @endcode
     */
    AV_PKT_DATA_H263_MB_INFO,

    /**
     * This side data should be associated with an audio stream and contains
     * ReplayGain information in form of the AVReplayGain struct.
     */
    AV_PKT_DATA_REPLAYGAIN,

    /**
     * This side data contains a 3x3 transformation matrix describing an affine
     * transformation that needs to be applied to the decoded video frames for
     * correct presentation.
     *
     * See libavutil/display.h for a detailed description of the data.
     */
    AV_PKT_DATA_DISPLAYMATRIX,

    /**
     * This side data should be associated with a video stream and contains
     * Stereoscopic 3D information in form of the AVStereo3D struct.
     */
    AV_PKT_DATA_STEREO3D,

    /**
     * This side data should be associated with an audio stream and corresponds
     * to enum AVAudioServiceType.
     */
    AV_PKT_DATA_AUDIO_SERVICE_TYPE,

    /**
     * This side data contains quality related information from the encoder.
     * @code
     * u32le quality factor of the compressed frame. Allowed range is between 1 (good) and FF_LAMBDA_MAX (bad).
     * u8    picture type
     * u8    error count
     * u16   reserved
     * u64le[error count] sum of squared differences between encoder in and output
     * @endcode
     */
    AV_PKT_DATA_QUALITY_STATS,

    /**
     * This side data contains an integer value representing the stream index
     * of a "fallback" track.  A fallback track indicates an alternate
     * track to use when the current track can not be decoded for some reason.
     * e.g. no decoder available for codec.
     */
    AV_PKT_DATA_FALLBACK_TRACK,

    /**
     * This side data corresponds to the AVCPBProperties struct.
     */
    AV_PKT_DATA_CPB_PROPERTIES,

    /**
     * Recommmends skipping the specified number of samples
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_PKT_DATA_SKIP_SAMPLES,

    /**
     * An AV_PKT_DATA_JP_DUALMONO side data packet indicates that
     * the packet may contain "dual mono" audio specific to Japanese DTV
     * and if it is true, recommends only the selected channel to be used.
     * @code
     * u8    selected channels (0=main/left, 1=sub/right, 2=both)
     * @endcode
     */
    AV_PKT_DATA_JP_DUALMONO,

    /**
     * A list of zero terminated key/value strings. There is no end marker for
     * the list, so it is required to rely on the side data size to stop.
     */
    AV_PKT_DATA_STRINGS_METADATA,

    /**
     * Subtitle event position
     * @code
     * u32le x1
     * u32le y1
     * u32le x2
     * u32le y2
     * @endcode
     */
    AV_PKT_DATA_SUBTITLE_POSITION,

    /**
     * Data found in BlockAdditional element of matroska container. There is
     * no end marker for the data, so it is required to rely on the side data
     * size to recognize the end. 8 byte id (as found in BlockAddId) followed
     * by data.
     */
    AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,

    /**
     * The optional first identifier line of a WebVTT cue.
     */
    AV_PKT_DATA_WEBVTT_IDENTIFIER,

    /**
     * The optional settings (rendering instructions) that immediately
     * follow the timestamp specifier of a WebVTT cue.
     */
    AV_PKT_DATA_WEBVTT_SETTINGS,

    /**
     * A list of zero terminated key/value strings. There is no end marker for
     * the list, so it is required to rely on the side data size to stop. This
     * side data includes updated metadata which appeared in the stream.
     */
    AV_PKT_DATA_METADATA_UPDATE,

    /**
     * MPEGTS stream ID as uint8_t, this is required to pass the stream ID
     * information from the demuxer to the corresponding muxer.
     */
    AV_PKT_DATA_MPEGTS_STREAM_ID,

    /**
     * Mastering display metadata (based on SMPTE-2086:2014). This metadata
     * should be associated with a video stream and contains data in the form
     * of the AVMasteringDisplayMetadata struct.
     */
    AV_PKT_DATA_MASTERING_DISPLAY_METADATA,

    /**
     * This side data should be associated with a video stream and corresponds
     * to the AVSphericalMapping structure.
     */
    AV_PKT_DATA_SPHERICAL,

    /**
     * Content light level (based on CTA-861.3). This metadata should be
     * associated with a video stream and contains data in the form of the
     * AVContentLightMetadata struct.
     */
    AV_PKT_DATA_CONTENT_LIGHT_LEVEL,

    /**
     * ATSC A53 Part 4 Closed Captions. This metadata should be associated with
     * a video stream. A53 CC bitstream is stored as uint8_t in AVPacketSideData.data.
     * The number of bytes of CC data is AVPacketSideData.size.
     */
    AV_PKT_DATA_A53_CC,

    /**
     * This side data is encryption initialization data.
     * The format is not part of ABI, use av_encryption_init_info_* methods to
     * access.
     */
    AV_PKT_DATA_ENCRYPTION_INIT_INFO,

    /**
     * This side data contains encryption info for how to decrypt the packet.
     * The format is not part of ABI, use av_encryption_info_* methods to access.
     */
    AV_PKT_DATA_ENCRYPTION_INFO,

    /**
     * Active Format Description data consisting of a single byte as specified
     * in ETSI TS 101 154 using AVActiveFormatDescription enum.
     */
    AV_PKT_DATA_AFD,

    /**
     * Producer Reference Time data corresponding to the AVProducerReferenceTime struct,
     * usually exported by some encoders (on demand through the prft flag set in the
     * AVCodecContext export_side_data field).
     */
    AV_PKT_DATA_PRFT,

    /**
     * ICC profile data consisting of an opaque octet buffer following the
     * format described by ISO 15076-1.
     */
    AV_PKT_DATA_ICC_PROFILE,

    /**
     * DOVI configuration
     * ref:
     * dolby-vision-bitstreams-within-the-iso-base-media-file-format-v2.1.2, section 2.2
     * dolby-vision-bitstreams-in-mpeg-2-transport-stream-multiplex-v1.2, section 3.3
     * Tags are stored in struct AVDOVIDecoderConfigurationRecord.
     */
    AV_PKT_DATA_DOVI_CONF,

    /**
     * Timecode which conforms to SMPTE ST 12-1:2014. The data is an array of 4 uint32_t
     * where the first uint32_t describes how many (1-3) of the other timecodes are used.
     * The timecode format is described in the documentation of av_timecode_get_smpte_from_framenum()
     * function in libavutil/timecode.h.
     */
    AV_PKT_DATA_S12M_TIMECODE,

    /**
     * HDR10+ dynamic metadata associated with a video frame. The metadata is in
     * the form of the AVDynamicHDRPlus struct and contains
     * information for color volume transform - application 4 of
     * SMPTE 2094-40:2016 standard.
     */
    AV_PKT_DATA_DYNAMIC_HDR10_PLUS,

    /**
     * The number of side data types.
     * This is not part of the public API/ABI in the sense that it may
     * change when new side data types are added.
     * This must stay the last enum value.
     * If its value becomes huge, some code using it
     * needs to be updated as it assumes it to be smaller than other limits.
     */
    AV_PKT_DATA_NB
};

/**
 * Pixel format.
 *
 * @note
 * AV_PIX_FMT_RGB32 is handled in an endian-specific manner. An RGBA
 * color is put together as:
 *  (A << 24) | (R << 16) | (G << 8) | B
 * This is stored as BGRA on little-endian CPU architectures and ARGB on
 * big-endian CPUs.
 *
 * @note
 * If the resolution is not a multiple of the chroma subsampling factor
 * then the chroma plane resolution must be rounded up.
 *
 * @par
 * When the pixel format is palettized RGB32 (AV_PIX_FMT_PAL8), the palettized
 * image data is stored in AVFrame.data[0]. The palette is transported in
 * AVFrame.data[1], is 1024 bytes long (256 4-byte entries) and is
 * formatted the same as in AV_PIX_FMT_RGB32 described above (i.e., it is
 * also endian-specific). Note also that the individual RGB32 palette
 * components stored in AVFrame.data[1] should be in the range 0..255.
 * This is important as many custom PAL8 video codecs that were designed
 * to run on the IBM VGA graphics adapter use 6-bit palette components.
 *
 * @par
 * For all the 8 bits per pixel formats, an RGB32 palette is in data[1] like
 * for pal8. This palette is filled in automatically by the function
 * allocating the picture.
 */
enum AVPixelFormat
{
    AV_PIX_FMT_NONE = -1,
    AV_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    AV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    AV_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
    AV_PIX_FMT_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    AV_PIX_FMT_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    AV_PIX_FMT_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    AV_PIX_FMT_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    AV_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
    AV_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    AV_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    AV_PIX_FMT_PAL8,      ///< 8 bits with AV_PIX_FMT_RGB32 palette
    AV_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
    AV_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
    AV_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
    AV_PIX_FMT_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    AV_PIX_FMT_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    AV_PIX_FMT_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    AV_PIX_FMT_BGR4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    AV_PIX_FMT_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    AV_PIX_FMT_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    AV_PIX_FMT_RGB4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    AV_PIX_FMT_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
    AV_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    AV_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped

    AV_PIX_FMT_ARGB, ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    AV_PIX_FMT_RGBA, ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    AV_PIX_FMT_ABGR, ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    AV_PIX_FMT_BGRA, ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    AV_PIX_FMT_GRAY16BE, ///<        Y        , 16bpp, big-endian
    AV_PIX_FMT_GRAY16LE, ///<        Y        , 16bpp, little-endian
    AV_PIX_FMT_YUV440P,  ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
    AV_PIX_FMT_YUVJ440P, ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
    AV_PIX_FMT_YUVA420P, ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    AV_PIX_FMT_RGB48BE,  ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_RGB48LE,  ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

    AV_PIX_FMT_RGB565BE, ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    AV_PIX_FMT_RGB565LE, ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    AV_PIX_FMT_RGB555BE, ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
    AV_PIX_FMT_RGB555LE, ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

    AV_PIX_FMT_BGR565BE, ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    AV_PIX_FMT_BGR565LE, ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    AV_PIX_FMT_BGR555BE, ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
    AV_PIX_FMT_BGR555LE, ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

    /**
     *  Hardware acceleration through VA-API, data[3] contains a
     *  VASurfaceID.
     */
    AV_PIX_FMT_VAAPI,

    AV_PIX_FMT_YUV420P16LE, ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P16BE, ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV422P16LE, ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P16BE, ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P16LE, ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P16BE, ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_DXVA2_VLD,   ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

    AV_PIX_FMT_RGB444LE, ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_RGB444BE, ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
    AV_PIX_FMT_BGR444LE, ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_BGR444BE, ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
    AV_PIX_FMT_YA8,      ///< 8 bits gray, 8 bits alpha

    AV_PIX_FMT_Y400A = AV_PIX_FMT_YA8,  ///< alias for AV_PIX_FMT_YA8
    AV_PIX_FMT_GRAY8A = AV_PIX_FMT_YA8, ///< alias for AV_PIX_FMT_YA8

    AV_PIX_FMT_BGR48BE, ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_BGR48LE, ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

    /**
     * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
     * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
     * If you want to support multiple bit depths, then using AV_PIX_FMT_YUV420P16* with the bpp stored separately is better.
     */
    AV_PIX_FMT_YUV420P9BE,               ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P9LE,               ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P10BE,              ///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P10LE,              ///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV422P10BE,              ///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P10LE,              ///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P9BE,               ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P9LE,               ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P10BE,              ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P10LE,              ///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P9BE,               ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P9LE,               ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_GBRP,                     ///< planar GBR 4:4:4 24bpp
    AV_PIX_FMT_GBR24P = AV_PIX_FMT_GBRP, // alias for #AV_PIX_FMT_GBRP
    AV_PIX_FMT_GBRP9BE,                  ///< planar GBR 4:4:4 27bpp, big-endian
    AV_PIX_FMT_GBRP9LE,                  ///< planar GBR 4:4:4 27bpp, little-endian
    AV_PIX_FMT_GBRP10BE,                 ///< planar GBR 4:4:4 30bpp, big-endian
    AV_PIX_FMT_GBRP10LE,                 ///< planar GBR 4:4:4 30bpp, little-endian
    AV_PIX_FMT_GBRP16BE,                 ///< planar GBR 4:4:4 48bpp, big-endian
    AV_PIX_FMT_GBRP16LE,                 ///< planar GBR 4:4:4 48bpp, little-endian
    AV_PIX_FMT_YUVA422P,                 ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    AV_PIX_FMT_YUVA444P,                 ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    AV_PIX_FMT_YUVA420P9BE,              ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
    AV_PIX_FMT_YUVA420P9LE,              ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
    AV_PIX_FMT_YUVA422P9BE,              ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
    AV_PIX_FMT_YUVA422P9LE,              ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
    AV_PIX_FMT_YUVA444P9BE,              ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    AV_PIX_FMT_YUVA444P9LE,              ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AV_PIX_FMT_YUVA420P10BE,             ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA420P10LE,             ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA422P10BE,             ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA422P10LE,             ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA444P10BE,             ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA444P10LE,             ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA420P16BE,             ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA420P16LE,             ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA422P16BE,             ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA422P16LE,             ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA444P16BE,             ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA444P16LE,             ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

    AV_PIX_FMT_VDPAU, ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

    AV_PIX_FMT_XYZ12LE, ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
    AV_PIX_FMT_XYZ12BE, ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
    AV_PIX_FMT_NV16,    ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    AV_PIX_FMT_NV20LE,  ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_NV20BE,  ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

    AV_PIX_FMT_RGBA64BE, ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_RGBA64LE, ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    AV_PIX_FMT_BGRA64BE, ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_BGRA64LE, ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

    AV_PIX_FMT_YVYU422, ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

    AV_PIX_FMT_YA16BE, ///< 16 bits gray, 16 bits alpha (big-endian)
    AV_PIX_FMT_YA16LE, ///< 16 bits gray, 16 bits alpha (little-endian)

    AV_PIX_FMT_GBRAP,     ///< planar GBRA 4:4:4:4 32bpp
    AV_PIX_FMT_GBRAP16BE, ///< planar GBRA 4:4:4:4 64bpp, big-endian
    AV_PIX_FMT_GBRAP16LE, ///< planar GBRA 4:4:4:4 64bpp, little-endian
    /**
     * HW acceleration through QSV, data[3] contains a pointer to the
     * mfxFrameSurface1 structure.
     *
     * Before FFmpeg 5.0:
     * mfxFrameSurface1.Data.MemId contains a pointer when importing
     * the following frames as QSV frames:
     *
     * VAAPI:
     * mfxFrameSurface1.Data.MemId contains a pointer to VASurfaceID
     *
     * DXVA2:
     * mfxFrameSurface1.Data.MemId contains a pointer to IDirect3DSurface9
     *
     * FFmpeg 5.0 and above:
     * mfxFrameSurface1.Data.MemId contains a pointer to the mfxHDLPair
     * structure when importing the following frames as QSV frames:
     *
     * VAAPI:
     * mfxHDLPair.first contains a VASurfaceID pointer.
     * mfxHDLPair.second is always MFX_INFINITE.
     *
     * DXVA2:
     * mfxHDLPair.first contains IDirect3DSurface9 pointer.
     * mfxHDLPair.second is always MFX_INFINITE.
     *
     * D3D11:
     * mfxHDLPair.first contains a ID3D11Texture2D pointer.
     * mfxHDLPair.second contains the texture array index of the frame if the
     * ID3D11Texture2D is an array texture, or always MFX_INFINITE if it is a
     * normal texture.
     */
    AV_PIX_FMT_QSV,
    /**
     * HW acceleration though MMAL, data[3] contains a pointer to the
     * MMAL_BUFFER_HEADER_T structure.
     */
    AV_PIX_FMT_MMAL,

    AV_PIX_FMT_D3D11VA_VLD, ///< HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

    /**
     * HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
     * exactly as for system memory frames.
     */
    AV_PIX_FMT_CUDA,

    AV_PIX_FMT_0RGB, ///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
    AV_PIX_FMT_RGB0, ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
    AV_PIX_FMT_0BGR, ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
    AV_PIX_FMT_BGR0, ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

    AV_PIX_FMT_YUV420P12BE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P12LE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P14BE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P14LE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P14BE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P14LE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P14BE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P14LE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_GBRP12BE,    ///< planar GBR 4:4:4 36bpp, big-endian
    AV_PIX_FMT_GBRP12LE,    ///< planar GBR 4:4:4 36bpp, little-endian
    AV_PIX_FMT_GBRP14BE,    ///< planar GBR 4:4:4 42bpp, big-endian
    AV_PIX_FMT_GBRP14LE,    ///< planar GBR 4:4:4 42bpp, little-endian
    AV_PIX_FMT_YUVJ411P,    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range

    AV_PIX_FMT_BAYER_BGGR8,    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_RGGB8,    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_GBRG8,    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_GRBG8,    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples
    AV_PIX_FMT_BAYER_BGGR16LE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_BGGR16BE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian
    AV_PIX_FMT_BAYER_RGGB16LE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_RGGB16BE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian
    AV_PIX_FMT_BAYER_GBRG16LE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_GBRG16BE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian
    AV_PIX_FMT_BAYER_GRBG16LE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian
    AV_PIX_FMT_BAYER_GRBG16BE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian

#if FF_API_XVMC
    AV_PIX_FMT_XVMC, ///< XVideo Motion Acceleration via common packet passing
#endif

    AV_PIX_FMT_YUV440P10LE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    AV_PIX_FMT_YUV440P10BE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    AV_PIX_FMT_YUV440P12LE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    AV_PIX_FMT_YUV440P12BE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    AV_PIX_FMT_AYUV64LE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AV_PIX_FMT_AYUV64BE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

    AV_PIX_FMT_VIDEOTOOLBOX, ///< hardware decoding through Videotoolbox

    AV_PIX_FMT_P010LE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
    AV_PIX_FMT_P010BE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

    AV_PIX_FMT_GBRAP12BE, ///< planar GBR 4:4:4:4 48bpp, big-endian
    AV_PIX_FMT_GBRAP12LE, ///< planar GBR 4:4:4:4 48bpp, little-endian

    AV_PIX_FMT_GBRAP10BE, ///< planar GBR 4:4:4:4 40bpp, big-endian
    AV_PIX_FMT_GBRAP10LE, ///< planar GBR 4:4:4:4 40bpp, little-endian

    AV_PIX_FMT_MEDIACODEC, ///< hardware decoding through MediaCodec

    AV_PIX_FMT_GRAY12BE, ///<        Y        , 12bpp, big-endian
    AV_PIX_FMT_GRAY12LE, ///<        Y        , 12bpp, little-endian
    AV_PIX_FMT_GRAY10BE, ///<        Y        , 10bpp, big-endian
    AV_PIX_FMT_GRAY10LE, ///<        Y        , 10bpp, little-endian

    AV_PIX_FMT_P016LE, ///< like NV12, with 16bpp per component, little-endian
    AV_PIX_FMT_P016BE, ///< like NV12, with 16bpp per component, big-endian

    /**
     * Hardware surfaces for Direct3D11.
     *
     * This is preferred over the legacy AV_PIX_FMT_D3D11VA_VLD. The new D3D11
     * hwaccel API and filtering support AV_PIX_FMT_D3D11 only.
     *
     * data[0] contains a ID3D11Texture2D pointer, and data[1] contains the
     * texture array index of the frame as intptr_t if the ID3D11Texture2D is
     * an array texture (or always 0 if it's a normal texture).
     */
    AV_PIX_FMT_D3D11,

    AV_PIX_FMT_GRAY9BE, ///<        Y        , 9bpp, big-endian
    AV_PIX_FMT_GRAY9LE, ///<        Y        , 9bpp, little-endian

    AV_PIX_FMT_GBRPF32BE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
    AV_PIX_FMT_GBRPF32LE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
    AV_PIX_FMT_GBRAPF32BE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
    AV_PIX_FMT_GBRAPF32LE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

    /**
     * DRM-managed buffers exposed through PRIME buffer sharing.
     *
     * data[0] points to an AVDRMFrameDescriptor.
     */
    AV_PIX_FMT_DRM_PRIME,
    /**
     * Hardware surfaces for OpenCL.
     *
     * data[i] contain 2D image objects (typed in C as cl_mem, used
     * in OpenCL as image2d_t) for each plane of the surface.
     */
    AV_PIX_FMT_OPENCL,

    AV_PIX_FMT_GRAY14BE, ///<        Y        , 14bpp, big-endian
    AV_PIX_FMT_GRAY14LE, ///<        Y        , 14bpp, little-endian

    AV_PIX_FMT_GRAYF32BE, ///< IEEE-754 single precision Y, 32bpp, big-endian
    AV_PIX_FMT_GRAYF32LE, ///< IEEE-754 single precision Y, 32bpp, little-endian

    AV_PIX_FMT_YUVA422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, big-endian
    AV_PIX_FMT_YUVA422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, little-endian
    AV_PIX_FMT_YUVA444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, big-endian
    AV_PIX_FMT_YUVA444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, little-endian

    AV_PIX_FMT_NV24, ///< planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    AV_PIX_FMT_NV42, ///< as above, but U and V bytes are swapped

    /**
     * Vulkan hardware images.
     *
     * data[0] points to an AVVkFrame
     */
    AV_PIX_FMT_VULKAN,

    AV_PIX_FMT_Y210BE, ///< packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, big-endian
    AV_PIX_FMT_Y210LE, ///< packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, little-endian

    AV_PIX_FMT_X2RGB10LE, ///< packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_X2RGB10BE, ///< packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), big-endian, X=unused/undefined
    AV_PIX_FMT_X2BGR10LE, ///< packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_X2BGR10BE, ///< packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), big-endian, X=unused/undefined

    AV_PIX_FMT_P210BE, ///< interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, big-endian
    AV_PIX_FMT_P210LE, ///< interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, little-endian

    AV_PIX_FMT_P410BE, ///< interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, big-endian
    AV_PIX_FMT_P410LE, ///< interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, little-endian

    AV_PIX_FMT_P216BE, ///< interleaved chroma YUV 4:2:2, 32bpp, big-endian
    AV_PIX_FMT_P216LE, ///< interleaved chroma YUV 4:2:2, 32bpp, little-endian

    AV_PIX_FMT_P416BE, ///< interleaved chroma YUV 4:4:4, 48bpp, big-endian
    AV_PIX_FMT_P416LE, ///< interleaved chroma YUV 4:4:4, 48bpp, little-endian

    AV_PIX_FMT_VUYA, ///< packed VUYA 4:4:4, 32bpp, VUYAVUYA...

    AV_PIX_FMT_RGBAF16BE, ///< IEEE-754 half precision packed RGBA 16:16:16:16, 64bpp, RGBARGBA..., big-endian
    AV_PIX_FMT_RGBAF16LE, ///< IEEE-754 half precision packed RGBA 16:16:16:16, 64bpp, RGBARGBA..., little-endian

    AV_PIX_FMT_VUYX, ///< packed VUYX 4:4:4, 32bpp, Variant of VUYA where alpha channel is left undefined

    AV_PIX_FMT_P012LE, ///< like NV12, with 12bpp per component, data in the high bits, zeros in the low bits, little-endian
    AV_PIX_FMT_P012BE, ///< like NV12, with 12bpp per component, data in the high bits, zeros in the low bits, big-endian

    AV_PIX_FMT_Y212BE, ///< packed YUV 4:2:2 like YUYV422, 24bpp, data in the high bits, zeros in the low bits, big-endian
    AV_PIX_FMT_Y212LE, ///< packed YUV 4:2:2 like YUYV422, 24bpp, data in the high bits, zeros in the low bits, little-endian

    AV_PIX_FMT_XV30BE, ///< packed XVYU 4:4:4, 32bpp, (msb)2X 10V 10Y 10U(lsb), big-endian, variant of Y410 where alpha channel is left undefined
    AV_PIX_FMT_XV30LE, ///< packed XVYU 4:4:4, 32bpp, (msb)2X 10V 10Y 10U(lsb), little-endian, variant of Y410 where alpha channel is left undefined

    AV_PIX_FMT_XV36BE, ///< packed XVYU 4:4:4, 48bpp, data in the high bits, zeros in the low bits, big-endian, variant of Y412 where alpha channel is left undefined
    AV_PIX_FMT_XV36LE, ///< packed XVYU 4:4:4, 48bpp, data in the high bits, zeros in the low bits, little-endian, variant of Y412 where alpha channel is left undefined

    AV_PIX_FMT_RGBF32BE, ///< IEEE-754 single precision packed RGB 32:32:32, 96bpp, RGBRGB..., big-endian
    AV_PIX_FMT_RGBF32LE, ///< IEEE-754 single precision packed RGB 32:32:32, 96bpp, RGBRGB..., little-endian

    AV_PIX_FMT_RGBAF32BE, ///< IEEE-754 single precision packed RGBA 32:32:32:32, 128bpp, RGBARGBA..., big-endian
    AV_PIX_FMT_RGBAF32LE, ///< IEEE-754 single precision packed RGBA 32:32:32:32, 128bpp, RGBARGBA..., little-endian

    AV_PIX_FMT_NB ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
};

/**
 * @}
 * @}
 * @defgroup lavu_picture Image related
 *
 * AVPicture types, pixel formats and basic image planes manipulation.
 *
 * @{
 */
enum AVPictureType
{
    AV_PICTURE_TYPE_NONE = 0, ///< Undefined
    AV_PICTURE_TYPE_I,        ///< Intra
    AV_PICTURE_TYPE_P,        ///< Predicted
    AV_PICTURE_TYPE_B,        ///< Bi-dir predicted
    AV_PICTURE_TYPE_S,        ///< S(GMC)-VOP MPEG-4
    AV_PICTURE_TYPE_SI,       ///< Switching Intra
    AV_PICTURE_TYPE_SP,       ///< Switching Predicted
    AV_PICTURE_TYPE_BI,       ///< BI type
};

/**
 * @defgroup lavu_frame AVFrame
 * @ingroup lavu_data
 *
 * @{
 * AVFrame is an abstraction for reference-counted raw multimedia data.
 */

enum AVFrameSideDataType
{
    /**
     * The data is the AVPanScan struct defined in libavcodec.
     */
    AV_FRAME_DATA_PANSCAN,
    /**
     * ATSC A53 Part 4 Closed Captions.
     * A53 CC bitstream is stored as uint8_t in AVFrameSideData.data.
     * The number of bytes of CC data is AVFrameSideData.size.
     */
    AV_FRAME_DATA_A53_CC,
    /**
     * Stereoscopic 3d metadata.
     * The data is the AVStereo3D struct defined in libavutil/stereo3d.h.
     */
    AV_FRAME_DATA_STEREO3D,
    /**
     * The data is the AVMatrixEncoding enum defined in libavutil/channel_layout.h.
     */
    AV_FRAME_DATA_MATRIXENCODING,
    /**
     * Metadata relevant to a downmix procedure.
     * The data is the AVDownmixInfo struct defined in libavutil/downmix_info.h.
     */
    AV_FRAME_DATA_DOWNMIX_INFO,
    /**
     * ReplayGain information in the form of the AVReplayGain struct.
     */
    AV_FRAME_DATA_REPLAYGAIN,
    /**
     * This side data contains a 3x3 transformation matrix describing an affine
     * transformation that needs to be applied to the frame for correct
     * presentation.
     *
     * See libavutil/display.h for a detailed description of the data.
     */
    AV_FRAME_DATA_DISPLAYMATRIX,
    /**
     * Active Format Description data consisting of a single byte as specified
     * in ETSI TS 101 154 using AVActiveFormatDescription enum.
     */
    AV_FRAME_DATA_AFD,
    /**
     * Motion vectors exported by some codecs (on demand through the export_mvs
     * flag set in the libavcodec AVCodecContext flags2 option).
     * The data is the AVMotionVector struct defined in
     * libavutil/motion_vector.h.
     */
    AV_FRAME_DATA_MOTION_VECTORS,
    /**
     * Recommmends skipping the specified number of samples. This is exported
     * only if the "skip_manual" AVOption is set in libavcodec.
     * This has the same format as AV_PKT_DATA_SKIP_SAMPLES.
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_FRAME_DATA_SKIP_SAMPLES,
    /**
     * This side data must be associated with an audio frame and corresponds to
     * enum AVAudioServiceType defined in avcodec.h.
     */
    AV_FRAME_DATA_AUDIO_SERVICE_TYPE,
    /**
     * Mastering display metadata associated with a video frame. The payload is
     * an AVMasteringDisplayMetadata type and contains information about the
     * mastering display color volume.
     */
    AV_FRAME_DATA_MASTERING_DISPLAY_METADATA,
    /**
     * The GOP timecode in 25 bit timecode format. Data format is 64-bit integer.
     * This is set on the first frame of a GOP that has a temporal reference of 0.
     */
    AV_FRAME_DATA_GOP_TIMECODE,

    /**
     * The data represents the AVSphericalMapping structure defined in
     * libavutil/spherical.h.
     */
    AV_FRAME_DATA_SPHERICAL,

    /**
     * Content light level (based on CTA-861.3). This payload contains data in
     * the form of the AVContentLightMetadata struct.
     */
    AV_FRAME_DATA_CONTENT_LIGHT_LEVEL,

    /**
     * The data contains an ICC profile as an opaque octet buffer following the
     * format described by ISO 15076-1 with an optional name defined in the
     * metadata key entry "name".
     */
    AV_FRAME_DATA_ICC_PROFILE,

    /**
     * Timecode which conforms to SMPTE ST 12-1. The data is an array of 4 uint32_t
     * where the first uint32_t describes how many (1-3) of the other timecodes are used.
     * The timecode format is described in the documentation of av_timecode_get_smpte_from_framenum()
     * function in libavutil/timecode.h.
     */
    AV_FRAME_DATA_S12M_TIMECODE,

    /**
     * HDR dynamic metadata associated with a video frame. The payload is
     * an AVDynamicHDRPlus type and contains information for color
     * volume transform - application 4 of SMPTE 2094-40:2016 standard.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_PLUS,

    /**
     * Regions Of Interest, the data is an array of AVRegionOfInterest type, the number of
     * array element is implied by AVFrameSideData.size / AVRegionOfInterest.self_size.
     */
    AV_FRAME_DATA_REGIONS_OF_INTEREST,

    /**
     * Encoding parameters for a video frame, as described by AVVideoEncParams.
     */
    AV_FRAME_DATA_VIDEO_ENC_PARAMS,

    /**
     * User data unregistered metadata associated with a video frame.
     * This is the H.26[45] UDU SEI message, and shouldn't be used for any other purpose
     * The data is stored as uint8_t in AVFrameSideData.data which is 16 bytes of
     * uuid_iso_iec_11578 followed by AVFrameSideData.size - 16 bytes of user_data_payload_byte.
     */
    AV_FRAME_DATA_SEI_UNREGISTERED,

    /**
     * Film grain parameters for a frame, described by AVFilmGrainParams.
     * Must be present for every frame which should have film grain applied.
     */
    AV_FRAME_DATA_FILM_GRAIN_PARAMS,

    /**
     * Bounding boxes for object detection and classification,
     * as described by AVDetectionBBoxHeader.
     */
    AV_FRAME_DATA_DETECTION_BBOXES,

    /**
     * Dolby Vision RPU raw data, suitable for passing to x265
     * or other libraries. Array of uint8_t, with NAL emulation
     * bytes intact.
     */
    AV_FRAME_DATA_DOVI_RPU_BUFFER,

    /**
     * Parsed Dolby Vision metadata, suitable for passing to a software
     * implementation. The payload is the AVDOVIMetadata struct defined in
     * libavutil/dovi_meta.h.
     */
    AV_FRAME_DATA_DOVI_METADATA,

    /**
     * HDR Vivid dynamic metadata associated with a video frame. The payload is
     * an AVDynamicHDRVivid type and contains information for color
     * volume transform - CUVA 005.1-2021.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_VIVID,

    /**
     * Ambient viewing environment metadata, as defined by H.274.
     */
    AV_FRAME_DATA_AMBIENT_VIEWING_ENVIRONMENT,
};

enum AVColorRange
{
    AVCOL_RANGE_UNSPECIFIED = 0,

    /**
     * Narrow or limited range content.
     *
     * - For luma planes:
     *
     *       (219 * E + 16) * 2^(n-8)
     *
     *   F.ex. the range of 16-235 for 8 bits
     *
     * - For chroma planes:
     *
     *       (224 * E + 128) * 2^(n-8)
     *
     *   F.ex. the range of 16-240 for 8 bits
     */
    AVCOL_RANGE_MPEG = 1,

    /**
     * Full range content.
     *
     * - For RGB and luma planes:
     *
     *       (2^n - 1) * E
     *
     *   F.ex. the range of 0-255 for 8 bits
     *
     * - For chroma planes:
     *
     *       (2^n - 1) * E + 2^(n - 1)
     *
     *   F.ex. the range of 1-255 for 8 bits
     */
    AVCOL_RANGE_JPEG = 2,
    AVCOL_RANGE_NB ///< Not part of ABI
};

/**
 * Chromaticity coordinates of the source primaries.
 * These values match the ones defined by ISO/IEC 23091-2_2019 subclause 8.1 and ITU-T H.273.
 */
enum AVColorPrimaries
{
    AVCOL_PRI_RESERVED0 = 0,
    AVCOL_PRI_BT709 = 1, ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP 177 Annex B
    AVCOL_PRI_UNSPECIFIED = 2,
    AVCOL_PRI_RESERVED = 3,
    AVCOL_PRI_BT470M = 4, ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

    AVCOL_PRI_BT470BG = 5,   ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    AVCOL_PRI_SMPTE170M = 6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    AVCOL_PRI_SMPTE240M = 7, ///< identical to above, also called "SMPTE C" even though it uses D65
    AVCOL_PRI_FILM = 8,      ///< colour filters using Illuminant C
    AVCOL_PRI_BT2020 = 9,    ///< ITU-R BT2020
    AVCOL_PRI_SMPTE428 = 10, ///< SMPTE ST 428-1 (CIE 1931 XYZ)
    AVCOL_PRI_SMPTEST428_1 = AVCOL_PRI_SMPTE428,
    AVCOL_PRI_SMPTE431 = 11, ///< SMPTE ST 431-2 (2011) / DCI P3
    AVCOL_PRI_SMPTE432 = 12, ///< SMPTE ST 432-1 (2010) / P3 D65 / Display P3
    AVCOL_PRI_EBU3213 = 22,  ///< EBU Tech. 3213-E (nothing there) / one of JEDEC P22 group phosphors
    AVCOL_PRI_JEDEC_P22 = AVCOL_PRI_EBU3213,
    AVCOL_PRI_NB ///< Not part of ABI
};

/**
 * Color Transfer Characteristic.
 * These values match the ones defined by ISO/IEC 23091-2_2019 subclause 8.2.
 */
enum AVColorTransferCharacteristic
{
    AVCOL_TRC_RESERVED0 = 0,
    AVCOL_TRC_BT709 = 1, ///< also ITU-R BT1361
    AVCOL_TRC_UNSPECIFIED = 2,
    AVCOL_TRC_RESERVED = 3,
    AVCOL_TRC_GAMMA22 = 4,   ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    AVCOL_TRC_GAMMA28 = 5,   ///< also ITU-R BT470BG
    AVCOL_TRC_SMPTE170M = 6, ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    AVCOL_TRC_SMPTE240M = 7,
    AVCOL_TRC_LINEAR = 8,        ///< "Linear transfer characteristics"
    AVCOL_TRC_LOG = 9,           ///< "Logarithmic transfer characteristic (100:1 range)"
    AVCOL_TRC_LOG_SQRT = 10,     ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
    AVCOL_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
    AVCOL_TRC_BT1361_ECG = 12,   ///< ITU-R BT1361 Extended Colour Gamut
    AVCOL_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
    AVCOL_TRC_BT2020_10 = 14,    ///< ITU-R BT2020 for 10-bit system
    AVCOL_TRC_BT2020_12 = 15,    ///< ITU-R BT2020 for 12-bit system
    AVCOL_TRC_SMPTE2084 = 16,    ///< SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
    AVCOL_TRC_SMPTEST2084 = AVCOL_TRC_SMPTE2084,
    AVCOL_TRC_SMPTE428 = 17, ///< SMPTE ST 428-1
    AVCOL_TRC_SMPTEST428_1 = AVCOL_TRC_SMPTE428,
    AVCOL_TRC_ARIB_STD_B67 = 18, ///< ARIB STD-B67, known as "Hybrid log-gamma"
    AVCOL_TRC_NB                 ///< Not part of ABI
};

/**
 * YUV colorspace type.
 * These values match the ones defined by ISO/IEC 23091-2_2019 subclause 8.3.
 */
enum AVColorSpace
{
    AVCOL_SPC_RGB = 0,   ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB), YZX and ST 428-1
    AVCOL_SPC_BT709 = 1, ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / derived in SMPTE RP 177 Annex B
    AVCOL_SPC_UNSPECIFIED = 2,
    AVCOL_SPC_RESERVED = 3,  ///< reserved for future use by ITU-T and ISO/IEC just like 15-255 are
    AVCOL_SPC_FCC = 4,       ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    AVCOL_SPC_BT470BG = 5,   ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    AVCOL_SPC_SMPTE170M = 6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
    AVCOL_SPC_SMPTE240M = 7, ///< derived from 170M primaries and D65 white point, 170M is derived from BT470 System M's primaries
    AVCOL_SPC_YCGCO = 8,     ///< used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    AVCOL_SPC_YCOCG = AVCOL_SPC_YCGCO,
    AVCOL_SPC_BT2020_NCL = 9,          ///< ITU-R BT2020 non-constant luminance system
    AVCOL_SPC_BT2020_CL = 10,          ///< ITU-R BT2020 constant luminance system
    AVCOL_SPC_SMPTE2085 = 11,          ///< SMPTE 2085, Y'D'zD'x
    AVCOL_SPC_CHROMA_DERIVED_NCL = 12, ///< Chromaticity-derived non-constant luminance system
    AVCOL_SPC_CHROMA_DERIVED_CL = 13,  ///< Chromaticity-derived constant luminance system
    AVCOL_SPC_ICTCP = 14,              ///< ITU-R BT.2100-0, ICtCp
    AVCOL_SPC_NB                       ///< Not part of ABI
};

/**
 * Location of chroma samples.
 *
 * Illustration showing the location of the first (top left) chroma sample of the
 * image, the left shows only luma, the right
 * shows the location of the chroma sample, the 2 could be imagined to overlay
 * each other but are drawn separately due to limitations of ASCII
 *
 *                1st 2nd       1st 2nd horizontal luma sample positions
 *                 v   v         v   v
 *                 ______        ______
 *1st luma line > |X   X ...    |3 4 X ...     X are luma samples,
 *                |             |1 2           1-6 are possible chroma positions
 *2nd luma line > |X   X ...    |5 6 X ...     0 is undefined/unknown position
 */
enum AVChromaLocation
{
    AVCHROMA_LOC_UNSPECIFIED = 0,
    AVCHROMA_LOC_LEFT = 1,    ///< MPEG-2/4 4:2:0, H.264 default for 4:2:0
    AVCHROMA_LOC_CENTER = 2,  ///< MPEG-1 4:2:0, JPEG 4:2:0, H.263 4:2:0
    AVCHROMA_LOC_TOPLEFT = 3, ///< ITU-R 601, SMPTE 274M 296M S314M(DV 4:1:1), mpeg2 4:2:2
    AVCHROMA_LOC_TOP = 4,
    AVCHROMA_LOC_BOTTOMLEFT = 5,
    AVCHROMA_LOC_BOTTOM = 6,
    AVCHROMA_LOC_NB ///< Not part of ABI
};

enum AVChannelOrder
{
    /**
     * Only the channel count is specified, without any further information
     * about the channel order.
     */
    AV_CHANNEL_ORDER_UNSPEC,
    /**
     * The native channel order, i.e. the channels are in the same order in
     * which they are defined in the AVChannel enum. This supports up to 63
     * different channels.
     */
    AV_CHANNEL_ORDER_NATIVE,
    /**
     * The channel order does not correspond to any other predefined order and
     * is stored as an explicit map. For example, this could be used to support
     * layouts with 64 or more channels, or with empty/skipped (AV_CHAN_SILENCE)
     * channels at arbitrary positions.
     */
    AV_CHANNEL_ORDER_CUSTOM,
    /**
     * The audio is represented as the decomposition of the sound field into
     * spherical harmonics. Each channel corresponds to a single expansion
     * component. Channels are ordered according to ACN (Ambisonic Channel
     * Number).
     *
     * The channel with the index n in the stream contains the spherical
     * harmonic of degree l and order m given by
     * @code{.unparsed}
     *   l   = floor(sqrt(n)),
     *   m   = n - l * (l + 1).
     * @endcode
     *
     * Conversely given a spherical harmonic of degree l and order m, the
     * corresponding channel index n is given by
     * @code{.unparsed}
     *   n = l * (l + 1) + m.
     * @endcode
     *
     * Normalization is assumed to be SN3D (Schmidt Semi-Normalization)
     * as defined in AmbiX format $ 2.1.
     */
    AV_CHANNEL_ORDER_AMBISONIC,
};

/**
 * @defgroup lavu_audio_channels Audio channels
 * @ingroup lavu_audio
 *
 * Audio channel layout utility functions
 *
 * @{
 */

enum AVChannel
{
    ///< Invalid channel index
    AV_CHAN_NONE = -1,
    AV_CHAN_FRONT_LEFT,
    AV_CHAN_FRONT_RIGHT,
    AV_CHAN_FRONT_CENTER,
    AV_CHAN_LOW_FREQUENCY,
    AV_CHAN_BACK_LEFT,
    AV_CHAN_BACK_RIGHT,
    AV_CHAN_FRONT_LEFT_OF_CENTER,
    AV_CHAN_FRONT_RIGHT_OF_CENTER,
    AV_CHAN_BACK_CENTER,
    AV_CHAN_SIDE_LEFT,
    AV_CHAN_SIDE_RIGHT,
    AV_CHAN_TOP_CENTER,
    AV_CHAN_TOP_FRONT_LEFT,
    AV_CHAN_TOP_FRONT_CENTER,
    AV_CHAN_TOP_FRONT_RIGHT,
    AV_CHAN_TOP_BACK_LEFT,
    AV_CHAN_TOP_BACK_CENTER,
    AV_CHAN_TOP_BACK_RIGHT,
    /** Stereo downmix. */
    AV_CHAN_STEREO_LEFT = 29,
    /** See above. */
    AV_CHAN_STEREO_RIGHT,
    AV_CHAN_WIDE_LEFT,
    AV_CHAN_WIDE_RIGHT,
    AV_CHAN_SURROUND_DIRECT_LEFT,
    AV_CHAN_SURROUND_DIRECT_RIGHT,
    AV_CHAN_LOW_FREQUENCY_2,
    AV_CHAN_TOP_SIDE_LEFT,
    AV_CHAN_TOP_SIDE_RIGHT,
    AV_CHAN_BOTTOM_FRONT_CENTER,
    AV_CHAN_BOTTOM_FRONT_LEFT,
    AV_CHAN_BOTTOM_FRONT_RIGHT,

    /** Channel is empty can be safely skipped. */
    AV_CHAN_UNUSED = 0x200,

    /** Channel contains data, but its position is unknown. */
    AV_CHAN_UNKNOWN = 0x300,

    /**
     * Range of channels between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END represent Ambisonic components using the ACN system.
     *
     * Given a channel id `<i>` between AV_CHAN_AMBISONIC_BASE and
     * AV_CHAN_AMBISONIC_END (inclusive), the ACN index of the channel `<n>` is
     * `<n> = <i> - AV_CHAN_AMBISONIC_BASE`.
     *
     * @note these values are only used for AV_CHANNEL_ORDER_CUSTOM channel
     * orderings, the AV_CHANNEL_ORDER_AMBISONIC ordering orders the channels
     * implicitly by their position in the stream.
     */
    AV_CHAN_AMBISONIC_BASE = 0x400,
    // leave space for 1024 ids, which correspond to maximum order-32 harmonics,
    // which should be enough for the foreseeable use cases
    AV_CHAN_AMBISONIC_END = 0x7ff,
};

typedef enum memory_order
{
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

typedef enum {
    AV_CLASS_CATEGORY_NA = 0,
    AV_CLASS_CATEGORY_INPUT,
    AV_CLASS_CATEGORY_OUTPUT,
    AV_CLASS_CATEGORY_MUXER,
    AV_CLASS_CATEGORY_DEMUXER,
    AV_CLASS_CATEGORY_ENCODER,
    AV_CLASS_CATEGORY_DECODER,
    AV_CLASS_CATEGORY_FILTER,
    AV_CLASS_CATEGORY_BITSTREAM_FILTER,
    AV_CLASS_CATEGORY_SWSCALER,
    AV_CLASS_CATEGORY_SWRESAMPLER,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_INPUT,
    AV_CLASS_CATEGORY_NB  ///< not part of ABI/API
}AVClassCategory;

enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    AVMEDIA_TYPE_NB
};

enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar
    AV_SAMPLE_FMT_S64,         ///< signed 64 bits
    AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

/**
 * Pixel format is big-endian.
 */
#define AV_PIX_FMT_FLAG_BE           (1 << 0)
/**
 * Pixel format has a palette in data[1], values are indexes in this palette.
 */
#define AV_PIX_FMT_FLAG_PAL          (1 << 1)
/**
 * All values of a component are bit-wise packed end to end.
 */
#define AV_PIX_FMT_FLAG_BITSTREAM    (1 << 2)
/**
 * Pixel format is an HW accelerated format.
 */
#define AV_PIX_FMT_FLAG_HWACCEL      (1 << 3)
/**
 * At least one pixel component is not in the first data plane.
 */
#define AV_PIX_FMT_FLAG_PLANAR       (1 << 4)
/**
 * The pixel format contains RGB-like data (as opposed to YUV/grayscale).
 */
#define AV_PIX_FMT_FLAG_RGB          (1 << 5)

/**
 * The pixel format has an alpha channel. This is set on all formats that
 * support alpha in some way, including AV_PIX_FMT_PAL8. The alpha is always
 * straight, never pre-multiplied.
 *
 * If a codec or a filter does not support alpha, it should set all alpha to
 * opaque, or use the equivalent pixel formats without alpha component, e.g.
 * AV_PIX_FMT_RGB0 (or AV_PIX_FMT_RGB24 etc.) instead of AV_PIX_FMT_RGBA.
 */
#define AV_PIX_FMT_FLAG_ALPHA        (1 << 7)

/**
 * The pixel format is following a Bayer pattern
 */
#define AV_PIX_FMT_FLAG_BAYER        (1 << 8)

/**
 * The pixel format contains IEEE-754 floating point values. Precision (double,
 * single, or half) should be determined by the pixel size (64, 32, or 16 bits).
 */
#define AV_PIX_FMT_FLAG_FLOAT        (1 << 9)

typedef struct AVComponentDescriptor {
    /**
     * Which of the 4 planes contains the component.
     */
    int plane;

    /**
     * Number of elements between 2 horizontally consecutive pixels.
     * Elements are bits for bitstream formats, bytes otherwise.
     */
    int step;

    /**
     * Number of elements before the component of the first pixel.
     * Elements are bits for bitstream formats, bytes otherwise.
     */
    int offset;

    /**
     * Number of least significant bits that must be shifted away
     * to get the value.
     */
    int shift;

    /**
     * Number of bits in the component.
     */
    int depth;
} AVComponentDescriptor;

typedef struct AVPixFmtDescriptor {
    const char *name;
    uint8_t nb_components;  ///< The number of components each pixel has, (1-4)

    /**
     * Amount to shift the luma width right to find the chroma width.
     * For YV12 this is 1 for example.
     * chroma_width = AV_CEIL_RSHIFT(luma_width, log2_chroma_w)
     * The note above is needed to ensure rounding up.
     * This value only refers to the chroma components.
     */
    uint8_t log2_chroma_w;

    /**
     * Amount to shift the luma height right to find the chroma height.
     * For YV12 this is 1 for example.
     * chroma_height= AV_CEIL_RSHIFT(luma_height, log2_chroma_h)
     * The note above is needed to ensure rounding up.
     * This value only refers to the chroma components.
     */
    uint8_t log2_chroma_h;

    /**
     * Combination of AV_PIX_FMT_FLAG_... flags.
     */
    uint64_t flags;

    /**
     * Parameters that describe how pixels are packed.
     * If the format has 1 or 2 components, then luma is 0.
     * If the format has 3 or 4 components:
     *   if the RGB flag is set then 0 is red, 1 is green and 2 is blue;
     *   otherwise 0 is luma, 1 is chroma-U and 2 is chroma-V.
     *
     * If present, the Alpha channel is always the last component.
     */
    AVComponentDescriptor comp[4];

    /**
     * Alternative comma-separated names.
     */
    const char *alias;
} AVPixFmtDescriptor;

static const AVPixFmtDescriptor av_pix_fmt_descriptors[AV_PIX_FMT_NB] = {
    [AV_PIX_FMT_YUV420P] = {
        .name = "yuv420p",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUYV422] = {
        .name = "yuyv422",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 8 },        /* Y */
            { 0, 4, 1, 0, 8 },        /* U */
            { 0, 4, 3, 0, 8 },        /* V */
        },
    },
    [AV_PIX_FMT_YVYU422] = {
        .name = "yvyu422",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 8 },        /* Y */
            { 0, 4, 3, 0, 8 },        /* U */
            { 0, 4, 1, 0, 8 },        /* V */
        },
    },
    [AV_PIX_FMT_Y210LE] = {
        .name = "y210le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 6, 10 },        /* Y */
            { 0, 8, 2, 6, 10 },        /* U */
            { 0, 8, 6, 6, 10 },        /* V */
        },
    },
    [AV_PIX_FMT_Y210BE] = {
        .name = "y210be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 6, 10 },        /* Y */
            { 0, 8, 2, 6, 10 },        /* U */
            { 0, 8, 6, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_RGB24] = {
        .name = "rgb24",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 3, 0, 0, 8 },        /* R */
            { 0, 3, 1, 0, 8 },        /* G */
            { 0, 3, 2, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR24] = {
        .name = "bgr24",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 3, 2, 0, 8 },        /* R */
            { 0, 3, 1, 0, 8 },        /* G */
            { 0, 3, 0, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_X2RGB10LE] = {
        .name = "x2rgb10le",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 2, 4, 10 },       /* R */
            { 0, 4, 1, 2, 10 },       /* G */
            { 0, 4, 0, 0, 10 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_X2RGB10BE] = {
        .name = "x2rgb10be",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 0, 4, 10 },       /* R */
            { 0, 4, 1, 2, 10 },       /* G */
            { 0, 4, 2, 0, 10 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_X2BGR10LE] = {
        .name = "x2bgr10le",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 0, 0, 10 },       /* R */
            { 0, 4, 1, 2, 10 },       /* G */
            { 0, 4, 2, 4, 10 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_X2BGR10BE] = {
        .name = "x2bgr10be",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 2, 0, 10 },       /* R */
            { 0, 4, 1, 2, 10 },       /* G */
            { 0, 4, 0, 4, 10 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_YUV422P] = {
        .name = "yuv422p",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P] = {
        .name = "yuv444p",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV410P] = {
        .name = "yuv410p",
        .nb_components = 3,
        .log2_chroma_w = 2,
        .log2_chroma_h = 2,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV411P] = {
        .name = "yuv411p",
        .nb_components = 3,
        .log2_chroma_w = 2,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUVJ411P] = {
        .name = "yuvj411p",
        .nb_components = 3,
        .log2_chroma_w = 2,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_GRAY8] = {
        .name = "gray",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
        },
        .alias = "gray8,y8",
    },
    [AV_PIX_FMT_MONOWHITE] = {
        .name = "monow",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 1 },        /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BITSTREAM,
    },
    [AV_PIX_FMT_MONOBLACK] = {
        .name = "monob",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 7, 1 },        /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BITSTREAM,
    },
    [AV_PIX_FMT_PAL8] = {
        .name = "pal8",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },
        },
        .flags = AV_PIX_FMT_FLAG_PAL | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVJ420P] = {
        .name = "yuvj420p",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUVJ422P] = {
        .name = "yuvj422p",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUVJ444P] = {
        .name = "yuvj444p",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
#if FF_API_XVMC
    [AV_PIX_FMT_XVMC] = {
        .name = "xvmc",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
#endif
    [AV_PIX_FMT_UYVY422] = {
        .name = "uyvy422",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 1, 0, 8 },        /* Y */
            { 0, 4, 0, 0, 8 },        /* U */
            { 0, 4, 2, 0, 8 },        /* V */
        },
    },
    [AV_PIX_FMT_UYYVYY411] = {
        .name = "uyyvyy411",
        .nb_components = 3,
        .log2_chroma_w = 2,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 1, 0, 8 },        /* Y */
            { 0, 6, 0, 0, 8 },        /* U */
            { 0, 6, 3, 0, 8 },        /* V */
        },
    },
    [AV_PIX_FMT_BGR8] = {
        .name = "bgr8",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 3 },        /* R */
            { 0, 1, 0, 3, 3 },        /* G */
            { 0, 1, 0, 6, 2 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR4] = {
        .name = "bgr4",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 3, 0, 1 },        /* R */
            { 0, 4, 1, 0, 2 },        /* G */
            { 0, 4, 0, 0, 1 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BITSTREAM | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR4_BYTE] = {
        .name = "bgr4_byte",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 1 },        /* R */
            { 0, 1, 0, 1, 2 },        /* G */
            { 0, 1, 0, 3, 1 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB8] = {
        .name = "rgb8",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 6, 2 },        /* R */
            { 0, 1, 0, 3, 3 },        /* G */
            { 0, 1, 0, 0, 3 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB4] = {
        .name = "rgb4",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 0, 1 },        /* R */
            { 0, 4, 1, 0, 2 },        /* G */
            { 0, 4, 3, 0, 1 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BITSTREAM | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB4_BYTE] = {
        .name = "rgb4_byte",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 3, 1 },        /* R */
            { 0, 1, 0, 1, 2 },        /* G */
            { 0, 1, 0, 0, 1 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_NV12] = {
        .name = "nv12",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 2, 0, 0, 8 },        /* U */
            { 1, 2, 1, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_NV21] = {
        .name = "nv21",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 2, 1, 0, 8 },        /* U */
            { 1, 2, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_ARGB] = {
        .name = "argb",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 1, 0, 8 },        /* R */
            { 0, 4, 2, 0, 8 },        /* G */
            { 0, 4, 3, 0, 8 },        /* B */
            { 0, 4, 0, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_RGBA] = {
        .name = "rgba",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 0, 8 },        /* R */
            { 0, 4, 1, 0, 8 },        /* G */
            { 0, 4, 2, 0, 8 },        /* B */
            { 0, 4, 3, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_ABGR] = {
        .name = "abgr",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 3, 0, 8 },        /* R */
            { 0, 4, 2, 0, 8 },        /* G */
            { 0, 4, 1, 0, 8 },        /* B */
            { 0, 4, 0, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_BGRA] = {
        .name = "bgra",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 2, 0, 8 },        /* R */
            { 0, 4, 1, 0, 8 },        /* G */
            { 0, 4, 0, 0, 8 },        /* B */
            { 0, 4, 3, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_0RGB] = {
        .name = "0rgb",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 1, 0, 8 },        /* R */
            { 0, 4, 2, 0, 8 },        /* G */
            { 0, 4, 3, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB0] = {
        .name = "rgb0",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 0, 0, 8 },        /* R */
            { 0, 4, 1, 0, 8 },        /* G */
            { 0, 4, 2, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_0BGR] = {
        .name = "0bgr",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 3, 0, 8 },        /* R */
            { 0, 4, 2, 0, 8 },        /* G */
            { 0, 4, 1, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR0] = {
        .name = "bgr0",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 2, 0, 8 },        /* R */
            { 0, 4, 1, 0, 8 },        /* G */
            { 0, 4, 0, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GRAY9BE] = {
        .name = "gray9be",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
        .alias = "y9be",
    },
    [AV_PIX_FMT_GRAY9LE] = {
        .name = "gray9le",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },       /* Y */
        },
        .alias = "y9le",
    },
    [AV_PIX_FMT_GRAY10BE] = {
        .name = "gray10be",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
        .alias = "y10be",
    },
    [AV_PIX_FMT_GRAY10LE] = {
        .name = "gray10le",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },       /* Y */
        },
        .alias = "y10le",
    },
    [AV_PIX_FMT_GRAY12BE] = {
        .name = "gray12be",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
        .alias = "y12be",
    },
    [AV_PIX_FMT_GRAY12LE] = {
        .name = "gray12le",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },       /* Y */
        },
        .alias = "y12le",
    },
    [AV_PIX_FMT_GRAY14BE] = {
        .name = "gray14be",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 14 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
        .alias = "y14be",
    },
    [AV_PIX_FMT_GRAY14LE] = {
        .name = "gray14le",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 14 },       /* Y */
        },
        .alias = "y14le",
    },
    [AV_PIX_FMT_GRAY16BE] = {
        .name = "gray16be",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
        .alias = "y16be",
    },
    [AV_PIX_FMT_GRAY16LE] = {
        .name = "gray16le",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },       /* Y */
        },
        .alias = "y16le",
    },
    [AV_PIX_FMT_YUV440P] = {
        .name = "yuv440p",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUVJ440P] = {
        .name = "yuvj440p",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV440P10LE] = {
        .name = "yuv440p10le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV440P10BE] = {
        .name = "yuv440p10be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV440P12LE] = {
        .name = "yuv440p12le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV440P12BE] = {
        .name = "yuv440p12be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUVA420P] = {
        .name = "yuva420p",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
            { 3, 1, 0, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P] = {
        .name = "yuva422p",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
            { 3, 1, 0, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P] = {
        .name = "yuva444p",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 1, 0, 0, 8 },        /* U */
            { 2, 1, 0, 0, 8 },        /* V */
            { 3, 1, 0, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA420P9BE] = {
        .name = "yuva420p9be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
            { 3, 2, 0, 0, 9 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA420P9LE] = {
        .name = "yuva420p9le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
            { 3, 2, 0, 0, 9 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P9BE] = {
        .name = "yuva422p9be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
            { 3, 2, 0, 0, 9 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P9LE] = {
        .name = "yuva422p9le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
            { 3, 2, 0, 0, 9 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P9BE] = {
        .name = "yuva444p9be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
            { 3, 2, 0, 0, 9 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P9LE] = {
        .name = "yuva444p9le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
            { 3, 2, 0, 0, 9 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA420P10BE] = {
        .name = "yuva420p10be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
            { 3, 2, 0, 0, 10 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA420P10LE] = {
        .name = "yuva420p10le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
            { 3, 2, 0, 0, 10 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P10BE] = {
        .name = "yuva422p10be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
            { 3, 2, 0, 0, 10 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P10LE] = {
        .name = "yuva422p10le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
            { 3, 2, 0, 0, 10 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P10BE] = {
        .name = "yuva444p10be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
            { 3, 2, 0, 0, 10 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P10LE] = {
        .name = "yuva444p10le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
            { 3, 2, 0, 0, 10 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA420P16BE] = {
        .name = "yuva420p16be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
            { 3, 2, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA420P16LE] = {
        .name = "yuva420p16le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
            { 3, 2, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P16BE] = {
        .name = "yuva422p16be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
            { 3, 2, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P16LE] = {
        .name = "yuva422p16le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
            { 3, 2, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P16BE] = {
        .name = "yuva444p16be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
            { 3, 2, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P16LE] = {
        .name = "yuva444p16le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
            { 3, 2, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_RGB48BE] = {
        .name = "rgb48be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 6, 0, 0, 16 },       /* R */
            { 0, 6, 2, 0, 16 },       /* G */
            { 0, 6, 4, 0, 16 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_RGB48LE] = {
        .name = "rgb48le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 6, 0, 0, 16 },       /* R */
            { 0, 6, 2, 0, 16 },       /* G */
            { 0, 6, 4, 0, 16 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGBA64BE] = {
        .name = "rgba64be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 0, 0, 16 },       /* R */
            { 0, 8, 2, 0, 16 },       /* G */
            { 0, 8, 4, 0, 16 },       /* B */
            { 0, 8, 6, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_RGBA64LE] = {
        .name = "rgba64le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 0, 0, 16 },       /* R */
            { 0, 8, 2, 0, 16 },       /* G */
            { 0, 8, 4, 0, 16 },       /* B */
            { 0, 8, 6, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_RGB565BE] = {
        .name = "rgb565be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, -1, 3, 5 },        /* R */
            { 0, 2,  0, 5, 6 },        /* G */
            { 0, 2,  0, 0, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB565LE] = {
        .name = "rgb565le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 1, 3, 5 },        /* R */
            { 0, 2, 0, 5, 6 },        /* G */
            { 0, 2, 0, 0, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB555BE] = {
        .name = "rgb555be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, -1, 2, 5 },        /* R */
            { 0, 2,  0, 5, 5 },        /* G */
            { 0, 2,  0, 0, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB555LE] = {
        .name = "rgb555le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 1, 2, 5 },        /* R */
            { 0, 2, 0, 5, 5 },        /* G */
            { 0, 2, 0, 0, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB444BE] = {
        .name = "rgb444be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, -1, 0, 4 },        /* R */
            { 0, 2,  0, 4, 4 },        /* G */
            { 0, 2,  0, 0, 4 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_RGB444LE] = {
        .name = "rgb444le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 1, 0, 4 },        /* R */
            { 0, 2, 0, 4, 4 },        /* G */
            { 0, 2, 0, 0, 4 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR48BE] = {
        .name = "bgr48be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 6, 4, 0, 16 },       /* R */
            { 0, 6, 2, 0, 16 },       /* G */
            { 0, 6, 0, 0, 16 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR48LE] = {
        .name = "bgr48le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 6, 4, 0, 16 },       /* R */
            { 0, 6, 2, 0, 16 },       /* G */
            { 0, 6, 0, 0, 16 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGRA64BE] = {
        .name = "bgra64be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 4, 0, 16 },       /* R */
            { 0, 8, 2, 0, 16 },       /* G */
            { 0, 8, 0, 0, 16 },       /* B */
            { 0, 8, 6, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_BGRA64LE] = {
        .name = "bgra64le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 4, 0, 16 },       /* R */
            { 0, 8, 2, 0, 16 },       /* G */
            { 0, 8, 0, 0, 16 },       /* B */
            { 0, 8, 6, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_BGR565BE] = {
        .name = "bgr565be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2,  0, 0, 5 },        /* R */
            { 0, 2,  0, 5, 6 },        /* G */
            { 0, 2, -1, 3, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR565LE] = {
        .name = "bgr565le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 5 },        /* R */
            { 0, 2, 0, 5, 6 },        /* G */
            { 0, 2, 1, 3, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR555BE] = {
        .name = "bgr555be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2,  0, 0, 5 },       /* R */
            { 0, 2,  0, 5, 5 },       /* G */
            { 0, 2, -1, 2, 5 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
     },
    [AV_PIX_FMT_BGR555LE] = {
        .name = "bgr555le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 5 },        /* R */
            { 0, 2, 0, 5, 5 },        /* G */
            { 0, 2, 1, 2, 5 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_BGR444BE] = {
        .name = "bgr444be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2,  0, 0, 4 },       /* R */
            { 0, 2,  0, 4, 4 },       /* G */
            { 0, 2, -1, 0, 4 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB,
     },
    [AV_PIX_FMT_BGR444LE] = {
        .name = "bgr444le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 4 },        /* R */
            { 0, 2, 0, 4, 4 },        /* G */
            { 0, 2, 1, 0, 4 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_VAAPI] = {
        .name = "vaapi",
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_YUV420P9LE] = {
        .name = "yuv420p9le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P9BE] = {
        .name = "yuv420p9be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P10LE] = {
        .name = "yuv420p10le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P10BE] = {
        .name = "yuv420p10be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P12LE] = {
        .name = "yuv420p12le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P12BE] = {
        .name = "yuv420p12be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P14LE] = {
        .name = "yuv420p14le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 14 },        /* Y */
            { 1, 2, 0, 0, 14 },        /* U */
            { 2, 2, 0, 0, 14 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P14BE] = {
        .name = "yuv420p14be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 14 },        /* Y */
            { 1, 2, 0, 0, 14 },        /* U */
            { 2, 2, 0, 0, 14 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P16LE] = {
        .name = "yuv420p16le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV420P16BE] = {
        .name = "yuv420p16be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P9LE] = {
        .name = "yuv422p9le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P9BE] = {
        .name = "yuv422p9be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P10LE] = {
        .name = "yuv422p10le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P10BE] = {
        .name = "yuv422p10be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P12LE] = {
        .name = "yuv422p12le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P12BE] = {
        .name = "yuv422p12be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P14LE] = {
        .name = "yuv422p14le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 14 },        /* Y */
            { 1, 2, 0, 0, 14 },        /* U */
            { 2, 2, 0, 0, 14 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P14BE] = {
        .name = "yuv422p14be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 14 },        /* Y */
            { 1, 2, 0, 0, 14 },        /* U */
            { 2, 2, 0, 0, 14 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P16LE] = {
        .name = "yuv422p16le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV422P16BE] = {
        .name = "yuv422p16be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P16LE] = {
        .name = "yuv444p16le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P16BE] = {
        .name = "yuv444p16be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 2, 0, 0, 16 },        /* U */
            { 2, 2, 0, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P10LE] = {
        .name = "yuv444p10le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P10BE] = {
        .name = "yuv444p10be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 2, 0, 0, 10 },        /* U */
            { 2, 2, 0, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P9LE] = {
        .name = "yuv444p9le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P9BE] = {
        .name = "yuv444p9be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 9 },        /* Y */
            { 1, 2, 0, 0, 9 },        /* U */
            { 2, 2, 0, 0, 9 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P12LE] = {
        .name = "yuv444p12le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P12BE] = {
        .name = "yuv444p12be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P14LE] = {
        .name = "yuv444p14le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 14 },        /* Y */
            { 1, 2, 0, 0, 14 },        /* U */
            { 2, 2, 0, 0, 14 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_YUV444P14BE] = {
        .name = "yuv444p14be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 14 },        /* Y */
            { 1, 2, 0, 0, 14 },        /* U */
            { 2, 2, 0, 0, 14 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_D3D11VA_VLD] = {
        .name = "d3d11va_vld",
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_DXVA2_VLD] = {
        .name = "dxva2_vld",
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_YA8] = {
        .name = "ya8",
        .nb_components = 2,
        .comp = {
            { 0, 2, 0, 0, 8 },        /* Y */
            { 0, 2, 1, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_ALPHA,
        .alias = "gray8a",
    },
    [AV_PIX_FMT_YA16LE] = {
        .name = "ya16le",
        .nb_components = 2,
        .comp = {
            { 0, 4, 0, 0, 16 },        /* Y */
            { 0, 4, 2, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YA16BE] = {
        .name = "ya16be",
        .nb_components = 2,
        .comp = {
            { 0, 4, 0, 0, 16 },        /* Y */
            { 0, 4, 2, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_VIDEOTOOLBOX] = {
        .name = "videotoolbox_vld",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_GBRP] = {
        .name = "gbrp",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 1, 0, 0, 8 },        /* R */
            { 0, 1, 0, 0, 8 },        /* G */
            { 1, 1, 0, 0, 8 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP9LE] = {
        .name = "gbrp9le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 9 },        /* R */
            { 0, 2, 0, 0, 9 },        /* G */
            { 1, 2, 0, 0, 9 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP9BE] = {
        .name = "gbrp9be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 9 },        /* R */
            { 0, 2, 0, 0, 9 },        /* G */
            { 1, 2, 0, 0, 9 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP10LE] = {
        .name = "gbrp10le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 10 },        /* R */
            { 0, 2, 0, 0, 10 },        /* G */
            { 1, 2, 0, 0, 10 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP10BE] = {
        .name = "gbrp10be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 10 },        /* R */
            { 0, 2, 0, 0, 10 },        /* G */
            { 1, 2, 0, 0, 10 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP12LE] = {
        .name = "gbrp12le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 12 },        /* R */
            { 0, 2, 0, 0, 12 },        /* G */
            { 1, 2, 0, 0, 12 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP12BE] = {
        .name = "gbrp12be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 12 },        /* R */
            { 0, 2, 0, 0, 12 },        /* G */
            { 1, 2, 0, 0, 12 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP14LE] = {
        .name = "gbrp14le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 14 },        /* R */
            { 0, 2, 0, 0, 14 },        /* G */
            { 1, 2, 0, 0, 14 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP14BE] = {
        .name = "gbrp14be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 14 },        /* R */
            { 0, 2, 0, 0, 14 },        /* G */
            { 1, 2, 0, 0, 14 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP16LE] = {
        .name = "gbrp16le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 16 },       /* R */
            { 0, 2, 0, 0, 16 },       /* G */
            { 1, 2, 0, 0, 16 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRP16BE] = {
        .name = "gbrp16be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 16 },       /* R */
            { 0, 2, 0, 0, 16 },       /* G */
            { 1, 2, 0, 0, 16 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRAP] = {
        .name = "gbrap",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 1, 0, 0, 8 },        /* R */
            { 0, 1, 0, 0, 8 },        /* G */
            { 1, 1, 0, 0, 8 },        /* B */
            { 3, 1, 0, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_GBRAP16LE] = {
        .name = "gbrap16le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 16 },       /* R */
            { 0, 2, 0, 0, 16 },       /* G */
            { 1, 2, 0, 0, 16 },       /* B */
            { 3, 2, 0, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_GBRAP16BE] = {
        .name = "gbrap16be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 16 },       /* R */
            { 0, 2, 0, 0, 16 },       /* G */
            { 1, 2, 0, 0, 16 },       /* B */
            { 3, 2, 0, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                 AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_VDPAU] = {
        .name = "vdpau",
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_XYZ12LE] = {
        .name = "xyz12le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 6, 0, 4, 12 },       /* X */
            { 0, 6, 2, 4, 12 },       /* Y */
            { 0, 6, 4, 4, 12 },       /* Z */
      },
      /*.flags = -- not used*/
    },
    [AV_PIX_FMT_XYZ12BE] = {
        .name = "xyz12be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 6, 0, 4, 12 },       /* X */
            { 0, 6, 2, 4, 12 },       /* Y */
            { 0, 6, 4, 4, 12 },       /* Z */
       },
        .flags = AV_PIX_FMT_FLAG_BE,
    },

#define BAYER8_DESC_COMMON \
        .nb_components= 3, \
        .log2_chroma_w= 0, \
        .log2_chroma_h= 0, \
        .comp = {          \
            { 0, 1, 0, 0, 2 }, \
            { 0, 1, 0, 0, 4 }, \
            { 0, 1, 0, 0, 2 }, \
        },                 \

#define BAYER16_DESC_COMMON \
        .nb_components= 3, \
        .log2_chroma_w= 0, \
        .log2_chroma_h= 0, \
        .comp = {          \
            { 0, 2, 0, 0, 4 }, \
            { 0, 2, 0, 0, 8 }, \
            { 0, 2, 0, 0, 4 }, \
        },                 \

    [AV_PIX_FMT_BAYER_BGGR8] = {
        .name = "bayer_bggr8",
        BAYER8_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_BGGR16LE] = {
        .name = "bayer_bggr16le",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_BGGR16BE] = {
        .name = "bayer_bggr16be",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_RGGB8] = {
        .name = "bayer_rggb8",
        BAYER8_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_RGGB16LE] = {
        .name = "bayer_rggb16le",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_RGGB16BE] = {
        .name = "bayer_rggb16be",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_GBRG8] = {
        .name = "bayer_gbrg8",
        BAYER8_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_GBRG16LE] = {
        .name = "bayer_gbrg16le",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_GBRG16BE] = {
        .name = "bayer_gbrg16be",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_GRBG8] = {
        .name = "bayer_grbg8",
        BAYER8_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_GRBG16LE] = {
        .name = "bayer_grbg16le",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_BAYER_GRBG16BE] = {
        .name = "bayer_grbg16be",
        BAYER16_DESC_COMMON
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_BAYER,
    },
    [AV_PIX_FMT_NV16] = {
        .name = "nv16",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 2, 0, 0, 8 },        /* U */
            { 1, 2, 1, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_NV20LE] = {
        .name = "nv20le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 4, 0, 0, 10 },        /* U */
            { 1, 4, 2, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_NV20BE] = {
        .name = "nv20be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 10 },        /* Y */
            { 1, 4, 0, 0, 10 },        /* U */
            { 1, 4, 2, 0, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_QSV] = {
        .name = "qsv",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_MEDIACODEC] = {
        .name = "mediacodec",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_MMAL] = {
        .name = "mmal",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_CUDA] = {
        .name = "cuda",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_AYUV64LE] = {
        .name = "ayuv64le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 2, 0, 16 },        /* Y */
            { 0, 8, 4, 0, 16 },        /* U */
            { 0, 8, 6, 0, 16 },        /* V */
            { 0, 8, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_AYUV64BE] = {
        .name = "ayuv64be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 2, 0, 16 },        /* Y */
            { 0, 8, 4, 0, 16 },        /* U */
            { 0, 8, 6, 0, 16 },        /* V */
            { 0, 8, 0, 0, 16 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_P010LE] = {
        .name = "p010le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 6, 10 },        /* Y */
            { 1, 4, 0, 6, 10 },        /* U */
            { 1, 4, 2, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_P010BE] = {
        .name = "p010be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 6, 10 },        /* Y */
            { 1, 4, 0, 6, 10 },        /* U */
            { 1, 4, 2, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_P012LE] = {
        .name = "p012le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 4, 12 },        /* Y */
            { 1, 4, 0, 4, 12 },        /* U */
            { 1, 4, 2, 4, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_P012BE] = {
        .name = "p012be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 4, 12 },        /* Y */
            { 1, 4, 0, 4, 12 },        /* U */
            { 1, 4, 2, 4, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_P016LE] = {
        .name = "p016le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 16 },       /* Y */
            { 1, 4, 0, 0, 16 },       /* U */
            { 1, 4, 2, 0, 16 },       /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_P016BE] = {
        .name = "p016be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 1,
        .comp = {
            { 0, 2, 0, 0, 16 },       /* Y */
            { 1, 4, 0, 0, 16 },       /* U */
            { 1, 4, 2, 0, 16 },       /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_GBRAP12LE] = {
        .name = "gbrap12le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 12 },       /* R */
            { 0, 2, 0, 0, 12 },       /* G */
            { 1, 2, 0, 0, 12 },       /* B */
            { 3, 2, 0, 0, 12 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_GBRAP12BE] = {
        .name = "gbrap12be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 12 },       /* R */
            { 0, 2, 0, 0, 12 },       /* G */
            { 1, 2, 0, 0, 12 },       /* B */
            { 3, 2, 0, 0, 12 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                 AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_GBRAP10LE] = {
        .name = "gbrap10le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 10 },       /* R */
            { 0, 2, 0, 0, 10 },       /* G */
            { 1, 2, 0, 0, 10 },       /* B */
            { 3, 2, 0, 0, 10 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_GBRAP10BE] = {
        .name = "gbrap10be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 2, 0, 0, 10 },       /* R */
            { 0, 2, 0, 0, 10 },       /* G */
            { 1, 2, 0, 0, 10 },       /* B */
            { 3, 2, 0, 0, 10 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                 AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_D3D11] = {
        .name = "d3d11",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_GBRPF32BE] = {
        .name = "gbrpf32be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 4, 0, 0, 32 },        /* R */
            { 0, 4, 0, 0, 32 },        /* G */
            { 1, 4, 0, 0, 32 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                 AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_GBRPF32LE] = {
        .name = "gbrpf32le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 4, 0, 0, 32 },        /* R */
            { 0, 4, 0, 0, 32 },        /* G */
            { 1, 4, 0, 0, 32 },        /* B */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_FLOAT | AV_PIX_FMT_FLAG_RGB,
    },
    [AV_PIX_FMT_GBRAPF32BE] = {
        .name = "gbrapf32be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 4, 0, 0, 32 },        /* R */
            { 0, 4, 0, 0, 32 },        /* G */
            { 1, 4, 0, 0, 32 },        /* B */
            { 3, 4, 0, 0, 32 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                 AV_PIX_FMT_FLAG_ALPHA | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_GBRAPF32LE] = {
        .name = "gbrapf32le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 2, 4, 0, 0, 32 },        /* R */
            { 0, 4, 0, 0, 32 },        /* G */
            { 1, 4, 0, 0, 32 },        /* B */
            { 3, 4, 0, 0, 32 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA |
                 AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_DRM_PRIME] = {
        .name = "drm_prime",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_OPENCL] = {
        .name  = "opencl",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_GRAYF32BE] = {
        .name = "grayf32be",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 0, 32 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_FLOAT,
        .alias = "yf32be",
    },
    [AV_PIX_FMT_GRAYF32LE] = {
        .name = "grayf32le",
        .nb_components = 1,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 0, 32 },       /* Y */
        },
        .flags = AV_PIX_FMT_FLAG_FLOAT,
        .alias = "yf32le",
    },
    [AV_PIX_FMT_YUVA422P12BE] = {
        .name = "yuva422p12be",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
            { 3, 2, 0, 0, 12 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA422P12LE] = {
        .name = "yuva422p12le",
        .nb_components = 4,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
            { 3, 2, 0, 0, 12 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P12BE] = {
        .name = "yuva444p12be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
            { 3, 2, 0, 0, 12 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_YUVA444P12LE] = {
        .name = "yuva444p12le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 12 },        /* Y */
            { 1, 2, 0, 0, 12 },        /* U */
            { 2, 2, 0, 0, 12 },        /* V */
            { 3, 2, 0, 0, 12 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_NV24] = {
        .name = "nv24",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 2, 0, 0, 8 },        /* U */
            { 1, 2, 1, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_NV42] = {
        .name = "nv42",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 1, 0, 0, 8 },        /* Y */
            { 1, 2, 1, 0, 8 },        /* U */
            { 1, 2, 0, 0, 8 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_VULKAN] = {
        .name = "vulkan",
        .flags = AV_PIX_FMT_FLAG_HWACCEL,
    },
    [AV_PIX_FMT_P210BE] = {
        .name = "p210be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 6, 10 },        /* Y */
            { 1, 4, 0, 6, 10 },        /* U */
            { 1, 4, 2, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_P210LE] = {
        .name = "p210le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 6, 10 },        /* Y */
            { 1, 4, 0, 6, 10 },        /* U */
            { 1, 4, 2, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_P410BE] = {
        .name = "p410be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 6, 10 },        /* Y */
            { 1, 4, 0, 6, 10 },        /* U */
            { 1, 4, 2, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_P410LE] = {
        .name = "p410le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 6, 10 },        /* Y */
            { 1, 4, 0, 6, 10 },        /* U */
            { 1, 4, 2, 6, 10 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_P216BE] = {
        .name = "p216be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 4, 0, 0, 16 },        /* U */
            { 1, 4, 2, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_P216LE] = {
        .name = "p216le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 4, 0, 0, 16 },        /* U */
            { 1, 4, 2, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_P416BE] = {
        .name = "p416be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 4, 0, 0, 16 },        /* U */
            { 1, 4, 2, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_P416LE] = {
        .name = "p416le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 2, 0, 0, 16 },        /* Y */
            { 1, 4, 0, 0, 16 },        /* U */
            { 1, 4, 2, 0, 16 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_PLANAR,
    },
    [AV_PIX_FMT_VUYA] = {
        .name = "vuya",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 2, 0, 8 },        /* Y */
            { 0, 4, 1, 0, 8 },        /* U */
            { 0, 4, 0, 0, 8 },        /* V */
            { 0, 4, 3, 0, 8 },        /* A */
        },
        .flags = AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_VUYX] = {
        .name = "vuyx",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 2, 0, 8 },        /* Y */
            { 0, 4, 1, 0, 8 },        /* U */
            { 0, 4, 0, 0, 8 },        /* V */
        },
    },
    [AV_PIX_FMT_RGBAF16BE] = {
        .name = "rgbaf16be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 0, 0, 16 },       /* R */
            { 0, 8, 2, 0, 16 },       /* G */
            { 0, 8, 4, 0, 16 },       /* B */
            { 0, 8, 6, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_ALPHA | AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_RGBAF16LE] = {
        .name = "rgbaf16le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 8, 0, 0, 16 },       /* R */
            { 0, 8, 2, 0, 16 },       /* G */
            { 0, 8, 4, 0, 16 },       /* B */
            { 0, 8, 6, 0, 16 },       /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_ALPHA |
                 AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_Y212LE] = {
        .name = "y212le",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 4, 12 },        /* Y */
            { 0, 8, 2, 4, 12 },        /* U */
            { 0, 8, 6, 4, 12 },        /* V */
        },
    },
    [AV_PIX_FMT_Y212BE] = {
        .name = "y212be",
        .nb_components = 3,
        .log2_chroma_w = 1,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 4, 0, 4, 12 },        /* Y */
            { 0, 8, 2, 4, 12 },        /* U */
            { 0, 8, 6, 4, 12 },        /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_XV30LE] = {
        .name = "xv30le",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 4, 1, 2, 10 },       /* Y */
            { 0, 4, 0, 0, 10 },       /* U */
            { 0, 4, 2, 4, 10 },       /* V */
        },
    },
    [AV_PIX_FMT_XV30BE] = {
        .name = "xv30be",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 32, 10, 0, 10 },       /* Y */
            { 0, 32,  0, 0, 10 },       /* U */
            { 0, 32, 20, 0, 10 },       /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_BITSTREAM,
    },
    [AV_PIX_FMT_XV36LE] = {
        .name = "xv36le",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 8, 2, 4, 12 },       /* Y */
            { 0, 8, 0, 4, 12 },       /* U */
            { 0, 8, 4, 4, 12 },       /* V */
        },
    },
    [AV_PIX_FMT_XV36BE] = {
        .name = "xv36be",
        .nb_components= 3,
        .log2_chroma_w= 0,
        .log2_chroma_h= 0,
        .comp = {
            { 0, 8, 2, 4, 12 },       /* Y */
            { 0, 8, 0, 4, 12 },       /* U */
            { 0, 8, 4, 4, 12 },       /* V */
        },
        .flags = AV_PIX_FMT_FLAG_BE,
    },
    [AV_PIX_FMT_RGBF32BE] = {
        .name = "rgbf32be",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 12, 0, 0, 32 },       /* R */
            { 0, 12, 4, 0, 32 },       /* G */
            { 0, 12, 8, 0, 32 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_RGBF32LE] = {
        .name = "rgbf32le",
        .nb_components = 3,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 12, 0, 0, 32 },       /* R */
            { 0, 12, 4, 0, 32 },       /* G */
            { 0, 12, 8, 0, 32 },       /* B */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_FLOAT,
    },
    [AV_PIX_FMT_RGBAF32BE] = {
        .name = "rgbaf32be",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 16,  0, 0, 32 },      /* R */
            { 0, 16,  4, 0, 32 },      /* G */
            { 0, 16,  8, 0, 32 },      /* B */
            { 0, 16, 12, 0, 32 },      /* A */
        },
        .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
                 AV_PIX_FMT_FLAG_FLOAT | AV_PIX_FMT_FLAG_ALPHA,
    },
    [AV_PIX_FMT_RGBAF32LE] = {
        .name = "rgbaf32le",
        .nb_components = 4,
        .log2_chroma_w = 0,
        .log2_chroma_h = 0,
        .comp = {
            { 0, 16,  0, 0, 32 },      /* R */
            { 0, 16,  4, 0, 32 },      /* G */
            { 0, 16,  8, 0, 32 },      /* B */
            { 0, 16, 12, 0, 32 },      /* A */
        },
        .flags = AV_PIX_FMT_FLAG_RGB | AV_PIX_FMT_FLAG_FLOAT |
                 AV_PIX_FMT_FLAG_ALPHA,
    },
};

/**
 * @name AVDictionary Flags
 * Flags that influence behavior of the matching of keys or insertion to the dictionary.
 * @{
 */
#define AV_DICT_MATCH_CASE 1      /**< Only get an entry with exact-case key match. Only relevant in av_dict_get(). */
#define AV_DICT_IGNORE_SUFFIX 2   /**< Return first entry in a dictionary whose first part corresponds to the search key, \
                                       ignoring the suffix of the found key string. Only relevant in av_dict_get(). */
#define AV_DICT_DONT_STRDUP_KEY 4 /**< Take ownership of a key that's been \
                                       allocated with av_malloc() or another memory allocation function. */
#define AV_DICT_DONT_STRDUP_VAL 8 /**< Take ownership of a value that's been \
                                       allocated with av_malloc() or another memory allocation function. */
#define AV_DICT_DONT_OVERWRITE 16 /**< Don't overwrite existing entries. */
#define AV_DICT_APPEND 32         /**< If the entry already exists, append to it.  Note that no \
                                       delimiter is added, the strings are simply concatenated. */
#define AV_DICT_MULTIKEY 64       /**< Allow to store several equal keys in the dictionary */
/**
 * @}
 */

#define MAX_PLANES 4
#define MAX_STRIPS 32
#define AV_LOG_ERROR 16
#define AVPALETTE_SIZE 1024
#define AV_NOPTS_VALUE ((int64_t)UINT64_C(0x8000000000000000))
#define FF_DISABLE_DEPRECATION_WARNINGS

#define av_assert1(cond) ((void)0)
#define av_assert2(cond) ((void)0)
#define av_always_inline __attribute__((always_inline)) inline
#define av_cold __attribute__((cold))
#define av_const __attribute__((const))
#define AVERROR(e) (-(e)) ///< Returns a negative error code from a POSIX error code, to return from library functions.
#define FFMIN(a, b) ((a) > (b) ? (b) : (a))
#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#define FFERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))
#define AVERROR_INVALIDDATA FFERRTAG('I', 'N', 'D', 'A')  ///< Invalid data found when processing input
#define AVERROR_PATCHWELCOME FFERRTAG('P', 'A', 'W', 'E') ///< Not yet implemented in FFmpeg, patches welcome

#define atomic_fetch_add(object, operand) \
    __sync_fetch_and_add(object, operand)

#define atomic_fetch_add_explicit(object, operand, order) \
    atomic_fetch_add(object, operand)

#define atomic_load(object) \
    (*(object))

#define atomic_load_explicit(object, order) \
    atomic_load(object)

#define atomic_init(obj, value) \
    do                          \
    {                           \
        *(obj) = (value);       \
    } while (0)

#define AV_RB16(x)                      \
    ((((const uint8_t *)(x))[0] << 8) | \
     ((const uint8_t *)(x))[1])

#define AV_RB24(x)                       \
    ((((const uint8_t *)(x))[0] << 16) | \
     (((const uint8_t *)(x))[1] << 8) |  \
     ((const uint8_t *)(x))[2])

#define AV_RB32(x)                                 \
    (((uint32_t)((const uint8_t *)(x))[0] << 24) | \
     (((const uint8_t *)(x))[1] << 16) |           \
     (((const uint8_t *)(x))[2] << 8) |            \
     ((const uint8_t *)(x))[3])

#define av_clip_uint8 av_clip_uint8_c
#define avpriv_request_sample(avc, msg, ...) log_i(msg, ...)
#define av_log(avcl, level, fmt, ...) log_i(fmt, ...)

#define BUFFER_FLAG_NO_FREE (1 << 1)

#define atomic_fetch_sub(object, operand) \
    __sync_fetch_and_sub(object, operand)

#define atomic_fetch_sub_explicit(object, operand, order) \
    atomic_fetch_sub(object, operand)

typedef intptr_t atomic_size_t;
typedef intptr_t atomic_uint;
typedef uint8_t cvid_codebook[12];

/* NOTE: if you want to override these functions with your own
 * implementations (not recommended) you have to link libav* as
 * dynamic libraries and remove -Wl,-Bsymbolic from the linker flags.
 * Note that this will cost performance. */
static atomic_size_t max_alloc_size = ATOMIC_VAR_INIT(INT_MAX);

#define av_popcount av_popcount_c
#define av_popcount64 av_popcount64_c
static av_always_inline av_const int av_popcount_c(uint32_t x)
{
    x -= (x >> 1) & 0x55555555;
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x += x >> 8;
    return (x + (x >> 16)) & 0x3F;
}
static av_always_inline av_const int av_popcount64_c(uint64_t x)
{
    return av_popcount((uint32_t)x) + av_popcount((uint32_t)(x >> 32));
}

typedef struct AVDictionaryEntry
{
    char *key;
    char *value;
} AVDictionaryEntry;

struct AVDictionary
{
    int count;
    AVDictionaryEntry *elems;
};

void *av_malloc(size_t size)
{
    void *ptr = NULL;

    if (size > atomic_load_explicit(&max_alloc_size, memory_order_relaxed))
        return NULL;

#if HAVE_POSIX_MEMALIGN
    if (size) // OS X on SDK 10.6 has a broken posix_memalign implementation
        if (posix_memalign(&ptr, ALIGN, size))
            ptr = NULL;
#elif HAVE_ALIGNED_MALLOC
    ptr = _aligned_malloc(size, ALIGN);
#elif HAVE_MEMALIGN
#ifndef __DJGPP__
    ptr = memalign(ALIGN, size);
#else
    ptr = memalign(size, ALIGN);
#endif
    /* Why 64?
     * Indeed, we should align it:
     *   on  4 for 386
     *   on 16 for 486
     *   on 32 for 586, PPro - K6-III
     *   on 64 for K7 (maybe for P3 too).
     * Because L1 and L2 caches are aligned on those values.
     * But I don't want to code such logic here!
     */
    /* Why 32?
     * For AVX ASM. SSE / NEON needs only 16.
     * Why not larger? Because I did not see a difference in benchmarks ...
     */
    /* benchmarks with P3
     * memalign(64) + 1          3071, 3051, 3032
     * memalign(64) + 2          3051, 3032, 3041
     * memalign(64) + 4          2911, 2896, 2915
     * memalign(64) + 8          2545, 2554, 2550
     * memalign(64) + 16         2543, 2572, 2563
     * memalign(64) + 32         2546, 2545, 2571
     * memalign(64) + 64         2570, 2533, 2558
     *
     * BTW, malloc seems to do 8-byte alignment by default here.
     */
#else
    ptr = malloc(size);
#endif
    if (!ptr && !size)
    {
        size = 1;
        ptr = av_malloc(1);
    }
#if CONFIG_MEMORY_POISONING
    if (ptr)
        memset(ptr, FF_MEMORY_POISON, size);
#endif
    return ptr;
}

void av_free(void *ptr)
{
#if HAVE_ALIGNED_MALLOC
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void av_buffer_default_free(void *opaque, uint8_t *data)
{
    av_free(data);
}

void av_freep(void *arg)
{
    void *val;

    memcpy(&val, arg, sizeof(val));
    // memcpy(arg, &(void *){ NULL }, sizeof(val));
    av_free(val);
}

void *av_mallocz(size_t size)
{
    void *ptr = av_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void av_dict_free(AVDictionary **pm)
{
    AVDictionary *m = *pm;

    if (m)
    {
        while (m->count--)
        {
            av_freep(&m->elems[m->count].key);
            av_freep(&m->elems[m->count].value);
        }
        av_freep(&m->elems);
    }
    av_freep(pm);
}

/**
 * Clip a signed integer value into the 0-255 range.
 * @param a value to clip
 * @return clipped value
 */
static av_always_inline av_const uint8_t av_clip_uint8_c(int a)
{
    if (a & (~0xFF))
        return (~a) >> 31;
    else
        return a;
}

/**
 * An AVChannelCustom defines a single channel within a custom order layout
 *
 * Unlike most structures in FFmpeg, sizeof(AVChannelCustom) is a part of the
 * public ABI.
 *
 * No new fields may be added to it without a major version bump.
 */
typedef struct AVChannelCustom
{
    enum AVChannel id;
    char name[16];
    void *opaque;
} AVChannelCustom;

typedef struct AVChannelLayout
{
    /**
     * Channel order used in this layout.
     * This is a mandatory field.
     */
    enum AVChannelOrder order;

    /**
     * Number of channels in this layout. Mandatory field.
     */
    int nb_channels;

    /**
     * Details about which channels are present in this layout.
     * For AV_CHANNEL_ORDER_UNSPEC, this field is undefined and must not be
     * used.
     */
    union
    {
        /**
         * This member must be used for AV_CHANNEL_ORDER_NATIVE, and may be used
         * for AV_CHANNEL_ORDER_AMBISONIC to signal non-diegetic channels.
         * It is a bitmask, where the position of each set bit means that the
         * AVChannel with the corresponding value is present.
         *
         * I.e. when (mask & (1 << AV_CHAN_FOO)) is non-zero, then AV_CHAN_FOO
         * is present in the layout. Otherwise it is not present.
         *
         * @note when a channel layout using a bitmask is constructed or
         * modified manually (i.e.  not using any of the av_channel_layout_*
         * functions), the code doing it must ensure that the number of set bits
         * is equal to nb_channels.
         */
        uint64_t mask;
        /**
         * This member must be used when the channel order is
         * AV_CHANNEL_ORDER_CUSTOM. It is a nb_channels-sized array, with each
         * element signalling the presence of the AVChannel with the
         * corresponding value in map[i].id.
         *
         * I.e. when map[i].id is equal to AV_CHAN_FOO, then AV_CH_FOO is the
         * i-th channel in the audio data.
         *
         * When map[i].id is in the range between AV_CHAN_AMBISONIC_BASE and
         * AV_CHAN_AMBISONIC_END (inclusive), the channel contains an ambisonic
         * component with ACN index (as defined above)
         * n = map[i].id - AV_CHAN_AMBISONIC_BASE.
         *
         * map[i].name may be filled with a 0-terminated string, in which case
         * it will be used for the purpose of identifying the channel with the
         * convenience functions below. Otherise it must be zeroed.
         */
        AVChannelCustom *map;
    } u;

    /**
     * For some private data of the user.
     */
    void *opaque;
} AVChannelLayout;

void av_channel_layout_uninit(AVChannelLayout *channel_layout)
{
    if (channel_layout->order == AV_CHANNEL_ORDER_CUSTOM)
        av_freep(&channel_layout->u.map);
    memset(channel_layout, 0, sizeof(*channel_layout));
}

/**
 * Rational number (pair of numerator and denominator).
 */
typedef struct AVRational
{
    int num; ///< Numerator
    int den; ///< Denominator
} AVRational;

const AVDictionaryEntry *av_dict_iterate(const AVDictionary *m,
                                         const AVDictionaryEntry *prev)
{
    int i = 0;

    if (!m)
        return NULL;

    if (prev)
        i = prev - m->elems + 1;

    av_assert2(i >= 0);
    if (i >= m->count)
        return NULL;

    return &m->elems[i];
}

/**
 * Locale-independent conversion of ASCII characters to uppercase.
 */
static inline av_const int av_toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        c ^= 0x20;
    return c;
}

static int size_mult(size_t a, size_t b, size_t *r)
{
    size_t t;

    t = a * b;
    /* Hack inspired from glibc: don't try the division if nelem and elsize
     * are both less than sqrt(SIZE_MAX). */
    if ((a | b) >= ((size_t)1 << (sizeof(size_t) * 4)) && a && t / a != b)
        return AVERROR(EINVAL);

    *r = t;
    return 0;
}

static void *av_realloc(void *ptr, size_t size)
{
    void *ret;
    if (size > atomic_load_explicit(&max_alloc_size, memory_order_relaxed))
        return NULL;

#if HAVE_ALIGNED_MALLOC
    ret = _aligned_realloc(ptr, size + !size, ALIGN);
#else
    ret = realloc(ptr, size + !size);
#endif
#if CONFIG_MEMORY_POISONING
    if (ret && !ptr)
        memset(ret, FF_MEMORY_POISON, size);
#endif
    return ret;
}

void *av_realloc_array(void *ptr, size_t nmemb, size_t size)
{
    size_t result;
    if (size_mult(nmemb, size, &result) < 0)
        return NULL;
    return av_realloc(ptr, result);
}

static char *av_strdup(const char *s)
{
    char *ptr = NULL;
    if (s)
    {
        size_t len = strlen(s) + 1;
        ptr = (char *)av_realloc(NULL, len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}

AVDictionaryEntry *av_dict_get(const AVDictionary *m, const char *key,
                               const AVDictionaryEntry *prev, int flags)
{
    const AVDictionaryEntry *entry = prev;
    unsigned int j;

    if (!key)
        return NULL;

    while ((entry = av_dict_iterate(m, entry)))
    {
        const char *s = entry->key;
        if (flags & AV_DICT_MATCH_CASE)
            for (j = 0; s[j] == key[j] && key[j]; j++)
                ;
        else
            for (j = 0; av_toupper(s[j]) == av_toupper(key[j]) && key[j]; j++)
                ;
        if (key[j])
            continue;
        if (s[j] && !(flags & AV_DICT_IGNORE_SUFFIX))
            continue;
        return (AVDictionaryEntry *)entry;
    }
    return NULL;
}

static int av_dict_set(AVDictionary **pm, const char *key, const char *value,
                       int flags)
{
    AVDictionary *m = *pm;
    AVDictionaryEntry *tag = NULL;
    char *copy_key = NULL, *copy_value = NULL;
    int err;

    if (flags & AV_DICT_DONT_STRDUP_VAL)
        copy_value = (char *)value;
    else if (value)
        copy_value = av_strdup(value);
    if (!key)
    {
        err = AVERROR(EINVAL);
        goto err_out;
    }
    if (!(flags & AV_DICT_MULTIKEY))
    {
        tag = av_dict_get(m, key, NULL, flags);
    }
    if (flags & AV_DICT_DONT_STRDUP_KEY)
        copy_key = (char *)key;
    else
        copy_key = av_strdup(key);
    if (!m)
        m = *pm = (AVDictionary *)av_mallocz(sizeof(*m));
    if (!m || !copy_key || (value && !copy_value))
        goto enomem;

    if (tag)
    {
        if (flags & AV_DICT_DONT_OVERWRITE)
        {
            av_free(copy_key);
            av_free(copy_value);
            return 0;
        }
        if (copy_value && flags & AV_DICT_APPEND)
        {
            size_t oldlen = strlen(tag->value);
            size_t new_part_len = strlen(copy_value);
            size_t len = oldlen + new_part_len + 1;
            char *newval = (char *)av_realloc(tag->value, len);
            if (!newval)
                goto enomem;
            memcpy(newval + oldlen, copy_value, new_part_len + 1);
            av_freep(&copy_value);
            copy_value = newval;
        }
        else
            av_free(tag->value);
        av_free(tag->key);
        *tag = m->elems[--m->count];
    }
    else if (copy_value)
    {
        AVDictionaryEntry *tmp = (AVDictionaryEntry *)av_realloc_array(m->elems,
                                                                       m->count + 1, sizeof(*m->elems));
        if (!tmp)
            goto enomem;
        m->elems = tmp;
    }
    if (copy_value)
    {
        m->elems[m->count].key = copy_key;
        m->elems[m->count].value = copy_value;
        m->count++;
    }
    else
    {
        if (!m->count)
        {
            av_freep(&m->elems);
            av_freep(pm);
        }
        av_freep(&copy_key);
    }

    return 0;

enomem:
    err = AVERROR(ENOMEM);
err_out:
    if (m && !m->count)
    {
        av_freep(&m->elems);
        av_freep(pm);
    }
    av_free(copy_key);
    av_free(copy_value);
    return err;
}

static int av_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags)
{
    const AVDictionaryEntry *t = NULL;

    while ((t = av_dict_iterate(src, t)))
    {
        int ret = av_dict_set(dst, t->key, t->value, flags);
        if (ret < 0)
            return ret;
    }

    return 0;
}

struct AVBuffer
{
    uint8_t *data; /**< data described by this buffer */
    size_t size;   /**< size of data in bytes */

    /**
     *  number of existing AVBufferRef instances referring to this buffer
     */
    atomic_uint refcount;

    /**
     * a callback for freeing the data
     */
    void (*free)(void *opaque, uint8_t *data);

    /**
     * an opaque pointer, to be used by the freeing callback
     */
    void *opaque;

    /**
     * A combination of AV_BUFFER_FLAG_*
     */
    int flags;

    /**
     * A combination of BUFFER_FLAG_*
     */
    int flags_internal;
};

/**
 * A reference to a data buffer.
 *
 * The size of this struct is not a part of the public ABI and it is not meant
 * to be allocated directly.
 */
typedef struct AVBufferRef
{
    AVBuffer *buffer;

    /**
     * The data buffer. It is considered writable if and only if
     * this is the only reference to the buffer, in which case
     * av_buffer_is_writable() returns 1.
     */
    uint8_t *data;
    /**
     * Size of data in bytes.
     */
    size_t size;
} AVBufferRef;

static AVBufferRef *buffer_create(AVBuffer *buf, uint8_t *data, size_t size,
                                  void (*free)(void *opaque, uint8_t *data),
                                  void *opaque, int flags)
{
    AVBufferRef *ref = NULL;

    buf->data = data;
    buf->size = size;
    buf->free = free ? free : av_buffer_default_free;
    buf->opaque = opaque;

    atomic_init(&buf->refcount, 1);

    buf->flags = flags;

    ref = (AVBufferRef *)av_mallocz(sizeof(*ref));
    if (!ref)
        return NULL;

    ref->buffer = buf;
    ref->data = data;
    ref->size = size;

    return ref;
}

static AVBufferRef *av_buffer_create(uint8_t *data, size_t size,
                                     void (*free)(void *opaque, uint8_t *data),
                                     void *opaque, int flags)
{
    AVBufferRef *ret;
    AVBuffer *buf = (AVBuffer *)av_mallocz(sizeof(*buf));
    if (!buf)
        return NULL;

    ret = buffer_create(buf, data, size, free, opaque, flags);
    if (!ret)
    {
        av_free(buf);
        return NULL;
    }
    return ret;
}

typedef struct AVFrameSideData
{
    enum AVFrameSideDataType type;
    uint8_t *data;
    size_t size;
    AVDictionary *metadata;
    AVBufferRef *buf;
} AVFrameSideData;

static void buffer_replace(AVBufferRef **dst, AVBufferRef **src)
{
    AVBuffer *b;

    b = (*dst)->buffer;

    if (src)
    {
        **dst = **src;
        av_freep(src);
    }
    else
        av_freep(dst);

    if (atomic_fetch_sub_explicit(&b->refcount, 1, memory_order_acq_rel) == 1)
    {
        /* b->free below might already free the structure containing *b,
         * so we have to read the flag now to avoid use-after-free. */
        int free_avbuffer = !(b->flags_internal & BUFFER_FLAG_NO_FREE);
        b->free(b->opaque, b->data);
        if (free_avbuffer)
            av_free(b);
    }
}

static void av_buffer_unref(AVBufferRef **buf)
{
    if (!buf || !*buf)
        return;

    buffer_replace(buf, NULL);
}

static void free_side_data(AVFrameSideData **ptr_sd)
{
    AVFrameSideData *sd = *ptr_sd;

    av_buffer_unref(&sd->buf);
    av_dict_free(&sd->metadata);
    av_freep(ptr_sd);
}

typedef struct AVFrame
{
#define AV_NUM_DATA_POINTERS 8
    /**
     * pointer to the picture/channel planes.
     * This might be different from the first allocated byte. For video,
     * it could even point to the end of the image data.
     *
     * All pointers in data and extended_data must point into one of the
     * AVBufferRef in buf or extended_buf.
     *
     * Some decoders access areas outside 0,0 - width,height, please
     * see avcodec_align_dimensions2(). Some filters and swscale can read
     * up to 16 bytes beyond the planes, if these filters are to be used,
     * then 16 extra bytes must be allocated.
     *
     * NOTE: Pointers not needed by the format MUST be set to NULL.
     *
     * @attention In case of video, the data[] pointers can point to the
     * end of image data in order to reverse line order, when used in
     * combination with negative values in the linesize[] array.
     */
    uint8_t *data[AV_NUM_DATA_POINTERS];

    /**
     * For video, a positive or negative value, which is typically indicating
     * the size in bytes of each picture line, but it can also be:
     * - the negative byte size of lines for vertical flipping
     *   (with data[n] pointing to the end of the data
     * - a positive or negative multiple of the byte size as for accessing
     *   even and odd fields of a frame (possibly flipped)
     *
     * For audio, only linesize[0] may be set. For planar audio, each channel
     * plane must be the same size.
     *
     * For video the linesizes should be multiples of the CPUs alignment
     * preference, this is 16 or 32 for modern desktop CPUs.
     * Some code requires such alignment other code can be slower without
     * correct alignment, for yet other it makes no difference.
     *
     * @note The linesize may be larger than the size of usable data -- there
     * may be extra padding present for performance reasons.
     *
     * @attention In case of video, line size values can be negative to achieve
     * a vertically inverted iteration over image lines.
     */
    int linesize[AV_NUM_DATA_POINTERS];

    /**
     * pointers to the data planes/channels.
     *
     * For video, this should simply point to data[].
     *
     * For planar audio, each channel has a separate data pointer, and
     * linesize[0] contains the size of each channel buffer.
     * For packed audio, there is just one data pointer, and linesize[0]
     * contains the total size of the buffer for all channels.
     *
     * Note: Both data and extended_data should always be set in a valid frame,
     * but for planar audio with more channels that can fit in data,
     * extended_data must be used in order to access all channels.
     */
    uint8_t **extended_data;

    /**
     * @name Video dimensions
     * Video frames only. The coded dimensions (in pixels) of the video frame,
     * i.e. the size of the rectangle that contains some well-defined values.
     *
     * @note The part of the frame intended for display/presentation is further
     * restricted by the @ref cropping "Cropping rectangle".
     * @{
     */
    int width, height;
    /**
     * @}
     */

    /**
     * number of audio samples (per channel) described by this frame
     */
    int nb_samples;

    /**
     * format of the frame, -1 if unknown or unset
     * Values correspond to enum AVPixelFormat for video frames,
     * enum AVSampleFormat for audio)
     */
    int format;

    /**
     * 1 -> keyframe, 0-> not
     */
    int key_frame;

    /**
     * Picture type of the frame.
     */
    enum AVPictureType pict_type;

    /**
     * Sample aspect ratio for the video frame, 0/1 if unknown/unspecified.
     */
    AVRational sample_aspect_ratio;

    /**
     * Presentation timestamp in time_base units (time when frame should be shown to user).
     */
    int64_t pts;

    /**
     * DTS copied from the AVPacket that triggered returning this frame. (if frame threading isn't used)
     * This is also the Presentation time of this AVFrame calculated from
     * only AVPacket.dts values without pts values.
     */
    int64_t pkt_dts;

    /**
     * Time base for the timestamps in this frame.
     * In the future, this field may be set on frames output by decoders or
     * filters, but its value will be by default ignored on input to encoders
     * or filters.
     */
    AVRational time_base;

    /**
     * picture number in bitstream order
     */
    int coded_picture_number;
    /**
     * picture number in display order
     */
    int display_picture_number;

    /**
     * quality (between 1 (good) and FF_LAMBDA_MAX (bad))
     */
    int quality;

    /**
     * for some private data of the user
     */
    void *opaque;

    /**
     * When decoding, this signals how much the picture must be delayed.
     * extra_delay = repeat_pict / (2*fps)
     */
    int repeat_pict;

    /**
     * The content of the picture is interlaced.
     */
    int interlaced_frame;

    /**
     * If the content is interlaced, is top field displayed first.
     */
    int top_field_first;

    /**
     * Tell user application that palette has changed from previous frame.
     */
    int palette_has_changed;

    /**
     * reordered opaque 64 bits (generally an integer or a double precision float
     * PTS but can be anything).
     * The user sets AVCodecContext.reordered_opaque to represent the input at
     * that time,
     * the decoder reorders values as needed and sets AVFrame.reordered_opaque
     * to exactly one of the values provided by the user through AVCodecContext.reordered_opaque
     */
    int64_t reordered_opaque;

    /**
     * Sample rate of the audio data.
     */
    int sample_rate;

#if FF_API_OLD_CHANNEL_LAYOUT
    /**
     * Channel layout of the audio data.
     * @deprecated use ch_layout instead
     */
    attribute_deprecated
        uint64_t channel_layout;
#endif

    /**
     * AVBuffer references backing the data for this frame. All the pointers in
     * data and extended_data must point inside one of the buffers in buf or
     * extended_buf. This array must be filled contiguously -- if buf[i] is
     * non-NULL then buf[j] must also be non-NULL for all j < i.
     *
     * There may be at most one AVBuffer per data plane, so for video this array
     * always contains all the references. For planar audio with more than
     * AV_NUM_DATA_POINTERS channels, there may be more buffers than can fit in
     * this array. Then the extra AVBufferRef pointers are stored in the
     * extended_buf array.
     */
    AVBufferRef *buf[AV_NUM_DATA_POINTERS];

    /**
     * For planar audio which requires more than AV_NUM_DATA_POINTERS
     * AVBufferRef pointers, this array will hold all the references which
     * cannot fit into AVFrame.buf.
     *
     * Note that this is different from AVFrame.extended_data, which always
     * contains all the pointers. This array only contains the extra pointers,
     * which cannot fit into AVFrame.buf.
     *
     * This array is always allocated using av_malloc() by whoever constructs
     * the frame. It is freed in av_frame_unref().
     */
    AVBufferRef **extended_buf;
    /**
     * Number of elements in extended_buf.
     */
    int nb_extended_buf;

    AVFrameSideData **side_data;
    int nb_side_data;

/**
 * @defgroup lavu_frame_flags AV_FRAME_FLAGS
 * @ingroup lavu_frame
 * Flags describing additional frame properties.
 *
 * @{
 */

/**
 * The frame data may be corrupted, e.g. due to decoding errors.
 */
#define AV_FRAME_FLAG_CORRUPT (1 << 0)
/**
 * A flag to mark the frames which need to be decoded, but shouldn't be output.
 */
#define AV_FRAME_FLAG_DISCARD (1 << 2)
    /**
     * @}
     */

    /**
     * Frame flags, a combination of @ref lavu_frame_flags
     */
    int flags;

    /**
     * MPEG vs JPEG YUV range.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorRange color_range;

    enum AVColorPrimaries color_primaries;

    enum AVColorTransferCharacteristic color_trc;

    /**
     * YUV colorspace type.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorSpace colorspace;

    enum AVChromaLocation chroma_location;

    /**
     * frame timestamp estimated using various heuristics, in stream time base
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    int64_t best_effort_timestamp;

    /**
     * reordered pos from the last AVPacket that has been input into the decoder
     * - encoding: unused
     * - decoding: Read by user.
     */
    int64_t pkt_pos;

#if FF_API_PKT_DURATION
    /**
     * duration of the corresponding packet, expressed in
     * AVStream->time_base units, 0 if unknown.
     * - encoding: unused
     * - decoding: Read by user.
     *
     * @deprecated use duration instead
     */
    attribute_deprecated
        int64_t pkt_duration;
#endif

    /**
     * metadata.
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    AVDictionary *metadata;

    /**
     * decode error flags of the frame, set to a combination of
     * FF_DECODE_ERROR_xxx flags if the decoder produced a frame, but there
     * were errors during the decoding.
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    int decode_error_flags;
#define FF_DECODE_ERROR_INVALID_BITSTREAM 1
#define FF_DECODE_ERROR_MISSING_REFERENCE 2
#define FF_DECODE_ERROR_CONCEALMENT_ACTIVE 4
#define FF_DECODE_ERROR_DECODE_SLICES 8

#if FF_API_OLD_CHANNEL_LAYOUT
    /**
     * number of audio channels, only used for audio.
     * - encoding: unused
     * - decoding: Read by user.
     * @deprecated use ch_layout instead
     */
    attribute_deprecated int channels;
#endif

    /**
     * size of the corresponding packet containing the compressed
     * frame.
     * It is set to a negative value if unknown.
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    int pkt_size;

    /**
     * For hwaccel-format frames, this should be a reference to the
     * AVHWFramesContext describing the frame.
     */
    AVBufferRef *hw_frames_ctx;

    /**
     * AVBufferRef for free use by the API user. FFmpeg will never check the
     * contents of the buffer ref. FFmpeg calls av_buffer_unref() on it when
     * the frame is unreferenced. av_frame_copy_props() calls create a new
     * reference with av_buffer_ref() for the target frame's opaque_ref field.
     *
     * This is unrelated to the opaque field, although it serves a similar
     * purpose.
     */
    AVBufferRef *opaque_ref;

    /**
     * @anchor cropping
     * @name Cropping
     * Video frames only. The number of pixels to discard from the the
     * top/bottom/left/right border of the frame to obtain the sub-rectangle of
     * the frame intended for presentation.
     * @{
     */
    size_t crop_top;
    size_t crop_bottom;
    size_t crop_left;
    size_t crop_right;
    /**
     * @}
     */

    /**
     * AVBufferRef for internal use by a single libav* library.
     * Must not be used to transfer data between libraries.
     * Has to be NULL when ownership of the frame leaves the respective library.
     *
     * Code outside the FFmpeg libs should never check or change the contents of the buffer ref.
     *
     * FFmpeg calls av_buffer_unref() on it when the frame is unreferenced.
     * av_frame_copy_props() calls create a new reference with av_buffer_ref()
     * for the target frame's private_ref field.
     */
    AVBufferRef *private_ref;

    /**
     * Channel layout of the audio data.
     */
    AVChannelLayout ch_layout;

    /**
     * Duration of the frame, in the same units as pts. 0 if unknown.
     */
    int64_t duration;
} AVFrame;

static void get_frame_defaults(AVFrame *frame)
{
    memset(frame, 0, sizeof(*frame));

    frame->pts =
        frame->pkt_dts = AV_NOPTS_VALUE;
    frame->best_effort_timestamp = AV_NOPTS_VALUE;
    frame->duration = 0;
#if FF_API_PKT_DURATION
    FF_DISABLE_DEPRECATION_WARNINGS
    frame->pkt_duration = 0;
    FF_ENABLE_DEPRECATION_WARNINGS
#endif
    frame->pkt_pos = -1;
    frame->pkt_size = -1;
    frame->time_base = (AVRational){0, 1};
    frame->key_frame = 1;
    frame->sample_aspect_ratio = (AVRational){0, 1};
    frame->format = -1; /* unknown */
    frame->extended_data = frame->data;
    frame->color_primaries = AVCOL_PRI_UNSPECIFIED;
    frame->color_trc = AVCOL_TRC_UNSPECIFIED;
    frame->colorspace = AVCOL_SPC_UNSPECIFIED;
    frame->color_range = AVCOL_RANGE_UNSPECIFIED;
    frame->chroma_location = AVCHROMA_LOC_UNSPECIFIED;
    frame->flags = 0;
}

static AVBufferRef *av_buffer_alloc(size_t size)
{
    AVBufferRef *ret = NULL;
    uint8_t *data = NULL;

    data = (uint8_t *)av_malloc(size);
    if (!data)
        return NULL;

    ret = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
    if (!ret)
        av_freep(&data);

    return ret;
}

static AVFrameSideData *av_frame_new_side_data_from_buf(AVFrame *frame,
                                                        enum AVFrameSideDataType type,
                                                        AVBufferRef *buf)
{
    AVFrameSideData *ret, **tmp;

    if (!buf)
        return NULL;

    if (frame->nb_side_data > INT_MAX / sizeof(*frame->side_data) - 1)
        return NULL;

    tmp = (AVFrameSideData **)av_realloc(frame->side_data,
                                         (frame->nb_side_data + 1) * sizeof(*frame->side_data));
    if (!tmp)
        return NULL;
    frame->side_data = tmp;

    ret = (AVFrameSideData *)av_mallocz(sizeof(*ret));
    if (!ret)
        return NULL;

    ret->buf = buf;
    ret->data = ret->buf->data;
    ret->size = buf->size;
    ret->type = type;

    frame->side_data[frame->nb_side_data++] = ret;

    return ret;
}

static AVFrameSideData *av_frame_new_side_data(AVFrame *frame,
                                               enum AVFrameSideDataType type,
                                               size_t size)
{
    AVFrameSideData *ret;
    AVBufferRef *buf = av_buffer_alloc(size);
    ret = av_frame_new_side_data_from_buf(frame, type, buf);
    if (!ret)
        av_buffer_unref(&buf);
    return ret;
}

static void wipe_side_data(AVFrame *frame)
{
    int i;

    for (i = 0; i < frame->nb_side_data; i++)
    {
        free_side_data(&frame->side_data[i]);
    }
    frame->nb_side_data = 0;

    av_freep(&frame->side_data);
}

static AVFrame *av_frame_alloc(void)
{
    AVFrame *frame = (AVFrame *)malloc(sizeof(*frame));

    if (!frame)
        return NULL;

    frame->key_frame = 1;

    return frame;
}

void av_frame_unref(AVFrame *frame)
{
    int i;

    if (!frame)
        return;

    wipe_side_data(frame);

    for (i = 0; i < FF_ARRAY_ELEMS(frame->buf); i++)
        av_buffer_unref(&frame->buf[i]);
    for (i = 0; i < frame->nb_extended_buf; i++)
        av_buffer_unref(&frame->extended_buf[i]);
    av_freep(&frame->extended_buf);
    av_dict_free(&frame->metadata);

    av_buffer_unref(&frame->hw_frames_ctx);

    av_buffer_unref(&frame->opaque_ref);
    av_buffer_unref(&frame->private_ref);

    if (frame->extended_data != frame->data)
        av_freep(&frame->extended_data);

    av_channel_layout_uninit(&frame->ch_layout);

    get_frame_defaults(frame);
}

typedef struct AVPacketSideData
{
    uint8_t *data;
    size_t size;
    enum AVPacketSideDataType type;
} AVPacketSideData;

typedef struct AVPacket
{
    uint8_t *data;
    int size;
    AVPacketSideData *side_data;
    int side_data_elems;
} AVPacket;

static AVBufferRef *av_buffer_ref(const AVBufferRef *buf)
{
    AVBufferRef *ret = (AVBufferRef *)av_mallocz(sizeof(*ret));

    if (!ret)
        return NULL;

    *ret = *buf;

    atomic_fetch_add_explicit(&buf->buffer->refcount, 1, memory_order_relaxed);

    return ret;
}

static int av_buffer_replace(AVBufferRef **pdst, const AVBufferRef *src)
{
    AVBufferRef *dst = *pdst;
    AVBufferRef *tmp;

    if (!src)
    {
        av_buffer_unref(pdst);
        return 0;
    }

    if (dst && dst->buffer == src->buffer)
    {
        /* make sure the data pointers match */
        dst->data = src->data;
        dst->size = src->size;
        return 0;
    }

    tmp = av_buffer_ref(src);
    if (!tmp)
        return AVERROR(ENOMEM);

    av_buffer_unref(pdst);
    *pdst = tmp;
    return 0;
}

static int frame_copy_props(AVFrame *dst, const AVFrame *src, int force_copy)
{
    int ret, i;

    dst->key_frame = src->key_frame;
    dst->pict_type = src->pict_type;
    dst->sample_aspect_ratio = src->sample_aspect_ratio;
    dst->crop_top = src->crop_top;
    dst->crop_bottom = src->crop_bottom;
    dst->crop_left = src->crop_left;
    dst->crop_right = src->crop_right;
    dst->pts = src->pts;
    dst->duration = src->duration;
    dst->repeat_pict = src->repeat_pict;
    dst->interlaced_frame = src->interlaced_frame;
    dst->top_field_first = src->top_field_first;
    dst->palette_has_changed = src->palette_has_changed;
    dst->sample_rate = src->sample_rate;
    dst->opaque = src->opaque;
    dst->pkt_dts = src->pkt_dts;
    dst->pkt_pos = src->pkt_pos;
    dst->pkt_size = src->pkt_size;
#if FF_API_PKT_DURATION
    FF_DISABLE_DEPRECATION_WARNINGS
    dst->pkt_duration = src->pkt_duration;
    FF_ENABLE_DEPRECATION_WARNINGS
#endif
    dst->time_base = src->time_base;
    dst->reordered_opaque = src->reordered_opaque;
    dst->quality = src->quality;
    dst->best_effort_timestamp = src->best_effort_timestamp;
    dst->coded_picture_number = src->coded_picture_number;
    dst->display_picture_number = src->display_picture_number;
    dst->flags = src->flags;
    dst->decode_error_flags = src->decode_error_flags;
    dst->color_primaries = src->color_primaries;
    dst->color_trc = src->color_trc;
    dst->colorspace = src->colorspace;
    dst->color_range = src->color_range;
    dst->chroma_location = src->chroma_location;

    av_dict_copy(&dst->metadata, src->metadata, 0);

    for (i = 0; i < src->nb_side_data; i++)
    {
        const AVFrameSideData *sd_src = src->side_data[i];
        AVFrameSideData *sd_dst;
        if (sd_src->type == AV_FRAME_DATA_PANSCAN && (src->width != dst->width || src->height != dst->height))
            continue;
        if (force_copy)
        {
            sd_dst = av_frame_new_side_data(dst, sd_src->type,
                                            sd_src->size);
            if (!sd_dst)
            {
                wipe_side_data(dst);
                return AVERROR(ENOMEM);
            }
            memcpy(sd_dst->data, sd_src->data, sd_src->size);
        }
        else
        {
            AVBufferRef *ref = av_buffer_ref(sd_src->buf);
            sd_dst = av_frame_new_side_data_from_buf(dst, sd_src->type, ref);
            if (!sd_dst)
            {
                av_buffer_unref(&ref);
                wipe_side_data(dst);
                return AVERROR(ENOMEM);
            }
        }
        av_dict_copy(&sd_dst->metadata, sd_src->metadata, 0);
    }

    ret = av_buffer_replace(&dst->opaque_ref, src->opaque_ref);
    ret |= av_buffer_replace(&dst->private_ref, src->private_ref);
    return ret;
}

static uint8_t *av_packet_get_side_data(const AVPacket *pkt, enum AVPacketSideDataType type,
                                        size_t *size)
{
    int i;

    for (i = 0; i < pkt->side_data_elems; i++)
    {
        if (pkt->side_data[i].type == type)
        {
            if (size)
                *size = pkt->side_data[i].size;
            return pkt->side_data[i].data;
        }
    }
    if (size)
        *size = 0;
    return NULL;
}

static int av_channel_layout_check(const AVChannelLayout *channel_layout)
{
    if (channel_layout->nb_channels <= 0)
        return 0;

    switch (channel_layout->order)
    {
    case AV_CHANNEL_ORDER_NATIVE:
        return av_popcount64(channel_layout->u.mask) == channel_layout->nb_channels;
    case AV_CHANNEL_ORDER_CUSTOM:
        if (!channel_layout->u.map)
            return 0;
        for (int i = 0; i < channel_layout->nb_channels; i++)
        {
            if (channel_layout->u.map[i].id == AV_CHAN_NONE)
                return 0;
        }
        return 1;
    case AV_CHANNEL_ORDER_AMBISONIC:
        /* If non-diegetic channels are present, ensure they are taken into account */
        return av_popcount64(channel_layout->u.mask) < channel_layout->nb_channels;
    case AV_CHANNEL_ORDER_UNSPEC:
        return 1;
    default:
        return 0;
    }
}

static int ff_copy_palette(void *dst, const AVPacket *src, void *logctx)
{
    size_t size;
    const void *pal = av_packet_get_side_data(src, AV_PKT_DATA_PALETTE, &size);

    if (pal && size == AVPALETTE_SIZE)
    {
        memcpy(dst, pal, AVPALETTE_SIZE);
        return 1;
    }
    else if (pal)
    {
        av_log(logctx, AV_LOG_ERROR,
               "Palette size %" SIZE_SPECIFIER " is wrong\n", size);
    }
    return 0;
}

static void *av_malloc_array(size_t nmemb, size_t size)
{
    size_t result;
    if (size_mult(nmemb, size, &result) < 0)
        return NULL;
    return av_malloc(result);
}

static int av_channel_layout_copy(AVChannelLayout *dst, const AVChannelLayout *src)
{
    av_channel_layout_uninit(dst);
    *dst = *src;
    if (src->order == AV_CHANNEL_ORDER_CUSTOM)
    {
        dst->u.map = (AVChannelCustom *)av_malloc_array(src->nb_channels, sizeof(*dst->u.map));
        if (!dst->u.map)
            return AVERROR(ENOMEM);
        memcpy(dst->u.map, src->u.map, src->nb_channels * sizeof(*src->u.map));
    }
    return 0;
}

typedef struct AVClass {
    /**
     * The name of the class; usually it is the same name as the
     * context structure type to which the AVClass is associated.
     */
    const char* class_name;

    /**
     * A pointer to a function which returns the name of a context
     * instance ctx associated with the class.
     */
    const char* (*item_name)(void* ctx);

    /**
     * a pointer to the first option specified in the class if any or NULL
     *
     * @see av_set_default_options()
     */
    const struct AVOption *option;

    /**
     * LIBAVUTIL_VERSION with which this structure was created.
     * This is used to allow fields to be added without requiring major
     * version bumps everywhere.
     */

    int version;

    /**
     * Offset in the structure where log_level_offset is stored.
     * 0 means there is no such variable
     */
    int log_level_offset_offset;

    /**
     * Offset in the structure where a pointer to the parent context for
     * logging is stored. For example a decoder could pass its AVCodecContext
     * to eval as such a parent context, which an av_log() implementation
     * could then leverage to display the parent context.
     * The offset can be NULL.
     */
    int parent_log_context_offset;

    /**
     * Category used for visualization (like color)
     * This is only set if the category is equal for all objects using this class.
     * available since version (51 << 16 | 56 << 8 | 100)
     */
    AVClassCategory category;

    /**
     * Callback to return the category.
     * available since version (51 << 16 | 59 << 8 | 100)
     */
    AVClassCategory (*get_category)(void* ctx);

    /**
     * Callback to return the supported/allowed ranges.
     * available since version (52.12)
     */
    int (*query_ranges)(struct AVOptionRanges **, void *obj, const char *key, int flags);

    /**
     * Return next AVOptions-enabled child or NULL
     */
    void* (*child_next)(void *obj, void *prev);

    /**
     * Iterate over the AVClasses corresponding to potential AVOptions-enabled
     * children.
     *
     * @param iter pointer to opaque iteration state. The caller must initialize
     *             *iter to NULL before the first call.
     * @return AVClass for the next AVOptions-enabled child or NULL if there are
     *         no more such children.
     *
     * @note The difference between child_next and this is that child_next
     *       iterates over _already existing_ objects, while child_class_iterate
     *       iterates over _all possible_ children.
     */
    const struct AVClass* (*child_class_iterate)(void **iter);
} AVClass;

typedef struct AVFilterContext AVFilterContext;
typedef struct AVFilterLink    AVFilterLink;
typedef struct AVFilterPad     AVFilterPad;
typedef struct AVFilterFormats AVFilterFormats;
typedef struct AVFilterChannelLayouts AVFilterChannelLayouts;

typedef struct AVFilter {
    /**
     * Filter name. Must be non-NULL and unique among filters.
     */
    const char *name;

    /**
     * A description of the filter. May be NULL.
     *
     * You should use the NULL_IF_CONFIG_SMALL() macro to define it.
     */
    const char *description;

    /**
     * List of static inputs.
     *
     * NULL if there are no (static) inputs. Instances of filters with
     * AVFILTER_FLAG_DYNAMIC_INPUTS set may have more inputs than present in
     * this list.
     */
    const AVFilterPad *inputs;

    /**
     * List of static outputs.
     *
     * NULL if there are no (static) outputs. Instances of filters with
     * AVFILTER_FLAG_DYNAMIC_OUTPUTS set may have more outputs than present in
     * this list.
     */
    const AVFilterPad *outputs;

    /**
     * A class for the private data, used to declare filter private AVOptions.
     * This field is NULL for filters that do not declare any options.
     *
     * If this field is non-NULL, the first member of the filter private data
     * must be a pointer to AVClass, which will be set by libavfilter generic
     * code to this class.
     */
    const AVClass *priv_class;

    /**
     * A combination of AVFILTER_FLAG_*
     */
    int flags;

    /*****************************************************************
     * All fields below this line are not part of the public API. They
     * may not be used outside of libavfilter and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */

    /**
     * The number of entries in the list of inputs.
     */
    uint8_t nb_inputs;

    /**
     * The number of entries in the list of outputs.
     */
    uint8_t nb_outputs;

    /**
     * This field determines the state of the formats union.
     * It is an enum FilterFormatsState value.
     */
    uint8_t formats_state;

    /**
     * Filter pre-initialization function
     *
     * This callback will be called immediately after the filter context is
     * allocated, to allow allocating and initing sub-objects.
     *
     * If this callback is not NULL, the uninit callback will be called on
     * allocation failure.
     *
     * @return 0 on success,
     *         AVERROR code on failure (but the code will be
     *           dropped and treated as ENOMEM by the calling code)
     */
    int (*preinit)(AVFilterContext *ctx);

    /**
     * Filter initialization function.
     *
     * This callback will be called only once during the filter lifetime, after
     * all the options have been set, but before links between filters are
     * established and format negotiation is done.
     *
     * Basic filter initialization should be done here. Filters with dynamic
     * inputs and/or outputs should create those inputs/outputs here based on
     * provided options. No more changes to this filter's inputs/outputs can be
     * done after this callback.
     *
     * This callback must not assume that the filter links exist or frame
     * parameters are known.
     *
     * @ref AVFilter.uninit "uninit" is guaranteed to be called even if
     * initialization fails, so this callback does not have to clean up on
     * failure.
     *
     * @return 0 on success, a negative AVERROR on failure
     */
    int (*init)(AVFilterContext *ctx);

    /**
     * Filter uninitialization function.
     *
     * Called only once right before the filter is freed. Should deallocate any
     * memory held by the filter, release any buffer references, etc. It does
     * not need to deallocate the AVFilterContext.priv memory itself.
     *
     * This callback may be called even if @ref AVFilter.init "init" was not
     * called or failed, so it must be prepared to handle such a situation.
     */
    void (*uninit)(AVFilterContext *ctx);

    /**
     * The state of the following union is determined by formats_state.
     * See the documentation of enum FilterFormatsState in internal.h.
     */
    union {
        /**
         * Query formats supported by the filter on its inputs and outputs.
         *
         * This callback is called after the filter is initialized (so the inputs
         * and outputs are fixed), shortly before the format negotiation. This
         * callback may be called more than once.
         *
         * This callback must set ::AVFilterLink's
         * @ref AVFilterFormatsConfig.formats "outcfg.formats"
         * on every input link and
         * @ref AVFilterFormatsConfig.formats "incfg.formats"
         * on every output link to a list of pixel/sample formats that the filter
         * supports on that link.
         * For audio links, this filter must also set
         * @ref AVFilterFormatsConfig.samplerates "incfg.samplerates"
         *  /
         * @ref AVFilterFormatsConfig.samplerates "outcfg.samplerates"
         * and @ref AVFilterFormatsConfig.channel_layouts "incfg.channel_layouts"
         *  /
         * @ref AVFilterFormatsConfig.channel_layouts "outcfg.channel_layouts"
         * analogously.
         *
         * This callback must never be NULL if the union is in this state.
         *
         * @return zero on success, a negative value corresponding to an
         * AVERROR code otherwise
         */
        int (*query_func)(AVFilterContext *);
        /**
         * A pointer to an array of admissible pixel formats delimited
         * by AV_PIX_FMT_NONE. The generic code will use this list
         * to indicate that this filter supports each of these pixel formats,
         * provided that all inputs and outputs use the same pixel format.
         *
         * This list must never be NULL if the union is in this state.
         * The type of all inputs and outputs of filters using this must
         * be AVMEDIA_TYPE_VIDEO.
         */
        const enum AVPixelFormat *pixels_list;
        /**
         * Analogous to pixels, but delimited by AV_SAMPLE_FMT_NONE
         * and restricted to filters that only have AVMEDIA_TYPE_AUDIO
         * inputs and outputs.
         *
         * In addition to that the generic code will mark all inputs
         * and all outputs as supporting all sample rates and every
         * channel count and channel layout, as long as all inputs
         * and outputs use the same sample rate and channel count/layout.
         */
        const enum AVSampleFormat *samples_list;
        /**
         * Equivalent to { pix_fmt, AV_PIX_FMT_NONE } as pixels_list.
         */
        enum AVPixelFormat  pix_fmt;
        /**
         * Equivalent to { sample_fmt, AV_SAMPLE_FMT_NONE } as samples_list.
         */
        enum AVSampleFormat sample_fmt;
    } formats;

    int priv_size;      ///< size of private data to allocate for the filter

    int flags_internal; ///< Additional flags for avfilter internal use only.

    /**
     * Make the filter instance process a command.
     *
     * @param cmd    the command to process, for handling simplicity all commands must be alphanumeric only
     * @param arg    the argument for the command
     * @param res    a buffer with size res_size where the filter(s) can return a response. This must not change when the command is not supported.
     * @param flags  if AVFILTER_CMD_FLAG_FAST is set and the command would be
     *               time consuming then a filter should treat it like an unsupported command
     *
     * @returns >=0 on success otherwise an error code.
     *          AVERROR(ENOSYS) on unsupported commands
     */
    int (*process_command)(AVFilterContext *, const char *cmd, const char *arg, char *res, int res_len, int flags);

    /**
     * Filter activation function.
     *
     * Called when any processing is needed from the filter, instead of any
     * filter_frame and request_frame on pads.
     *
     * The function must examine inlinks and outlinks and perform a single
     * step of processing. If there is nothing to do, the function must do
     * nothing and not return an error. If more steps are or may be
     * possible, it must use ff_filter_set_ready() to schedule another
     * activation.
     */
    int (*activate)(AVFilterContext *ctx);
} AVFilter;

typedef int (avfilter_action_func)(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs);
typedef int (avfilter_execute_func)(AVFilterContext *ctx, avfilter_action_func *func,
                                    void *arg, int *ret, int nb_jobs);

typedef struct AVFilterInternal AVFilterInternal;
struct AVFilterInternal {
    avfilter_execute_func *execute;
};

struct AVFilterContext {
    const AVClass *av_class;        ///< needed for av_log() and filters common options

    const AVFilter *filter;         ///< the AVFilter of which this is an instance

    char *name;                     ///< name of this filter instance

    AVFilterPad   *input_pads;      ///< array of input pads
    AVFilterLink **inputs;          ///< array of pointers to input links
    unsigned    nb_inputs;          ///< number of input pads

    AVFilterPad   *output_pads;     ///< array of output pads
    AVFilterLink **outputs;         ///< array of pointers to output links
    unsigned    nb_outputs;         ///< number of output pads

    void *priv;                     ///< private data for use by the filter

    struct AVFilterGraph *graph;    ///< filtergraph this filter belongs to

    /**
     * Type of multithreading being allowed/used. A combination of
     * AVFILTER_THREAD_* flags.
     *
     * May be set by the caller before initializing the filter to forbid some
     * or all kinds of multithreading for this filter. The default is allowing
     * everything.
     *
     * When the filter is initialized, this field is combined using bit AND with
     * AVFilterGraph.thread_type to get the final mask used for determining
     * allowed threading types. I.e. a threading type needs to be set in both
     * to be allowed.
     *
     * After the filter is initialized, libavfilter sets this field to the
     * threading type that is actually used (0 for no multithreading).
     */
    int thread_type;

    /**
     * An opaque struct for libavfilter internal use.
     */
    AVFilterInternal *internal;

    struct AVFilterCommand *command_queue;

    char *enable_str;               ///< enable expression string
    void *enable;                   ///< parsed expression (AVExpr*)
    double *var_values;             ///< variable values for the enable expression
    int is_disabled;                ///< the enabled state from the last expression evaluation

    /**
     * For filters which will create hardware frames, sets the device the
     * filter should create them in.  All other filters will ignore this field:
     * in particular, a filter which consumes or processes hardware frames will
     * instead use the hw_frames_ctx field in AVFilterLink to carry the
     * hardware context information.
     */
    AVBufferRef *hw_device_ctx;

    /**
     * Max number of threads allowed in this filter instance.
     * If <= 0, its value is ignored.
     * Overrides global number of threads set per filter graph.
     */
    int nb_threads;

    /**
     * Ready status of the filter.
     * A non-0 value means that the filter needs activating;
     * a higher value suggests a more urgent activation.
     */
    unsigned ready;

    /**
     * Sets the number of extra hardware frames which the filter will
     * allocate on its output links for use in following filters or by
     * the caller.
     *
     * Some hardware filters require all frames that they will use for
     * output to be defined in advance before filtering starts.  For such
     * filters, any hardware frame pools used for output must therefore be
     * of fixed size.  The extra frames set here are on top of any number
     * that the filter needs internally in order to operate normally.
     *
     * This field must be set before the graph containing this filter is
     * configured.
     */
    int extra_hw_frames;
};


typedef struct AVFilterFormatsConfig {

    /**
     * List of supported formats (pixel or sample).
     */
    AVFilterFormats *formats;

    /**
     * Lists of supported sample rates, only for audio.
     */
    AVFilterFormats  *samplerates;

    /**
     * Lists of supported channel layouts, only for audio.
     */
    AVFilterChannelLayouts  *channel_layouts;

} AVFilterFormatsConfig;

struct AVFilterLink {
    AVFilterContext *src;       ///< source filter
    AVFilterPad *srcpad;        ///< output pad on the source filter

    AVFilterContext *dst;       ///< dest filter
    AVFilterPad *dstpad;        ///< input pad on the dest filter

    enum AVMediaType type;      ///< filter media type

    /* These parameters apply only to video */
    int w;                      ///< agreed upon image width
    int h;                      ///< agreed upon image height
    AVRational sample_aspect_ratio; ///< agreed upon sample aspect ratio
    /* These parameters apply only to audio */
#if FF_API_OLD_CHANNEL_LAYOUT
    /**
     * channel layout of current buffer (see libavutil/channel_layout.h)
     * @deprecated use ch_layout
     */
    attribute_deprecated
    uint64_t channel_layout;
#endif
    int sample_rate;            ///< samples per second

    int format;                 ///< agreed upon media format

    /**
     * Define the time base used by the PTS of the frames/samples
     * which will pass through this link.
     * During the configuration stage, each filter is supposed to
     * change only the output timebase, while the timebase of the
     * input link is assumed to be an unchangeable property.
     */
    AVRational time_base;

    AVChannelLayout ch_layout;  ///< channel layout of current buffer (see libavutil/channel_layout.h)

    /*****************************************************************
     * All fields below this line are not part of the public API. They
     * may not be used outside of libavfilter and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */

    /**
     * Lists of supported formats / etc. supported by the input filter.
     */
    AVFilterFormatsConfig incfg;

    /**
     * Lists of supported formats / etc. supported by the output filter.
     */
    AVFilterFormatsConfig outcfg;

    /** stage of the initialization of the link properties (dimensions, etc) */
    enum {
        AVLINK_UNINIT = 0,      ///< not started
        AVLINK_STARTINIT,       ///< started, but incomplete
        AVLINK_INIT             ///< complete
    } init_state;

    /**
     * Graph the filter belongs to.
     */
    struct AVFilterGraph *graph;

    /**
     * Current timestamp of the link, as defined by the most recent
     * frame(s), in link time_base units.
     */
    int64_t current_pts;

    /**
     * Current timestamp of the link, as defined by the most recent
     * frame(s), in AV_TIME_BASE units.
     */
    int64_t current_pts_us;

    /**
     * Index in the age array.
     */
    int age_index;

    /**
     * Frame rate of the stream on the link, or 1/0 if unknown or variable;
     * if left to 0/0, will be automatically copied from the first input
     * of the source filter if it exists.
     *
     * Sources should set it to the best estimation of the real frame rate.
     * If the source frame rate is unknown or variable, set this to 1/0.
     * Filters should update it if necessary depending on their function.
     * Sinks can use it to set a default output frame rate.
     * It is similar to the r_frame_rate field in AVStream.
     */
    AVRational frame_rate;

    /**
     * Minimum number of samples to filter at once. If filter_frame() is
     * called with fewer samples, it will accumulate them in fifo.
     * This field and the related ones must not be changed after filtering
     * has started.
     * If 0, all related fields are ignored.
     */
    int min_samples;

    /**
     * Maximum number of samples to filter at once. If filter_frame() is
     * called with more samples, it will split them.
     */
    int max_samples;

    /**
     * Number of past frames sent through the link.
     */
    int64_t frame_count_in, frame_count_out;

    /**
     * Number of past samples sent through the link.
     */
    int64_t sample_count_in, sample_count_out;

    /**
     * A pointer to a FFFramePool struct.
     */
    void *frame_pool;

    /**
     * True if a frame is currently wanted on the output of this filter.
     * Set when ff_request_frame() is called by the output,
     * cleared when a frame is filtered.
     */
    int frame_wanted_out;

    /**
     * For hwaccel pixel formats, this should be a reference to the
     * AVHWFramesContext describing the frames.
     */
    AVBufferRef *hw_frames_ctx;

#ifndef FF_INTERNAL_FIELDS

    /**
     * Internal structure members.
     * The fields below this limit are internal for libavfilter's use
     * and must in no way be accessed by applications.
     */
    char reserved[0xF000];

#else /* FF_INTERNAL_FIELDS */

    /**
     * Queue of frames waiting to be filtered.
     */
    FFFrameQueue fifo;

    /**
     * If set, the source filter can not generate a frame as is.
     * The goal is to avoid repeatedly calling the request_frame() method on
     * the same link.
     */
    int frame_blocked_in;

    /**
     * Link input status.
     * If not zero, all attempts of filter_frame will fail with the
     * corresponding code.
     */
    int status_in;

    /**
     * Timestamp of the input status change.
     */
    int64_t status_in_pts;

    /**
     * Link output status.
     * If not zero, all attempts of request_frame will fail with the
     * corresponding code.
     */
    int status_out;

#endif /* FF_INTERNAL_FIELDS */

};

struct AVFilterPad {
    /**
     * Pad name. The name is unique among inputs and among outputs, but an
     * input may have the same name as an output. This may be NULL if this
     * pad has no need to ever be referenced by name.
     */
    const char *name;

    /**
     * AVFilterPad type.
     */
    enum AVMediaType type;

    /**
     * The filter expects writable frames from its input link,
     * duplicating data buffers if needed.
     *
     * input pads only.
     */
#define AVFILTERPAD_FLAG_NEEDS_WRITABLE                  (1 << 0)

    /**
     * The pad's name is allocated and should be freed generically.
     */
#define AVFILTERPAD_FLAG_FREE_NAME                       (1 << 1)

    /**
     * A combination of AVFILTERPAD_FLAG_* flags.
     */
    int flags;

    /**
     * Callback functions to get a video/audio buffers. If NULL,
     * the filter system will use ff_default_get_video_buffer() for video
     * and ff_default_get_audio_buffer() for audio.
     *
     * The state of the union is determined by type.
     *
     * Input pads only.
     */
    // union {
    //     AVFrame *(*video)(AVFilterLink *link, int w, int h);
    //     AVFrame *(*audio)(AVFilterLink *link, int nb_samples);
    // } get_buffer;

    /**
     * Filtering callback. This is where a filter receives a frame with
     * audio/video data and should do its processing.
     *
     * Input pads only.
     *
     * @return >= 0 on success, a negative AVERROR on error. This function
     * must ensure that frame is properly unreferenced on error if it
     * hasn't been passed on to another filter.
     */
    int (*filter_frame)(AVFilterLink *link, AVFrame *frame);

    /**
     * Frame request callback. A call to this should result in some progress
     * towards producing output over the given link. This should return zero
     * on success, and another value on error.
     *
     * Output pads only.
     */
    int (*request_frame)(AVFilterLink *link);

    /**
     * Link configuration callback.
     *
     * For output pads, this should set the link properties such as
     * width/height. This should NOT set the format property - that is
     * negotiated between filters by the filter system using the
     * query_formats() callback before this function is called.
     *
     * For input pads, this should check the properties of the link, and update
     * the filter's internal state as necessary.
     *
     * For both input and output filters, this should return zero on success,
     * and another value on error.
     */
    int (*config_props)(AVFilterLink *link);
};

struct AVFilterFormats {
    unsigned nb_formats;        ///< number of formats
    int *formats;               ///< list of media formats

    unsigned refcount;          ///< number of references to this list
    struct AVFilterFormats ***refs; ///< references to this list
};

struct AVFilterChannelLayouts {
    AVChannelLayout *channel_layouts; ///< list of channel layouts
    int    nb_channel_layouts;  ///< number of channel layouts
    char all_layouts;           ///< accept any known channel layout
    char all_counts;            ///< accept any channel layout or count

    unsigned refcount;          ///< number of references to this list
    struct AVFilterChannelLayouts ***refs; ///< references to this list
};

typedef struct FFDrawContext {
    const struct AVPixFmtDescriptor *desc;
    enum AVPixelFormat format;
    unsigned nb_planes;
    int pixelstep[MAX_PLANES]; /*< offset between pixels */
    uint8_t hsub[MAX_PLANES];  /*< horizontal subsampling */
    uint8_t vsub[MAX_PLANES];  /*< vertical subsampling */
    uint8_t hsub_max;
    uint8_t vsub_max;
    enum AVColorRange range;
    unsigned flags;
    enum AVColorSpace csp;
    double rgb2yuv[3][3];
} FFDrawContext;

typedef struct FFDrawColor {
    uint8_t rgba[4];
    union {
        uint32_t u32[4];
        uint16_t u16[8];
        uint8_t  u8[16];
    } comp[MAX_PLANES];
} FFDrawColor;

typedef struct PadContext {
    const AVClass *avclass;
    int w, h;               ///< output dimensions, a value of 0 will result in the input size
    int x, y;               ///< offsets of the input area with respect to the padded area
    int in_w, in_h;         ///< width and height for the padded input video, which has to be aligned to the chroma values in order to avoid chroma issues
    int inlink_w, inlink_h;
    AVRational aspect;

    char *w_expr;           ///< width  expression string
    char *h_expr;           ///< height expression string
    char *x_expr;           ///< width  expression string
    char *y_expr;           ///< height expression string
    uint8_t rgba_color[4];  ///< color for the padding area
    FFDrawContext draw;
    FFDrawColor color;

    int eval_mode;          ///< expression evaluation mode
} PadContext;

#define FF_INLINK_IDX(link)  ((int)((link)->dstpad - (link)->dst->input_pads))
#define FF_OUTLINK_IDX(link) ((int)((link)->srcpad - (link)->src->output_pads))

const AVPixFmtDescriptor *av_pix_fmt_desc_get(enum AVPixelFormat pix_fmt)
{
    if (pix_fmt < 0 || pix_fmt >= AV_PIX_FMT_NB)
        return NULL;
    return &av_pix_fmt_descriptors[pix_fmt];
}

static int get_video_buffer(AVFrame *frame, int align)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(frame->format);
    int ret, i, padded_height, total_size;
    int plane_padding = FFMAX(16 + 16/*STRIDE_ALIGN*/, align);
    ptrdiff_t linesizes[4];
    size_t sizes[4];

    if (!desc)
        return AVERROR(EINVAL);

    if ((ret = av_image_check_size(frame->width, frame->height, 0, NULL)) < 0)
        return ret;

    if (!frame->linesize[0]) {
        if (align <= 0)
            align = 32; /* STRIDE_ALIGN. Should be av_cpu_max_align() */

        for(i=1; i<=align; i+=i) {
            ret = av_image_fill_linesizes(frame->linesize, frame->format,
                                          FFALIGN(frame->width, i));
            if (ret < 0)
                return ret;
            if (!(frame->linesize[0] & (align-1)))
                break;
        }

        for (i = 0; i < 4 && frame->linesize[i]; i++)
            frame->linesize[i] = FFALIGN(frame->linesize[i], align);
    }

    for (i = 0; i < 4; i++)
        linesizes[i] = frame->linesize[i];

    padded_height = FFALIGN(frame->height, 32);
    if ((ret = av_image_fill_plane_sizes(sizes, frame->format,
                                         padded_height, linesizes)) < 0)
        return ret;

    total_size = 4*plane_padding;
    for (i = 0; i < 4; i++) {
        if (sizes[i] > INT_MAX - total_size)
            return AVERROR(EINVAL);
        total_size += sizes[i];
    }

    frame->buf[0] = av_buffer_alloc(total_size);
    if (!frame->buf[0]) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    if ((ret = av_image_fill_pointers(frame->data, frame->format, padded_height,
                                      frame->buf[0]->data, frame->linesize)) < 0)
        goto fail;

    for (i = 1; i < 4; i++) {
        if (frame->data[i])
            frame->data[i] += i * plane_padding;
    }

    frame->extended_data = frame->data;

    return 0;
fail:
    av_frame_unref(frame);
    return ret;
}

AVFrame *ff_get_video_buffer(AVFilterLink *link, int w, int h)
{
    AVFrame *ret = NULL;

    FF_TPRINTF_START(NULL, get_video_buffer); ff_tlog_link(NULL, link, 1);

    if (link->dstpad->get_buffer.video)
        ret = link->dstpad->get_buffer.video(link, w, h);

    if (!ret)
        ret = ff_default_get_video_buffer(link, w, h);

    return ret;
}

static AVFrame *get_video_buffer(AVFilterLink *inlink, int w, int h)
{
    AVFilterContext *ctx = inlink->dst;
    unsigned in_no = FF_INLINK_IDX(inlink);
    AVFilterLink *outlink = ctx->outputs[in_no % ctx->nb_outputs];

    return ff_get_video_buffer(outlink, w, h);
}

static AVFrame *ff_get_video_buffer(AVFilterLink *link, int w, int h)
{
    AVFrame *ret = NULL;

    FF_TPRINTF_START(NULL, get_video_buffer); ff_tlog_link(NULL, link, 1);

    if (link->dstpad->get_buffer.video)
        ret = link->dstpad->get_buffer.video(link, w, h);

    if (!ret)
        ret = ff_default_get_video_buffer(link, w, h);

    return ret;
}

static AVFrame *get_video_buffer(AVFilterLink *inlink, int w, int h)
{
    PadContext *s = inlink->dst->priv;
    AVFrame *frame;
    int plane;

    if (s->inlink_w <= 0)
        return NULL;

    frame = ff_get_video_buffer(inlink->dst->outputs[0],
                                w + (s->w - s->in_w),
                                h + (s->h - s->in_h) + (s->x > 0));

    if (!frame)
        return NULL;

    frame->width  = w;
    frame->height = h;

    for (plane = 0; plane < 4 && frame->data[plane] && frame->linesize[plane]; plane++) {
        int hsub = s->draw.hsub[plane];
        int vsub = s->draw.vsub[plane];
        frame->data[plane] += (s->x >> hsub) * s->draw.pixelstep[plane] +
                              (s->y >> vsub) * frame->linesize[plane];
    }

    return frame;
}

static int av_frame_get_buffer(AVFrame *frame, int align)
{
    if (frame->format < 0)
        return AVERROR(EINVAL);

    FF_DISABLE_DEPRECATION_WARNINGS
    if (frame->width > 0 && frame->height > 0)
        return get_video_buffer(frame, align);
    else if (frame->nb_samples > 0 &&
             (av_channel_layout_check(&frame->ch_layout)
#if FF_API_OLD_CHANNEL_LAYOUT
              || frame->channel_layout || frame->channels > 0
#endif
              ))
        return get_audio_buffer(frame, align);
    FF_ENABLE_DEPRECATION_WARNINGS

    return AVERROR(EINVAL);
}

static int av_frame_ref(AVFrame *dst, const AVFrame *src)
{
    int i, ret = 0;

    av_assert1(dst->width == 0 && dst->height == 0);
#if FF_API_OLD_CHANNEL_LAYOUT
    FF_DISABLE_DEPRECATION_WARNINGS
    av_assert1(dst->channels == 0);
    FF_ENABLE_DEPRECATION_WARNINGS
#endif
    av_assert1(dst->ch_layout.nb_channels == 0 &&
               dst->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC);

    dst->format = src->format;
    dst->width = src->width;
    dst->height = src->height;
    dst->nb_samples = src->nb_samples;
#if FF_API_OLD_CHANNEL_LAYOUT
    FF_DISABLE_DEPRECATION_WARNINGS
    dst->channels = src->channels;
    dst->channel_layout = src->channel_layout;
    if (!av_channel_layout_check(&src->ch_layout))
    {
        if (src->channel_layout)
            av_channel_layout_from_mask(&dst->ch_layout, src->channel_layout);
        else
        {
            dst->ch_layout.nb_channels = src->channels;
            dst->ch_layout.order = AV_CHANNEL_ORDER_UNSPEC;
        }
    }
    FF_ENABLE_DEPRECATION_WARNINGS
#endif

    ret = frame_copy_props(dst, src, 0);
    if (ret < 0)
        goto fail;

    // this check is needed only until FF_API_OLD_CHANNEL_LAYOUT is out
    if (av_channel_layout_check(&src->ch_layout))
    {
        ret = av_channel_layout_copy(&dst->ch_layout, &src->ch_layout);
        if (ret < 0)
            goto fail;
    }

    /* duplicate the frame data if it's not refcounted */
    if (!src->buf[0])
    {
        ret = av_frame_get_buffer(dst, 0);
        if (ret < 0)
            goto fail;

        ret = av_frame_copy(dst, src);
        if (ret < 0)
            goto fail;

        return 0;
    }

    /* ref the buffers */
    for (i = 0; i < FF_ARRAY_ELEMS(src->buf); i++)
    {
        if (!src->buf[i])
            continue;
        dst->buf[i] = av_buffer_ref(src->buf[i]);
        if (!dst->buf[i])
        {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }

    if (src->extended_buf)
    {
        dst->extended_buf = av_calloc(src->nb_extended_buf,
                                      sizeof(*dst->extended_buf));
        if (!dst->extended_buf)
        {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        dst->nb_extended_buf = src->nb_extended_buf;

        for (i = 0; i < src->nb_extended_buf; i++)
        {
            dst->extended_buf[i] = av_buffer_ref(src->extended_buf[i]);
            if (!dst->extended_buf[i])
            {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
        }
    }

    if (src->hw_frames_ctx)
    {
        dst->hw_frames_ctx = av_buffer_ref(src->hw_frames_ctx);
        if (!dst->hw_frames_ctx)
        {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }

    /* duplicate extended data */
    if (src->extended_data != src->data)
    {
        int ch = dst->ch_layout.nb_channels;

        if (!ch)
        {
            ret = AVERROR(EINVAL);
            goto fail;
        }

        dst->extended_data = av_malloc_array(sizeof(*dst->extended_data), ch);
        if (!dst->extended_data)
        {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        memcpy(dst->extended_data, src->extended_data, sizeof(*src->extended_data) * ch);
    }
    else
        dst->extended_data = dst->data;

    memcpy(dst->data, src->data, sizeof(src->data));
    memcpy(dst->linesize, src->linesize, sizeof(src->linesize));

    return 0;

fail:
    av_frame_unref(dst);
    return ret;
}

typedef struct cvid_strip
{
    uint16_t id;
    uint16_t x1, y1;
    uint16_t x2, y2;
    cvid_codebook v4_codebook[256];
    cvid_codebook v1_codebook[256];
} cvid_strip;

typedef struct CinepakContext
{

    // AVCodecContext *avctx;
    int discard_damaged_percentage;
    int bits_per_coded_sample;
    enum AVPixelFormat pix_fmt;

    AVFrame *frame;

    const unsigned char *data;
    int size;

    int width, height;

    int palette_video;
    cvid_strip strips[MAX_STRIPS];

    int sega_film_skip_bytes;

    uint32_t pal[256];
} CinepakContext;

static void cinepak_decode_codebook(cvid_codebook *codebook,
                                    int chunk_id, int size, const uint8_t *data)
{
    const uint8_t *eod = (data + size);
    uint32_t flag, mask;
    int i, n;
    uint8_t *p;

    /* check if this chunk contains 4- or 6-element vectors */
    n = (chunk_id & 0x04) ? 4 : 6;
    flag = 0;
    mask = 0;

    p = codebook[0];
    for (i = 0; i < 256; i++)
    {
        if ((chunk_id & 0x01) && !(mask >>= 1))
        {
            if ((data + 4) > eod)
                break;

            flag = AV_RB32(data);
            data += 4;
            mask = 0x80000000;
        }

        if (!(chunk_id & 0x01) || (flag & mask))
        {
            int k, kk;

            if ((data + n) > eod)
                break;

            for (k = 0; k < 4; ++k)
            {
                int r = *data++;
                for (kk = 0; kk < 3; ++kk)
                    *p++ = r;
            }
            if (n == 6)
            {
                int r, g, b, u, v;
                u = *(int8_t *)data++;
                v = *(int8_t *)data++;
                p -= 12;
                for (k = 0; k < 4; ++k)
                {
                    r = *p++ + v * 2;
                    g = *p++ - (u / 2) - v;
                    b = *p + u * 2;
                    p -= 2;
                    *p++ = av_clip_uint8(r);
                    *p++ = av_clip_uint8(g);
                    *p++ = av_clip_uint8(b);
                }
            }
        }
        else
        {
            p += 12;
        }
    }
}

static int cinepak_decode_vectors(CinepakContext *s, cvid_strip *strip,
                                  int chunk_id, int size, const uint8_t *data)
{
    const uint8_t *eod = (data + size);
    uint32_t flag, mask;
    uint8_t *cb0, *cb1, *cb2, *cb3;
    int x, y;
    uint8_t *ip0, *ip1, *ip2, *ip3;

    flag = 0;
    mask = 0;

    for (y = strip->y1; y < strip->y2; y += 4)
    {
        /* take care of y dimension not being multiple of 4, such streams exist */
        ip0 = ip1 = ip2 = ip3 = s->frame->data[0] +
                                (s->palette_video ? strip->x1 : strip->x1 * 3) + (y * s->frame->linesize[0]);
        if (s->height - y > 1)
        {
            ip1 = ip0 + s->frame->linesize[0];
            if (s->height - y > 2)
            {
                ip2 = ip1 + s->frame->linesize[0];
                if (s->height - y > 3)
                {
                    ip3 = ip2 + s->frame->linesize[0];
                }
            }
        }
        /* to get the correct picture for not-multiple-of-4 cases let us fill each
         * block from the bottom up, thus possibly overwriting the bottommost line
         * more than once but ending with the correct data in place
         * (instead of in-loop checking) */

        for (x = strip->x1; x < strip->x2; x += 4)
        {
            if ((chunk_id & 0x01) && !(mask >>= 1))
            {
                if ((data + 4) > eod)
                    return AVERROR_INVALIDDATA;

                flag = AV_RB32(data);
                data += 4;
                mask = 0x80000000;
            }

            if (!(chunk_id & 0x01) || (flag & mask))
            {
                if (!(chunk_id & 0x02) && !(mask >>= 1))
                {
                    if ((data + 4) > eod)
                        return AVERROR_INVALIDDATA;

                    flag = AV_RB32(data);
                    data += 4;
                    mask = 0x80000000;
                }

                if ((chunk_id & 0x02) || (~flag & mask))
                {
                    uint8_t *p;
                    if (data >= eod)
                        return AVERROR_INVALIDDATA;

                    p = strip->v1_codebook[*data++];
                    if (s->palette_video)
                    {
                        ip3[0] = ip3[1] = ip2[0] = ip2[1] = p[6];
                        ip3[2] = ip3[3] = ip2[2] = ip2[3] = p[9];
                        ip1[0] = ip1[1] = ip0[0] = ip0[1] = p[0];
                        ip1[2] = ip1[3] = ip0[2] = ip0[3] = p[3];
                    }
                    else
                    {
                        p += 6;
                        memcpy(ip3 + 0, p, 3);
                        memcpy(ip3 + 3, p, 3);
                        memcpy(ip2 + 0, p, 3);
                        memcpy(ip2 + 3, p, 3);
                        p += 3; /* ... + 9 */
                        memcpy(ip3 + 6, p, 3);
                        memcpy(ip3 + 9, p, 3);
                        memcpy(ip2 + 6, p, 3);
                        memcpy(ip2 + 9, p, 3);
                        p -= 9; /* ... + 0 */
                        memcpy(ip1 + 0, p, 3);
                        memcpy(ip1 + 3, p, 3);
                        memcpy(ip0 + 0, p, 3);
                        memcpy(ip0 + 3, p, 3);
                        p += 3; /* ... + 3 */
                        memcpy(ip1 + 6, p, 3);
                        memcpy(ip1 + 9, p, 3);
                        memcpy(ip0 + 6, p, 3);
                        memcpy(ip0 + 9, p, 3);
                    }
                }
                else if (flag & mask)
                {
                    if ((data + 4) > eod)
                        return AVERROR_INVALIDDATA;

                    cb0 = strip->v4_codebook[*data++];
                    cb1 = strip->v4_codebook[*data++];
                    cb2 = strip->v4_codebook[*data++];
                    cb3 = strip->v4_codebook[*data++];
                    if (s->palette_video)
                    {
                        uint8_t *p;
                        p = ip3;
                        *p++ = cb2[6];
                        *p++ = cb2[9];
                        *p++ = cb3[6];
                        *p = cb3[9];
                        p = ip2;
                        *p++ = cb2[0];
                        *p++ = cb2[3];
                        *p++ = cb3[0];
                        *p = cb3[3];
                        p = ip1;
                        *p++ = cb0[6];
                        *p++ = cb0[9];
                        *p++ = cb1[6];
                        *p = cb1[9];
                        p = ip0;
                        *p++ = cb0[0];
                        *p++ = cb0[3];
                        *p++ = cb1[0];
                        *p = cb1[3];
                    }
                    else
                    {
                        memcpy(ip3 + 0, cb2 + 6, 6);
                        memcpy(ip3 + 6, cb3 + 6, 6);
                        memcpy(ip2 + 0, cb2 + 0, 6);
                        memcpy(ip2 + 6, cb3 + 0, 6);
                        memcpy(ip1 + 0, cb0 + 6, 6);
                        memcpy(ip1 + 6, cb1 + 6, 6);
                        memcpy(ip0 + 0, cb0 + 0, 6);
                        memcpy(ip0 + 6, cb1 + 0, 6);
                    }
                }
            }

            if (s->palette_video)
            {
                ip0 += 4;
                ip1 += 4;
                ip2 += 4;
                ip3 += 4;
            }
            else
            {
                ip0 += 12;
                ip1 += 12;
                ip2 += 12;
                ip3 += 12;
            }
        }
    }

    return 0;
}

static int cinepak_decode_strip(CinepakContext *s,
                                cvid_strip *strip, const uint8_t *data, int size)
{
    const uint8_t *eod = (data + size);
    int chunk_id, chunk_size;

    /* coordinate sanity checks */
    if (strip->x2 > s->width ||
        strip->y2 > s->height ||
        strip->x1 >= strip->x2 || strip->y1 >= strip->y2)
        return AVERROR_INVALIDDATA;

    while ((data + 4) <= eod)
    {
        chunk_id = data[0];
        chunk_size = AV_RB24(&data[1]) - 4;
        if (chunk_size < 0)
            return AVERROR_INVALIDDATA;

        data += 4;
        chunk_size = ((data + chunk_size) > eod) ? (eod - data) : chunk_size;

        switch (chunk_id)
        {

        case 0x20:
        case 0x21:
        case 0x24:
        case 0x25:
            cinepak_decode_codebook(strip->v4_codebook, chunk_id,
                                    chunk_size, data);
            break;

        case 0x22:
        case 0x23:
        case 0x26:
        case 0x27:
            cinepak_decode_codebook(strip->v1_codebook, chunk_id,
                                    chunk_size, data);
            break;

        case 0x30:
        case 0x31:
        case 0x32:
            return cinepak_decode_vectors(s, strip, chunk_id,
                                          chunk_size, data);
        }

        data += chunk_size;
    }

    return AVERROR_INVALIDDATA;
}

static int cinepak_predecode_check(CinepakContext *s)
{
    int num_strips;
    int encoded_buf_size;

    num_strips = AV_RB16(&s->data[8]);
    encoded_buf_size = AV_RB24(&s->data[1]);

    if (s->size < encoded_buf_size * (int64_t)(100 - s->discard_damaged_percentage) / 100)
        return AVERROR_INVALIDDATA;

    /* if this is the first frame, check for deviant Sega FILM data */
    if (s->sega_film_skip_bytes == -1)
    {
        if (!encoded_buf_size)
        {
            avpriv_request_sample(s, "encoded_buf_size 0");
            return AVERROR_PATCHWELCOME;
        }
        if (encoded_buf_size != s->size && (s->size % encoded_buf_size) != 0)
        {
            /* If the encoded frame size differs from the frame size as indicated
             * by the container file, this data likely comes from a Sega FILM/CPK file.
             * If the frame header is followed by the bytes FE 00 00 06 00 00 then
             * this is probably one of the two known files that have 6 extra bytes
             * after the frame header. Else, assume 2 extra bytes. The container
             * size also cannot be a multiple of the encoded size. */
            if (s->size >= 16 &&
                (s->data[10] == 0xFE) &&
                (s->data[11] == 0x00) &&
                (s->data[12] == 0x00) &&
                (s->data[13] == 0x06) &&
                (s->data[14] == 0x00) &&
                (s->data[15] == 0x00))
                s->sega_film_skip_bytes = 6;
            else
                s->sega_film_skip_bytes = 2;
        }
        else
            s->sega_film_skip_bytes = 0;
    }

    if (s->size < 10 + s->sega_film_skip_bytes + num_strips * 12)
        return AVERROR_INVALIDDATA;

    if (num_strips)
    {
        const uint8_t *data = s->data + 10 + s->sega_film_skip_bytes;
        int strip_size = AV_RB24(data + 1);
        if (strip_size < 12 || strip_size > encoded_buf_size)
            return AVERROR_INVALIDDATA;
    }

    return 0;
}

static int cinepak_decode(CinepakContext *s)
{
    const uint8_t *eod = (s->data + s->size);
    int i, result, strip_size, frame_flags, num_strips;
    int y0 = 0;

    frame_flags = s->data[0];
    num_strips = AV_RB16(&s->data[8]);

    s->data += 10 + s->sega_film_skip_bytes;

    num_strips = FFMIN(num_strips, MAX_STRIPS);

    s->frame->key_frame = 0;

    for (i = 0; i < num_strips; i++)
    {
        if ((s->data + 12) > eod)
            return AVERROR_INVALIDDATA;

        s->strips[i].id = s->data[0];
        /* zero y1 means "relative to the previous stripe" */
        if (!(s->strips[i].y1 = AV_RB16(&s->data[4])))
            s->strips[i].y2 = (s->strips[i].y1 = y0) + AV_RB16(&s->data[8]);
        else
            s->strips[i].y2 = AV_RB16(&s->data[8]);
        s->strips[i].x1 = AV_RB16(&s->data[6]);
        s->strips[i].x2 = AV_RB16(&s->data[10]);

        if (s->strips[i].id == 0x10)
            s->frame->key_frame = 1;

        strip_size = AV_RB24(&s->data[1]) - 12;
        if (strip_size < 0)
            return AVERROR_INVALIDDATA;
        s->data += 12;
        strip_size = ((s->data + strip_size) > eod) ? (eod - s->data) : strip_size;

        if ((i > 0) && !(frame_flags & 0x01))
        {
            memcpy(s->strips[i].v4_codebook, s->strips[i - 1].v4_codebook,
                   sizeof(s->strips[i].v4_codebook));
            memcpy(s->strips[i].v1_codebook, s->strips[i - 1].v1_codebook,
                   sizeof(s->strips[i].v1_codebook));
        }

        result = cinepak_decode_strip(s, &s->strips[i], s->data, strip_size);

        if (result != 0)
            return result;

        s->data += strip_size;
        y0 = s->strips[i].y2;
    }
    return 0;
}

static av_cold int cinepak_decode_init(CinepakContext *s)
{
    // s->width = (avctx->width + 3) & ~3;
    // s->height = (avctx->height + 3) & ~3;

    s->sega_film_skip_bytes = -1; /* uninitialized state */

    // check for paletted data
    if (s->bits_per_coded_sample != 8)
    {
        s->palette_video = 0;
        s->pix_fmt = AV_PIX_FMT_RGB24;
    }
    else
    {
        s->palette_video = 1;
        s->pix_fmt = AV_PIX_FMT_PAL8;
    }

    s->frame = av_frame_alloc();
    if (!s->frame)
        return AVERROR(ENOMEM);

    return 0;
}

static int cinepak_decode_frame(CinepakContext *s, AVFrame *rframe,
                                int *got_frame, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int ret = 0, buf_size = avpkt->size;
    int num_strips;

    s->data = buf;
    s->size = buf_size;

    if (s->size < 10)
        return AVERROR_INVALIDDATA;

    num_strips = AV_RB16(&s->data[8]);

    // Empty frame, do not waste time
    if (!num_strips && (!s->palette_video || !av_packet_get_side_data(avpkt, AV_PKT_DATA_PALETTE, NULL)))
        return buf_size;

    if ((ret = cinepak_predecode_check(s)) < 0)
    {
        av_log(s, AV_LOG_ERROR, "cinepak_predecode_check failed\n");
        return ret;
    }

    // if ((ret = ff_reget_buffer(s, s->frame, 0)) < 0)
    //     return ret;

    if (s->palette_video)
    {
        s->frame->palette_has_changed = ff_copy_palette(s->pal, avpkt, s);
    }

    if ((ret = cinepak_decode(s)) < 0)
    {
        av_log(s, AV_LOG_ERROR, "cinepak_decode failed\n");
    }

    if (s->palette_video)
        memcpy(s->frame->data[1], s->pal, AVPALETTE_SIZE);

    if ((ret = av_frame_ref(rframe, s->frame)) < 0)
        return ret;

    *got_frame = 1;

    /* report that the buffer was completely consumed */
    return buf_size;
}

#endif // _CINEPAK_H_
