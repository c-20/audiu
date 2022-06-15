
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long           LONG;

#pragma pack(push)
#pragma pack(1)
typedef struct {
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BitmapHeader;
#pragma pack(pop)

typedef struct {
  int width, height;
  int bytes;
  int dlen;
  unsigned char *data;
} Texture;


//Texture *
bool loadtexture(const char *filename, GLuint *texture) {
  Texture t;
	// Test if the file exists
//	if (FILE *file = fopen(filename, "r")) {	// File exists?
//		fclose(file);
//	} else {
//		MSGBOX("Texture file does not exist.");
//		return false;
//	}
//	RGBImage textureImage;
	// Load the texture image
//	AUX_RGBImageRec *textureImage;
//	if (!(textureImage = auxDIBImageLoad(filename))) {
//		MSGBOX("Texture image could not be read.");
//		return false;
//	}

  FILE *file = fopen(filename, "rb");
  IF (!file) { LOG("TEXTURE READ FAILURE"); RT false; }
  BitmapHeader header;
  size_t readsize = fread(&header, sizeof(BitmapHeader), 1, file);
  IF (readsize EQ 0)
    { LOG("TEXTURE HEADER READ FAILURE"); fclose(file); RT false; }
  EF (header.bfType NQ 0x4D42)
    { LOG("TEXTURE NOT A BITMAP"); fclose(file); RT false; }
  t.width = (int)header.biWidth;
  t.height = abs((int)header.biHeight);
  IF (t.width <= 0 || t.height <= 0)
    { LOG("TEXTURE IMAGE SIZE FAIL"); fclose(file); RT false; }
  t.bytes = ((int)header.biBitCount) >> 3;	// number of bytes... because 2^3 == 8
  t.dlen = t.width * t.height * t.bytes;
  t.data = (unsigned char *)malloc(sizeof(unsigned char) * t.dlen);
  fseek(file, header.bfOffBits, SEEK_SET);
//	t->nv_n = (texheight * texwidth) << 1;
//	t->nv = (Coord *)malloc(sizeof(Coord) * (t->nv_n + extraSpace * 2));
//	unsigned char *line = (unsigned char *)malloc(sizeof(unsigned char) * texwidth * bytes);
	// Set the smaller axis to have points between 0 and 1
//	GLfloat size = max(1.0f / (GLfloat)(texwidth - 1), 1.0f / (GLfloat)(texheight - 1));
	// Read each character and define a vertex
  unsigned char *datap = t.data;
  int linelen = t.width * t.bytes;
  char upsidedown = ((int)header.biHeight < 0) ? 1 : 0;
  if (upsidedown) { datap = &t.data[t.dlen - linelen]; }
  int h = -1;
  while (++h < t.height) {
    readsize = fread(datap, 1, linelen, file);
    IF (readsize EQ 0)
      { LOG("TEXTURE IMAGE READ FAILURE"); fclose(file); RT false; }
    EF (upsidedown) { datap -= linelen; }
    EL { datap += linelen; }
  }
  GLenum format = (t.bytes == 3) ? GL_RGB : (t.bytes == 4) ? GL_RGBA : GL_RED;
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width, t.height, 0, format, GL_UNSIGNED_BYTE, t.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  free(t.data);
//    return t;
  RT true;
}
