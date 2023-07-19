#pragma once

/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "list.h"
#include "cinepak_tables.h"

/** Template method to clip the value @p v so that it remains between @p amin and @p amax. */
template <typename T>
inline T CLIP(T v, T amin, T amax)
{
#if !defined(RELEASE_BUILD)
	// Debug builds use this assert to pinpoint
	// any problematic cases, where amin and amax
	// are incorrectly ordered
	// and thus CLIP() would return an invalid result.
	assert(amin <= amax);
#endif
	if (v < amin)
		return amin;
	else if (v > amax)
		return amax;
	return v;
}

class FrameData
{
public:
	void setData(uint8_t *data, size_t size)
	{
		_buffer = data;
		size = _size;
		_pos = 0;
	}
	size_t size()
	{
		return _size;
	}
	size_t pos()
	{
		return _pos;
	}
	void seek(size_t pos)
	{
		_pos = pos;
	}
	void seek_delta(size_t delta)
	{
		_pos += delta;
	}
	bool eos()
	{
		return _pos >= (_size - 1);
	}
	uint8_t read(uint8_t *dst, size_t len)
	{
		while (len--)
		{
			*dst++ = _buffer[_pos++];
		}
	}
	uint8_t readByte()
	{
		return _buffer[_pos++];
	}
	uint16_t readUint16BE()
	{
		uint16_t a = _buffer[_pos++];
		uint16_t b = _buffer[_pos++];
		return (a << 8) | b;
	}
	uint32_t readUint32BE()
	{
		uint32_t a = _buffer[_pos++];
		uint32_t b = _buffer[_pos++];
		uint32_t c = _buffer[_pos++];
		uint32_t d = _buffer[_pos++];
		return (a << 24) | (b << 16) | (c << 8) | d;
	}

private:
	size_t _size;
	uint8_t *_buffer;
	size_t _pos;
};

/**
 * A type of dithering.
 */
enum DitherType
{
	/** Unknown */
	kDitherTypeUnknown,

	/** Video for Windows dithering */
	kDitherTypeVFW,

	/** QuickTime dithering */
	kDitherTypeQT
};

struct Rect
{
	int16_t top, left;								/*!< The point at the top left of the rectangle (part of the Rect). */
	int16_t bottom, right;							/*!< The point at the bottom right of the rectangle (not part of the Rect). */
	int16_t width() const { return right - left; }	/*!< Return the width of a rectangle. */
	int16_t height() const { return bottom - top; } /*!< Return the height of a rectangle. */
};

struct Surface
{
	int16_t w;
	int16_t h;
	int32_t pitch;
	void *pixels;

	void create(int16_t w, int16_t h)
	{
		this->w = w;
		this->h = h;
		this->pixels = malloc(w * h * 2);
	}

	void free()
	{
		::free(pixels);
		pixels = 0;
		w = h = 0;
	}

	inline const void *getBasePtr(int x, int y) const
	{
		return (const uint8_t *)(pixels) + y * pitch + x * 2;
	}
};

struct CinepakCodebook
{
	// These are not in the normal YUV colorspace, but in the Cinepak YUV colorspace instead.
	uint8_t y[4]; // [0, 255]
	int8_t u, v;  // [-128, 127]
};

struct CinepakStrip
{
	uint16_t id;
	uint16_t length;
	Rect rect;
	CinepakCodebook v1_codebook[256], v4_codebook[256];
	uint8_t v1_dither[256 * 4 * 4 * 4], v4_dither[256 * 4 * 4 * 4];
};

struct CinepakFrame
{
	uint8_t flags;
	uint32_t length;
	uint16_t width;
	uint16_t height;
	uint16_t stripCount;
	CinepakStrip *strips;

	Surface *surface;
};

template <typename PixelInt, typename CodebookConverter>
void decodeVectorsTmpl(CinepakFrame &frame, const uint8_t *clipTable, const uint8_t *colorMap, FrameData &stream, uint16_t strip, uint8_t chunkID, uint32_t chunkSize)
{
	uint32_t flag = 0, mask = 0;
	PixelInt *iy[4];
	int32_t startPos = stream.pos();

	for (uint16_t y = frame.strips[strip].rect.top; y < frame.strips[strip].rect.bottom; y += 4)
	{
		iy[0] = (PixelInt *)frame.surface->getBasePtr(frame.strips[strip].rect.left, +y);
		iy[1] = iy[0] + frame.width;
		iy[2] = iy[1] + frame.width;
		iy[3] = iy[2] + frame.width;

		for (uint16_t x = frame.strips[strip].rect.left; x < frame.strips[strip].rect.right; x += 4)
		{
			if ((chunkID & 0x01) && !(mask >>= 1))
			{
				if ((stream.pos() - startPos + 4) > (int32_t)chunkSize)
					return;

				flag = stream.readUint32BE();
				mask = 0x80000000;
			}

			if (!(chunkID & 0x01) || (flag & mask))
			{
				if (!(chunkID & 0x02) && !(mask >>= 1))
				{
					if ((stream.pos() - startPos + 4) > (int32_t)chunkSize)
						return;

					flag = stream.readUint32BE();
					mask = 0x80000000;
				}

				if ((chunkID & 0x02) || (~flag & mask))
				{
					if ((stream.pos() - startPos + 1) > (int32_t)chunkSize)
						return;

					// Get the codebook
					uint8_t codebook = stream.readByte();
					CodebookConverter::decodeBlock1(codebook, frame.strips[strip], iy, clipTable, colorMap);
				}
				else if (flag & mask)
				{
					if ((stream.pos() - startPos + 4) > (int32_t)chunkSize)
						return;

					uint8_t codebook[4];
					stream.read(codebook, 4);
					CodebookConverter::decodeBlock4(codebook, frame.strips[strip], iy, clipTable, colorMap);
				}
			}

			for (uint8_t i = 0; i < 4; i++)
				iy[i] += 4;
		}
	}
}

static inline void convertYUVToRGB(const uint8_t *clipTable, uint8_t y, int8_t u, int8_t v, uint8_t &r, uint8_t &g, uint8_t &b)
{
	log_i("convertYUVToRGB(%d, %d, %d)", y, u, v);
	r = clipTable[(int)y + (v << 1)];
	g = clipTable[(int)y - (u >> 1) - v];
	b = clipTable[(int)y + (u << 1)];
}

static inline uint32_t convertYUVToColor(const uint8_t *clipTable, uint8_t y, uint8_t u, uint8_t v)
{
	uint8_t r, g, b;
	convertYUVToRGB(clipTable, y, u, v, r, g, b);
	return ((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | ((b) >> 3));
}

static inline uint16_t createDitherTableIndex(const uint8_t *clipTable, uint8_t y, int8_t u, int8_t v)
{
	uint8_t r, g, b;
	convertYUVToRGB(clipTable, y, u, v, r, g, b);
	return ((r & 0xF8) << 6) |
		   ((g & 0xF8) << 1) |
		   ((b & 0xF0) >> 4);
}

static inline uint16_t makeQuickTimeDitherColor(uint8_t r, uint8_t g, uint8_t b)
{
	// RGB554
	return ((r & 0xF8) << 6) | ((g & 0xF8) << 1) | (b >> 4);
}

static inline uint16_t READ_UINT16(const void *ptr)
{
	const uint8_t *b = (const uint8_t *)ptr;
	return (b[1] << 8) | b[0];
}

static inline uint32_t READ_UINT32(const void *ptr)
{
	const uint8_t *b = (const uint8_t *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | (b[0]);
}

static inline void WRITE_UINT16(void *ptr, uint16_t value)
{
	uint8_t *b = (uint8_t *)ptr;
	b[0] = (uint8_t)(value >> 8);
	b[1] = (uint8_t)(value >> 0);
}

static inline void WRITE_UINT32(void *ptr, uint32_t value)
{
	uint8_t *b = (uint8_t *)ptr;
	b[0] = (uint8_t)(value >> 0);
	b[1] = (uint8_t)(value >> 8);
	b[2] = (uint8_t)(value >> 16);
	b[3] = (uint8_t)(value >> 24);
}

static inline uint8_t adjustColorRange(uint8_t currentColor, uint8_t correctColor, uint8_t palColor)
{
	return CLIP<int>(currentColor - palColor + correctColor, 0, 255);
}

static inline void addColorToQueue(uint16_t color, uint16_t index, uint8_t *checkBuffer, Common::List<uint16_t> &checkQueue)
{
	if ((READ_UINT16(checkBuffer + color * 2) & 0xFF) == 0)
	{
		// Previously unfound color
		WRITE_UINT16(checkBuffer + color * 2, index);
		checkQueue.push_back(color);
	}
}

template <typename PixelInt>
static inline void putPixelRaw(PixelInt *dst, const uint8_t *clipTable, uint8_t y, uint8_t u, uint8_t v)
{
	*dst = convertYUVToColor(clipTable, y, u, v);
}

struct CodebookConverterRaw
{
	template <typename PixelInt>
	static inline void decodeBlock1(uint8_t codebookIndex, const CinepakStrip &strip, PixelInt *(&rows)[4], const uint8_t *clipTable, const uint8_t *colorMap)
	{
		const CinepakCodebook &codebook = strip.v1_codebook[codebookIndex];
		putPixelRaw(rows[0] + 0, clipTable, codebook.y[0], codebook.u, codebook.v);
		putPixelRaw(rows[0] + 1, clipTable, codebook.y[0], codebook.u, codebook.v);
		putPixelRaw(rows[1] + 0, clipTable, codebook.y[0], codebook.u, codebook.v);
		putPixelRaw(rows[1] + 1, clipTable, codebook.y[0], codebook.u, codebook.v);

		putPixelRaw(rows[0] + 2, clipTable, codebook.y[1], codebook.u, codebook.v);
		putPixelRaw(rows[0] + 3, clipTable, codebook.y[1], codebook.u, codebook.v);
		putPixelRaw(rows[1] + 2, clipTable, codebook.y[1], codebook.u, codebook.v);
		putPixelRaw(rows[1] + 3, clipTable, codebook.y[1], codebook.u, codebook.v);

		putPixelRaw(rows[2] + 0, clipTable, codebook.y[2], codebook.u, codebook.v);
		putPixelRaw(rows[2] + 1, clipTable, codebook.y[2], codebook.u, codebook.v);
		putPixelRaw(rows[3] + 0, clipTable, codebook.y[2], codebook.u, codebook.v);
		putPixelRaw(rows[3] + 1, clipTable, codebook.y[2], codebook.u, codebook.v);

		putPixelRaw(rows[2] + 2, clipTable, codebook.y[3], codebook.u, codebook.v);
		putPixelRaw(rows[2] + 3, clipTable, codebook.y[3], codebook.u, codebook.v);
		putPixelRaw(rows[3] + 2, clipTable, codebook.y[3], codebook.u, codebook.v);
		putPixelRaw(rows[3] + 3, clipTable, codebook.y[3], codebook.u, codebook.v);
	}

	template <typename PixelInt>
	static inline void decodeBlock4(const uint8_t (&codebookIndex)[4], const CinepakStrip &strip, PixelInt *(&rows)[4], const uint8_t *clipTable, const uint8_t *colorMap)
	{
		const CinepakCodebook &codebook1 = strip.v4_codebook[codebookIndex[0]];
		putPixelRaw(rows[0] + 0, clipTable, codebook1.y[0], codebook1.u, codebook1.v);
		putPixelRaw(rows[0] + 1, clipTable, codebook1.y[1], codebook1.u, codebook1.v);
		putPixelRaw(rows[1] + 0, clipTable, codebook1.y[2], codebook1.u, codebook1.v);
		putPixelRaw(rows[1] + 1, clipTable, codebook1.y[3], codebook1.u, codebook1.v);

		const CinepakCodebook &codebook2 = strip.v4_codebook[codebookIndex[1]];
		putPixelRaw(rows[0] + 2, clipTable, codebook2.y[0], codebook2.u, codebook2.v);
		putPixelRaw(rows[0] + 3, clipTable, codebook2.y[1], codebook2.u, codebook2.v);
		putPixelRaw(rows[1] + 2, clipTable, codebook2.y[2], codebook2.u, codebook2.v);
		putPixelRaw(rows[1] + 3, clipTable, codebook2.y[3], codebook2.u, codebook2.v);

		const CinepakCodebook &codebook3 = strip.v4_codebook[codebookIndex[2]];
		putPixelRaw(rows[2] + 0, clipTable, codebook3.y[0], codebook3.u, codebook3.v);
		putPixelRaw(rows[2] + 1, clipTable, codebook3.y[1], codebook3.u, codebook3.v);
		putPixelRaw(rows[3] + 0, clipTable, codebook3.y[2], codebook3.u, codebook3.v);
		putPixelRaw(rows[3] + 1, clipTable, codebook3.y[3], codebook3.u, codebook3.v);

		const CinepakCodebook &codebook4 = strip.v4_codebook[codebookIndex[3]];
		putPixelRaw(rows[2] + 2, clipTable, codebook4.y[0], codebook4.u, codebook4.v);
		putPixelRaw(rows[2] + 3, clipTable, codebook4.y[1], codebook4.u, codebook4.v);
		putPixelRaw(rows[3] + 2, clipTable, codebook4.y[2], codebook4.u, codebook4.v);
		putPixelRaw(rows[3] + 3, clipTable, codebook4.y[3], codebook4.u, codebook4.v);
	}
};

struct CodebookConverterDitherVFW
{
	static inline void decodeBlock1(uint8_t codebookIndex, const CinepakStrip &strip, uint8_t *(&rows)[4], const uint8_t *clipTable, const uint8_t *colorMap)
	{
		const CinepakCodebook &codebook = strip.v1_codebook[codebookIndex];
		uint8_t blockBuffer[16];
		ditherCodebookSmooth(codebook, blockBuffer, colorMap);
		rows[0][0] = blockBuffer[0];
		rows[0][1] = blockBuffer[1];
		rows[0][2] = blockBuffer[2];
		rows[0][3] = blockBuffer[3];
		rows[1][0] = blockBuffer[4];
		rows[1][1] = blockBuffer[5];
		rows[1][2] = blockBuffer[6];
		rows[1][3] = blockBuffer[7];
		rows[2][0] = blockBuffer[8];
		rows[2][1] = blockBuffer[9];
		rows[2][2] = blockBuffer[10];
		rows[2][3] = blockBuffer[11];
		rows[3][0] = blockBuffer[12];
		rows[3][1] = blockBuffer[13];
		rows[3][2] = blockBuffer[14];
		rows[3][3] = blockBuffer[15];
	}

	static inline void decodeBlock4(const uint8_t (&codebookIndex)[4], const CinepakStrip &strip, uint8_t *(&rows)[4], const uint8_t *clipTable, const uint8_t *colorMap)
	{
		uint8_t blockBuffer[16];
		ditherCodebookDetail(strip.v4_codebook[codebookIndex[0]], blockBuffer, colorMap);
		rows[0][0] = blockBuffer[0];
		rows[0][1] = blockBuffer[1];
		rows[1][0] = blockBuffer[4];
		rows[1][1] = blockBuffer[5];

		ditherCodebookDetail(strip.v4_codebook[codebookIndex[1]], blockBuffer, colorMap);
		rows[0][2] = blockBuffer[2];
		rows[0][3] = blockBuffer[3];
		rows[1][2] = blockBuffer[6];
		rows[1][3] = blockBuffer[7];

		ditherCodebookDetail(strip.v4_codebook[codebookIndex[2]], blockBuffer, colorMap);
		rows[2][0] = blockBuffer[8];
		rows[2][1] = blockBuffer[9];
		rows[3][0] = blockBuffer[12];
		rows[3][1] = blockBuffer[13];

		ditherCodebookDetail(strip.v4_codebook[codebookIndex[3]], blockBuffer, colorMap);
		rows[2][2] = blockBuffer[10];
		rows[2][3] = blockBuffer[11];
		rows[3][2] = blockBuffer[14];
		rows[3][3] = blockBuffer[15];
	}

private:
	static inline void ditherCodebookDetail(const CinepakCodebook &codebook, uint8_t *dst, const uint8_t *colorMap)
	{
		int uLookup = (uint8_t)codebook.u * 2;
		int vLookup = (uint8_t)codebook.v * 2;
		uint32_t uv1 = s_uLookup[uLookup] | s_vLookup[vLookup];
		uint32_t uv2 = s_uLookup[uLookup + 1] | s_vLookup[vLookup + 1];

		int yLookup1 = codebook.y[0] * 2;
		int yLookup2 = codebook.y[1] * 2;
		int yLookup3 = codebook.y[2] * 2;
		int yLookup4 = codebook.y[3] * 2;

		uint32_t pixelGroup1 = uv2 | s_yLookup[yLookup1 + 1];
		uint32_t pixelGroup2 = uv2 | s_yLookup[yLookup2 + 1];
		uint32_t pixelGroup3 = uv1 | s_yLookup[yLookup3];
		uint32_t pixelGroup4 = uv1 | s_yLookup[yLookup4];
		uint32_t pixelGroup5 = uv1 | s_yLookup[yLookup1];
		uint32_t pixelGroup6 = uv1 | s_yLookup[yLookup2];
		uint32_t pixelGroup7 = uv2 | s_yLookup[yLookup3 + 1];
		uint32_t pixelGroup8 = uv2 | s_yLookup[yLookup4 + 1];

		dst[0] = getRGBLookupEntry(colorMap, pixelGroup1 & 0xFFFF);
		dst[1] = getRGBLookupEntry(colorMap, pixelGroup2 >> 16);
		dst[2] = getRGBLookupEntry(colorMap, pixelGroup5 & 0xFFFF);
		dst[3] = getRGBLookupEntry(colorMap, pixelGroup6 >> 16);
		dst[4] = getRGBLookupEntry(colorMap, pixelGroup3 & 0xFFFF);
		dst[5] = getRGBLookupEntry(colorMap, pixelGroup4 >> 16);
		dst[6] = getRGBLookupEntry(colorMap, pixelGroup7 & 0xFFFF);
		dst[7] = getRGBLookupEntry(colorMap, pixelGroup8 >> 16);
		dst[8] = getRGBLookupEntry(colorMap, pixelGroup1 >> 16);
		dst[9] = getRGBLookupEntry(colorMap, pixelGroup6 & 0xFFFF);
		dst[10] = getRGBLookupEntry(colorMap, pixelGroup5 >> 16);
		dst[11] = getRGBLookupEntry(colorMap, pixelGroup2 & 0xFFFF);
		dst[12] = getRGBLookupEntry(colorMap, pixelGroup3 >> 16);
		dst[13] = getRGBLookupEntry(colorMap, pixelGroup8 & 0xFFFF);
		dst[14] = getRGBLookupEntry(colorMap, pixelGroup7 >> 16);
		dst[15] = getRGBLookupEntry(colorMap, pixelGroup4 & 0xFFFF);
	}

	static inline void ditherCodebookSmooth(const CinepakCodebook &codebook, uint8_t *dst, const uint8_t *colorMap)
	{
		int uLookup = (uint8_t)codebook.u * 2;
		int vLookup = (uint8_t)codebook.v * 2;
		uint32_t uv1 = s_uLookup[uLookup] | s_vLookup[vLookup];
		uint32_t uv2 = s_uLookup[uLookup + 1] | s_vLookup[vLookup + 1];

		int yLookup1 = codebook.y[0] * 2;
		int yLookup2 = codebook.y[1] * 2;
		int yLookup3 = codebook.y[2] * 2;
		int yLookup4 = codebook.y[3] * 2;

		uint32_t pixelGroup1 = uv2 | s_yLookup[yLookup1 + 1];
		uint32_t pixelGroup2 = uv1 | s_yLookup[yLookup2];
		uint32_t pixelGroup3 = uv1 | s_yLookup[yLookup1];
		uint32_t pixelGroup4 = uv2 | s_yLookup[yLookup2 + 1];
		uint32_t pixelGroup5 = uv2 | s_yLookup[yLookup3 + 1];
		uint32_t pixelGroup6 = uv1 | s_yLookup[yLookup3];
		uint32_t pixelGroup7 = uv1 | s_yLookup[yLookup4];
		uint32_t pixelGroup8 = uv2 | s_yLookup[yLookup4 + 1];

		dst[0] = getRGBLookupEntry(colorMap, pixelGroup1 & 0xFFFF);
		dst[1] = getRGBLookupEntry(colorMap, pixelGroup1 >> 16);
		dst[2] = getRGBLookupEntry(colorMap, pixelGroup2 & 0xFFFF);
		dst[3] = getRGBLookupEntry(colorMap, pixelGroup2 >> 16);
		dst[4] = getRGBLookupEntry(colorMap, pixelGroup3 & 0xFFFF);
		dst[5] = getRGBLookupEntry(colorMap, pixelGroup3 >> 16);
		dst[6] = getRGBLookupEntry(colorMap, pixelGroup4 & 0xFFFF);
		dst[7] = getRGBLookupEntry(colorMap, pixelGroup4 >> 16);
		dst[8] = getRGBLookupEntry(colorMap, pixelGroup5 >> 16);
		dst[9] = getRGBLookupEntry(colorMap, pixelGroup6 & 0xFFFF);
		dst[10] = getRGBLookupEntry(colorMap, pixelGroup7 >> 16);
		dst[11] = getRGBLookupEntry(colorMap, pixelGroup8 & 0xFFFF);
		dst[12] = getRGBLookupEntry(colorMap, pixelGroup6 >> 16);
		dst[13] = getRGBLookupEntry(colorMap, pixelGroup5 & 0xFFFF);
		dst[14] = getRGBLookupEntry(colorMap, pixelGroup8 >> 16);
		dst[15] = getRGBLookupEntry(colorMap, pixelGroup7 & 0xFFFF);
	}

	static inline uint8_t getRGBLookupEntry(const uint8_t *colorMap, uint16_t index)
	{
		return colorMap[s_defaultPaletteLookup[CLIP<int>(index, 0, 1023)]];
	}
};

struct CodebookConverterDitherQT
{
	static inline void decodeBlock1(uint8_t codebookIndex, const CinepakStrip &strip, uint8_t *(&rows)[4], const uint8_t *clipTable, const uint8_t *colorMap)
	{
		const uint8_t *colorPtr = strip.v1_dither + (codebookIndex << 2);
		WRITE_UINT32(rows[0], READ_UINT32(colorPtr));
		WRITE_UINT32(rows[1], READ_UINT32(colorPtr + 1024));
		WRITE_UINT32(rows[2], READ_UINT32(colorPtr + 2048));
		WRITE_UINT32(rows[3], READ_UINT32(colorPtr + 3072));
	}

	static inline void decodeBlock4(const uint8_t (&codebookIndex)[4], const CinepakStrip &strip, uint8_t *(&rows)[4], const uint8_t *clipTable, const uint8_t *colorMap)
	{
		const uint8_t *colorPtr = strip.v4_dither + (codebookIndex[0] << 2);
		WRITE_UINT16(rows[0] + 0, READ_UINT16(colorPtr + 0));
		WRITE_UINT16(rows[1] + 0, READ_UINT16(colorPtr + 2));

		colorPtr = strip.v4_dither + (codebookIndex[1] << 2);
		WRITE_UINT16(rows[0] + 2, READ_UINT16(colorPtr + 1024));
		WRITE_UINT16(rows[1] + 2, READ_UINT16(colorPtr + 1026));

		colorPtr = strip.v4_dither + (codebookIndex[2] << 2);
		WRITE_UINT16(rows[2] + 0, READ_UINT16(colorPtr + 2048));
		WRITE_UINT16(rows[3] + 0, READ_UINT16(colorPtr + 2050));

		colorPtr = strip.v4_dither + (codebookIndex[3] << 2);
		WRITE_UINT16(rows[2] + 2, READ_UINT16(colorPtr + 3072));
		WRITE_UINT16(rows[3] + 2, READ_UINT16(colorPtr + 3074));
	}
};

/**
 * Cinepak decoder.
 *
 * Used by BMP/AVI and PICT/QuickTime.
 *
 * Used in engines:
 *  - sherlock
 */
class CinepakDecoder
{
public:
	CinepakDecoder()
	{
		_curFrame.surface = 0;
		_curFrame.strips = 0;
		_y = 0;
		_colorMap = 0;
		_ditherPalette = 0;
		_ditherType = kDitherTypeUnknown;

		// Create a lookup for the clip function
		// This dramatically improves the performance of the color conversion
		_clipTableBuf = new uint8_t[1024];

		for (uint i = 0; i < 1024; i++)
		{
			if (i <= 512)
				_clipTableBuf[i] = 0;
			else if (i >= 768)
				_clipTableBuf[i] = 255;
			else
				_clipTableBuf[i] = i - 512;
		}

		_clipTable = _clipTableBuf + 512;
	}

	~CinepakDecoder()
	{
		if (_curFrame.surface)
		{
			_curFrame.surface->free();
			delete _curFrame.surface;
		}

		delete[] _curFrame.strips;
		delete[] _clipTableBuf;

		delete[] _colorMap;
		delete[] _ditherPalette;
	}

	const Surface *decodeFrame(FrameData &stream)
	{
		_curFrame.flags = stream.readByte();
		_curFrame.length = (stream.readByte() << 16);
		_curFrame.length |= stream.readUint16BE();
		_curFrame.width = stream.readUint16BE();
		_curFrame.height = stream.readUint16BE();
		_curFrame.stripCount = stream.readUint16BE();

		log_i("Cinepak Frame: Width = %d, Height = %d, Strip Count = %d", _curFrame.width, _curFrame.height, _curFrame.stripCount);

		if (!_curFrame.strips)
		{
			_curFrame.strips = new CinepakStrip[_curFrame.stripCount];
			for (uint16_t i = 0; i < _curFrame.stripCount; i++)
			{
				initializeCodebook(i, 1);
				initializeCodebook(i, 4);
			}
		}

		// Borrowed from FFMPEG. This should cut out the extra data Cinepak for Sega has (which is useless).
		// The theory behind this is that this is here to confuse standard Cinepak decoders. But, we won't let that happen! ;)
		if (_curFrame.length != (uint32_t)stream.size())
		{
			if (stream.readUint16BE() == 0xFE00)
				stream.readUint32BE();
			else if ((stream.size() % _curFrame.length) == 0)
				stream.seek_delta(-2);
		}

		if (!_curFrame.surface)
		{
			_curFrame.surface = new Surface();
			_curFrame.surface->create(_curFrame.width, _curFrame.height);
		}

		_y = 0;

		for (uint16_t i = 0; i < _curFrame.stripCount; i++)
		{
			if (i > 0 && !(_curFrame.flags & 1))
			{ // Use codebooks from last strip

				for (uint16_t j = 0; j < 256; j++)
				{
					_curFrame.strips[i].v1_codebook[j] = _curFrame.strips[i - 1].v1_codebook[j];
					_curFrame.strips[i].v4_codebook[j] = _curFrame.strips[i - 1].v4_codebook[j];
				}

				// Copy the QuickTime dither tables
				memcpy(_curFrame.strips[i].v1_dither, _curFrame.strips[i - 1].v1_dither, 256 * 4 * 4 * 4);
				memcpy(_curFrame.strips[i].v4_dither, _curFrame.strips[i - 1].v4_dither, 256 * 4 * 4 * 4);
			}

			_curFrame.strips[i].id = stream.readUint16BE();
			_curFrame.strips[i].length = stream.readUint16BE() - 12; // Subtract the 12 uint8_t header
			_curFrame.strips[i].rect.top = _y;
			stream.readUint16BE(); // Ignore, substitute with our own.
			_curFrame.strips[i].rect.left = 0;
			stream.readUint16BE(); // Ignore, substitute with our own
			_curFrame.strips[i].rect.bottom = _y + stream.readUint16BE();
			_curFrame.strips[i].rect.right = _curFrame.width;
			stream.readUint16BE(); // Ignore, substitute with our own

			// Sanity check. Because Cinepak is based on 4x4 blocks, the width and height of each strip needs to be divisible by 4.
			assert(!(_curFrame.strips[i].rect.width() % 4) && !(_curFrame.strips[i].rect.height() % 4));

			uint32_t pos = stream.pos();

			while ((uint32_t)stream.pos() < (pos + _curFrame.strips[i].length) && !stream.eos())
			{
				uint8_t chunkID = stream.readByte();

				if (stream.eos())
					break;

				// Chunk Size is 24-bit, ignore the first 4 bytes
				uint32_t chunkSize = stream.readByte() << 16;
				chunkSize += stream.readUint16BE() - 4;

				int32_t startPos = stream.pos();

				switch (chunkID)
				{
				case 0x20:
				case 0x21:
				case 0x24:
				case 0x25:
					loadCodebook(stream, i, 4, chunkID, chunkSize);
					break;
				case 0x22:
				case 0x23:
				case 0x26:
				case 0x27:
					loadCodebook(stream, i, 1, chunkID, chunkSize);
					break;
				case 0x30:
				case 0x31:
				case 0x32:
					if (_ditherPalette)
						ditherVectors(stream, i, chunkID, chunkSize);
					else
						decodeVectors(stream, i, chunkID, chunkSize);
					break;
				default:
					// warning("Unknown Cinepak chunk ID %02x", chunkID);
					return _curFrame.surface;
				}

				if (stream.pos() != startPos + (int32_t)chunkSize)
					stream.seek(startPos + chunkSize);
			}

			_y = _curFrame.strips[i].rect.bottom;
		}

		return _curFrame.surface;
	}

	bool containsPalette() const { return _ditherPalette != 0; }
	const uint8_t *getPalette()
	{
		_dirtyPalette = false;
		return _ditherPalette;
	}
	bool hasDirtyPalette() const { return _dirtyPalette; }
	bool canDither(DitherType type) const { return (type == kDitherTypeVFW || type == kDitherTypeQT) && _bitsPerPixel == 24; }
	void setDither(DitherType type, const uint8_t *palette)
	{
		assert(canDither(type));

		delete[] _colorMap;
		delete[] _ditherPalette;

		_ditherPalette = new uint8_t[256 * 3];
		memcpy(_ditherPalette, palette, 256 * 3);

		_dirtyPalette = true;
		_ditherType = type;

		if (type == kDitherTypeVFW)
		{
			_colorMap = new uint8_t[221];

			for (int i = 0; i < 221; i++)
				_colorMap[i] = findNearestRGB(i);
		}
		else
		{
			// Generate QuickTime dither table
			// 4 blocks of 0x4000 bytes (RGB554 lookup)
			_colorMap = createQuickTimeDitherTable(palette, 256);
		}
	}

private:
	CinepakFrame _curFrame;
	int32_t _y;
	int _bitsPerPixel;
	uint8_t *_clipTable, *_clipTableBuf;

	uint8_t *_ditherPalette;
	bool _dirtyPalette;
	uint8_t *_colorMap;
	DitherType _ditherType;

	uint8_t *createQuickTimeDitherTable(const uint8_t *palette, uint colorCount)
	{
		uint8_t *buf = new uint8_t[0x10000]();

		Common::List<uint16_t> checkQueue;

		bool foundBlack = false;
		bool foundWhite = false;

		const uint8_t *palPtr = palette;

		// See what colors we have, and add them to the queue to check
		for (uint i = 0; i < colorCount; i++)
		{
			uint8_t r = *palPtr++;
			uint8_t g = *palPtr++;
			uint8_t b = *palPtr++;
			uint16_t n = (i << 8) | 1;
			uint16_t col = makeQuickTimeDitherColor(r, g, b);

			if (col == 0)
			{
				// Special case for close-to-black
				// The original did more here, but it effectively discarded the value
				// due to a poor if-check (whole 16-bit value instead of lower 8-bits).
				WRITE_UINT16(buf, n);
				foundBlack = true;
			}
			else if (col == 0x3FFF)
			{
				// Special case for close-to-white
				// The original did more here, but it effectively discarded the value
				// due to a poor if-check (whole 16-bit value instead of lower 8-bits).
				WRITE_UINT16(buf + 0x7FFE, n);
				foundWhite = true;
			}
			else
			{
				// Previously unfound color
				addColorToQueue(col, n, buf, checkQueue);
			}
		}

		// More special handling for white
		if (foundWhite)
			checkQueue.push_front(0x3FFF);

		// More special handling for black
		if (foundBlack)
			checkQueue.push_front(0);

		// Go through the list of colors we have and match up similar colors
		// to fill in the table as best as we can.
		while (!checkQueue.empty())
		{
			uint16_t col = checkQueue.front();
			checkQueue.pop_front();
			uint16_t index = READ_UINT16(buf + col * 2);

			uint32_t x = col << 4;
			if ((x & 0xFF) < 0xF0)
				addColorToQueue((x + 0x10) >> 4, index, buf, checkQueue);
			if ((x & 0xFF) >= 0x10)
				addColorToQueue((x - 0x10) >> 4, index, buf, checkQueue);

			uint32_t y = col << 7;
			if ((y & 0xFF00) < 0xF800)
				addColorToQueue((y + 0x800) >> 7, index, buf, checkQueue);
			if ((y & 0xFF00) >= 0x800)
				addColorToQueue((y - 0x800) >> 7, index, buf, checkQueue);

			uint32_t z = col << 2;
			if ((z & 0xFF00) < 0xF800)
				addColorToQueue((z + 0x800) >> 2, index, buf, checkQueue);
			if ((z & 0xFF00) >= 0x800)
				addColorToQueue((z - 0x800) >> 2, index, buf, checkQueue);
		}

		// Contract the table back to just palette entries
		for (int i = 0; i < 0x4000; i++)
			buf[i] = READ_UINT16(buf + i * 2) >> 8;

		// Now go through and distribute the error to three more pixels
		uint8_t *bufPtr = buf;
		for (uint realR = 0; realR < 0x100; realR += 8)
		{
			for (uint realG = 0; realG < 0x100; realG += 8)
			{
				for (uint realB = 0; realB < 0x100; realB += 16)
				{
					uint8_t palIndex = *bufPtr;
					uint8_t r = realR;
					uint8_t g = realG;
					uint8_t b = realB;

					uint8_t palR = palette[palIndex * 3] & 0xF8;
					uint8_t palG = palette[palIndex * 3 + 1] & 0xF8;
					uint8_t palB = palette[palIndex * 3 + 2] & 0xF0;

					r = adjustColorRange(r, realR, palR);
					g = adjustColorRange(g, realG, palG);
					b = adjustColorRange(b, realB, palB);
					palIndex = buf[makeQuickTimeDitherColor(r, g, b)];
					bufPtr[0x4000] = palIndex;

					palR = palette[palIndex * 3] & 0xF8;
					palG = palette[palIndex * 3 + 1] & 0xF8;
					palB = palette[palIndex * 3 + 2] & 0xF0;

					r = adjustColorRange(r, realR, palR);
					g = adjustColorRange(g, realG, palG);
					b = adjustColorRange(b, realB, palB);
					palIndex = buf[makeQuickTimeDitherColor(r, g, b)];
					bufPtr[0x8000] = palIndex;

					palR = palette[palIndex * 3] & 0xF8;
					palG = palette[palIndex * 3 + 1] & 0xF8;
					palB = palette[palIndex * 3 + 2] & 0xF0;

					r = adjustColorRange(r, realR, palR);
					g = adjustColorRange(g, realG, palG);
					b = adjustColorRange(b, realB, palB);
					palIndex = buf[makeQuickTimeDitherColor(r, g, b)];
					bufPtr[0xC000] = palIndex;

					bufPtr++;
				}
			}
		}

		return buf;
	}

	void initializeCodebook(uint16_t strip, uint8_t codebookType)
	{
		CinepakCodebook *codebook = (codebookType == 1) ? _curFrame.strips[strip].v1_codebook : _curFrame.strips[strip].v4_codebook;

		for (uint16_t i = 0; i < 256; i++)
		{
			memset(codebook[i].y, 0, 4);
			codebook[i].u = 0;
			codebook[i].v = 0;

			if (_ditherType == kDitherTypeQT)
				ditherCodebookQT(strip, codebookType, i);
		}
	}

	void loadCodebook(FrameData &stream, uint16_t strip, uint8_t codebookType, uint8_t chunkID, uint32_t chunkSize)
	{
		CinepakCodebook *codebook = (codebookType == 1) ? _curFrame.strips[strip].v1_codebook : _curFrame.strips[strip].v4_codebook;

		int32_t startPos = stream.pos();
		uint32_t flag = 0, mask = 0;

		for (uint16_t i = 0; i < 256; i++)
		{
			if ((chunkID & 0x01) && !(mask >>= 1))
			{
				if ((stream.pos() - startPos + 4) > (int32_t)chunkSize)
					break;

				flag = stream.readUint32BE();
				mask = 0x80000000;
			}

			if (!(chunkID & 0x01) || (flag & mask))
			{
				uint8_t n = (chunkID & 0x04) ? 4 : 6;
				if ((stream.pos() - startPos + n) > (int32_t)chunkSize)
					break;

				stream.read(codebook[i].y, 4);

				if (n == 6)
				{
					codebook[i].u = stream.readByte();
					codebook[i].v = stream.readByte();
				}
				else
				{
					// This codebook type indicates either greyscale or
					// palettized video. For greyscale, default us to
					// 0 for both u and v.
					codebook[i].u = 0;
					codebook[i].v = 0;
				}

				// Dither the codebook if we're dithering for QuickTime
				if (_ditherType == kDitherTypeQT)
					ditherCodebookQT(strip, codebookType, i);
			}
		}
	}

	void decodeVectors(FrameData &stream, uint16_t strip, uint8_t chunkID, uint32_t chunkSize)
	{
		decodeVectorsTmpl<uint16_t, CodebookConverterRaw>(_curFrame, _clipTable, _colorMap, stream, strip, chunkID, chunkSize);
	}

	uint8_t findNearestRGB(int index) const
	{
		int r = s_defaultPalette[index * 3];
		int g = s_defaultPalette[index * 3 + 1];
		int b = s_defaultPalette[index * 3 + 2];

		uint8_t result = 0;
		int diff = 0x7FFFFFFF;

		for (int i = 0; i < 256; i++)
		{
			int bDiff = b - (int)_ditherPalette[i * 3 + 2];
			int curDiffB = diff - (bDiff * bDiff);

			if (curDiffB > 0)
			{
				int gDiff = g - (int)_ditherPalette[i * 3 + 1];
				int curDiffG = curDiffB - (gDiff * gDiff);

				if (curDiffG > 0)
				{
					int rDiff = r - (int)_ditherPalette[i * 3];
					int curDiffR = curDiffG - (rDiff * rDiff);

					if (curDiffR > 0)
					{
						diff -= curDiffR;
						result = i;

						if (diff == 0)
							break;
					}
				}
			}
		}

		return result;
	}

	void ditherCodebookQT(uint16_t strip, uint8_t codebookType, uint16_t codebookIndex)
	{
		if (codebookType == 1)
		{
			const CinepakCodebook &codebook = _curFrame.strips[strip].v1_codebook[codebookIndex];
			uint8_t *output = _curFrame.strips[strip].v1_dither + (codebookIndex << 2);

			uint8_t *ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[0], codebook.u, codebook.v);
			output[0x000] = ditherEntry[0x0000];
			output[0x001] = ditherEntry[0x4000];
			output[0x400] = ditherEntry[0xC000];
			output[0x401] = ditherEntry[0x0000];

			ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[1], codebook.u, codebook.v);
			output[0x002] = ditherEntry[0x8000];
			output[0x003] = ditherEntry[0xC000];
			output[0x402] = ditherEntry[0x4000];
			output[0x403] = ditherEntry[0x8000];

			ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[2], codebook.u, codebook.v);
			output[0x800] = ditherEntry[0x4000];
			output[0x801] = ditherEntry[0x8000];
			output[0xC00] = ditherEntry[0x8000];
			output[0xC01] = ditherEntry[0xC000];

			ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[3], codebook.u, codebook.v);
			output[0x802] = ditherEntry[0xC000];
			output[0x803] = ditherEntry[0x0000];
			output[0xC02] = ditherEntry[0x0000];
			output[0xC03] = ditherEntry[0x4000];
		}
		else
		{
			const CinepakCodebook &codebook = _curFrame.strips[strip].v4_codebook[codebookIndex];
			uint8_t *output = _curFrame.strips[strip].v4_dither + (codebookIndex << 2);

			uint8_t *ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[0], codebook.u, codebook.v);
			output[0x000] = ditherEntry[0x0000];
			output[0x400] = ditherEntry[0x8000];
			output[0x800] = ditherEntry[0x4000];
			output[0xC00] = ditherEntry[0xC000];

			ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[1], codebook.u, codebook.v);
			output[0x001] = ditherEntry[0x4000];
			output[0x401] = ditherEntry[0xC000];
			output[0x801] = ditherEntry[0x8000];
			output[0xC01] = ditherEntry[0x0000];

			ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[2], codebook.u, codebook.v);
			output[0x002] = ditherEntry[0xC000];
			output[0x402] = ditherEntry[0x4000];
			output[0x802] = ditherEntry[0x8000];
			output[0xC02] = ditherEntry[0x0000];

			ditherEntry = _colorMap + createDitherTableIndex(_clipTable, codebook.y[3], codebook.u, codebook.v);
			output[0x003] = ditherEntry[0x0000];
			output[0x403] = ditherEntry[0x8000];
			output[0x803] = ditherEntry[0xC000];
			output[0xC03] = ditherEntry[0x4000];
		}
	}

	void ditherVectors(FrameData &stream, uint16_t strip, uint8_t chunkID, uint32_t chunkSize)
	{
		if (_ditherType == kDitherTypeVFW)
			decodeVectorsTmpl<uint8_t, CodebookConverterDitherVFW>(_curFrame, _clipTable, _colorMap, stream, strip, chunkID, chunkSize);
		else
			decodeVectorsTmpl<uint8_t, CodebookConverterDitherQT>(_curFrame, _clipTable, _colorMap, stream, strip, chunkID, chunkSize);
	}
};
