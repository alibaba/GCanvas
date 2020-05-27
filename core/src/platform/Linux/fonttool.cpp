#include "FontTool.hpp"
#include "utils.hpp"
#include "argh.h"
#include <codecvt>
#include <ft2build.h>
#include <iostream>
#include <locale>
#include <memory>
#include FT_FREETYPE_H

using NSFontTool::TypefaceLoader;
using NSFontTool::TypefaceProvider;
using NSFontTool::TextManager;

void usage(const std::string &cmd) {
  std::cerr << "Usage: " << cmd << " <command> <params>" << std::endl << std::endl;
  std::cerr << "Available commands:" << std::endl;
  std::cerr << "  buildCache <dir> [<dir2> ...]              Build font cache from directories." << std::endl;
  std::cerr << "  dumpCache <cacheFile>                      Dump list of typefaces from cache file." << std::endl;
  std::cerr << "  outline <text> [<font name>]               Export text outlines using prebuilt font cache." << std::endl;
  std::cerr << "  raster <text> [<font name>] [-s/--size=14] Raster text to image using prebuilt font cache." << std::endl;
  std::cerr << std::endl << "For simplicity, cache file defaults to `.fontcache`, and image defaults to `out_<font name>.ppm`)" << std::endl;
  std::cerr << std::endl;
}

int main(int argc, const char *argv[]) {
  argh::parser cmdl(argc, argv);

  if (cmdl.pos_args().size() < 3 || cmdl["h"] || cmdl["help"]) {
    usage(cmdl[0]);
    return 1;
  }

  if (cmdl[1] == "buildCache") {
    TypefaceLoader *tl = TypefaceLoader::getInstance();
    ASSERT(tl);
    for (size_t i = 2; i < cmdl.size(); i++) {
      tl->buildFontCacheFromDir(cmdl[i]);
    }
    bool res = tl->exportFontCache(".fontcache");
    return res ? 0 : 1;
  } else if (cmdl[1] == "dumpCache") {
    std::string cacheFile = cmdl[2];
    TypefaceLoader *tl = TypefaceLoader::getInstance();
    ASSERT(tl);
    if (!tl->importFontCache(cacheFile)) {
      return 1;
    }
    tl->dumpFontCache();

    return 0;
  } else if (cmdl[1] == "outline") {
    std::string text = cmdl[2];
    std::string fontName = cmdl[3];

    TypefaceLoader *tl = TypefaceLoader::getInstance();
    ASSERT(tl);
    if (!tl->importFontCache(".fontcache")) {
      INFO("Please use `buildCache` to build font cache first.");
      return 1;
    }
    tl->dumpFontCache();

    TypefaceProvider *tp = TypefaceProvider::getInstance();
    ASSERT(tp);
    const std::vector<TypefaceProvider::Typeface *> &faces = tp->selectTypefaces(fontName);

    TextManager tm;
    for (size_t i = 0; i < faces.size(); i++) {
      INFO("Using typeface: %s", faces[i]->psName.c_str());
      tm.setFont(faces[i]->psName, 14);
      ASSERT(tm.exportTextOutlines(text, 0, 0));
    }

    return 0;
  } else if (cmdl[1] == "raster") {
    std::string text = cmdl[2];
    std::string fontName = cmdl[3];
    long fontSize = 14;
    cmdl({"-s", "--size"}) >> fontSize;

    TypefaceLoader *tl = TypefaceLoader::getInstance();
    ASSERT(tl);
    if (!tl->importFontCache(".fontcache")) {
      INFO("Please use `buildCache` to build font cache first.");
      return 1;
    }
    tl->dumpFontCache();

    TypefaceProvider *tp = TypefaceProvider::getInstance();
    ASSERT(tp);
    const std::vector<TypefaceProvider::Typeface *> &faces = tp->selectTypefaces(fontName);

    TextManager tm;
    for (size_t i = 0; i < faces.size(); i++) {
      INFO("Using typeface: %s", faces[i]->psName.c_str());
      tm.setFont(faces[i]->psName, fontSize);
      ASSERT(tm.drawText(text, 0, 0));
    }

    return 0;
  } else {
    std::cerr << "Invalid command: " << cmdl[1] << std::endl << std::endl;
    usage(cmdl[0]);
    return 1;
  }

  return 0;
}