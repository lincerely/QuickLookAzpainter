#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "apd_v4.h"

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

#define INBUF_SIZE 1024
int decode_pict(FILE* fp, uint32_t csize, aPicture *pict)
{
	unsigned char inbuf[INBUF_SIZE];
	uint16_t ynum = 0;
	uint32_t encsize = 0; //encoded block size
	int ret;

	// init zlib
	z_stream z = {0};
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

int load_picture(char *fpath, aPicture *pict)
{
	FILE *p;
	p = fopen(fpath, "r");
	if (!p) {
		fprintf(stderr, "failed to open file\n");
		return -1;
	}

	Apd apd = {0};
	fread(&apd.id, 7, 1, p);
	if (strncmp(apd.id, "AZPDATA", 7) != 0) {
		fprintf(stderr, "not azpainter data\n");
		fclose(p);
		return -1;
	}
	fread(&apd.ver, 1, 1, p);
	if (apd.ver != 3) {
		fprintf(stderr, "need version 4\n");
		fclose(p);
		return -1;
	}
	mFILEreadBE16(p, &apd.header_size);
	mFILEreadBE32(p, &apd.width);
	mFILEreadBE32(p, &apd.height);
	mFILEreadBE32(p, &apd.dpi);
	fread(&apd.bits, 1, 1, p);
	fread(&apd.coltype, 1, 1, p);
	fread(&apd.bgcol, 3, 1, p);
	mFILEreadBE16(p, &apd.layer_count);

	pict->width = apd.width;
	pict->height = apd.height;
	pict->rgbdata = malloc(apd.width*apd.height*3);
	if (!pict->rgbdata) {
		fprintf(stderr, "failed to allocate image data\n");
		fclose(p);
		return -1;
	}
	
	char pid[4];
	uint32_t csize;
	fread(pid, 4, 1, p);
	if (strncmp(pid, "pict", 4) != 0) {
		fprintf(stderr, "file doesn't contain full image\n");
		fclose(p);
		return -1;
	}
	mFILEreadBE32(p, &csize);
	if(decode_pict(p, csize, pict) != 0) {
		fclose(p);
		return -1;
	}

	fclose(p);
	return 0;
}
