#ifndef apd_h
#define apd_h

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef struct {
	uint16_t header_size;
	uint32_t width,
			 height,
			 dpi;
	uint8_t bits;
	uint8_t coltype;
	uint8_t bgcol[3];
	uint16_t layer_count;
} Apdv4;

typedef struct {
	uint32_t width,
			 height;
	uint8_t unit;
	uint32_t dpi;
} Apdv3;

typedef struct {
	void* rgbdata;
	uint32_t width,
			 height;
} aPicture;

int load_picture(char *fpath, aPicture *pict);

#endif /* apd_h */
