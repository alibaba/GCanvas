#ifndef __FONT_TOOL_HPP__
#define __FONT_TOOL_HPP__

#include "utils.hpp"
#include "./PortableMap.hpp"
#include <boost/filesystem.hpp>
#include <codecvt>
#include <fstream>
#include <ft2build.h>
#include <iostream>
#include <locale>
#include <memory>
#include <unordered_map>
#include <vector>
#include FT_FREETYPE_H

namespace NSFontTool
{

  inline bool walkDirectoryFiles(const std::string &dir,
                                 std::vector<std::string> &files)
  {
    if (!boost::filesystem::exists(dir))
    {
      FAIL("Directory not exists: %s", dir.c_str());
      return false;
    }
    if (!boost::filesystem::is_directory(dir))
    {
      FAIL("Not a directory: %s", dir.c_str());
      return false;
    }
    bool res = false;
    for (boost::filesystem::directory_entry &x :
         boost::filesystem::directory_iterator(dir))
    {
      auto &p = x.path();
      if (boost::filesystem::is_regular_file(p))
      {
        files.push_back(p.c_str());
        res = true;
      }
      else if (boost::filesystem::is_directory(p))
      {
        res = walkDirectoryFiles(p.c_str(), files) || res;
      }
    }
    return res;
  }

  inline std::string getLoweredFileExt(const std::string &fp)
  {
    int e = fp.find_last_of(".");
    if (e == fp.npos)
    {
      return fp;
    }
    std::string ext = fp.substr(e + 1, fp.size() - e);
    for (size_t i = 0; i < ext.size(); i++)
    {
      ext[i] = tolower(ext[i]);
    }
    return ext;
  }

  /**
 * TypefaceLoader
 *
 * TypefaceLoader is responsible for:
 * 1) loading typeface from font
 * 2) building typeface cache
 * 3) exporting/importing typeface cache
 */
  class TypefaceLoader
  {
  public:
    enum TypefaceSourceType
    {
      TST_LOCAL,
      TST_NET,
    };
    struct Typeface
    {
      // source info
      TypefaceSourceType sourceType;
      std::string source;
      size_t index;

      // actual face data
      FT_Face face;

      // search indices
      std::string psName;
      std::string familyName;
      std::string styleName;

      FT_Face getFace()
      {
        if (face)
        {
          return face;
        }

        TypefaceLoader *tl = TypefaceLoader::getInstance();
        ASSERT(tl);

        if (sourceType == TST_LOCAL)
        {
          if (0 != FT_New_Face(tl->getFreetype(), source.c_str(), index, &face))
          {
            FAIL("Failed to load typeface %lu from font: %s", index,
                 source.c_str());
          }
          ASSERT(face);
          // INFO("the get face called \n");
          return face;
        }
        else if (sourceType == TST_NET)
        {
          FAIL("Unimplemented: loadFace from TST_NET");
          return nullptr;
        }

        return nullptr;
      }
    };

  private:
    bool mInited;
    FT_Library mFreetype;
    std::vector<Typeface> mFaces;

    TypefaceLoader() : mInited(false), mFreetype(nullptr) {}

    TypefaceLoader(const TypefaceLoader &) = delete;

    TypefaceLoader &operator=(const TypefaceLoader &) = delete;

    TypefaceLoader *init()
    {
      if (mInited)
      {
        return this;
      }
      if (0 != FT_Init_FreeType(&mFreetype))
      {
        FAIL("Failed to init freetype.");
        return nullptr;
      }
      mInited = true;
      return this;
    }

  public:
    ~TypefaceLoader()
    {
      for (size_t i = 0; i < mFaces.size(); i++)
      {
        if (mFaces[i].face)
        {
          FT_Done_Face(mFaces[i].face);
        }
      }
      if (mFreetype)
      {
        FT_Done_FreeType(mFreetype);
      }
    }

    void clearFontCache()
    {
      for (size_t i = 0; i < mFaces.size(); i++)
      {
        if (mFaces[i].face)
        {
          FT_Done_Face(mFaces[i].face);
        }
      }
      mFaces.clear();
    }

    void dumpFontCache()
    {
      DEBUG("Number of cached typefaces: %lu", mFaces.size());
      for (size_t i = 0; i < mFaces.size(); i++)
      {
        auto &face = mFaces[i];
        DEBUG("Typeface: %s, %s. (%s)", face.familyName.c_str(),
              face.styleName.c_str(), face.psName.c_str());
        DEBUG("    source type: %s",
              (face.sourceType == TST_LOCAL) ? "TST_LOCAL" : "TST_NET");
        DEBUG("    source: %s", face.source.c_str());
        DEBUG("    index: %lu", face.index);
        DEBUG("    face: %p", face.face);
      }
    }

    FT_Library &getFreetype() { return mFreetype; }

    std::vector<Typeface> &getFaces() { return mFaces; }

    static TypefaceLoader *getInstance()
    {
      static std::unique_ptr<TypefaceLoader> instance(new TypefaceLoader());
      ASSERT(instance);
      return instance->init();
    }

  public:
    bool buildFontCacheFromDir(const std::string &dir)
    {
      std::vector<std::string> files;
      if (!walkDirectoryFiles(dir, files))
      {
        return false;
      }
      bool res = false;
      for (size_t i = 0; i < files.size(); i++)
      {
        const std::string &ext = getLoweredFileExt(files[i]);
        if (ext == "ttf" || ext == "ttc" || ext == "otf")
        {
          res = loadFontFromFile(files[i], true) || res;
        }
      }
      return res;
    }

    bool buildFontCacheFromUrl(const std::string &url)
    {
      return loadFontFromUrl(url, true);
    }

    bool exportFontCache(const std::string &fp)
    {
      // TODO add validity info in cache file

      std::ofstream cacheFile(fp, std::ios::out | std::ios::binary);
      if (!cacheFile.is_open())
      {
        FAIL("Failed to open file to export font cache: %s", fp.c_str());
        return false;
      }
      size_t nFaces = mFaces.size();
      cacheFile.write((const char *)(&nFaces), sizeof(size_t));
      INFO("Exporting %lu typefaces...", nFaces);
      for (size_t i = 0; i < nFaces; i++)
      {
        const Typeface &face = mFaces[i];
        cacheFile.write((const char *)(&face.sourceType),
                        sizeof(TypefaceSourceType));
        cacheFile.write((const char *)(&face.index), sizeof(size_t));
#define WRITE_STDSTR(s)                                    \
  do                                                       \
  {                                                        \
    size_t len = s.size();                                 \
    cacheFile.write((const char *)(&len), sizeof(size_t)); \
    if (len > 0)                                           \
    {                                                      \
      cacheFile.write((const char *)(&s[0]), len);         \
    }                                                      \
  } while (0)
        WRITE_STDSTR(face.source);
        WRITE_STDSTR(face.psName);
        WRITE_STDSTR(face.familyName);
        WRITE_STDSTR(face.styleName);
#undef WRITE_STDSTR
      }
      cacheFile.close();
      return true;
    }

    bool importFontCache(const std::string &fp)
    {
      std::ifstream cacheFile(fp, std::ios::in | std::ios::binary);
      if (!cacheFile.is_open())
      {
        FAIL("Failed to open file to import font cache: %s", fp.c_str());
        return false;
      }

      // TODO check validity before reading cache file

      size_t nFaces = 0;
      cacheFile.read((char *)(&nFaces), sizeof(size_t));
      // INFO("Importing %lu typefaces...", nFaces);
      mFaces.resize(nFaces);
      std::string home(getenv("HOME"));
      std::string pathPreFix = home + FONT_PATH;
      for (size_t i = 0; i < nFaces; i++)
      {
        Typeface &face = mFaces[i];
        cacheFile.read((char *)(&face.sourceType), sizeof(TypefaceSourceType));
        cacheFile.read((char *)(&face.index), sizeof(size_t));
        face.face = nullptr;
#define READ_STDSTR(s)                              \
  do                                                \
  {                                                 \
    size_t len = 0;                                 \
    cacheFile.read((char *)(&len), sizeof(size_t)); \
    if (len > 0)                                    \
    {                                               \
      s.resize(len);                                \
      cacheFile.read((char *)(&s[0]), len);         \
    }                                               \
  } while (0)
        READ_STDSTR(face.source);
        READ_STDSTR(face.psName);
        READ_STDSTR(face.familyName);
        READ_STDSTR(face.styleName);
#undef READ_STDSTR

        face.source = pathPreFix + face.source;
        // INFO("the face.source is %s \n",face.source.c_str());
        // INFO("the face.psName is %s \n",face.psName.c_str());
        // INFO("the face.familyName is %s \n",face.familyName.c_str());
        // INFO("the face.styleName is %s \n",face.styleName.c_str());
        // this->dumpFontCache();
      }

      cacheFile.close();
      return true;
    }

    bool loadFontFromFile(const std::string &fp, bool cache = false)
    {
      ASSERT(mInited);
      FT_Face face;
      size_t nLoadedTypeface = 0;
      INFO("Loading font from file: %s", fp.c_str());

      // find out the number of typefaces in this font
      if (0 != FT_New_Face(mFreetype, fp.c_str(), -1, &face))
      {
        FAIL("Failed to load typefaces from font: %s", fp.c_str());
        return false;
      }
      if (face->num_faces < 1)
      {
        FAIL("Zero typeface in this font: %s", fp.c_str());
        return false;
      }

      // load actual typefaces
      size_t nFaces = face->num_faces;
      for (size_t i = 0; i < nFaces; i++)
      {
        if (0 != FT_New_Face(mFreetype, fp.c_str(), i, &face))
        {
          FAIL("Failed to load typeface %lu from font: %s", i, fp.c_str());
          continue;
        }

        // get typeface names
        const char *ps = FT_Get_Postscript_Name(face);
        std::string psName = ps ? ps : "UNKNOWN_POSTSCRIPT_NAME";
        std::string faName = face->family_name ? face->family_name : "UNKNOWN_FAMILY_NAME";
        std::string stName = face->style_name ? face->style_name : "UNKNOWN_STYLE_NAME";
        INFO("Loading typeface: %s, %s. (%s)", faName.c_str(), stName.c_str(),
             psName.c_str());

        // skip fonts of invalid names
        if (!((psName.find("?") == psName.npos) && (faName.find("?") == faName.npos) && (stName.find("?") == stName.npos)) || psName.empty() || psName == "-" || stName.empty() || psName == "UNKNOWN_POSTSCRIPT_NAME" || faName == "UNKNOWN_FAMILY_NAME" || stName == "UNKNOWN_STYLE_NAME")
        {
          WARN("Skipping typeface of invalid names: %s, %s. (%s)",
               faName.c_str(), stName.c_str(), psName.c_str());
          FT_Done_Face(face);
          continue;
        }

        // reject non-vector-format typeface
        if (0 == (face->face_flags & FT_FACE_FLAG_SCALABLE))
        {
          WARN("Skipping non-vector-format typeface: %s, %s. (%s)",
               faName.c_str(), stName.c_str(), psName.c_str());
          FT_Done_Face(face);
          continue;
        }

        // if load only cache, destroy face to reduce memory usage
        if (cache)
        {
          if (0 != FT_Done_Face(face))
          {
            FAIL("Failed to destroy typeface for making cache: %s, %s. (%s)",
                 faName.c_str(), stName.c_str(), psName.c_str());
          }
          face = nullptr;
        }

        // push to face vector
        mFaces.push_back(Typeface({
            .sourceType = TypefaceSourceType::TST_LOCAL,
            .source = fp,
            .index = i,
            .face = face,
            .psName = psName,
            .familyName = faName,
            .styleName = stName,
        }));

        // increase counter
        nLoadedTypeface += 1;
      }

      if (nLoadedTypeface > 0)
      {
        return true;
      }
      return false;
    }

    bool loadFontFromUrl(const std::string &url, bool cache = false)
    {
      FAIL("Unimplemented: loadFontFromUrl");
      return false;
    }
  };

  /**
 * TypefaceProvider
 *
 * TypefaceProvider is responsible for:
 * 1) provide matching typefaces given name
 *   a) css-compatible style match
 *   b) sketch-compatible style match
 * 2) provide mathcing typeface given charcode and name
 * 3) provide fallback typeface given charcode and name
 * 4) provide some caching mechanism to improve matching
 */
  class TypefaceProvider
  {
  public:
    typedef TypefaceLoader::Typeface Typeface;

    struct CssFont
    {
      enum FontStyle
      {
        FS_NORMAL,
        FS_ITALIC,
        FS_OBLIQUE,
      };
      enum FontWeight
      {
        FW_NUMBER_100 = 100,
        FW_NUMBER_200 = 200,
        FW_NUMBER_300 = 300,
        FW_NORMAL = 400,
        FW_NUMBER_500 = 500,
        FW_NUMBER_600 = 600,
        FW_BOLD = 700,
        FW_NUMBER_800 = 800,
      };
      enum FontStretch
      {
        FST_ULTRA_CONDENSED,
        FST_EXTRA_CONDENSED,
        FST_CONDENSED,
        FST_SEMI_CONDENSED,
        FST_NORMAL,
        FST_SEMI_EXPANDED,
        FST_EXPANDED,
        FST_EXTRA_EXPANDED,
        FST_ULTRA_EXPANDED,
      };

      FontStyle fontStyle;
      FontWeight fontWeight;
      FontStretch fontStretch;
      double fontSize;
      std::vector<std::string> fontFamilies;
    };

  private:
    TypefaceLoader *mTypefaceLoader;
    const std::vector<Typeface *> cEmptyFaces;
    std::unordered_map<std::string, std::vector<Typeface *>> mTypefacesCache;

    TypefaceProvider() : mTypefaceLoader(TypefaceLoader::getInstance()) {}

    std::vector<Typeface *>
    selectTypefacesByPostscriptName(const std::string &name)
    {
      ASSERT(mTypefaceLoader);
      std::vector<Typeface *> faces;
      auto &tfs = mTypefaceLoader->getFaces();
      for (size_t i = 0; i < tfs.size(); i++)
      {
        auto &tf = tfs[i];
        std::size_t found = name.find(tf.psName);
        if (found != std::string::npos)
        {
           faces.push_back(&tf);
        }
      }
      return std::move(faces);
    }

    std::vector<Typeface *> selectTypefacesByCssFont(const std::string &font)
    {
      ASSERT(mTypefaceLoader);
      std::vector<Typeface *> faces;

      // TODO implement this

      return std::move(faces);
    }

  public:
    static TypefaceProvider *getInstance()
    {
      static TypefaceProvider instance;
      return &instance;
    }

    const std::vector<Typeface *> &selectTypefaces(const std::string &key = "")
    {
      ASSERT(mTypefaceLoader);

      // return typefaces if already in cache
      auto it = mTypefacesCache.find(key);

      if (it != mTypefacesCache.end())
      {
        return it->second;
      }

      // assumption: latter typeface has higher priority
      std::vector<Typeface *> faces;

      // empty key means all available typefaces
      if (key.empty())
      {
        auto &tfs = mTypefaceLoader->getFaces();
        for (size_t i = 0; i < tfs.size(); i++)
        {
          faces.push_back(&tfs[i]);
        }

        // sort all available typeface by family name
        // in reverse order in accordance with assumption
        std::sort(faces.begin(), faces.end(), [](Typeface *a, Typeface *b) {
          return (a->familyName.compare(b->familyName) < 0);
        });

        mTypefacesCache[key] = std::move(faces);
        return mTypefacesCache[key];
      }

      // try postscript name first
      faces = selectTypefacesByPostscriptName(key);
      if (!faces.empty())
      {
        mTypefacesCache[key] = std::move(faces);
        return mTypefacesCache[key];
      }

      // then try css font rule
      faces = selectTypefacesByCssFont(key);
      if (!faces.empty())
      {
        mTypefacesCache[key] = std::move(faces);
        return mTypefacesCache[key];
      }

      // return empty result
      return cEmptyFaces;
    }

    Typeface *selectTypeface(wchar_t charcode, const std::string &key = "")
    {
      const std::vector<Typeface *> &faces = selectTypefaces(key);
      // INFO("the key is %s \n", key.c_str());
      for (auto it = faces.rbegin(); it != faces.rend(); it++)
      {
        FT_Face face = (*it)->getFace();
        std::string str = (*it)->psName;
        // INFO("the it fontName is %s \n", str.c_str());
        FT_UInt glyphIndex = FT_Get_Char_Index(face, charcode);
        if (glyphIndex != 0)
        {
          return (*it);
        }
      }
      return nullptr;
    }

    Typeface *selectFallbackTypeface(const std::string &key = "")
    {
      const std::vector<Typeface *> &faces = selectTypefaces(key);
      if (faces.empty())
      {
        if (key.empty())
        {
          WARN("Not even single fallback typeface is available.");
        }
        return nullptr;
      }
      return faces.back();
    }
  };

  /**
 * TextManager
 *
 * TextManager is responsible for:
 * 1) setting up and managing current typeface
 *   a) set up correct typeface given font name and charcode
 *   b) set up correct charmap of current typeface based on given charcode
 *   c) set up correct size of current typeface based on given font size
 * 2) processing glyph data
 *   a) load and raster glyph to bitmap if necessary
 *   b) load and convert glyph outlines if necessary
 *   c) caching processed glyph data for later usage
 *   d) making and managing text atlas for bitmap glyphs
 * 3) providing public text APIs
 *   a) deal with settings of text align, text baseline and text kerning
 *   b) provide text drawing APIs: drawText and exportTextOutlines
 *     *) support text layout in one line
 *     *) support text decorations: underline, strikethrough
 *   c) provide text measuring APIs: measureText
 */
  class TextManager
  {
  public:
    struct TextAtlases
    {
    };

    struct TextLayouter
    {
      enum TextAlign
      {
        TA_START,
        TA_END,
        TA_LEFT,
        TA_RIGHT,
        TA_CENTER,
      };
      enum TextBaseline
      {
        TB_TOP,
        TB_HANGING,
        TB_MIDDLE,
        TB_ALPHABETIC,
        TB_IDEOGRAPHIC,
        TB_BOTTOM,
      };

      TextAlign textAlign;
      TextBaseline textBaseline;
      double textKerning;

      TextLayouter()
          : textAlign(TA_START), textBaseline(TB_ALPHABETIC), textKerning(0.0) {}
    };
    typedef TextLayouter::TextAlign TextAlign;
    typedef TextLayouter::TextBaseline TextBaseline;

    enum TextDecoration
    {
      TD_NONE,
      TD_UNDERLINE,
      TD_STRIKE_THROUGH,
    };

    typedef TypefaceProvider::Typeface Typeface;

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf8converter;

  private:
    std::vector<TextAtlases> mAtlases;
    TextLayouter mLayouter;
    std::string mFontName;
    long mFontSize;
    TypefaceProvider *mTypefaceProvider;
    FT_Face mCurrFace;
    TextDecoration mTextDecoration;

    bool loadFaceOfChar(wchar_t charcode)
    {
      ASSERT(mTypefaceProvider);
      Typeface *t = nullptr;
      // INFO("loadFaceOfChar caled \n");
      // find best-matched typeface containing this glyph
      t = mTypefaceProvider->selectTypeface(charcode, mFontName);

      // if not exists, find substitute typeface containing this glyph
      if (!t)
      {
        t = mTypefaceProvider->selectTypeface(charcode);
      }

      // if not exsits, find fallback typeface mathcing font name
      if (!t)
      {
        t = mTypefaceProvider->selectFallbackTypeface(mFontName);
      }

      // if not exists, find any available typeface
      if (!t)
      {
        t = mTypefaceProvider->selectFallbackTypeface();
      }

      // if still not exists, fail the loading
      if (!t)
      {
        return false;
      }

      // get face and setup charmap if it's the first time to load this face
      FT_Face f = t->getFace();
      if (!f)
      {
        return false;
      }
      if (f != mCurrFace)
      {
        mCurrFace = f;

        // set to unicode charmap by default
        // TODO add support for different charmap based on encoding guessing of
        // charcodes and on the actual charmaps of this typeface
        if (0 != FT_Select_Charmap(mCurrFace, FT_ENCODING_UNICODE))
        {
          FAIL("Failed to set unicode encoding for typeface: %s, %s",
               mCurrFace->family_name, mCurrFace->style_name);
          return false;
        }
      }

      // set size
      if (0 != FT_Set_Char_Size(mCurrFace, mFontSize * 64, 0, 300, 300))
      {
        FAIL("Failed to set size for typeface: %s, %s", mCurrFace->family_name,
             mCurrFace->style_name);
        return false;
      }

      DEBUG("Current typeface: %s, %s", mCurrFace->family_name, mCurrFace->style_name);
      return true;
    }

    void dumpBitmap(const std::string &name, const FT_Bitmap &bitmap)
    {
      ASSERT(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
      std::string fp = name + ".ppm";

      PPMWriter ppm(bitmap.width, bitmap.rows);
      for (unsigned int i = 0; i < bitmap.width; i++)
      {
        for (unsigned int j = 0; j < bitmap.rows; j++)
        {
          double p = bitmap.buffer[i + bitmap.width * j];
          ppm.setPixel(i, j, p, p, p);
        }
      }
      ppm.writeFile(fp);
    }

    void dumpOutline(const FT_Outline &outline)
    {
      short start = 0;
      for (short i = 0; i < outline.n_contours; i++)
      {
        short end = outline.contours[i];

        printf("contour %d (%d points):", i, end - start + 1);
        for (short j = start; j <= end; j++)
        {
          short tag = outline.tags[j];
#define FP266(x) (x >> 6), (x & ((0x1 << 7) - 1))
          if ((tag & 0x1) == 1)
          {
            printf(" P(%ld.%ld, %ld.%ld)", FP266(outline.points[j].x),
                   FP266(outline.points[j].y));
          }
          else if ((tag & 0x3) == 0)
          {
            printf(" CP2(%ld.%ld, %ld.%ld)", FP266(outline.points[j].x),
                   FP266(outline.points[j].y));
          }
          else if ((tag & 0x3) == 2)
          {
            printf(" CP3(%ld.%ld, %ld.%ld)", FP266(outline.points[j].x),
                   FP266(outline.points[j].y));
          }
          else
          {
            printf(" (?)");
          }
#undef FP266
        }
        printf("\n");

        start = end + 1;
      }
    }

  public:
    TextManager()
        : mFontName("sans"), mFontSize(10),
          mTypefaceProvider(TypefaceProvider::getInstance()), mCurrFace(nullptr),
          mTextDecoration(TD_NONE) {}

    void setFont(const std::string &name, long size)
    {
      mFontName = name;
      mFontSize = size;
    }

    void setTextAlign(TextAlign ta) { mLayouter.textAlign = ta; }

    void setTextBaseline(TextBaseline tb) { mLayouter.textBaseline = tb; }

    void setTextKerning(double k) { mLayouter.textKerning = k; }

    void setTextDecoration(TextDecoration td) { mTextDecoration = td; }

  public:
    bool drawText(const std::string &text, double x, double y,
                  bool isStroke = false)
    {
      std::wstring wtext = utf8converter.from_bytes(text);
      for (size_t i = 0; i < wtext.size(); i++)
      {
        if (!loadFaceOfChar(wtext[i]))
        {
          return false;
        }
        ASSERT(mCurrFace);

        FT_UInt gi = FT_Get_Char_Index(mCurrFace, wtext[i]);
        if (0 != FT_Load_Glyph(mCurrFace, gi, 0))
        {
          FAIL("Failed to load glyph of index: %u", gi);
          return false;
        }

        std::string glyphStr = utf8converter.to_bytes(wtext[i]);
        if (0 != FT_Render_Glyph(mCurrFace->glyph, FT_RENDER_MODE_NORMAL))
        {
          FAIL("Failed to render glyph to bitmap: %s", glyphStr.c_str());
          return false;
        }

        DEBUG("dumping bitmap of glyph: %s", glyphStr.c_str());
        dumpBitmap(mFontName + "_" + glyphStr, mCurrFace->glyph->bitmap);
      }

      return true;
    }

    bool exportTextOutlines(const std::string &text, double x, double y)
    {
      std::wstring wtext = utf8converter.from_bytes(text);
      printf("exportTextOutlines called \n");
      for (size_t i = 0; i < wtext.size(); i++)
      {
        if (!loadFaceOfChar(wtext[i]))
        {
          return false;
        }
        ASSERT(mCurrFace);

        FT_UInt gi = FT_Get_Char_Index(mCurrFace, wtext[i]);
        if (0 != FT_Load_Glyph(mCurrFace, gi, 0))
        {
          FAIL("Failed to load glyph of index: %u", gi);
          return false;
        }

        DEBUG("dumping outline of glyph: %s", utf8converter.to_bytes(wtext[i]).c_str());
        dumpOutline(mCurrFace->glyph->outline);
      }

      return true;
    }

    double measureText(const std::string &text)
    {
      WARN("Not implemented yet: measureText");
      std::wstring wtext = utf8converter.from_bytes(text);
      return 0;
    }
  };

}; // namespace NSFontTool

#endif // __FONT_TOOL_HPP__
