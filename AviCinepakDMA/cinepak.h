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

typedef void(DRAW_CALLBACK)(uint16_t x, uint16_t y, uint16_t *p, uint16_t w, uint16_t h);

struct Rect
{
	int16_t top, left;						  /*!< The point at the top left of the rectangle (part of the Rect). */
	int16_t bottom, right;					  /*!< The point at the bottom right of the rectangle (not part of the Rect). */
	int16_t width() { return right - left; }  /*!< Return the width of a rectangle. */
	int16_t height() { return bottom - top; } /*!< Return the height of a rectangle. */
};

struct Surface
{
	int16_t w;
	int16_t h;
	void *pixels;

	void create(int16_t w, int16_t h)
	{
		this->w = w;
		this->h = h;
		this->pixels = malloc(w * h * sizeof(uint16_t));
	}

	void free()
	{
		::free(pixels);
		pixels = 0;
		w = h = 0;
	}
};

struct CinepakStrip
{
	uint16_t id;
	uint16_t length;
	Rect rect;
	uint16_t v1_codebook[1024], v4_codebook[1024];
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
		_strips = 0;
		_y = 0;

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
		delete[] _strips;
		delete[] _clipTableBuf;
	}

	void decodeFrame(uint8_t *data, size_t data_size, uint16_t *output_buf, size_t output_buf_size, DRAW_CALLBACK *draw, bool iskeyframe)
	{
		_data = data;
		_data_size = data_size;
		_data_pos = 0;
		_output_buf = output_buf;
		_output_buf_size = output_buf_size;
		_draw = draw;
		_iskeyframe = iskeyframe;

		_flags = readUint8();
		_length = (readUint8() << 16);
		_length |= readUint16BE();
		_width = readUint16BE();
		_height = readUint16BE();
		_stripCount = readUint16BE();

		// log_i("Cinepak Frame: Width = %d, Height = %d, Strip Count = %d", _width, _height, _stripCount);

		if (!_strips)
		{
			_strips = new CinepakStrip[_stripCount];
		}

		// Borrowed from FFMPEG. This should cut out the extra data Cinepak for Sega has (which is useless).
		// The theory behind this is that this is here to confuse standard Cinepak decoders. But, we won't let that happen! ;)
		if (_length != (uint32_t)_data_size)
		{
			if (readUint16BE() == 0xFE00)
				readUint32BE();
			else if ((_data_size % _length) == 0)
				_data_pos -= 2;
		}

		_y = 0;

		for (uint16_t i = 0; i < _stripCount; i++)
		{
			if (i > 0 && !(_flags & 1))
			{ // Use codebooks from last strip

				for (uint16_t j = 0; j < 256; j++)
				{
					_strips[i].v1_codebook[j] = _strips[i - 1].v1_codebook[j];
					_strips[i].v4_codebook[j] = _strips[i - 1].v4_codebook[j];
				}
			}

			_strips[i].id = readUint16BE();
			_strips[i].length = readUint16BE() - 12; // Subtract the 12 uint8_t header
			_strips[i].rect.top = _y;
			_data_pos += 2; // Ignore, substitute with our own.
			_strips[i].rect.left = 0;
			_data_pos += 2; // Ignore, substitute with our own.
			_strips[i].rect.bottom = _y + readUint16BE();
			_strips[i].rect.right = _width;
			_data_pos += 2; // Ignore, substitute with our own.

			// Sanity check. Because Cinepak is based on 4x4 blocks, the width and height of each strip needs to be divisible by 4.
			assert(!(_strips[i].rect.width() % 4) && !(_strips[i].rect.height() % 4));

			uint32_t pos = _data_pos;

			while ((uint32_t)_data_pos < (pos + _strips[i].length) && (_data_pos < (_data_size - 1)))
			{
				uint8_t chunkID = readUint8();

				if (_data_pos >= (_data_size - 1))
					break;

				// Chunk Size is 24-bit, ignore the first 4 bytes
				uint32_t chunkSize = readUint8() << 16;
				chunkSize += readUint16BE() - 4;

				int32_t startPos = _data_pos;

				switch (chunkID)
				{
				case 0x20:
				case 0x21:
				case 0x24:
				case 0x25:
					loadCodebook(_strips[i].v4_codebook, chunkID, chunkSize);
					break;
				case 0x22:
				case 0x23:
				case 0x26:
				case 0x27:
					loadCodebook(_strips[i].v1_codebook, chunkID, chunkSize);
					break;
				case 0x30:
				case 0x31:
				case 0x32:
					decodeVectors(i, chunkID, chunkSize);
					break;
				default:
					log_i("Unknown Cinepak chunk ID %02x", chunkID);
					return;
				}

				if (_data_pos != startPos + (int32_t)chunkSize)
					_data_pos = startPos + chunkSize;
			}

			_y = _strips[i].rect.bottom;
		}

		return;
	}

private:
	uint8_t *_data;
	size_t _data_size;
	size_t _data_pos;
	uint16_t *_output_buf;
	size_t _output_buf_size;
	DRAW_CALLBACK *_draw;
	bool _iskeyframe;

	uint8_t _flags;
	uint32_t _length;
	uint16_t _width;
	uint16_t _height;
	uint16_t _stripCount;
	CinepakStrip *_strips;

	int32_t _y;
	uint8_t *_clipTable, *_clipTableBuf;

	inline uint8_t readUint8()
	{
		return _data[_data_pos++];
	}

	inline uint16_t readUint16BE()
	{
		uint16_t a = _data[_data_pos++];
		uint16_t b = _data[_data_pos++];
		return (a << 8) | b;
	}

	inline uint32_t readUint32BE()
	{
		uint32_t a = _data[_data_pos++];
		uint32_t b = _data[_data_pos++];
		uint32_t c = _data[_data_pos++];
		uint32_t d = _data[_data_pos++];
		return (a << 24) | (b << 16) | (c << 8) | d;
	}

	void putPixelRaw(uint16_t *dst, uint8_t y)
	{
		// *dst = (((0xF8 & y) << 8) | ((0xFC & y) << 3) | ((y) >> 3));
		*dst = (((0xF8 & y)) | ((y) >> 5) | ((0x1C & y) << 11) | ((0xF8 & y) << 5));
	}

	void putPixelRaw(uint16_t *dst, uint8_t y, int8_t u, int8_t v)
	{
		uint8_t r = _clipTable[y + (v << 1)];
		uint8_t g = _clipTable[y - (u >> 1) - v];
		uint8_t b = _clipTable[y + (u << 1)];
		// *dst = (((0xF8 & r) << 8) | ((0xFC & g) << 3) | ((b) >> 3));
		*dst = (((0xF8 & r)) | ((g) >> 5) | ((0x1C & g) << 11) | ((0xF8 & b) << 5));
	}

	void loadCodebook(uint16_t *codebook, uint8_t chunkID, uint32_t chunkSize)
	{
		// log_i("loadCodebook(%d, %d, %d)", chunkID, chunkSize);

		int32_t startPos = _data_pos;
		uint32_t flag = 0, mask = 0;

		for (uint16_t i = 0; i < 256; i++)
		{
			if ((chunkID & 0x01) && !(mask >>= 1))
			{
				if ((_data_pos - startPos + 4) > (int32_t)chunkSize)
					break;

				flag = readUint32BE();
				mask = 0x80000000;
			}

			if (!(chunkID & 0x01) || (flag & mask))
			{
				uint8_t n = (chunkID & 0x04) ? 4 : 6;
				if ((_data_pos - startPos + n) > (int32_t)chunkSize)
					break;

				uint8_t *y = _data + _data_pos;
				_data_pos += 4;
				uint16_t *p = codebook + (i << 2);
				if (n == 6)
				{
					int8_t u = readUint8();
					int8_t v = readUint8();
					putPixelRaw(p++, y[0], u, v);
					putPixelRaw(p++, y[1], u, v);
					putPixelRaw(p++, y[2], u, v);
					putPixelRaw(p, y[3], u, v);
				}
				else
				{
					// This codebook type indicates either greyscale or
					// palettized video. For greyscale, default us to
					// 0 for both u and v.
					putPixelRaw(p++, y[0]);
					putPixelRaw(p++, y[1]);
					putPixelRaw(p++, y[2]);
					putPixelRaw(p, y[3]);
				}
			}
		}
	}

	void decodeBlock1(uint8_t codebookIndex, CinepakStrip &strip, uint16_t *(&rows)[4])
	{
		// log_i("CodebookConverterRaw.decodeBlock1()");

		uint16_t *codebook = &(strip.v1_codebook[codebookIndex << 2]);
		rows[0][0] = codebook[0];
		rows[0][1] = codebook[0];
		rows[1][0] = codebook[0];
		rows[1][1] = codebook[0];

		rows[0][2] = codebook[1];
		rows[0][3] = codebook[1];
		rows[1][2] = codebook[1];
		rows[1][3] = codebook[1];

		rows[2][0] = codebook[2];
		rows[2][1] = codebook[2];
		rows[3][0] = codebook[2];
		rows[3][1] = codebook[2];

		rows[2][2] = codebook[3];
		rows[2][3] = codebook[3];
		rows[3][2] = codebook[3];
		rows[3][3] = codebook[3];
	}

	void decodeBlock4(uint8_t *codebookIndex, CinepakStrip &strip, uint16_t *(&rows)[4])
	{
		// log_i("CodebookConverterRaw.decodeBlock4()");

		uint16_t *codebook = &(strip.v4_codebook[codebookIndex[0] << 2]);
		rows[0][0] = codebook[0];
		rows[0][1] = codebook[1];
		rows[1][0] = codebook[2];
		rows[1][1] = codebook[3];

		codebook = &(strip.v4_codebook[codebookIndex[1] << 2]);
		rows[0][2] = codebook[0];
		rows[0][3] = codebook[1];
		rows[1][2] = codebook[2];
		rows[1][3] = codebook[3];

		codebook = &(strip.v4_codebook[codebookIndex[2] << 2]);
		rows[2][0] = codebook[0];
		rows[2][1] = codebook[1];
		rows[3][0] = codebook[2];
		rows[3][1] = codebook[3];

		codebook = &(strip.v4_codebook[codebookIndex[3] << 2]);
		rows[2][2] = codebook[0];
		rows[2][3] = codebook[1];
		rows[3][2] = codebook[2];
		rows[3][3] = codebook[3];
	}

	void decodeVectors(uint16_t strip, uint8_t chunkID, uint32_t chunkSize)
	{
		uint32_t flag = 0, mask = 0;
		uint16_t *iy[4];
		int32_t startPos = _data_pos;

		for (uint16_t y = _strips[strip].rect.top; y < _strips[strip].rect.bottom; y += 4)
		{
			iy[0] = _output_buf;
			iy[1] = iy[0] + (_iskeyframe ? _width : 4);
			iy[2] = iy[1] + (_iskeyframe ? _width : 4);
			iy[3] = iy[2] + (_iskeyframe ? _width : 4);

			for (uint16_t x = _strips[strip].rect.left; x < _strips[strip].rect.right; x += 4)
			{
				if ((chunkID & 0x01) && !(mask >>= 1))
				{
					if ((_data_pos - startPos + 4) > (int32_t)chunkSize)
						return;

					flag = readUint32BE();
					mask = 0x80000000;
				}

				if (!(chunkID & 0x01) || (flag & mask))
				{
					if (!(chunkID & 0x02) && !(mask >>= 1))
					{
						if ((_data_pos - startPos + 4) > (int32_t)chunkSize)
							return;

						flag = readUint32BE();
						mask = 0x80000000;
					}

					if ((chunkID & 0x02) || (~flag & mask))
					{
						if ((_data_pos - startPos + 1) > (int32_t)chunkSize)
							return;

						// Get the codebook
						uint8_t codebookIndex = readUint8();
						decodeBlock1(codebookIndex, _strips[strip], iy);
						if (!_iskeyframe)
						{
							_draw(x, y, _output_buf, 4, 4);
						}
					}
					else if (flag & mask)
					{
						if ((_data_pos - startPos + 4) > (int32_t)chunkSize)
							return;

						uint8_t *codebookIndex;
						codebookIndex = _data + _data_pos;
						_data_pos += 4;
						decodeBlock4(codebookIndex, _strips[strip], iy);
						if (!_iskeyframe)
						{
							_draw(x, y, _output_buf, 4, 4);
						}
					}
				}

				if (_iskeyframe)
				{
					for (uint8_t i = 0; i < 4; i++)
						iy[i] += 4;
				}
			}
			if (_iskeyframe)
			{
				_draw(0, y, _output_buf, _width, 4);
			}
		}
	}
};