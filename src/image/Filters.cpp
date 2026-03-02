#include <merge/Filters.h>

void Filters::Resize(Layer* layer, size_t scale, byte filter) {
	size_t width, height;
	double k = (double)scale / 100.0;
	width = size_t(k * layer->getInfo().width);
	height = size_t(k * layer->getInfo().height);
	switch(filter) {
		case BILINEAR:
			try {
				BilinearResize(layer, width, height);
			} catch (Exception e) { throw e; }
			break;
		default:
			throw Exception(0x10, "Filter not supported");
	}
}

void Filters::BilinearResize(Layer* layer, size_t width, size_t height) {
	Header info = layer->getInfo();
	size_t i, j, l, c;
	double t, u, temp;
	// Memory allocation for result bitmap
	byte** rows = layer->getImage();
	byte** dest = (byte**)malloc(sizeof(byte*) * height);
	if (dest == NULL) throw Exception(ERROR_MALLOC, "Could not allocate memory for rescaled bitmap");
	for(size_t i = 0; i < height; i++) {
		dest[i] = (byte*)malloc(sizeof(byte) * width * 4);
		if(dest[i] == NULL) {
			while(--i >= 0) if (dest[i]) free(dest[i]);
			free(dest);
			throw Exception(ERROR_MALLOC, "Could not allocate memory for rescaled bitmap");
		}
		memset(dest[i], 0x0, sizeof(byte) * width * 4);
	}
	// Bilinear Interpolation
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			temp = double(i) / (double)(height - 1) * (info.height - 1);
			l = (size_t)floor(temp);
			if (l >= info.height - 1) {
				l = info.height - 2;
			} else if (l < 0) l = 0;
			u = temp - l;
			temp = double(j) / (double)(width - 1) * (info.width - 1);
			c = (size_t)floor(temp);
			if (c >= info.width - 1) {
				c = info.width - 2;
			} else if (c < 0) c = 0;
			t = temp - c;
			// f(x,y) = f(0,0)(1-x)(1-y) + f(1,0)x(1-y) + f(0,1)(1-x)y + f(1,1)xy
			dest[i][j * 4    ] =
				(1 - t) * (1 - u) * rows[l    ][ c      * 4    ] +
				     t  * (1 - u) * rows[l    ][(c + 1) * 4    ] +
				     t  *      u  * rows[l + 1][(c + 1) * 4    ] +
				(1 - t) *      u  * rows[l + 1][ c      * 4    ];
			dest[i][j * 4 + 1] =
				(1 - t) * (1 - u) * rows[l    ][ c      * 4 + 1] +
				     t  * (1 - u) * rows[l    ][(c + 1) * 4 + 1] +
				     t  *      u  * rows[l + 1][(c + 1) * 4 + 1] +
				(1 - t) *      u  * rows[l + 1][ c      * 4 + 1];
			dest[i][j * 4 + 2] =
				(1 - t) * (1 - u) * rows[l    ][ c      * 4 + 2] +
				     t  * (1 - u) * rows[l    ][(c + 1) * 4 + 2] +
				     t  *      u  * rows[l + 1][(c + 1) * 4 + 2] +
				(1 - t) *      u  * rows[l + 1][ c      * 4 + 2];
			dest[i][j * 4 + 3] =
				(1 - t) * (1 - u) * rows[l    ][ c      * 4 + 3] +
				     t  * (1 - u) * rows[l    ][(c + 1) * 4 + 3] +
				     t  *      u  * rows[l + 1][(c + 1) * 4 + 3] +
				(1 - t) *      u  * rows[l + 1][ c      * 4 + 3];
		}
	}
	// Destroy source bitmap
	for(size_t i = 0; i < layer->info.height; free(rows[i++]));
	free(rows);
	// Set layer bitmap pointer to rescaled image
	layer->info.height = height;
	layer->info.width = width;
	layer->rows = dest;
}