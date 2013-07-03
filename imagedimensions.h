/*	TOP-SURF: a visual words toolkit
	Copyright (C) 2010 LIACS Media Lab, Leiden University,
	                   Bart Thomee (bthomee@liacs.nl),
					   Erwin M. Bakker (erwin@liacs.nl)	and
					   Michael S. Lew (mlew@liacs.nl).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    See http://www.gnu.org/licenses/gpl.html for the full license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

	In addition, this work is covered under the Creative Commons
	Attribution license version 3.
    See http://creativecommons.org/licenses/by/3.0/ for the full license.
*/

#ifndef _IMAGEDIMENSIONSH
#define _IMAGEDIMENSIONSH

#pragma once

#include "config.h"

// swap endianness
inline void endian_swap_ui(unsigned int& x)
{
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

inline void endian_swap_us(unsigned short& x)
{
	x = ((x & 0xFF) << 8) | (x >> 8);
}

static bool ReadPngDimensions(FILE *file, int &width, int &height)
{
	// read the file header of the image to extract its width and height
	// Note: in PNG, the signature comes first, consisting of 8 bytes
	//       containing 0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A and then
	//       the length of the first chuck, the four character code of the
	//       chunk and then the chunk data itself. the IHDR must be the first
	//       chunk according to PNG specifications. the first four bytes of
	//       the IHDR is the width as integer, and the second four bytes is
	//       the height.
	// Note: PNG is stored in big-endian format, whereas x86 uses little-
	//       endian format, so we need to swap bytes around for everything
	//       larger than a byte
	unsigned char sig[8];
	unsigned int chunk_length;
	char chunk_code[4];
	unsigned int dim;
	// read the signature, chunk length and chunk code and compare them
	// to what they should be
	if (fread(&sig, sizeof(unsigned char), 8, file) != 8)
		return false;
	if (sig[0] != 0x89 || sig[1] != 0x50 || sig[2] != 0x4E || sig[3] != 0x47 ||
		sig[4] != 0x0D || sig[5] != 0x0A || sig[6] != 0x1A || sig[7] != 0x0A)
		return false;
	if (fread(&chunk_length, sizeof(unsigned int), 1, file) != 1)
		return false;
	if (chunk_length != 218103808) // Note: remember values are big-endian
		return false;
	if (fread(&chunk_code, sizeof(char), 4, file) != 4)
		return false;
	if (chunk_code[0] != 'I' || chunk_code[1] != 'H' || chunk_code[2] != 'D' || chunk_code[3] != 'R')
		return false;
	if (fread(&dim, sizeof(unsigned int), 1, file) != 1)
		return false;
	endian_swap_ui(dim);
	width = (int)dim;
	if (fread(&dim, sizeof(unsigned int), 1, file) != 1)
		return false;
	endian_swap_ui(dim);
	height = (int)dim;
	if (width == 0 || width > 10000 || height == 0 || height > 10000)
		return false;
	return true;
}

static bool ReadJpgDimensions(FILE *file, int &width, int &height)
{
	// read the file header of the image to extract its width and height
	// Note: in JPEG, the signature comes first, consisting of 2 SOI bytes
	//       containing 0xFF 0xD8 and 2 APP0 bytes containing 0xFF 0xE0.
	//       after that follows the length of the first chunk and the chunk
	//       data itself. in this first chunk, straight after the length, we
	//       find the five character code of the JPEG header chunk ('JFIF\0').
	//       after the first chunk, zero or more chunks will be found (each
	//       starting with 0xFF) before hitting the chunk that contains
	//       the width and height, where the character 0xC0 is found after
	//       the chunk starting character.
	// Note: JPEG is stored in big-endian format, whereas x86 uses little-
	//       endian format, so we need to swap bytes around for everything
	//       larger than a byte
	unsigned char soi[2];
	unsigned char app[2];
	unsigned short chunk_length;
	unsigned char temp;
	unsigned short dim;
	// check for valid JPEG image
	if (fread(&soi, sizeof(unsigned char), 2, file) != 2)
		return false;
	if (soi[0] != 0xFF || soi[1] != 0xD8)
		return false;
	if (fread(&app, sizeof(unsigned char), 2, file) != 2)
		return false;
	// we are looking for the APP0 marker (JFIF) or APP1 marker (EXIF)
	if (app[0] == 0xFF && app[1] == 0xE0)
	{
		char jfif[5];
		unsigned char chunk_code;
		// read the length of the first chunk and then the
		// header itself, which should be the jfif header
		if (fread(&chunk_length, sizeof(unsigned short), 1, file) != 1)
			return false;
		endian_swap_us(chunk_length);
		if (fread(&jfif, sizeof(char), 5, file) != 5)
			return false;
		if (jfif[0] != 'J' || jfif[1] != 'F' || jfif[2] != 'I' || jfif[3] != 'F' || jfif[4] != 0)
			return false;
		// jump to the first chunk
		// Note: length includes the field itself and the jfif marker
		for (unsigned short j = 0; j < chunk_length - sizeof(unsigned short) - sizeof(char) * 5; j++)
		{
			if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
				return false;
		}
		// start-of-frame marker
		while (true)
		{
			// check the first byte to see if we really are at the start
			// of a new chunk
			if (fread(&chunk_code, sizeof(unsigned char), 1, file) != 1)
				return false;
			if (chunk_code != 0xFF)
				return false;
			// check the next byte to see if we are at the start-of-frame
			if (fread(&chunk_code, sizeof(unsigned char), 1, file) != 1)
				return false;
			if (chunk_code == 0xC0)
			{
				// the structure of the this chunk is quite simple:
				// [ushort length][uchar precision][ushort height][ushort width]
				if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
					return false;
				if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
					return false;
				if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
					return false;
				if (fread(&dim, sizeof(unsigned short), 1, file) != 1)
					break;
				endian_swap_us(dim);
				height = (int)dim;
				if (fread(&dim, sizeof(unsigned short), 1, file) != 1)
					break;
				endian_swap_us(dim);
				width = (int)dim;
				if (width == 0 || width > 10000 || height == 0 || height > 10000)
					return false;
				break;
			}
			// read the next chunk length and jump to the next chunk
			if (fread(&chunk_length, sizeof(unsigned short), 1, file) != 1)
				return false;
			endian_swap_us(chunk_length);
			for (unsigned short j = 0; j < chunk_length - sizeof(unsigned short); j++)
			{
				if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
					return false;
			}
		};
	}
	else if (app[0] == 0xFF && app[1] == 0xE1)
	{
		char exif[6];
		char tiff[4];
		unsigned int offset;
		// read the length of the first chunk and then the
		// header itself, which should be the exif header
		if (fread(&chunk_length, sizeof(unsigned short), 1, file) != 1)
			return false;
		endian_swap_us(chunk_length);
		if (fread(&exif, sizeof(char), 6, file) != 6)
			return false;
		if (exif[0] != 'E' || exif[1] != 'x' || exif[2] != 'i' || exif[3] != 'f' ||	exif[4] != 0 || exif[5] != 0)
			return false;
		if (fread(&tiff, sizeof(char), 4, file) != 4)
			return false;
		// then we get the tiff header, which specifies the
		// alignment of the data
		if (tiff[0] == 'I' && tiff[1] == 'I')
		{
			// intel alignment
			if (tiff[2] != 0x2A || tiff[3] != 0x00)
				return false;
			// offset to the first IFD
			if (fread(&offset, sizeof(unsigned int), 1, file) != 1)
				return false;
			// jump to the first chunk
			// Note: length includes the field itself and the tiff marker
			for (unsigned int j = 0; j < offset - sizeof(char) * 4; j++)
			{
				if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
					return false;
			}
			// for now, indicate that it is likely a valid image, but
			// also return that at the moment we don't know what the
			// resolution exactly is, as this TIFF block is quite
			// complicated
			width = 0;
			height = 0;
		}
		else if (tiff[0] == 'M' && tiff[1] == 'M')
		{
			// motorola alignment
			if (tiff[2] != 0x00 || tiff[3] != 0x2A)
				return false;
			// offset to the first IFD
			if (fread(&offset, sizeof(unsigned int), 1, file) != 1)
				return false;
			endian_swap_ui(offset);
			// jump to the first chunk
			// Note: length includes the field itself and the tiff marker
			for (unsigned int j = 0; j < offset - sizeof(char) * 4; j++)
			{
				if (fread(&temp, sizeof(unsigned char), 1, file) != 1)
					return false;
			}
			// for now, indicate that it is likely a valid image, but
			// also return that at the moment we don't know what the
			// resolution exactly is, as this TIFF block is quite
			// complicated
			width = 0;
			height = 0;
		}
		else
			return false;
	}
	else
		return false;
	// indicate we successfully read the image
	return true;
}

#endif