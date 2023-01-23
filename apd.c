#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "apd.h"

#define INBUF_SIZE 1024

int mFILEreadBE16(FILE *fp, void *buf)
{
	uint8_t v[2];

	if(fread(v, 1, 2, fp) < 2)
		return 1;
	else
	{
		*((uint16_t *)buf) = ((uint16_t)v[0] << 8) | v[1];
		return 0;
	}
}

int mFILEreadBE32(FILE *fp, void *buf)
{
	uint8_t v[4];

	if(fread(v, 1, 4, fp) < 4)
		return 1;
	else
	{
		*((uint32_t *)buf) = ((uint32_t)v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
		return 0;
	}
}

static int _decode_pict_v4(FILE* fp, uint32_t csize, aPicture *pict)
{
	unsigned char inbuf[INBUF_SIZE];
	uint16_t ynum = 0;
	uint32_t encsize = 0; //encoded block size
	int ret;
	z_stream z = {0};

	// init zlib
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;
	if (inflateInit2(&z, -15) != Z_OK) { // window size 15, headerless zlib
		fprintf(stderr, "error: inflateInit: %s\n",
				(z.msg) ? z.msg : "???");
		inflateEnd(&z);
		return 1;
	}
	
	z.avail_out = pict->width * pict->height * 3;
	z.next_out = pict->rgbdata;
	
	while (csize > 0) {

		// reset zstream
		inflateReset(&z);

		// read block info
		mFILEreadBE16(fp, &ynum);
		mFILEreadBE32(fp, &encsize);
		csize -= (encsize + 6);

		// read
		z.avail_in = 0;
		while (1) {

			if (z.avail_in == 0 && encsize) {
				z.next_in = inbuf;
				if (encsize < INBUF_SIZE) {
					z.avail_in = fread(inbuf, 1, encsize, fp);
				} else {
					z.avail_in = fread(inbuf, 1, INBUF_SIZE, fp);
				}
				encsize -= z.avail_in;
			}

			ret = inflate(&z, Z_NO_FLUSH);
			if (ret == Z_STREAM_END) {
				break;
			} else if (ret != Z_OK) {
				fprintf(stderr, "error: inflate: %s\n",
						(z.msg) ? z.msg : "???");
				inflateEnd(&z);
				return 1;
			}
		}
	}

	if (inflateEnd(&z) != Z_OK) {
		fprintf(stderr, "error: inflateEnd: %s\n",
				(z.msg) ? z.msg : "???");
		return 1;
	}
	return 0;
}

static int _load_picture_v4(FILE *fp, aPicture *pict)
{
	Apdv4 apd = {0};
	char id[4];
	uint32_t csize;
	
	// read header
	mFILEreadBE16(fp, &apd.header_size);
	mFILEreadBE32(fp, &apd.width);
	mFILEreadBE32(fp, &apd.height);
	mFILEreadBE32(fp, &apd.dpi);
	fread(&apd.bits, 1, 1, fp);
	fread(&apd.coltype, 1, 1, fp);
	fread(&apd.bgcol, 3, 1, fp);
	mFILEreadBE16(fp, &apd.layer_count);

	pict->width = apd.width;
	pict->height = apd.height;
	pict->rgbdata = malloc(pict->width*pict->height*3);
	if (!pict->rgbdata) {
		fprintf(stderr, "failed to allocate image data\n");
		return -1;
	}
	
	fread(id, 4, 1, fp);
	if (strncmp(id, "pict", 4) != 0) {
		fprintf(stderr, "file doesn't contain full image\n");
		return -1;
	}
	mFILEreadBE32(fp, &csize);
	return _decode_pict_v4(fp, csize, pict);
}

static int _decode_pict_v3(FILE *fp, aPicture *pict)
{
	uint8_t col_type;
	uint8_t comp_type;
	uint32_t encsize;
	unsigned char inbuf[INBUF_SIZE];
	z_stream z = {0};
	int ret;
	
	fread(&col_type, 1, 1, fp);
	if (col_type != 0) {
		fprintf(stderr, "error: color type not supported (%d)\n", col_type);
		return 1;
	}
	fread(&comp_type, 1, 1, fp);
	if (comp_type != 0) {
		fprintf(stderr, "error: compress type not supported (%d)\n", comp_type);
		return 1;
	}
	mFILEreadBE32(fp, &encsize);

	// init zlib
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;
	if (inflateInit2(&z, 15) != Z_OK) { // window size 15, zlib with header
		fprintf(stderr, "error: inflateInit: %s\n",
				(z.msg) ? z.msg : "???");
		inflateEnd(&z);
		return 1;
	}
	
	z.avail_out = pict->width * pict->height * 3;
	z.next_out = pict->rgbdata;
	
	// read
	z.avail_in = 0;
	while (1) {

		if (z.avail_in == 0 && encsize) {
			z.next_in = inbuf;
			if (encsize < INBUF_SIZE) {
				z.avail_in = fread(inbuf, 1, encsize, fp);
			} else {
				z.avail_in = fread(inbuf, 1, INBUF_SIZE, fp);
			}
			encsize -= z.avail_in;
		}

		ret = inflate(&z, Z_NO_FLUSH);
		if (ret == Z_STREAM_END) {
			break;
		} else if (ret != Z_OK) {
			fprintf(stderr, "error: inflate: %s\n",
					(z.msg) ? z.msg : "???");
			inflateEnd(&z);
			return 1;
		}
	}

	if (inflateEnd(&z) != Z_OK) {
		fprintf(stderr, "error: inflateEnd: %s\n",
				(z.msg) ? z.msg : "???");
		return 1;
	}
	
	return 0;
}

static int _load_picture_v3(FILE *p, aPicture *pict)
{
	// read header
	Apdv3 apd = {0};
	mFILEreadBE32(p, &apd.width);
	mFILEreadBE32(p, &apd.height);
	fread(&apd.unit, 1, 1, p);
	mFILEreadBE32(p, &apd.dpi);
	
	if (apd.unit != 1 || apd.dpi < 1)
		apd.dpi = 96;

	pict->width = apd.width;
	pict->height = apd.height;
	pict->rgbdata = malloc(pict->width*pict->height*3);
	if (!pict->rgbdata) {
		fprintf(stderr, "failed to allocate image data\n");
		return -1;
	}
	
	char id[4];
	uint32_t block_size;
	while (1) {
		
		if (fread(&id, 4, 1, p) != 1) {
			fprintf(stderr, "error: failed to find BIMG chunk\n");
			return 1;
		}
		mFILEreadBE32(p, &block_size);
		if (strncmp(id, "BIMG", 4) == 0) {
			break;
		} else if (strncmp(id, "END ", 4) == 0) {
			fprintf(stderr, "error: block END reached\n");
			return 1;
		}
		fseek(p, block_size, SEEK_CUR);
		
	}
	
	return _decode_pict_v3(p, pict);
}

int load_picture(char *fpath, aPicture *pict)
{
	FILE *fp;
	char id[7];
	uint8_t ver;
	int ret;
	
	fp = fopen(fpath, "r");
	if (!fp) {
		fprintf(stderr, "failed to open file\n");
		return -1;
	}
	
	fread(&id, 7, 1, fp);
	if (strncmp(id, "AZPDATA", 7) != 0) {
		fprintf(stderr, "not azpainter data\n");
		fclose(fp);
		return -1;
	}
	
	fread(&ver, 1, 1, fp);
	if (ver == 3) {
		ret = _load_picture_v4(fp, pict);
	} else if (ver == 2) {
		ret = _load_picture_v3(fp, pict);
	} else {
		fprintf(stderr, "version %d is not supported\n", ver);
		fclose(fp);
		return -1;
	}
	
	fclose(fp);
	return ret;
}
