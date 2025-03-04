/*
 *  avilib.h
 *
 *  Copyright (C) Thomas streich - June 2001
 *  multiple audio track support Copyright (C) 2002 Thomas streich
 *
 *  Original code:
 *  Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de>
 *
 *  This file is part of transcode, a linux video stream processing tool
 *
 *  transcode is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  transcode is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <inttypes.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef AVILIB_H
#define AVILIB_H

#define AVI_MAX_TRACKS 8

typedef struct __attribute__((packed))
{
   off_t pos;
   uint16_t len;
   uint8_t key;
} video_index_entry;

typedef struct __attribute__((packed))
{
   off_t pos;
   uint16_t len;
   off_t tot;
} audio_index_entry;

typedef struct __attribute__((packed)) track_s
{
   long a_fmt;   /* Audio format, see #defines below */
   long a_chans; /* Audio channels, 0 for no audio */
   long a_rate;  /* Rate in Hz */
   long a_bits;  /* bits per audio sample */
   long mp3rate; /* mp3 bitrate kbs*/

   long audio_strn;   /* Audio stream number */
   off_t audio_bytes; /* Total number of bytes of audio data */
   long audio_chunks; /* Chunks of audio data in the file */

   char audio_tag[4]; /* Tag of audio data */
   long audio_posc;   /* Audio position: chunk */
   long audio_posb;   /* Audio position: byte within chunk */

   off_t a_codech_off; /* absolut offset of audio codec information */
   off_t a_codecf_off; /* absolut offset of audio codec information */

   audio_index_entry *audio_index;
} track_t;

typedef struct __attribute__((packed))
{
   uint32_t bi_size;
   uint32_t bi_width;
   uint32_t bi_height;
   uint16_t bi_planes;
   uint16_t bi_bit_count;
   uint32_t bi_compression;
   uint32_t bi_size_image;
   uint32_t bi_x_pels_per_meter;
   uint32_t bi_y_pels_per_meter;
   uint32_t bi_clr_used;
   uint32_t bi_clr_important;
} BITMAPINFOHEADER_avilib;

typedef struct __attribute__((packed))
{
   uint16_t w_format_tag;
   uint16_t n_channels;
   uint32_t n_samples_per_sec;
   uint32_t n_avg_bytes_per_sec;
   uint16_t n_block_align;
   uint16_t w_bits_per_sample;
   uint16_t cb_size;
} WAVEFORMATEX_avilib;

typedef struct __attribute__((packed))
{

   long fdes; /* File descriptor of AVI file */
   long mode; /* 0 for reading, 1 for writing */

   long width;          /* Width  of a video frame */
   long height;         /* Height of a video frame */
   double fps;          /* Frames per second */
   char compressor[8];  /* Type of compressor, 4 bytes + padding for 0 byte */
   char compressor2[8]; /* Type of compressor, 4 bytes + padding for 0 byte */
   long video_strn;     /* Video stream number */
   long video_frames;   /* Number of video frames */
   char video_tag[4];   /* Tag of video data */
   long video_pos;      /* Number of next frame to be read
               (if index present) */

   unsigned long max_len; /* maximum video chunk present */

   track_t track[AVI_MAX_TRACKS]; // up to AVI_MAX_TRACKS audio tracks supported

   off_t pos;    /* position in file */
   long n_idx;   /* number of index entries actually filled */
   long max_idx; /* number of index entries actually allocated */

   off_t v_codech_off; /* absolut offset of video codec (strh) info */
   off_t v_codecf_off; /* absolut offset of video codec (strf) info */

   video_index_entry *video_index;

   off_t last_pos;         /* Position of last frame written */
   unsigned long last_len; /* Length of last frame written */
   int must_use_index;     /* Flag if frames are duplicated */
   off_t movi_start;
   off_t idx1_start;

   int anum; // total number of audio tracks
   int aptr; // current audio working track

   BITMAPINFOHEADER_avilib *bitmap_info_header;
   WAVEFORMATEX_avilib *wave_format_ex[AVI_MAX_TRACKS];
} avi_t;

#define AVI_MODE_WRITE 0
#define AVI_MODE_READ 1

/* The error codes delivered by avi_open_input_file */

#define AVI_ERR_OPEN 2 /* Error opening the AVI file - wrong path \
                          name or file nor readable/writable */

#define AVI_ERR_READ 3 /* Error reading from AVI File */

#define AVI_ERR_WRITE 4 /* Error writing to AVI File, \
                           disk full ??? */

#define AVI_ERR_WRITE_INDEX 5 /* Could not write index to AVI file \
                                 during close, file may still be   \
                                 usable */

#define AVI_ERR_CLOSE 6 /* Could not write header to AVI file     \
                           or not truncate the file during close, \
                           file is most probably corrupted */

#define AVI_ERR_NOT_PERM 7 /* Operation not permitted:        \
                              trying to read from a file open \
                              for writing or vice versa */

#define AVI_ERR_NO_MEM 8 /* malloc failed */

#define AVI_ERR_NO_AVI 9 /* Not an AVI file */

#define AVI_ERR_NO_HDRL 10 /* AVI file has no has no header list, \
                              corrupted ??? */

#define AVI_ERR_NO_MOVI 11 /* AVI file has no has no MOVI list, \
                              corrupted ??? */

#define AVI_ERR_NO_VIDS 12 /* AVI file contains no video data */

#define AVI_ERR_NO_IDX 13 /* The file has been opened with          \
                             getIndex==0, but an operation has been \
                             performed that needs an index */

int AVI_close(avi_t *AVI)
{
   int ret, i;

   /* If the file was open for writing, the header and index still have
      to be written */

   // if(AVI->mode == AVI_MODE_WRITE)
   //    ret = avi_close_output_file(AVI);
   // else
   ret = 0;

   /* Even if there happened an error, we first clean up */

   close(AVI->fdes);
   if (AVI->video_index)
      free(AVI->video_index);
   // FIXME
   // if(AVI->audio_index) free(AVI->audio_index);
   if (AVI->bitmap_info_header)
      free(AVI->bitmap_info_header);
   for (i = 0; i < AVI->anum; i++)
   {
      if (AVI->wave_format_ex[i])
         free(AVI->wave_format_ex[i]);
      if (AVI->track[i].audio_chunks)
         free(AVI->track[i].audio_index);
   }
   free(AVI);

   return ret;
}

#define ERR_EXIT(x)   \
   {                  \
      AVI_close(AVI); \
      AVI_errno = x;  \
      return 0;       \
   }

/* The following variable indicates the kind of error */

long AVI_errno = 0;

#define MAX_INFO_STRLEN 64
static char id_str[MAX_INFO_STRLEN];

/* HEADERBYTES: The number of bytes to reserve for the header */

#define HEADERBYTES 2048

/* AVI_MAX_LEN: The maximum length of an AVI file, we stay a bit below
    the 2GB limit (Remember: 2*10^9 is smaller than 2 GB) */

#define AVI_MAX_LEN (UINT_MAX - (1 << 20) * 16 - HEADERBYTES)

#define PAD_EVEN(x) (((x) + 1) & ~1)

/* Copy n into dst as a 4 byte, little endian number.
   Should also work on big endian machines */

static void long2str(unsigned char *dst, int n)
{
   dst[0] = (n) & 0xff;
   dst[1] = (n >> 8) & 0xff;
   dst[2] = (n >> 16) & 0xff;
   dst[3] = (n >> 24) & 0xff;
}

/* Convert a string of 4 or 2 bytes to a number,
   also working on big endian machines */

static unsigned long str2ulong(unsigned char *str)
{
   return (str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24));
}
static unsigned long str2ushort(unsigned char *str)
{
   return (str[0] | (str[1] << 8));
}

/* Calculate audio sample size from number of bits and number of channels.
   This may have to be adjusted for eg. 12 bits and stereo */

static int avi_sampsize(avi_t *AVI, int j)
{
   int s;
   s = ((AVI->track[j].a_bits + 7) / 8) * AVI->track[j].a_chans;
   //   if(s==0) s=1; /* avoid possible zero divisions */
   if (s < 4)
      s = 4; /* avoid possible zero divisions */
   return s;
}

static size_t avi_read(int fd, char *buf, size_t len)
{
   size_t n = 0;
   size_t r = 0;

   while (r < len)
   {
      n = read(fd, buf + r, len - r);

      if ((ssize_t)n <= 0)
         return r;
      r += n;
   }

   return r;
}

int avi_parse_input_file(avi_t *AVI, int getIndex)
{
   long i, rate, scale, idx_type;
   off_t n;
   unsigned char *hdrl_data;
   long header_offset = 0, hdrl_len = 0;
   long nvi, nai[AVI_MAX_TRACKS], ioff;
   long tot[AVI_MAX_TRACKS];
   int j;
   int lasttag = 0;
   int vids_strh_seen = 0;
   int vids_strf_seen = 0;
   int auds_strh_seen = 0;
   //  int auds_strf_seen = 0;
   int num_stream = 0;
   char data[256];

   /* Read first 12 bytes and check that this is an AVI file */

   if (avi_read(AVI->fdes, data, 12) != 12)
      ERR_EXIT(AVI_ERR_READ)

   if (strncasecmp(data, "RIFF", 4) != 0 ||
       strncasecmp(data + 8, "AVI ", 4) != 0)
      ERR_EXIT(AVI_ERR_NO_AVI)

   /* Go through the AVI file and extract the header list,
      the start position of the 'movi' list and an optionally
      present idx1 tag */

   hdrl_data = 0;

   while (1)
   {
      if (avi_read(AVI->fdes, data, 8) != 8)
         break; /* We assume it's EOF */

      n = str2ulong((unsigned char *)data + 4);
      n = PAD_EVEN(n);

      if (strncasecmp(data, "LIST", 4) == 0)
      {
         if (avi_read(AVI->fdes, data, 4) != 4)
            ERR_EXIT(AVI_ERR_READ)
         n -= 4;
         if (strncasecmp(data, "hdrl", 4) == 0)
         {
            hdrl_len = n;
            log_i("malloc(hdrl_len): %d, free PSRAM: %d", hdrl_len, heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
            hdrl_data = (unsigned char *)malloc(n);
            if (hdrl_data == 0)
               ERR_EXIT(AVI_ERR_NO_MEM);

            // offset of header

            header_offset = lseek(AVI->fdes, 0, SEEK_CUR);

            if (avi_read(AVI->fdes, (char *)hdrl_data, n) != n)
               ERR_EXIT(AVI_ERR_READ)
         }
         else if (strncasecmp(data, "movi", 4) == 0)
         {
            AVI->movi_start = lseek(AVI->fdes, 0, SEEK_CUR);
            lseek(AVI->fdes, n, SEEK_CUR);
         }
         else
            lseek(AVI->fdes, n, SEEK_CUR);
      }
      else if (strncasecmp(data, "idx1", 4) == 0)
      {
         /* n must be a multiple of 16, but the reading does not
            break if this is not the case */

         AVI->n_idx = AVI->max_idx = n / 16;
         AVI->idx1_start = lseek(AVI->fdes, 0, SEEK_CUR);
      }
      else
         lseek(AVI->fdes, n, SEEK_CUR);
   }

   if (!hdrl_data)
      ERR_EXIT(AVI_ERR_NO_HDRL)
   if (!AVI->movi_start)
      ERR_EXIT(AVI_ERR_NO_MOVI)

   /* Interpret the header list */

   for (i = 0; i < hdrl_len;)
   {
      /* List tags are completly ignored */

      if (strncasecmp((char *)hdrl_data + i, "LIST", 4) == 0)
      {
         i += 12;
         continue;
      }

      n = str2ulong(hdrl_data + i + 4);
      n = PAD_EVEN(n);

      /* Interpret the tag and its args */

      if (strncasecmp((char *)hdrl_data + i, "strh", 4) == 0)
      {
         i += 8;
         if (strncasecmp((char *)hdrl_data + i, "vids", 4) == 0 && !vids_strh_seen)
         {
            memcpy(AVI->compressor, hdrl_data + i + 4, 4);
            AVI->compressor[4] = 0;

            // ThOe
            AVI->v_codech_off = header_offset + i + 4;

            scale = str2ulong(hdrl_data + i + 20);
            rate = str2ulong(hdrl_data + i + 24);
            if (scale != 0)
               AVI->fps = (double)rate / (double)scale;
            AVI->video_frames = str2ulong(hdrl_data + i + 32);
            AVI->video_strn = num_stream;
            AVI->max_len = 0;
            vids_strh_seen = 1;
            lasttag = 1; /* vids */
         }
         else if (strncasecmp((char *)hdrl_data + i, "auds", 4) == 0 && !auds_strh_seen)
         {

            // inc audio tracks
            AVI->aptr = AVI->anum;
            ++AVI->anum;

            if (AVI->anum > AVI_MAX_TRACKS)
            {
               fprintf(stderr, "error - only %d audio tracks supported\n", AVI_MAX_TRACKS);
               return (-1);
            }

            AVI->track[AVI->aptr].audio_bytes = str2ulong(hdrl_data + i + 32) * avi_sampsize(AVI, 0);
            AVI->track[AVI->aptr].audio_strn = num_stream;
            //	   auds_strh_seen = 1;
            lasttag = 2; /* auds */

            // ThOe
            AVI->track[AVI->aptr].a_codech_off = header_offset + i;
         }
         else
            lasttag = 0;
         num_stream++;
      }
      else if (strncasecmp((char *)hdrl_data + i, "strf", 4) == 0)
      {
         i += 8;
         if (lasttag == 1)
         {
            BITMAPINFOHEADER_avilib bih;

            memcpy(&bih, hdrl_data + i, sizeof(BITMAPINFOHEADER_avilib));
            log_i("malloc(bitmap_info_header): %d, free PSRAM: %d", bih.bi_size, heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
            AVI->bitmap_info_header = (BITMAPINFOHEADER_avilib *)malloc(bih.bi_size);
            if (AVI->bitmap_info_header != NULL)
               memcpy(AVI->bitmap_info_header, hdrl_data + i, bih.bi_size);

            AVI->width = str2ulong(hdrl_data + i + 4);
            AVI->height = str2ulong(hdrl_data + i + 8);
            vids_strf_seen = 1;
            // ThOe
            AVI->v_codecf_off = header_offset + i + 16;

            memcpy(AVI->compressor2, hdrl_data + i + 16, 4);
            AVI->compressor2[4] = 0;
         }
         else if (lasttag == 2)
         {
            WAVEFORMATEX_avilib *wfe;
            char *nwfe;
            int wfes;

            if ((hdrl_len - i) < sizeof(WAVEFORMATEX_avilib))
               wfes = hdrl_len - i;
            else
               wfes = sizeof(WAVEFORMATEX_avilib);
            log_i("malloc(wfe): %d, free PSRAM: %d", sizeof(WAVEFORMATEX_avilib), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
            wfe = (WAVEFORMATEX_avilib *)malloc(sizeof(WAVEFORMATEX_avilib));
            if (wfe != NULL)
            {
               memset(wfe, 0, sizeof(WAVEFORMATEX_avilib));
               memcpy(wfe, hdrl_data + i, wfes);
               if (wfe->cb_size != 0)
               {
                  nwfe = (char *)realloc(wfe, sizeof(WAVEFORMATEX_avilib) +
                                                  wfe->cb_size);
                  if (nwfe != 0)
                  {
                     off_t lpos = lseek(AVI->fdes, 0, SEEK_CUR);
                     lseek(AVI->fdes, header_offset + i + sizeof(WAVEFORMATEX_avilib),
                           SEEK_SET);
                     wfe = (WAVEFORMATEX_avilib *)nwfe;
                     nwfe = &nwfe[sizeof(WAVEFORMATEX_avilib)];
                     avi_read(AVI->fdes, nwfe, wfe->cb_size);
                     lseek(AVI->fdes, lpos, SEEK_SET);
                  }
               }
               AVI->wave_format_ex[AVI->aptr] = wfe;
            }

            AVI->track[AVI->aptr].a_fmt = str2ushort(hdrl_data + i);

            // ThOe
            AVI->track[AVI->aptr].a_codecf_off = header_offset + i;

            AVI->track[AVI->aptr].a_chans = str2ushort(hdrl_data + i + 2);
            AVI->track[AVI->aptr].a_rate = str2ulong(hdrl_data + i + 4);
            // ThOe: read mp3bitrate
            AVI->track[AVI->aptr].mp3rate = 8 * str2ulong(hdrl_data + i + 8) / 1000;
            //: ThOe
            AVI->track[AVI->aptr].a_bits = str2ushort(hdrl_data + i + 14);
            //            auds_strf_seen = 1;
         }
         lasttag = 0;
      }
      else
      {
         i += 8;
         lasttag = 0;
      }

      i += n;
   }

   free(hdrl_data);

   if (!vids_strh_seen || !vids_strf_seen)
      ERR_EXIT(AVI_ERR_NO_VIDS)

   AVI->video_tag[0] = AVI->video_strn / 10 + '0';
   AVI->video_tag[1] = AVI->video_strn % 10 + '0';
   AVI->video_tag[2] = 'd';
   AVI->video_tag[3] = 'b';

   /* Audio tag is set to "99wb" if no audio present */
   if (!AVI->track[0].a_chans)
      AVI->track[0].audio_strn = 99;

   for (j = 0; j < AVI->anum; ++j)
   {
      AVI->track[j].audio_tag[0] = (j + 1) / 10 + '0';
      AVI->track[j].audio_tag[1] = (j + 1) % 10 + '0';
      AVI->track[j].audio_tag[2] = 'w';
      AVI->track[j].audio_tag[3] = 'b';
   }

   lseek(AVI->fdes, AVI->movi_start, SEEK_SET);

   /* get index if wanted */

   if (!getIndex)
      return (0);

   /* if the file has an idx1, check if this is relative
      to the start of the file or to the start of the movi list */

   idx_type = 0;

   off_t pos, len;
   unsigned char cur_idx[16];

   /* Search the first videoframe in the idx1 and look where
      it is in the file */

   lseek(AVI->fdes, AVI->idx1_start, SEEK_SET);
   for (i = 0; i < AVI->n_idx; i++)
   {
      avi_read(AVI->fdes, (char *)cur_idx, 16);
      if (strncasecmp((char *)cur_idx, (char *)AVI->video_tag, 3) == 0)
         break;
   }
   if (i >= AVI->n_idx)
      ERR_EXIT(AVI_ERR_NO_VIDS)

   pos = str2ulong(cur_idx + 8);
   len = str2ulong(cur_idx + 12);

   lseek(AVI->fdes, pos, SEEK_SET);
   if (avi_read(AVI->fdes, data, 8) != 8)
      ERR_EXIT(AVI_ERR_READ)
   if (strncasecmp(data, (char *)cur_idx, 4) == 0 && str2ulong((unsigned char *)data + 4) == len)
   {
      idx_type = 1; /* Index from start of file */
   }
   else
   {
      lseek(AVI->fdes, pos + AVI->movi_start - 4, SEEK_SET);
      if (avi_read(AVI->fdes, data, 8) != 8)
         ERR_EXIT(AVI_ERR_READ)
      if (strncasecmp(data, (char *)cur_idx, 4) == 0 && str2ulong((unsigned char *)data + 4) == len)
      {
         idx_type = 2; /* Index from start of movi list */
      }
   }
   /* idx_type remains 0 if neither of the two tests above succeeds */

   if (idx_type == 0)
   {
      /* we must search through the file to get the index */

      lseek(AVI->fdes, AVI->movi_start, SEEK_SET);

      AVI->n_idx = 0;

      while (1)
      {
         if (avi_read(AVI->fdes, data, 8) != 8)
            break;
         n = str2ulong((unsigned char *)data + 4);

         /* The movi list may contain sub-lists, ignore them */

         if (strncasecmp(data, "LIST", 4) == 0)
         {
            lseek(AVI->fdes, 4, SEEK_CUR);
            continue;
         }

         /* Skip check if we got a tag ##db, ##dc or ##wb */

         lseek(AVI->fdes, PAD_EVEN(n), SEEK_CUR);
      }
      idx_type = 1;
   }

   /* Now generate the video index and audio index arrays */

   nvi = 0;
   for (j = 0; j < AVI->anum; ++j)
      nai[j] = 0;

   lseek(AVI->fdes, AVI->idx1_start, SEEK_SET);
   for (i = 0; i < AVI->n_idx; i++)
   {
      avi_read(AVI->fdes, (char *)cur_idx, 16);
      if (strncasecmp((char *)cur_idx, AVI->video_tag, 3) == 0)
         nvi++;

      for (j = 0; j < AVI->anum; ++j)
         if (strncasecmp((char *)cur_idx, AVI->track[j].audio_tag, 4) == 0)
            nai[j]++;
   }

   AVI->video_frames = nvi;
   for (j = 0; j < AVI->anum; ++j)
      AVI->track[j].audio_chunks = nai[j];

   //   fprintf(stderr, "chunks = %ld %d %s\n", AVI->track[0].audio_chunks, AVI->anum, AVI->track[0].audio_tag);

   if (AVI->video_frames == 0)
      ERR_EXIT(AVI_ERR_NO_VIDS);
   log_i("malloc(video_index_entry): %d, free PSRAM: %d", nvi * sizeof(video_index_entry), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
   AVI->video_index = (video_index_entry *)malloc(nvi * sizeof(video_index_entry));
   if (AVI->video_index == 0)
      ERR_EXIT(AVI_ERR_NO_MEM);

   for (j = 0; j < AVI->anum; ++j)
   {
      if (AVI->track[j].audio_chunks)
      {
         log_i("malloc(audio_index_entry): %d, free PSRAM: %d", (nai[j] + 1) * sizeof(audio_index_entry), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
         AVI->track[j].audio_index = (audio_index_entry *)malloc((nai[j] + 1) * sizeof(audio_index_entry));
         memset(AVI->track[j].audio_index, 0, (nai[j] + 1) * (sizeof(audio_index_entry)));
         if (AVI->track[j].audio_index == 0)
            ERR_EXIT(AVI_ERR_NO_MEM);
      }
   }

   nvi = 0;
   for (j = 0; j < AVI->anum; ++j)
      nai[j] = tot[j] = 0;

   ioff = idx_type == 1 ? 8 : AVI->movi_start + 4;

   lseek(AVI->fdes, AVI->idx1_start, SEEK_SET);
   for (i = 0; i < AVI->n_idx; i++)
   {
      avi_read(AVI->fdes, (char *)cur_idx, 16);

      // video
      if (strncasecmp((char *)cur_idx, AVI->video_tag, 3) == 0)
      {
         AVI->video_index[nvi].key = (uint8_t)str2ulong(cur_idx + 4);
         AVI->video_index[nvi].pos = str2ulong(cur_idx + 8) + ioff;
         AVI->video_index[nvi].len = str2ulong(cur_idx + 12);
         nvi++;
      }

      // audio
      for (j = 0; j < AVI->anum; ++j)
      {

         if (strncasecmp((char *)cur_idx, AVI->track[j].audio_tag, 4) == 0)
         {
            AVI->track[j].audio_index[nai[j]].pos = str2ulong(cur_idx + 8) + ioff;
            AVI->track[j].audio_index[nai[j]].len = str2ulong(cur_idx + 12);
            AVI->track[j].audio_index[nai[j]].tot = tot[j];
            tot[j] += AVI->track[j].audio_index[nai[j]].len;
            nai[j]++;
         }
      }
   }

   for (j = 0; j < AVI->anum; ++j)
      AVI->track[j].audio_bytes = tot[j];

   /* Reposition the file */

   lseek(AVI->fdes, AVI->movi_start, SEEK_SET);
   AVI->video_pos = 0;

   return (0);
}

avi_t *AVI_open_input_file(const char *filename, int getIndex)
{
   avi_t *AVI = NULL;

   /* Create avi_t structure */

   AVI = (avi_t *)malloc(sizeof(avi_t));
   if (AVI == NULL)
   {
      AVI_errno = AVI_ERR_NO_MEM;
      return 0;
   }
   memset((void *)AVI, 0, sizeof(avi_t));

   AVI->mode = AVI_MODE_READ; /* open for reading */

   /* Open the file */

   AVI->fdes = open(filename, O_RDONLY);
   if (AVI->fdes < 0)
   {
      AVI_errno = AVI_ERR_OPEN;
      free(AVI);
      return 0;
   }

   avi_parse_input_file(AVI, getIndex);

   AVI->aptr = 0; // reset

   return AVI;
}

long AVI_video_frames(avi_t *AVI)
{
   return AVI->video_frames;
}
int AVI_video_width(avi_t *AVI)
{
   return AVI->width;
}
int AVI_video_height(avi_t *AVI)
{
   return AVI->height;
}
double AVI_frame_rate(avi_t *AVI)
{
   return AVI->fps;
}
char *AVI_video_compressor(avi_t *AVI)
{
   return AVI->compressor2;
}

long AVI_max_video_chunk(avi_t *AVI)
{
   return AVI->max_len;
}

int AVI_audio_tracks(avi_t *AVI)
{
   return (AVI->anum);
}

int AVI_audio_channels(avi_t *AVI)
{
   return AVI->track[AVI->aptr].a_chans;
}

long AVI_audio_mp3rate(avi_t *AVI)
{
   return AVI->track[AVI->aptr].mp3rate;
}

int AVI_audio_bits(avi_t *AVI)
{
   return AVI->track[AVI->aptr].a_bits;
}

int AVI_audio_format(avi_t *AVI)
{
   return AVI->track[AVI->aptr].a_fmt;
}

long AVI_audio_rate(avi_t *AVI)
{
   return AVI->track[AVI->aptr].a_rate;
}

long AVI_audio_bytes(avi_t *AVI)
{
   return AVI->track[AVI->aptr].audio_bytes;
}

long AVI_audio_chunks(avi_t *AVI)
{
   return AVI->track[AVI->aptr].audio_chunks;
}

long AVI_audio_codech_offset(avi_t *AVI)
{
   return AVI->track[AVI->aptr].a_codech_off;
}

long AVI_audio_codecf_offset(avi_t *AVI)
{
   return AVI->track[AVI->aptr].a_codecf_off;
}

long AVI_video_codech_offset(avi_t *AVI)
{
   return AVI->v_codech_off;
}

long AVI_video_codecf_offset(avi_t *AVI)
{
   return AVI->v_codecf_off;
}

long AVI_frame_size(avi_t *AVI, long frame)
{
   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->video_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (frame < 0 || frame >= AVI->video_frames)
      return 0;
   return (AVI->video_index[frame].len);
}

long AVI_audio_size(avi_t *AVI, long frame)
{
   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->track[AVI->aptr].audio_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (frame < 0 || frame >= AVI->track[AVI->aptr].audio_chunks)
      return 0;
   return (AVI->track[AVI->aptr].audio_index[frame].len);
}

long AVI_get_video_position(avi_t *AVI, long frame)
{
   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->video_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (frame < 0 || frame >= AVI->video_frames)
      return 0;
   return (AVI->video_index[frame].pos);
}

int AVI_seek_start(avi_t *AVI)
{
   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }

   lseek(AVI->fdes, AVI->movi_start, SEEK_SET);
   AVI->video_pos = 0;
   return 0;
}

int AVI_set_video_position(avi_t *AVI, long frame)
{
   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->video_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (frame < 0)
      frame = 0;
   AVI->video_pos = frame;
   return 0;
}

int AVI_set_audio_bitrate(avi_t *AVI, long bitrate)
{
   if (AVI->mode == AVI_MODE_READ)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }

   AVI->track[AVI->aptr].mp3rate = bitrate;
   return 0;
}

long AVI_read_frame(avi_t *AVI, char *vidbuf, int *keyframe)
{
   long n;

   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->video_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (AVI->video_pos < 0 || AVI->video_pos >= AVI->video_frames)
      return -1;
   n = AVI->video_index[AVI->video_pos].len;

   *keyframe = (AVI->video_index[AVI->video_pos].key == 0x10) ? 1 : 0;

   lseek(AVI->fdes, AVI->video_index[AVI->video_pos].pos, SEEK_SET);

   if (avi_read(AVI->fdes, vidbuf, n) != n)
   {
      AVI_errno = AVI_ERR_READ;
      return -1;
   }

   AVI->video_pos++;

   return n;
}

int AVI_set_audio_position(avi_t *AVI, long byte)
{
   long n0, n1, n;

   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->track[AVI->aptr].audio_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (byte < 0)
      byte = 0;

   /* Binary search in the audio chunks */

   n0 = 0;
   n1 = AVI->track[AVI->aptr].audio_chunks;

   while (n0 < n1 - 1)
   {
      n = (n0 + n1) / 2;
      if (AVI->track[AVI->aptr].audio_index[n].tot > byte)
         n1 = n;
      else
         n0 = n;
   }

   AVI->track[AVI->aptr].audio_posc = n0;
   AVI->track[AVI->aptr].audio_posb = byte - AVI->track[AVI->aptr].audio_index[n0].tot;

   return 0;
}

long AVI_read_audio(avi_t *AVI, char *audbuf, long bytes)
{
   long nr, pos, left, todo;

   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->track[AVI->aptr].audio_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   nr = 0; /* total number of bytes read */

   while (bytes > 0)
   {
      left = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len - AVI->track[AVI->aptr].audio_posb;
      if (left == 0)
      {
         if (AVI->track[AVI->aptr].audio_posc >= AVI->track[AVI->aptr].audio_chunks - 1)
            return nr;
         AVI->track[AVI->aptr].audio_posc++;
         AVI->track[AVI->aptr].audio_posb = 0;
         continue;
      }
      if (bytes < left)
         todo = bytes;
      else
         todo = left;
      pos = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].pos + AVI->track[AVI->aptr].audio_posb;
      lseek(AVI->fdes, pos, SEEK_SET);
      if (avi_read(AVI->fdes, audbuf + nr, todo) != todo)
      {
         AVI_errno = AVI_ERR_READ;
         return -1;
      }
      bytes -= todo;
      nr += todo;
      AVI->track[AVI->aptr].audio_posb += todo;
   }

   return nr;
}

long AVI_read_audio_chunk(avi_t *AVI, char *audbuf)
{
   long pos, left;

   if (AVI->mode == AVI_MODE_WRITE)
   {
      AVI_errno = AVI_ERR_NOT_PERM;
      return -1;
   }
   if (!AVI->track[AVI->aptr].audio_index)
   {
      AVI_errno = AVI_ERR_NO_IDX;
      return -1;
   }

   if (AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len == 0)
      return 0;
   left = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len - AVI->track[AVI->aptr].audio_posb;

   if (audbuf == NULL)
      return left;

   if (left == 0)
      return 0;

   pos = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].pos + AVI->track[AVI->aptr].audio_posb;
   lseek(AVI->fdes, pos, SEEK_SET);
   if (avi_read(AVI->fdes, audbuf, left) != left)
   {
      AVI_errno = AVI_ERR_READ;
      return -1;
   }
   AVI->track[AVI->aptr].audio_posc++;
   AVI->track[AVI->aptr].audio_posb = 0;

   return left;
}

/* AVI_read_data: Special routine for reading the next audio or video chunk
                  without having an index of the file. */

int AVI_read_data(avi_t *AVI, char *vidbuf, long max_vidbuf,
                  char *audbuf, long max_audbuf,
                  long *len)
{

   /*
    * Return codes:
    *
    *    1 = video data read
    *    2 = audio data read
    *    0 = reached EOF
    *   -1 = video buffer too small
    *   -2 = audio buffer too small
    */

   off_t n;
   char data[8];

   if (AVI->mode == AVI_MODE_WRITE)
      return 0;

   while (1)
   {
      /* Read tag and length */

      if (avi_read(AVI->fdes, data, 8) != 8)
         return 0;

      /* if we got a list tag, ignore it */

      if (strncasecmp(data, "LIST", 4) == 0)
      {
         lseek(AVI->fdes, 4, SEEK_CUR);
         continue;
      }

      n = PAD_EVEN(str2ulong((unsigned char *)data + 4));

      if (strncasecmp(data, AVI->video_tag, 3) == 0)
      {
         *len = n;
         AVI->video_pos++;
         if (n > max_vidbuf)
         {
            lseek(AVI->fdes, n, SEEK_CUR);
            return -1;
         }
         if (avi_read(AVI->fdes, vidbuf, n) != n)
            return 0;
         return 1;
      }
      else if (strncasecmp(data, AVI->track[AVI->aptr].audio_tag, 4) == 0)
      {
         *len = n;
         if (n > max_audbuf)
         {
            lseek(AVI->fdes, n, SEEK_CUR);
            return -2;
         }
         if (avi_read(AVI->fdes, audbuf, n) != n)
            return 0;
         return 2;
         break;
      }
      else if (lseek(AVI->fdes, n, SEEK_CUR) < 0)
         return 0;
   }
}

/* AVI_print_error: Print most recent error (similar to perror) */

char *(avi_errors[]) =
    {
        /*  0 */ "avilib - No Error",
        /*  1 */ "avilib - AVI file size limit reached",
        /*  2 */ "avilib - Error opening AVI file",
        /*  3 */ "avilib - Error reading from AVI file",
        /*  4 */ "avilib - Error writing to AVI file",
        /*  5 */ "avilib - Error writing index (file may still be useable)",
        /*  6 */ "avilib - Error closing AVI file",
        /*  7 */ "avilib - Operation (read/write) not permitted",
        /*  8 */ "avilib - Out of memory (malloc failed)",
        /*  9 */ "avilib - Not an AVI file",
        /* 10 */ "avilib - AVI file has no header list (corrupted?)",
        /* 11 */ "avilib - AVI file has no MOVI list (corrupted?)",
        /* 12 */ "avilib - AVI file has no video data",
        /* 13 */ "avilib - operation needs an index",
        /* 14 */ "avilib - Unkown Error"};
static int num_avi_errors = sizeof(avi_errors) / sizeof(char *);

static char error_string[4096];

void AVI_print_error(char *str)
{
   int aerrno;

   aerrno = (AVI_errno >= 0 && AVI_errno < num_avi_errors) ? AVI_errno : num_avi_errors - 1;

   fprintf(stderr, "%s: %s\n", str, avi_errors[aerrno]);

   /* for the following errors, perror should report a more detailed reason: */

   if (AVI_errno == AVI_ERR_OPEN ||
       AVI_errno == AVI_ERR_READ ||
       AVI_errno == AVI_ERR_WRITE ||
       AVI_errno == AVI_ERR_WRITE_INDEX ||
       AVI_errno == AVI_ERR_CLOSE)
   {
      perror("REASON");
   }
}

char *AVI_strerror()
{
   int aerrno;

   aerrno = (AVI_errno >= 0 && AVI_errno < num_avi_errors) ? AVI_errno : num_avi_errors - 1;

   if (AVI_errno == AVI_ERR_OPEN ||
       AVI_errno == AVI_ERR_READ ||
       AVI_errno == AVI_ERR_WRITE ||
       AVI_errno == AVI_ERR_WRITE_INDEX ||
       AVI_errno == AVI_ERR_CLOSE)
   {
      sprintf(error_string, "%s - %s", avi_errors[aerrno], strerror(errno));
      return error_string;
   }
   else
   {
      return avi_errors[aerrno];
   }
}

uint64_t AVI_max_size()
{
   return ((uint64_t)AVI_MAX_LEN);
}

#endif
