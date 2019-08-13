/////////////////////////////////////////////////////////////////////////////
// Original Name: Pug XML Parser
// Author:    Kristen Wegner
// Copyright: Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Name:      ttCParseXML, ttCXMLBranch
// Author:    Ralph Walden
// Copyright: Copyright (c) 2003-2019 KeyWorks Software (Ralph Walden)
// Licence:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_KEYXML_H__
#define __TTLIB_KEYXML_H__

#include "ttheap.h"      // ttCHeap
#include "tthashpair.h"  // ttCHashPair
#include "ttarray.h"     // ttCArray
#include "ttstr.h"       // ttCStr
#include "ttfile.h"      // ttCFile

class ttCParseXML;   // forward definition
class ttCXMLBranch;  // forward definition

// clang-format off
#define PARSE_MINIMAL           0x00000000 // Unset the following flags.
#define PARSE_PI                0x00000002 // Parse '<?...?>'
#define PARSE_DOCTYPE           0x0000000  // Parse '<!DOCTYPE ...>' section, setting '[...]' as data member.
#define PARSE_COMMENTS          0x00000008 // Parse <!--...-->'
#define PARSE_CDATA             0x00000010 // Parse '<![CDATA[...]]>', and/or '<![INCLUDE[...]]>'
#define PARSE_ESCAPES           0x00000020 // Not implemented.
#define PARSE_TRIM_PCDATA       0x00000040 // Trim '>...<'
#define PARSE_TRIM_ATTRIBUTE    0x00000080 // Trim 'foo="..."'.
#define PARSE_TRIM_CDATA        0x00000100 // Trim '<![CDATA[...]]>', and/or '<![INCLUDE[...]]>'
#define PARSE_TRIM_ENTITY       0x00000200 // Trim '<!ENTITY name ...>', etc.
#define PARSE_TRIM_DOCTYPE      0x00000400 // Trim '<!DOCTYPE [...]>'
#define PARSE_TRIM_COMMENT      0x00000800 // Trim <!--...-->'
#define PARSE_NORMALIZE         0x00001000 // Normalize all entities that are flagged to be trimmed.
#define PARSE_DTD               0x00002000 // If PARSE_DOCTYPE set, then parse whatever is in data member ('[...]').
#define PARSE_DTD_ONLY          0x00004000 // If PARSE_DOCTYPE|PARSE_DTD set, then parse only '<!DOCTYPE [*]>'
#define PARSE_DEFAULT           0x0000FFFF
#define PARSE_DONT_SET          0x80000000
// clang-format on

namespace tt
{
    enum : size_t
    {
        DOCTYPE_XHTML_STRICT,
        DOCTYPE_XHTML_TRANSITIONAL,
        DOCTYPE_HTML_STRICT,
        DOCTYPE_HTML_TRANSITIONAL,
    };

    enum HTML_ELEMENT : size_t
    {
        // clang-format off
        ELEMENT_UNKNOWN,    // < Unknown tag!
        ELEMENT_A,          // < A
        ELEMENT_ABBR,       // < ABBR
        ELEMENT_ACRONYM,    // < ACRONYM
        ELEMENT_ADDRESS,    // < ADDRESS
        ELEMENT_ALIGN,      // < ALIGN
        ELEMENT_APPLET,     // < APPLET
        ELEMENT_AREA,       // < AREA
        ELEMENT_B,          // < B
        ELEMENT_BASE,       // < BASE
        ELEMENT_BASEFONT,   // < BASEFONT
        ELEMENT_BDO,        // < BDO
        ELEMENT_BGSOUND,    // < BGSOUND
        ELEMENT_BIG,        // < BIG
        ELEMENT_BLINK,      // < BLINK
        ELEMENT_BLOCKQUOTE, // < BLOCKQUOTE
        ELEMENT_BODY,       // < BODY
        ELEMENT_BR,         // < BR
        ELEMENT_BUTTON,     // < BUTTON
        ELEMENT_CAPTION,    // < CAPTION
        ELEMENT_CENTER,     // < CENTER
        ELEMENT_CITE,       // < CITE
        ELEMENT_CODE,       // < CODE
        ELEMENT_COL,        // < COL
        ELEMENT_COLGROUP,   // < COLGROUP
        ELEMENT_COMMENT,    // < COMMENT
        ELEMENT_DD,         // < DD
        ELEMENT_DEL,        // < DEL
        ELEMENT_DFN,        // < DFN
        ELEMENT_DIR,        // < DIR
        ELEMENT_DIV,        // < DIF
        ELEMENT_DL,         // < DL
        ELEMENT_DT,         // < DT
        ELEMENT_EM,         // < EM
        ELEMENT_EMBED,      // < EMBED
        ELEMENT_FIELDSET,   // < FIELDSET
        ELEMENT_FONT,       // < FONT
        ELEMENT_FORM,       // < FORM
        ELEMENT_FRAME,      // < FRAME
        ELEMENT_FRAMESET,   // < FRAMESET
        ELEMENT_H1,         // < H1
        ELEMENT_H2,         // < H2
        ELEMENT_H3,         // < H3
        ELEMENT_H4,         // < H4
        ELEMENT_H5,         // < H5
        ELEMENT_H6,         // < H6
        ELEMENT_HEAD,       // < HEAD
        ELEMENT_HR,         // < HR
        ELEMENT_HTML,       // < HTML
        ELEMENT_I,          // < I
        ELEMENT_IFRAME,     // < IFRAME
        ELEMENT_ILAYER,     // < ILAYER
        ELEMENT_IMG,        // < IMG
        ELEMENT_INPUT,      // < INPUT
        ELEMENT_INS,        // < INS
        ELEMENT_ISINDEX,    // < ISINDEX
        ELEMENT_KBD,        // < KBD
        ELEMENT_KEYGEN,     // < KEYGEN
        ELEMENT_LABEL,      // < LABEL
        ELEMENT_LAYER,      // < LAYER
        ELEMENT_LEGEND,     // < LEGEND
        ELEMENT_LI,         // < LI
        ELEMENT_LINK,       // < LINK
        ELEMENT_LISTING,    // < LISTING
        ELEMENT_MAP,        // < MAP
        ELEMENT_MARQUEE,    // < MARQUEE
        ELEMENT_MENU,       // < MENU
        ELEMENT_META,       // < META
        ELEMENT_MULTICOL,   // < MULTICOL
        ELEMENT_NEXTID,     // < NEXTID
        ELEMENT_NOBR,       // < NOBR
        ELEMENT_NOEMBED,    // < NOEMBED
        ELEMENT_NOFRAMES,   // < NOFRAMES
        ELEMENT_NOLAYER,    // < NOLAYER
        ELEMENT_NOSAVE,     // < NOSAVE
        ELEMENT_NOSCRIPT,   // < NOSCRIPT
        ELEMENT_OBJECT,     // < OBJECT
        ELEMENT_OL,         // < OL
        ELEMENT_OPTGROUP,   // < OPTGROUP
        ELEMENT_OPTION,     // < OPTION
        ELEMENT_P,          // < P
        ELEMENT_PARAM,      // < PARAM
        ELEMENT_PLAINTEXT,  // < PLAINTEXT
        ELEMENT_PRE,        // < PRE
        ELEMENT_Q,          // < Q
        ELEMENT_RB,         // < RB
        ELEMENT_RBC,        // < RBC
        ELEMENT_RP,         // < RP
        ELEMENT_RT,         // < RT
        ELEMENT_RTC,        // < RTC
        ELEMENT_RUBY,       // < RUBY
        ELEMENT_S,          // < S
        ELEMENT_SAMP,       // < SAMP
        ELEMENT_SCRIPT,     // < SCRIPT
        ELEMENT_SELECT,     // < SELECT
        ELEMENT_SERVER,     // < SERVER
        ELEMENT_SERVLET,    // < SERVLET
        ELEMENT_SMALL,      // < SMALL
        ELEMENT_SPACER,     // < SPACER
        ELEMENT_SPAN,       // < SPAN
        ELEMENT_STRIKE,     // < STRIKE
        ELEMENT_STRONG,     // < STRONG
        ELEMENT_STYLE,      // < STYLE
        ELEMENT_SUB,        // < SUB
        ELEMENT_SUP,        // < SUP
        ELEMENT_TABLE,      // < TABLE
        ELEMENT_TBODY,      // < TBODY
        ELEMENT_TD,         // < TD
        ELEMENT_TEXTAREA,   // < TEXTAREA
        ELEMENT_TFOOT,      // < TFOOT
        ELEMENT_TH,         // < TH
        ELEMENT_THEAD,      // < THEAD
        ELEMENT_TITLE,      // < TITLE
        ELEMENT_TR,         // < TR
        ELEMENT_TT,         // < TT
        ELEMENT_U,          // < U
        ELEMENT_UL,         // < UL
        ELEMENT_VAR,        // < VAR
        ELEMENT_WBR,        // < WBR
        ELEMENT_XMP,        // < XMP
        ELEMENT_NOLOC,      // < NOLOC
        ELEMENT_XML,        // < XML

        // MSHelp: tags

        ELEMENT_MSH_LINK,       // < MSHelp:link
        ELEMENT_MSH_TAG,        // < All single MSHelp: tags

        ELEMENT_UNKNOWN_XML_TAG,    // < any other unknown tag (presumably XML)

        // Following are for Sitemap files

        ELEMENT_FOLDER,
        ELEMENT_PAGE,

        //  Note: we could add MAML elements next if it made parsing easier
        // clang-format on
    };

    enum XMLENTITY : size_t
    {
        // clang-format off
        ENTITY_NULL,                        // An undifferentiated entity.
        ENTITY_ROOT,                        // A document tree's absolute root.
        ENTITY_ELEMENT,                     // E.g. '<...>'
        ENTITY_PCDATA,                      // E.g. '>...<'
        ENTITY_CDATA,                       // E.g. '<![CDATA[...]]>'
        ENTITY_COMMENT,                     // E.g. '<!--...-->'
        ENTITY_PI,                          // E.g. '<?...?>'
        ENTITY_INCLUDE,                     // E.g. '<![INCLUDE[...]]>'
        ENTITY_DOCTYPE,                     // E.g. '<!DOCTYPE ...>'.
        ENTITY_DTD_ENTITY,                  // E.g. '<!ENTITY ...>'.
        ENTITY_DTD_ATTLIST,                 // E.g. '<!ATTLIST ...>'.
        ENTITY_DTD_ELEMENT,                 // E.g. '<!ELEMENT ...>'.
        ENTITY_DTD_NOTATION                 // E.g. '<!NOTATION ...>'.
        // clang-format on
    };

    typedef struct
    {
        char* pszName;   // Pointer to attribute name.
        char* pszValue;  // Pointer to attribute value.
    } XMLATTR;

    typedef struct
    {
        ttCParseXML*   pKeyXML;          // pointer to the container class
        ttCXMLBranch*  parent;           // Pointer to parent
        char*          pszName;          // Pointer to element name.
        XMLENTITY      type;             // Branch type; see XMLENTITY.
        size_t         cAttributes;      // Count attributes.
        size_t         cAttributeSpace;  // Available pointer space in 'attribute'.
        XMLATTR**      aAttributes;      // Array of pointers to attributes; see XMLATTR.
        size_t         cChildren;        // Count children in member 'child'.
        size_t         cChildSpace;      // Available pointer space in 'child'.
        size_t         nextChild;        // Set by FindFirstElement(), updated by FindNextElement()
        ttCXMLBranch** aChildren;        // Array of pointers to children.
        char*          pszData;          // Pointer to any associated string data.
        HTML_ELEMENT   element;          // HTML Element -- only valid in an HTML file
    } XMLBRANCH;
}  // namespace tt

class ttCXMLBranch : public tt::XMLBRANCH
{
public:
    bool IsNull() const { return type == tt::ENTITY_NULL; }
    bool IsElement() const { return type == tt::ENTITY_ELEMENT; }
    bool IsComment() const { return type == tt::ENTITY_COMMENT; }
    bool IsPCDATA() const { return type == tt::ENTITY_PCDATA; }
    bool IsCDATA() const { return type == tt::ENTITY_CDATA; }
    bool IsINCLUDE() const { return type == tt::ENTITY_INCLUDE; }
    bool IsPI() const { return type == tt::ENTITY_PI; }
    bool IsDOCTYPE() const { return type == tt::ENTITY_DOCTYPE; }
    bool IsDTD() const { return type > tt::ENTITY_DOCTYPE; }
    bool IsDTD_ATTLIST() const { return type == tt::ENTITY_DTD_ATTLIST; }
    bool IsDTD_ELEMENT() const { return type == tt::ENTITY_DTD_ELEMENT; }
    bool IsDTD_ENTITY() const { return type == tt::ENTITY_DTD_ENTITY; }
    bool IsDTD_NOTATION() const { return type == tt::ENTITY_DTD_NOTATION; }
    bool IsNamed(const char* pszNamed) const { return pszName ? ttIsSameStrI(pszName, pszNamed) : false; }
    bool IsRoot() const { return this == parent; }

    // Class functions

    ttCXMLBranch* FindFirstElement(tt::HTML_ELEMENT element);
    ttCXMLBranch* FindNextElement(tt::HTML_ELEMENT element);  // Must call FindFirstElement() before this

    ttCXMLBranch* FindFirstElement(const char* pszName);
    ttCXMLBranch* FindNextElement(const char* pszName);  // Must call FindFirstElement() before this

    // Use GetAttribute() for the current branch, FindFirstAttribute to find a child branch containing the specified attribute/value

    ttCXMLBranch* FindFirstAttribute(const char* pszAttribute, const char* pszValue = nullptr);  // find first attribute with specified name and (optional) value

    const char*  GetAttribute(const char* pszName) const;
    tt::XMLATTR* GetAttributeAt(size_t i)
    {
        ttASSERT(i < cAttributes);
        return (i < cAttributes) ? aAttributes[i] : NULL;
    }
    size_t        GetAttributesCount() const { return cAttributes; }
    ttCXMLBranch* GetChildAt(size_t i)
    {
        ttASSERT(i < cChildren);
        return (i < cChildren) ? aChildren[i] : NULL;
    }
    size_t           GetChildrenCount() const { return cChildren; }
    const char*      GetData() { return pszData ? pszData : ""; }
    tt::HTML_ELEMENT GetElementTag() const { return element; }
    const char*      GetName() const { return pszName ? pszName : ""; }
    ttCXMLBranch*    GetSiblingAt(size_t i) { return (!IsRoot() && i < GetSiblingsCount()) ? parent->aChildren[i] : NULL; }
    size_t           GetSiblingNumber();
    size_t           GetSiblingsCount() const { return (!IsRoot()) ? parent->cChildren : 0; }
    tt::XMLENTITY    GetType() const { return type; }
    bool             RemoveChildAt(size_t i);
    bool             ReplaceAttributeValue(ttCParseXML* pxml, const char* pszName, const char* pszNewValue);
    const char*      GetFirstChildData()
    {
        if (cChildren)
            return GetChildAt(0)->GetData();
        else
            return NULL;
    }

    inline tt::XMLATTR* MapStringToAttributePtr(const char* pszString) const
    {
        for (size_t i = 0; i < cAttributes; i++)
        {
            if (ttIsSameStrI(pszString, aAttributes[i]->pszName))
                return aAttributes[i];
        }
        return nullptr;
    }

    ttCXMLBranch* operator[](size_t i) { return (ttCXMLBranch*) GetChildAt(i); }
};

class ttCParseXML : public ttCHeap
{
public:
    ttCParseXML();

    void SetDocType(size_t type = tt::DOCTYPE_XHTML_STRICT);

    HRESULT ParseXmlFile(const char* pszFile);  // returns S_OK if read, STG_E_FILENOTFOUND if not found
    HRESULT ParseHtmlFile(const char* pszFile);
    char*   ParseXmlString(char* pszXmlString, ttCXMLBranch* pRoot = nullptr);
    char*   ParseHtmlString(char* szXmlString, ttCXMLBranch* pRoot = nullptr);
    char*   ParseSitemapString(char* szXmlString, ttCXMLBranch* pRoot = nullptr);

    HRESULT SaveXmlFile(const char* pszFileName);
    void    SaveXmlFile(ttCFile* pkf) { WriteBranch(nullptr, *pkf, 0); }  // nullptr means no parent
    HRESULT SaveHtmlFile(const char* pszFileName);
    void    SaveHtmlFile(ttCFile* pkf) { WriteHtmlBranch(nullptr, *pkf); }

    ttCXMLBranch* GetRootBranch() { return m_pRoot; }

    ttCXMLBranch* GetBodyBranch() { return m_pBodyBranch; }
    ttCXMLBranch* GetHeadBranch() { return m_pHeadBranch; }
    ttCXMLBranch* GetTitleBranch() { return m_pTitleBranch; }

    // Call AddRoot() when creating XML from scratch (no input file or string)
    ttCXMLBranch* AddRoot()
    {
        m_pRoot = NewBranch(tt::ENTITY_ROOT);
        m_pRoot->parent = m_pRoot;
        return m_pRoot;
    }
    ttCXMLBranch* AddBranch(ttCXMLBranch* pParent, const char* pszBranchName, tt::XMLENTITY eType = tt::ENTITY_ELEMENT);
    void          AddAttribute(ttCXMLBranch* pBranch, const char* pszName, const char* pszValue, size_t iGrow = 4 /* estimated new attributes */);
    ttCXMLBranch* AddDataChild(ttCXMLBranch* pParent, const char* pszName, const char* pszData);
    ttCXMLBranch* GraftBranch(ttCXMLBranch* pParent, tt::XMLENTITY eType = tt::ENTITY_ELEMENT);

    const char* GetTitle()  // if an HTML/XHTML file was parsed, this will return the title (if any)
    {
        if (m_pTitleBranch && m_pTitleBranch->GetChildrenCount())
        {
            ttCXMLBranch* pData = m_pTitleBranch->GetChildAt(0);
            if (pData && pData->GetType() == tt::ENTITY_PCDATA && pData->pszData)
                return pData->GetData();
        }
        return NULL;
    }

    size_t        GetMSHLinkCount() { return m_aMSHLinks.GetCount(); }
    ttCXMLBranch* GetMSHLink(size_t pos) { return m_aMSHLinks[pos]; }

    size_t        GetObjectTagCount() { return m_aObjectTags.GetCount(); }
    ttCXMLBranch* GetObjectTag(size_t pos) { return m_aObjectTags[pos]; }

    char* AllocateBuffer(size_t cb) { return (char*) ttMalloc(cb); }
    void  AllocateStringBuffers(ttCXMLBranch* pBranch = nullptr);  // convert all strings to separately allocated buffers
    void  FreeBuffer(char* pszBuffer) { ttFree(pszBuffer); }
    bool  isAllocatedStrings() { return m_bAllocatedStrings; }

protected:
    // Class functions

    ttCXMLBranch*    NewBranch(tt::XMLENTITY eType = tt::ENTITY_ELEMENT);
    tt::XMLATTR*     AddAttribute(ttCXMLBranch* pBranch, long lGrow);
    HRESULT          WriteBranch(ttCXMLBranch* pBranch, ttCFile& kf, size_t iIndent);
    HRESULT          WriteHtmlBranch(ttCXMLBranch* pBranch, ttCFile& kf);
    tt::HTML_ELEMENT ParseElementTag(const char* pszName, const char* pszCurLoc, bool bEndTag = false);

    tt::XMLATTR* NewAttribute(void)
    {
        tt::XMLATTR* p = (tt::XMLATTR*) ttMalloc(sizeof(tt::XMLATTR));  // Allocate one attribute.
        p->pszName = p->pszValue = 0;                                   // No name or value.
        return p;
    }

    // Class members

    ttCXMLBranch* m_pRoot;              // Pointer to current XML Document tree root.
    size_t        m_uOptions;           // Parser options.
    bool          m_bAllocatedStrings;  // true if we allocated our own memory to hold strings
    bool          m_bXmlDataIsland;     // true if we encountered one or more data islands

    ttCXMLBranch* m_pBodyBranch;
    ttCXMLBranch* m_pHeadBranch;
    ttCXMLBranch* m_pTitleBranch;

    ttCArray<ttCXMLBranch*> m_aMSHLinks;    // array of all MSHelp:link elements that appear
    ttCArray<ttCXMLBranch*> m_aObjectTags;  // array of all <object> elements that appear
    ttCHashPair             m_lstXmlTags;
    ttCHashPair             m_lstUnknownTags;

    ttCStr  m_cszDocType;
    ttCFile m_kf;
};

#endif  // __TTLIB_KEYXML_H__
