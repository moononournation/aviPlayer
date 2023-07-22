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

class FrameData
{
public:
	void setData(uint8_t *data, int32_t size)
	{
		_buffer = data;
		_size = size;
		_pos = 0;
	}
	int32_t size()
	{
		return _size;
	}
	int32_t pos()
	{
		return _pos;
	}
	void seek(int32_t pos)
	{
		_pos = pos;
	}
	void seek_delta(int32_t delta)
	{
		_pos += delta;
	}
	bool eos()
	{
		return _pos >= (_size - 1);
	}
	void read(uint8_t *dst, int32_t len)
	{
		while (--len)
		{
			*dst++ = _buffer[_pos++];
		}
	}
	uint8_t readUint8()
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
	uint8_t *_buffer;
	int32_t _size;
	int32_t _pos;
};

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
		this->pixels = malloc(w * h * 2);
	}

	void free()
	{
		::free(pixels);
		pixels = 0;
		w = h = 0;
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
	}

	Surface *decodeFrame(FrameData &stream)
	{
		_curFrame.flags = stream.readUint8();
		_curFrame.length = (stream.readUint8() << 16);
		_curFrame.length |= stream.readUint16BE();
		_curFrame.width = stream.readUint16BE();
		_curFrame.height = stream.readUint16BE();
		_curFrame.stripCount = stream.readUint16BE();

		// log_i("Cinepak Frame: Width = %d, Height = %d, Strip Count = %d", _curFrame.width, _curFrame.height, _curFrame.stripCount);

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
				uint8_t chunkID = stream.readUint8();

				if (stream.eos())
					break;

				// Chunk Size is 24-bit, ignore the first 4 bytes
				uint32_t chunkSize = stream.readUint8() << 16;
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
					decodeVectors(stream, i, chunkID, chunkSize);
					break;
				default:
					log_i("Unknown Cinepak chunk ID %02x", chunkID);
					return _curFrame.surface;
				}

				if (stream.pos() != startPos + (int32_t)chunkSize)
					stream.seek(startPos + chunkSize);
			}

			_y = _curFrame.strips[i].rect.bottom;
		}

		return _curFrame.surface;
	}

private:
	CinepakFrame _curFrame;
	int32_t _y;
	int _bitsPerPixel;
	uint8_t *_clipTable, *_clipTableBuf;

	uint8_t *_colorMap;

	void initializeCodebook(uint16_t strip, uint8_t codebookType)
	{
		// log_i("initializeCodebook(%d, %d)", strip, codebookType);
		CinepakCodebook *codebook = (codebookType == 1) ? _curFrame.strips[strip].v1_codebook : _curFrame.strips[strip].v4_codebook;

		for (uint16_t i = 0; i < 256; i++)
		{
			memset(codebook[i].y, 0, 4);
			codebook[i].u = 0;
			codebook[i].v = 0;
		}
	}

	void loadCodebook(FrameData &stream, uint16_t strip, uint8_t codebookType, uint8_t chunkID, uint32_t chunkSize)
	{
		// log_i("loadCodebook(stream, %d, %d, %d, %d)", strip, codebookType, chunkID, chunkSize);
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
					codebook[i].u = stream.readUint8();
					codebook[i].v = stream.readUint8();
				}
				else
				{
					// This codebook type indicates either greyscale or
					// palettized video. For greyscale, default us to
					// 0 for both u and v.
					codebook[i].u = 0;
					codebook[i].v = 0;
				}
			}
		}
	}

	void putPixelRaw(uint16_t *dst, uint8_t y, int8_t u, int8_t v)
	{
		uint8_t r, g, b;
		r = _clipTable[y + (v << 1)];
		g = _clipTable[y - (u >> 1) - v];
		b = _clipTable[y + (u << 1)];
		*dst = ((( 0xF8 & r) << 8) | ((0xFC & g) << 3) | ((b) >> 3));
	}

	void decodeBlock1(uint8_t codebookIndex, CinepakStrip &strip, uint16_t *(&rows)[4])
	{
		// log_i("CodebookConverterRaw.decodeBlock1()");
		CinepakCodebook *codebook = &(strip.v1_codebook[codebookIndex]);
		putPixelRaw(rows[0] + 0, codebook->y[0], codebook->u, codebook->v);
		putPixelRaw(rows[0] + 1, codebook->y[0], codebook->u, codebook->v);
		putPixelRaw(rows[1] + 0, codebook->y[0], codebook->u, codebook->v);
		putPixelRaw(rows[1] + 1, codebook->y[0], codebook->u, codebook->v);

		putPixelRaw(rows[0] + 2, codebook->y[1], codebook->u, codebook->v);
		putPixelRaw(rows[0] + 3, codebook->y[1], codebook->u, codebook->v);
		putPixelRaw(rows[1] + 2, codebook->y[1], codebook->u, codebook->v);
		putPixelRaw(rows[1] + 3, codebook->y[1], codebook->u, codebook->v);

		putPixelRaw(rows[2] + 0, codebook->y[2], codebook->u, codebook->v);
		putPixelRaw(rows[2] + 1, codebook->y[2], codebook->u, codebook->v);
		putPixelRaw(rows[3] + 0, codebook->y[2], codebook->u, codebook->v);
		putPixelRaw(rows[3] + 1, codebook->y[2], codebook->u, codebook->v);

		putPixelRaw(rows[2] + 2, codebook->y[3], codebook->u, codebook->v);
		putPixelRaw(rows[2] + 3, codebook->y[3], codebook->u, codebook->v);
		putPixelRaw(rows[3] + 2, codebook->y[3], codebook->u, codebook->v);
		putPixelRaw(rows[3] + 3, codebook->y[3], codebook->u, codebook->v);
	}

	void decodeBlock4(uint8_t (&codebookIndex)[4], CinepakStrip &strip, uint16_t *(&rows)[4])
	{
		// log_i("CodebookConverterRaw.decodeBlock4()");
		CinepakCodebook *codebook1 = &(strip.v4_codebook[codebookIndex[0]]);
		putPixelRaw(rows[0] + 0, codebook1->y[0], codebook1->u, codebook1->v);
		putPixelRaw(rows[0] + 1, codebook1->y[1], codebook1->u, codebook1->v);
		putPixelRaw(rows[1] + 0, codebook1->y[2], codebook1->u, codebook1->v);
		putPixelRaw(rows[1] + 1, codebook1->y[3], codebook1->u, codebook1->v);

		CinepakCodebook *codebook2 = &(strip.v4_codebook[codebookIndex[1]]);
		putPixelRaw(rows[0] + 2, codebook2->y[0], codebook2->u, codebook2->v);
		putPixelRaw(rows[0] + 3, codebook2->y[1], codebook2->u, codebook2->v);
		putPixelRaw(rows[1] + 2, codebook2->y[2], codebook2->u, codebook2->v);
		putPixelRaw(rows[1] + 3, codebook2->y[3], codebook2->u, codebook2->v);

		CinepakCodebook *codebook3 = &(strip.v4_codebook[codebookIndex[2]]);
		putPixelRaw(rows[2] + 0, codebook3->y[0], codebook3->u, codebook3->v);
		putPixelRaw(rows[2] + 1, codebook3->y[1], codebook3->u, codebook3->v);
		putPixelRaw(rows[3] + 0, codebook3->y[2], codebook3->u, codebook3->v);
		putPixelRaw(rows[3] + 1, codebook3->y[3], codebook3->u, codebook3->v);

		CinepakCodebook *codebook4 = &(strip.v4_codebook[codebookIndex[3]]);
		putPixelRaw(rows[2] + 2, codebook4->y[0], codebook4->u, codebook4->v);
		putPixelRaw(rows[2] + 3, codebook4->y[1], codebook4->u, codebook4->v);
		putPixelRaw(rows[3] + 2, codebook4->y[2], codebook4->u, codebook4->v);
		putPixelRaw(rows[3] + 3, codebook4->y[3], codebook4->u, codebook4->v);
	}

	void decodeVectors(FrameData &stream, uint16_t strip, uint8_t chunkID, uint32_t chunkSize)
	{
		uint32_t flag = 0, mask = 0;
		uint16_t *iy[4];
		int32_t startPos = stream.pos();

		for (uint16_t y = _curFrame.strips[strip].rect.top; y < _curFrame.strips[strip].rect.bottom; y += 4)
		{
			iy[0] = ((uint16_t *)_curFrame.surface->pixels) + (y * _curFrame.width) + _curFrame.strips[strip].rect.left;
			iy[1] = iy[0] + _curFrame.width;
			iy[2] = iy[1] + _curFrame.width;
			iy[3] = iy[2] + _curFrame.width;

			for (uint16_t x = _curFrame.strips[strip].rect.left; x < _curFrame.strips[strip].rect.right; x += 4)
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
						uint8_t codebookIndex = stream.readUint8();
						decodeBlock1(codebookIndex, _curFrame.strips[strip], iy);
					}
					else if (flag & mask)
					{
						if ((stream.pos() - startPos + 4) > (int32_t)chunkSize)
							return;

						uint8_t codebookIndex[4];
						stream.read(codebookIndex, 4);
						decodeBlock4(codebookIndex, _curFrame.strips[strip], iy);
					}
				}

				for (uint8_t i = 0; i < 4; i++)
					iy[i] += 4;
			}
		}
	}
};