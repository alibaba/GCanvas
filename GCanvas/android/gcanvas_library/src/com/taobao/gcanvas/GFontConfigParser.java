/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas;

import android.os.Build;
import android.util.Log;

import com.taobao.gcanvas.util.GLog;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

public class GFontConfigParser {

    // Android font families path
    private static final String SYSTEM_FONTS_FILE = "/system/etc/system_fonts.xml";
    private static final String SYSTEM_FONTS_FILE_LOLLIPOP = "/system/etc/fonts.xml";
    // TODO: Fall-back fonts are not used yet. Leave the code here since we may do it in future.
    private static final String FALLBACK_FONTS_FILE = "/system/etc/fallback_fonts.xml";
    // Fallback font
    private static final String FALLBACK_FONT = "DroidSansFallback.ttf";
    // Fonts location
    private static final String SYSTEM_FONT_LOCATION = "/system/fonts/";

    private boolean mIsInitialized = false;
    private HashMap<List<String>, List<String>> mFontFamilies = null;
    private List<String> mFallbackFonts = null;

    public GFontConfigParser() {
        if (!mIsInitialized) {
            readConfigFile();
            readFallbackConfigFile();
            mIsInitialized = true;
        }
    }

    public HashMap<List<String>, List<String>> getFontFamilies() {
        return mFontFamilies;
    }

    public List<String> getFallbackFontsList() {
        return mFallbackFonts;
    }

    public String getFallbackFont() {
        //vivo
        if (new File(SYSTEM_FONT_LOCATION + "DroidSansFallbackBBK.ttf").exists()) {
            return "DroidSansFallbackBBK.ttf";
        } else if (new File(SYSTEM_FONT_LOCATION + "NotoSansHans-Regular.otf").exists()) {
            //meizu flyme4.0
            return "NotoSansHans-Regular.otf";
        } else if (new File(SYSTEM_FONT_LOCATION + "NotoSansSC-Regular.otf").exists()) {
            return "NotoSansSC-Regular.otf";
        } else if (new File(SYSTEM_FONT_LOCATION + "NotoSansCJK-Regular.ttc").exists()) {
            return "NotoSansCJK-Regular.ttc";
        } else if (new File(SYSTEM_FONT_LOCATION + FALLBACK_FONT).exists()) {
            return FALLBACK_FONT;
        } else {
            return "DroidSans.ttf";
        }

    }

    public String getSystemFontLocation() {
        return SYSTEM_FONT_LOCATION;
    }

    private void readFallbackConfigFile() {
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = null;
            db = dbf.newDocumentBuilder();

            File file = new File(FALLBACK_FONTS_FILE);
            if (!file.exists()) {
                return;
            }

            Document doc = db.parse(file);
            parseFallbackXML(doc);
        } catch (Exception e) {
            Log.e("GFontConfigParser", "readFallbackConfigFile " + e.getMessage());
        }
    }

    private void readConfigFile() {
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = null;
            db = dbf.newDocumentBuilder();

            Document doc;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                doc = db.parse(new File(SYSTEM_FONTS_FILE_LOLLIPOP));
            } else {
                doc = db.parse(new File(SYSTEM_FONTS_FILE));
            }
            parseXML(doc);
        } catch (Exception e) {
            GLog.e("GFontConfigParser", "readConfigFile exception:" + e.getMessage());
        }
    }

    private void parseXML(Document doc) throws Exception {
        if (mFontFamilies == null) {
            mFontFamilies = new HashMap<List<String>, List<String>>();
        } else {
            mFontFamilies.clear();
        }

        // family set - root node
        Element root = doc.getDocumentElement();
        if (!root.getTagName().equals("familyset")) {
            GLog.w("GFontConfigParser", "Can't find familyset.");
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            List<String> nameList = null;
            List<String> aliasNameList = null;
            List<String> fileList = null;

            //leon
            NodeList nodeList = root.getElementsByTagName("family");
            int length = nodeList.getLength();
            for (int i = 0; i < length; ++i) {
                Node familyNode = nodeList.item(i);

                // Acquire name-set node and file-set node
                NamedNodeMap n = familyNode.getAttributes();

                Node s = familyNode.getAttributes().getNamedItem("name");
                if (s == null) {
                    break;
                } else {
                    //GLog.w("GFontConfigParser", "family name: " + s.getNodeValue());
                }

                NodeList fontNodes = ((Element) familyNode).getElementsByTagName("font");

                if (fontNodes == null) {
                    GLog.w("GFontConfigParser", "nameset or fileset is invalid.");
                    break;
                }

                // Get the length of name-set, file-set.
                int namesetLength = fontNodes.getLength();

                nameList = new ArrayList<String>();
                nameList.add(s.getNodeValue());

                fileList = new ArrayList<String>();
                // Get all file name
                for (int j = 0; j < namesetLength; ++j) {
                    Node fileNode = fontNodes.item(j);
                    if (fileNode instanceof Element) {
                        Element fileElement = (Element) fileNode;
                        String text = fileElement.getTextContent();
                        fileList.add(text);
                    }
                }

                //alias
                NodeList aliasList = root.getElementsByTagName("alias");
                int aliasLength = aliasList.getLength();
                for (int k = 0; k < aliasLength; ++k) {
                    Node aliasNode = aliasList.item(k);

                    Node nameA = aliasNode.getAttributes().getNamedItem("name");
                    Node toA = aliasNode.getAttributes().getNamedItem("to");
                    if (nameA == null || toA == null) {
                        break;
                    } else {
                        //GLog.w("GCanvas - GFontConfigParser", "nameA: " + nameA + " toA: " + toA);
                    }

                    aliasNameList = new ArrayList<String>();
                    for (String name : nameList) {
                        if (name.equals(toA.getNodeValue())) {
                            aliasNameList.add(nameA.getNodeValue());
                            //GLog.w("GCanvas - GFontConfigParser", "alias " + nameA.getNodeValue() + " to: " + name);
                        }
                    }
                    nameList.addAll(aliasNameList);
                }

                // Add fonts, files to HashMap.
                mFontFamilies.put(nameList, fileList);
            }

            //leon
        } else {

            NodeList nodeList = root.getElementsByTagName("family");
            int length = nodeList.getLength();
            for (int i = 0; i < length; ++i) {
                Node familyNode = nodeList.item(i);

                // Acquire name-set node and file-set node
                Node namesetNode = null;
                Node filesetNode = null;

                if (familyNode instanceof Element) {
                    Element familyElement = (Element) familyNode;
                    NodeList namesetNodes = familyElement.getElementsByTagName("nameset");
                    NodeList filesetNodes = familyElement.getElementsByTagName("fileset");

                    if (namesetNodes == null
                            || filesetNodes == null
                            || namesetNodes.getLength() != 1
                            || filesetNodes.getLength() != 1) {
                        GLog.w("GFontConfigParser", "nameset or fileset node doesn't exist.");
                        return;
                    }

                    namesetNode = namesetNodes.item(0);
                    filesetNode = filesetNodes.item(0);
                }

                if (namesetNode == null || filesetNode == null) {
                    GLog.w("GFontConfigParser", "nameset or fileset is invalid.");
                    return;
                }

                // Get the node list.
                NodeList nameset = namesetNode.getChildNodes();
                NodeList fileset = filesetNode.getChildNodes();
                // Return if no node is found
                if (nameset == null || fileset == null) {
                    GLog.w("GFontConfigParser", "nameset or fileset is empty.");
                    return;
                }

                // Get the length of name-set, file-set.
                int namesetLength = nameset.getLength();
                int filesetLength = fileset.getLength();
                List<String> nameList = new ArrayList<String>();
                List<String> fileList = new ArrayList<String>();

                // Get all file name
                for (int j = 0; j < filesetLength; ++j) {
                    Node fileNode = fileset.item(j);
                    if (fileNode instanceof Element) {
                        Element fileElement = (Element) fileNode;
                        if (fileElement.getTagName().equals("file")) {
                            String text = fileElement.getTextContent();
                            fileList.add(text);
                        }
                    }
                }

                // Get all font names.
                for (int j = 0; j < namesetLength; ++j) {
                    Node nameNode = nameset.item(j);
                    if (nameNode instanceof Element) {
                        Element nameElement = (Element) nameNode;
                        if (nameElement.getTagName().equals("name")) {
                            String text = nameElement.getTextContent();
                            nameList.add(text);
                        }
                    }
                }

                // Add fonts, files to HashMap.
                mFontFamilies.put(nameList, fileList);
            }
        }
    }

    private void parseFallbackXML(Document doc) throws Exception {
        if (mFallbackFonts == null) {
            mFallbackFonts = new ArrayList<String>();
        } else {
            mFallbackFonts.clear();
        }

        // family set - root node
        Element root = doc.getDocumentElement();
        if (!root.getTagName().equals("familyset")) {
            GLog.w("GFontConfigParser", "Can't find familyset.");
            return;
        }

        NodeList nodeList = root.getElementsByTagName("family");
        int length = nodeList.getLength();
        for (int i = 0; i < length; ++i) {
            Node familyNode = nodeList.item(i);

            Node filesetNode = null;

            if (familyNode instanceof Element) {
                Element familyElement = (Element) familyNode;
                NodeList filesetNodes = familyElement.getElementsByTagName("fileset");

                if (filesetNodes == null || filesetNodes.getLength() != 1) {
                    GLog.w("GFontConfigParser", "nameset or fileset node doesn't exist.");
                    return;
                }

                filesetNode = filesetNodes.item(0);
            }

            if (filesetNode == null) {
                GLog.w("GFontConfigParser", "nameset or fileset is invalid.");
                return;
            }

            NodeList fileset = filesetNode.getChildNodes();
            if (fileset == null) {
                GLog.w("GFontConfigParser", "nameset or fileset is empty.");
                return;
            }

            int filesetLength = fileset.getLength();
            for (int j = 0; j < filesetLength; ++j) {
                Node fileNode = fileset.item(j);
                if (fileNode instanceof Element) {
                    Element fileElement = (Element) fileNode;
                    if (fileElement.getTagName().equals("file")) {
                        String text = fileElement.getTextContent();
                        mFallbackFonts.add(text);
                    }
                }
            }
        }
    }
}
