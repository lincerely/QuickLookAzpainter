#ifndef apd_v4_h
#define apd_v4_h

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef struct {
	char id[7];
	uint8_t ver;
	uint16_t header_size;
	uint32_t width,
			 height,
			 dpi;
	uint8_t bits;
	uint8_t coltype;
	uint8_t bgcol[3];
	uint16_t layer_count;
} Apd;

typedef struct {
	void* rgbdata;
	uint32_t width,
			 height;
} aPicture;

int load_picture(char *fpath, aPicture *pict);

#endif /* apd_v4_h */
