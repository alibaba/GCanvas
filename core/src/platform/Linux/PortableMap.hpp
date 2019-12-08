/**
 * Portable Anymap Format Utility.
 *
 * Utility for loading and saving Portable Anymap Format (PNM) file,
 * including PBM, PGM and PPM file (both in ascii and binary):
 * * PBM (Portable BitMap Format)
 * * PGM (Portable GreyMap Format)
 * * PPM (Portable PixMap Format)
 *
 * @author Chaoya Li <harry75369@gmail.com>
 * @date   May 2013
 * @see    http://en.wikipedia.org/wiki/Netpbm_format
 * @see    http://www.fileformat.info/format/pbm/spec/index.htm
 * @todo   support for wider bytes in binary format
 */
#ifndef __PORTABLE_MAP_HPP___
#define __PORTABLE_MAP_HPP___

#include <errno.h>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include "utils.hpp"

class PortableMap {
public:
  typedef size_t SizeType;
  enum MAGIC_NUMBER {
    NONE = 0,
    P1, ///< ascii  PBM
    P2, ///< ascii  PGM
    P3, ///< ascii  PPM
    P4, ///< binary PBM
    P5, ///< binary PGM
    P6  ///< binary PPM
  };
  static inline const std::string &getID(SizeType i) {
    const static std::string ids[7] = {"NONE", "P1", "P2", "P3",
                                       "P4",   "P5", "P6"};
    return ids[i];
  }
  static const int LENGTH_LIMIT = 70;
  inline bool isPBM() const { return (_identifier == P1 || _identifier == P4); }
  inline bool isPGM() const { return (_identifier == P2 || _identifier == P5); }
  inline bool isPPM() const { return (_identifier == P3 || _identifier == P6); }

public:
  /** PortableMap constructor */
  PortableMap() : _binary(0), _ascii(0) {}
  /** PortableMap copy-constructor */
  PortableMap(const PortableMap &other) {
    _identifier = other._identifier;
    _width = other._width;
    _height = other._height;
    _maxVal = other._maxVal;
    _filename = other._filename;
    _binary = 0;
    _ascii = 0;
    if (other._binary) {
      SizeType binarySize = getBinarySize();
      reAllocBinary(binarySize);
      ASSERT(_binary);
      if (0 == memcpy(_binary, other._binary, binarySize)) {
        WARN("binary data copy failed!");
      }
    }
    if (other._ascii) {
      SizeType asciiSize = getAsciiSize();
      reAllocAscii(asciiSize);
      ASSERT(_ascii);
      if (0 == memcpy(_ascii, other._ascii, asciiSize)) {
        WARN("ascii data copy failed!");
      }
    }
  }
  /** PortableMap deconstructor */
  virtual ~PortableMap() { clean(); }
  /** PortableMap assigenment operator */
  PortableMap &operator=(const PortableMap &other) {
    _identifier = other._identifier;
    _width = other._width;
    _height = other._height;
    _maxVal = other._maxVal;
    _filename = other._filename;
    _binary = 0;
    _ascii = 0;
    if (other._binary) {
      SizeType binarySize = getBinarySize();
      reAllocBinary(binarySize);
      ASSERT(_binary);
      if (0 == memcpy(_binary, other._binary, binarySize)) {
        WARN("binary data copy failed!");
      }
    }
    if (other._ascii) {
      SizeType asciiSize = getAsciiSize();
      reAllocAscii(asciiSize);
      ASSERT(_ascii);
      if (0 == memcpy(_ascii, other._ascii, asciiSize)) {
        WARN("ascii data copy failed!");
      }
    }
    return *this;
  }

  /* piles of getters */
  inline const MAGIC_NUMBER &getIdentifier() const { return _identifier; }
  inline const SizeType &getWidth() const { return _width; }
  inline const SizeType &getHeight() const { return _height; }
  inline const SizeType &getMaxValue() const { return _maxVal; }
  inline const std::string &getFilename() const { return _filename; }
  inline MAGIC_NUMBER &identifier() { return _identifier; }
  inline SizeType &width() { return _width; }
  inline SizeType &height() { return _height; }
  inline SizeType &maxValue() { return _maxVal; }
  inline std::string &filename() { return _filename; }
  inline const unsigned char *getBinaryData() {
    ASSERT(loaded());
    if (!_binary) {
      ascii2binary();
    }
    return _binary;
  }
  inline const SizeType *getAsciiData() {
    ASSERT(loaded());
    if (!_ascii) {
      binary2ascii();
    }
    return _ascii;
  }

  /* piles of setters */
  inline void setIdentifier(MAGIC_NUMBER id) { _identifier = id; }
  inline void setWidth(const SizeType &w) { _width = w; }
  inline void setHeight(const SizeType &h) { _height = h; }
  inline void setMaxValue(const SizeType &m) { _maxVal = m; }
  inline void setFilename(const std::string &f) { _filename = f; }
  inline void setBinaryData(unsigned char *b) { _binary = b; };
  inline void setAsciiData(SizeType *a) { _ascii = a; }

  /** load a portable map file.
   *
   * @param filename the name of the portable map file.
   */
  void load(const std::string &filename) {
    // process filename
    FILE *fp = fopen(filename.c_str(), "r");
    if (!fp) {
      WARN("File read failed: %s", strerror(errno));
      return;
    }
    setFilename(filename);

    // process magic number
    MAGIC_NUMBER id = NONE;
    std::string token = getNextToken(fp);
    for (int i = 1; i <= 6; i++)
      if (token == getID(i)) {
        id = (MAGIC_NUMBER)i;
        break;
      }
    if (id == NONE) {
      WARN("Invalid magic number!");
      fclose(fp);
      return;
    }
    setIdentifier(id);

    // process width and height
    token = getNextToken(fp);
    int w = str2int(token);
    if (w <= 0) {
      WARN("Invalid width!");
      fclose(fp);
      return;
    }
    setWidth(w);
    token = getNextToken(fp);
    int h = str2int(token);
    if (h <= 0) {
      WARN("Invalid height!");
      fclose(fp);
      return;
    }
    setHeight(h);

    // process max value
    if (_identifier == P1 || _identifier == P4) {
      setMaxValue(1);
    } else {
      token = getNextToken(fp);
      int m = str2int(token);
      if (m <= 0) {
        WARN("Invalid max value!");
        fclose(fp);
        return;
      } else if (_identifier > P3 && m > 255) {
        WARN("Invalid max value in binary file! (m=%d)", m);
        fclose(fp);
        return;
      }
      setMaxValue(m);
    }
    readMap(fp);
    fclose(fp);

    if (!_ascii && _binary) {
      binary2ascii();
    }
    if (!_binary && _ascii) {
      ascii2binary();
    }
  }

  inline bool loaded() const {
    if (_identifier != NONE && (_ascii || _binary)) {
      return true;
    }
    return false;
  }

  /** write a binary portable map file.
   *
   * @param filename the name of the portable map file.
   */
  void writeBinary(const std::string &filename) {
    if (_identifier < P4) // ascii format, need conversion
    {
      if (_maxVal > 255) {
        WARN("Invalid max value in binary file! (m=%ld)", _maxVal);
        return;
      }
      setIdentifier((MAGIC_NUMBER)((int)_identifier + 3));
    }
    write(filename);
  }

  /** write an ascii portable map file.
   *
   * @param filename the name of the portable map file.
   */
  void writeAscii(const std::string &filename) {
    if (_identifier > P3) // binary format, need conversion
    {
      setIdentifier((MAGIC_NUMBER)((int)_identifier - 3));
    }
    write(filename);
  }

  /** print the map */
  void print() {
    printf("Magic number: %s\n", getID(_identifier).c_str());
    printf("Width:        %ld\n", _width);
    printf("Height:       %ld\n", _height);
    printf("Max Value:    %ld\n", _maxVal);
    printf("Binary Size:  %ld\n", getBinarySize());
    printf("Ascii Size:   %ld\n", getAsciiSize());

    if (!_ascii && _binary) {
      binary2ascii();
    } else if (!_ascii) {
      WARN("no any data yet!");
      return;
    }
    // SizeType t = getAsciiSize();
    // SizeType width = _width;
    // if ( _identifier == P3 || _identifier == P6 ) width *= 3;
    // for ( SizeType i=0; i < t; i++ )
    //{
    // printf("%ld%c", _ascii[i], (((i+1)%width)?' ':'\n'));
    //}
  }

private:
  /** write a portable map file.
   *
   * @param filename the name of the portable map file.
   */
  void write(const std::string &filename) {
    // open file to write
    FILE *fp = fopen(filename.c_str(), "w");
    if (!fp) {
      WARN("File write failed: %s", strerror(errno));
      return;
    }

    // write magic number
    if (_identifier == NONE) {
      WARN("None magic number set!");
      fclose(fp);
      return;
    }
    if (_identifier > P3 && _maxVal > 255) {
      WARN("Max value greater than 255 in binary format!");
      fclose(fp);
      return;
    }
    fprintf(fp, "%s\n", getID(_identifier).c_str());

    // write width and height
    if (_width > 0 && _height > 0) {
      fprintf(fp, "%ld %ld\n", _width, _height);
    } else {
      WARN("Invalid width or height! (width=%ld, height=%ld)", _width, _height);
      fclose(fp);
      return;
    }

    // write max value
    if (_identifier != P1 && _identifier != P4 && _maxVal > 0) {
      fprintf(fp, "%ld\n", _maxVal);
    } else if (_maxVal <= 0) {
      WARN("Invalid max value! (maxVal=%ld)", _maxVal);
      fclose(fp);
      return;
    }
    writeMap(fp);
    fclose(fp);
  }

  void ascii2binary() {
    ASSERT(_ascii);
    SizeType binarySize = getBinarySize();
    SizeType asciiSize = getAsciiSize();
    reAllocBinary(binarySize); // assign binary data
    if (_identifier == P1 || _identifier == P4) {
      SizeType bc = 0;
      for (SizeType j = 0; j < _height; j++)
        for (SizeType i = 0; i < _width; i += 8) {
          unsigned char c = 0;
          for (SizeType k = 0; k < 8; k++) {
            c = c << 1;
            if (i + k < _width) {
              c |= ((_ascii[j * _width + i + k]) ? 1 : 0);
            }
          }
          _binary[bc++] = c;
        }
      ASSERT(bc == binarySize);
    } else {
      ASSERT(_maxVal < 256);
      ASSERT(binarySize == asciiSize);
      for (SizeType i = 0; i < asciiSize; i++)
        _binary[i] = (unsigned char)_ascii[i];
    }
  }

  void binary2ascii() {
    ASSERT(_binary);
    SizeType binarySize = getBinarySize();
    SizeType asciiSize = getAsciiSize();
    reAllocAscii(asciiSize); // alloc ascii data
    if (_identifier == P1 || _identifier == P4) {
      SizeType w = _width / 8 + ((_width % 8 == 0) ? 0 : 1);
      for (SizeType j = 0; j < _height; j++)
        for (SizeType i = 0; i < _width; i++) {
          _ascii[j * _width + i] =
              ((_binary[j * w + i / 8] & (1 << (7 - (i % 8)))) ? 1 : 0);
        }
    } else {
      ASSERT(binarySize == asciiSize);
      for (SizeType i = 0; i < asciiSize; i++)
        _ascii[i] = (SizeType)_binary[i];
    }
  }

private:
  /** eat white spaces in a file.
   *
   * @param fp reference to pointer to FILE.
   */
  static void eatWhitespaces(FILE *&fp) {
    if (!fp) {
      return;
    }

    // find a non-space character
    char c;
    do {
      c = fgetc(fp);
    } while (c != EOF && isspace(c));

    // eat comments
    while (c == '#') {
      // to the end of the line
      do {
        c = fgetc(fp);
      } while (c != EOF && c != '\n' && c != '\r');

      // and find a non-space charecter again
      while (c != EOF && isspace(c)) {
        c = fgetc(fp);
      }
    }

    // push the non-space character back for token reading
    if (c != EOF) {
      ASSERT_MSG(ungetc(c, fp) != EOF, "Unknown IO error!");
    }
  }

  /** get next token in a file.
   *
   * @param fp reference to pointer to FILE.
   * @param isP1 if true then read one character as a token.
   * @return token as std::string.
   */
  static std::string getNextToken(FILE *&fp, bool isP1 = false) {
    if (!fp) {
      return "";
    }

    eatWhitespaces(fp);

    std::ostringstream buf;
    char c;
    SizeType count = 0;
    SizeType max = 0x7fffffff; // should be enough?
    if (isP1) {
      max = 1;
    }
    while (count < max && !isspace(c = fgetc(fp))) {
      buf << c;
      count++;
    }
    return buf.str();
  }

  /** convert string to integer.
   *
   * @param str string representation of a integer.
   * @return corresponding integer if successfull, -1 otherwise.
   */
  static int str2int(const std::string &str) {
    int result;
    return (std::stringstream(str) >> result) ? result : -1;
  }

public:
  /** find actual number of bytes for binary data.
   *
   * @return actual number of bytes.
   */
  SizeType getBinarySize() const {
    ASSERT(_width > 0 && _height > 0);
    SizeType n = _width * _height;
    SizeType t = 0;
    switch (_identifier) {
    case P1:
    case P4:
      t = (_width / 8 + ((_width % 8 == 0) ? 0 : 1)) * _height;
      break;
    case P2:
    case P5:
      t = n;
      break;
    case P3:
    case P6:
      t = 3 * n;
      break;
    default:
      ASSERT_MSG(false, "Error magic number!");
    }
    return t;
  }

  /** find actual number of values for ascii data.
   *
   * @return actual number of values.
   */
  SizeType getAsciiSize() const {
    ASSERT(_width > 0 && _height > 0);
    SizeType n = _width * _height;
    SizeType t = n;
    if (_identifier == P3 || _identifier == P6) {
      t *= 3;
    }
    return t;
  }

private:
  /** write map body to a file.
   *
   * @param fp reference to pointer to FILE.
   */
  void writeMap(FILE *&fp) {
    ASSERT(_identifier != NONE);
    if (_identifier > P3) // binary format
    {
      SizeType t = getBinarySize();
      if (!_binary && _ascii) {
        ascii2binary();
      } else if (!_binary) {
        WARN("no any data yet!");
        return;
      }
      ASSERT(_binary);
      fwrite(_binary, sizeof(unsigned char), t, fp);
    } else // ascii format
    {
      SizeType t = getAsciiSize();
      if (!_ascii && _binary) {
        binary2ascii();
      } else if (!_ascii) {
        WARN("no any data yet!");
        return;
      }
      ASSERT(_ascii);
      int len = 0;
      for (int i = 0; i < t; i++) {
        int d = fprintf(fp, "%ld ", _ascii[i]);
        len += d;
        if (len > LENGTH_LIMIT) {
          fprintf(fp, "\n");
          len = 0;
        }
      }
    }
  }

  /** read map body from a file.
   *
   * @param fp reference to pointer to FILE.
   */
  void readMap(FILE *&fp) {
    ASSERT(_identifier != NONE);
    if (_identifier > P3) // binary format
    {
      SizeType t = getBinarySize();
      reAllocBinary(t);
      ASSERT(_binary);
      fread(_binary, sizeof(unsigned char), t, fp);
    } else // ascii format
    {
      SizeType t = getAsciiSize();
      reAllocAscii(t);
      ASSERT(_ascii);
      std::string token;
      SizeType rT = 0;
      while (rT != t) {
        token = getNextToken(fp, _identifier == P1);
        int v = str2int(token);
        if (v < 0) {
          WARN("Invalid pixel value! (%ld th value is %d)", rT, v);
          return;
        }
        _ascii[rT] = (SizeType)v;
        rT++;
      }
    }
  }

private:
  /** Re-alloc memory for binary data */
  inline void reAllocBinary(SizeType size) {
    if (_binary) {
      delete[] _binary;
    }
    _binary = new unsigned char[size];
  }
  /** Re-alloc memory for ascii data */
  inline void reAllocAscii(SizeType size) {
    if (_ascii) {
      delete[] _ascii;
    }
    _ascii = new SizeType[size];
  }
  /** Clean allocated memories */
  inline void clean() {
    if (_binary) {
      delete[] _binary;
    }
    if (_ascii) {
      delete[] _ascii;
    }
  }

private:
  MAGIC_NUMBER _identifier;
  SizeType _width;
  SizeType _height;
  SizeType _maxVal;
  std::string _filename;
  unsigned char *_binary; ///< binary data
  SizeType *_ascii;       ///< ascii data
};

class PPMWriter {
public:
  PPMWriter(int w, int h) : m_width(w), m_height(h), m_data(nullptr) {
    m_ppm.setIdentifier(PortableMap::P6);
    m_ppm.setWidth(m_width);
    m_ppm.setHeight(m_height);
    m_ppm.setMaxValue(255);
    m_data = new size_t[w * h * 3];
    ASSERT(m_data);
    m_ppm.setAsciiData(m_data);
  }

  ~PPMWriter() {
    m_ppm.setAsciiData(0);
    delete[] m_data;
  }

  void setPixel(int x, int y, double r, double g, double b) {
    m_data[3 * (x + y * m_width) + 0] = (size_t)r;
    m_data[3 * (x + y * m_width) + 1] = (size_t)g;
    m_data[3 * (x + y * m_width) + 2] = (size_t)b;
  }

  void writeFile(const std::string &fp) { m_ppm.writeBinary(fp); }

private:
  PortableMap m_ppm;
  int m_width;
  int m_height;
  size_t *m_data;
};

#endif //__PORTABLE_MAP_HPP__
