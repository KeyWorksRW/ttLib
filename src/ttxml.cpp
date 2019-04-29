/////////////////////////////////////////////////////////////////////////////
// Original Name: Pug XML Parser
// Author:		Kristen Wegner
// Copyright:	Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Name:		CKeyXML, ttCXMLBranch
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2003-2019 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttxml.h"
#include "../include/ttfile.h"	// ttCFile
#include "../include/ttmem.h"	// ttMem, ttCTMem

#pragma warning(disable: 4996)	// 'function' : function may be unsafe

#define GROW_SIZE 1 // Default child element & attribute space growth increment.

inline bool isSpace(char c) { return (c > -1 && c < '!'); }		// REVIEW: [randalphwa - 10/1/2018] should this be changed to IsWhiteSpace?

using namespace tt;

typedef struct {
	HTML_ELEMENT element;
	const char* pszElementName;
} ELEMENT_PAIRS;

namespace {
	const ELEMENT_PAIRS aElementPairs[] = {
		{ ELEMENT_A,		  "a",			 },
		{ ELEMENT_ABBR,		  "abbr",		 },
		{ ELEMENT_ACRONYM,	  "acronym",	 },
		{ ELEMENT_ADDRESS,	  "address",	 },
		{ ELEMENT_ALIGN,	  "align",		 },
		{ ELEMENT_APPLET,	  "applet",		 },
		{ ELEMENT_AREA,		  "area",		 },
		{ ELEMENT_B,		  "b",			 },
		{ ELEMENT_BASE,		  "base",		 },
		{ ELEMENT_BASEFONT,	  "basefont",	 },
		{ ELEMENT_BDO,		  "bdo",		 },
		{ ELEMENT_BGSOUND,	  "bgsound",	 },
		{ ELEMENT_BIG,		  "big",		 },
		{ ELEMENT_BLINK,	  "blink",		 },
		{ ELEMENT_BLOCKQUOTE, "blockquote",	 },
		{ ELEMENT_BODY,		  "body",		 },
		{ ELEMENT_BR,		  "br",			 },
		{ ELEMENT_BUTTON,	  "button",		 },
		{ ELEMENT_CAPTION,	  "caption",	 },
		{ ELEMENT_CENTER,	  "center",		 },
		{ ELEMENT_CITE,		  "cite",		 },
		{ ELEMENT_CODE,		  "code",		 },
		{ ELEMENT_COL,		  "col",		 },
		{ ELEMENT_COLGROUP,	  "colgroup",	 },
		{ ELEMENT_COMMENT,	  "comment",	 },
		{ ELEMENT_DD,		  "dd",			 },
		{ ELEMENT_DEL,		  "del",		 },
		{ ELEMENT_DFN,		  "dfn",		 },
		{ ELEMENT_DIR,		  "dir",		 },
		{ ELEMENT_DIV,		  "div",		 },
		{ ELEMENT_DL,		  "dl",			 },
		{ ELEMENT_DT,		  "dt",			 },
		{ ELEMENT_EM,		  "em",			 },
		{ ELEMENT_EMBED,	  "embed",		 },
		{ ELEMENT_FIELDSET,	  "fieldset",	 },
		{ ELEMENT_FONT,		  "font",		 },
		{ ELEMENT_FORM,		  "form",		 },
		{ ELEMENT_FRAME,	  "frame",		 },
		{ ELEMENT_FRAMESET,	  "frameset",	 },
		{ ELEMENT_H1,		  "h1",			 },
		{ ELEMENT_H2,		  "h2",			 },
		{ ELEMENT_H3,		  "h3",			 },
		{ ELEMENT_H4,		  "h4",			 },
		{ ELEMENT_H5,		  "h5",			 },
		{ ELEMENT_H6,		  "h6",			 },
		{ ELEMENT_HEAD,		  "head",		 },
		{ ELEMENT_HR,		  "hr",			 },
		{ ELEMENT_HTML,		  "html",		 },
		{ ELEMENT_I,		  "i",			 },
		{ ELEMENT_IFRAME,	  "iframe",		 },
		{ ELEMENT_ILAYER,	  "ilayer",		 },
		{ ELEMENT_IMG,		  "img",		 },
		{ ELEMENT_INPUT,	  "input",		 },
		{ ELEMENT_INS,		  "ins",		 },
		{ ELEMENT_ISINDEX,	  "isindex",	 },
		{ ELEMENT_KBD,		  "kbd",		 },
		{ ELEMENT_KEYGEN,	  "keygen",		 },
		{ ELEMENT_LABEL,	  "label",		 },
		{ ELEMENT_LAYER,	  "layer",		 },
		{ ELEMENT_LEGEND,	  "legend",		 },
		{ ELEMENT_LI,		  "li",			 },
		{ ELEMENT_LINK,		  "link",		 },
		{ ELEMENT_LISTING,	  "listing",	 },
		{ ELEMENT_MAP,		  "map",		 },
		{ ELEMENT_MARQUEE,	  "marquee",	 },
		{ ELEMENT_MENU,		  "menu",		 },
		{ ELEMENT_META,		  "meta",		 },
		{ ELEMENT_MULTICOL,	  "multicol"	 },
		{ ELEMENT_NEXTID,	  "nextid",		 },
		{ ELEMENT_NOBR,		  "nobr",		 },
		{ ELEMENT_NOEMBED,	  "noembed",	 },
		{ ELEMENT_NOFRAMES,	  "noframes",	 },
		{ ELEMENT_NOLAYER,	  "nolayer",	 },
		{ ELEMENT_NOSAVE,	  "nosave",		 },
		{ ELEMENT_NOSCRIPT,	  "noscript",	 },
		{ ELEMENT_OBJECT,	  "object",		 },
		{ ELEMENT_OL,		  "ol",			 },
		{ ELEMENT_OPTGROUP,	  "optgroup",	 },
		{ ELEMENT_OPTION,	  "option",		 },
		{ ELEMENT_P,		  "p",			 },
		{ ELEMENT_PARAM,	  "param",		 },
		{ ELEMENT_PLAINTEXT,  "plaintext"	 },
		{ ELEMENT_PRE,		  "pre",		 },
		{ ELEMENT_Q,		  "q",			 },
		{ ELEMENT_RB,		  "rb",			 },
		{ ELEMENT_RBC,		  "rbc",		 },
		{ ELEMENT_RP,		  "rp",			 },
		{ ELEMENT_RT,		  "rt",			 },
		{ ELEMENT_RTC,		  "rtc",		 },
		{ ELEMENT_RUBY,		  "ruby",		 },
		{ ELEMENT_S,		  "s",			 },
		{ ELEMENT_SAMP,		  "samp",		 },
		{ ELEMENT_SCRIPT,	  "script",		 },
		{ ELEMENT_SELECT,	  "select",		 },
		{ ELEMENT_SERVER,	  "server",		 },
		{ ELEMENT_SERVLET,	  "servlet",	 },
		{ ELEMENT_SMALL,	  "small",		 },
		{ ELEMENT_SPACER,	  "spacer",		 },
		{ ELEMENT_SPAN,		  "span",		 },
		{ ELEMENT_STRIKE,	  "strike",		 },
		{ ELEMENT_STRONG,	  "strong",		 },
		{ ELEMENT_STYLE,	  "style",		 },
		{ ELEMENT_SUB,		  "sub",		 },
		{ ELEMENT_SUP,		  "sup",		 },
		{ ELEMENT_TABLE,	  "table",		 },
		{ ELEMENT_TBODY,	  "tbody",		 },
		{ ELEMENT_TD,		  "td",			 },
		{ ELEMENT_TEXTAREA,	  "textarea",	 },
		{ ELEMENT_TFOOT,	  "tfoot",		 },
		{ ELEMENT_TH,		  "th",			 },
		{ ELEMENT_THEAD,	  "thead",		 },
		{ ELEMENT_TITLE,	  "title",		 },
		{ ELEMENT_TR,		  "tr",			 },
		{ ELEMENT_TT,		  "tt",			 },
		{ ELEMENT_U,		  "u",			 },
		{ ELEMENT_UL,		  "ul",			 },
		{ ELEMENT_VAR,		  "var",		 },
		{ ELEMENT_WBR,		  "wbr",		 },
		{ ELEMENT_XMP,		  "xmp",		 },
		{ ELEMENT_NOLOC,	  "noloc",		 },
		{ ELEMENT_XML,		  "xml",		 },

		{ ELEMENT_MSH_LINK, "MSHelp:link",	},
		{ ELEMENT_MSH_TAG,	"MSHelp:",		},

		{ ELEMENT_UNKNOWN, NULL }
	};
} // end of anonymous namespace

HTML_ELEMENT ttCParseXML::ParseElementTag(const char* pszName, const char* pszCurLoc, bool bEndTag)
{
	const ELEMENT_PAIRS* pElem = aElementPairs;
	while (pElem->pszElementName) {
		if (tt::IsSameStrI(pszName, pElem->pszElementName))
			return pElem->element;
		pElem++;
	}

	if (tt::IsSameStrI(pszName, "MSHelp:")) {
		return ELEMENT_MSH_TAG;
	}

	// The tag name is unknown. We look for a close tag with the same name, and if found, we assume this is an XML tag.

	if (pszCurLoc && ttstrlen(pszName) < 254) {
		char szClose[256];
		szClose[0] = '\\';
		ttstrcpy(szClose, sizeof(szClose), pszName);
		if (tt::FindStrI(pszCurLoc, szClose)) {
			if (!m_lstXmlTags.Find(pszName)) {
				m_lstXmlTags.Add(pszName);
			}
			return ELEMENT_UNKNOWN_XML_TAG;
		}
	}
	if (bEndTag) {
		if (m_lstXmlTags.Find(pszName)) {
			return ELEMENT_UNKNOWN_XML_TAG;
		}
	}

	return ELEMENT_UNKNOWN;
}

bool StrWtrim(char** s)
{
	ttASSERT(s);
	if (!s || !*s)
		return false;

	while(**s > 0 && **s < '!') // skip over leading whitespace
		++(*s);

	char* pszEnd = *s + (ttstrlen(*s) - 1);
	while (isSpace(*pszEnd) && pszEnd > *s)
		pszEnd--;
	pszEnd++;
	*pszEnd = 0;

	return true;
}

void _StrWnorm(char** s)
{
	ttASSERT(s);
	if (!s || !*s)
		return;

	if (!StrWtrim(s))
		return;

	// Now we "normalize" by combining multiple spaces into a single space

	size_t n = tt::StrByteLen(*s);
	ttCTMem<char*> pszNorm(sizeof(char) * n);
	size_t j = 1;
	pszNorm[0] = (*s)[0];
	n--;	// ignore zero-terminating character
	for (size_t i = 1; i < n; ++i) {  // For each character, starting at offset 1.
		if ((*s)[i] < '!') {  // Whitespace-like?
			if ((*s)[i-1] >= '!') { // Previous was not whitespace-like.
				pszNorm[j++] = ' '; // Convert to a space char.
			}
		}
		else {
			pszNorm[j++] = (*s)[i];	 // Not whitespace, so just copy over.
		}
	}
	if (j < n) {	// Normalization buffer is actually different then input.
		pszNorm[j] = 0;
		ttstrcpy(*s, pszNorm); // Copy it back to input.
	}
}

ttCParseXML::ttCParseXML()
{
	m_pRoot = NULL;
	m_uOptions = PARSE_DEFAULT;
	m_bAllocatedStrings = false;
	m_pTitleBranch = NULL;
	m_pHeadBranch = NULL;
	m_pBodyBranch = NULL;
}

ttCXMLBranch* ttCParseXML::GraftBranch(ttCXMLBranch* pParent, XMLENTITY eType)
{
	ttASSERT(pParent);
	if (!pParent)
		return NULL; // Must have a parent.
	if (pParent->cChildren == pParent->cChildSpace) { //Out of pointer space.
		ttCXMLBranch** t = (ttCXMLBranch**) ttReAlloc(pParent->aChildren, sizeof(ttCXMLBranch*) * (pParent->cChildSpace + GROW_SIZE)); // Grow pointer space.
		if (t) { //Reallocation succeeded.
			pParent->aChildren = t;
			pParent->cChildSpace += GROW_SIZE; //Update the available space.
		}
	}
	ttCXMLBranch* pChild = NewBranch(eType); // Allocate a new child.
	pChild->parent = pParent; // Set it's parent pointer.
	pParent->aChildren[pParent->cChildren] = pChild; // Set the parent's child pointer.
	pParent->cChildren++; //One more child.
	return pChild;
}

ttCXMLBranch* ttCParseXML::NewBranch(XMLENTITY eType)
{
	ttCXMLBranch* p = (ttCXMLBranch*) ttCalloc(sizeof(ttCXMLBranch)); // Allocate one branch.
	p->pKeyXML = this;
	p->type = eType; // Set the desired type.
	p->element = ELEMENT_UNKNOWN;
	if (
			eType != ENTITY_ROOT	&& // None of these will have attributes.
			eType != ENTITY_PCDATA	&&
			eType != ENTITY_CDATA	&&
			eType != ENTITY_INCLUDE &&
			eType != ENTITY_COMMENT) {
		p->aAttributes = (XMLATTR**) ttMalloc(sizeof(XMLATTR*));	// Allocate one attribute pointer.
		p->cAttributeSpace = 1;
	}
	if (
			eType == ENTITY_ELEMENT || //Only these will have children.
			eType == ENTITY_DOCTYPE ||
			eType == ENTITY_ROOT) {
		p->aChildren = (ttCXMLBranch**) ttMalloc(sizeof(ttCXMLBranch*));	// Allocate one child.
		p->cChildSpace = 1;
	}
	return p;
}

XMLATTR* ttCParseXML::AddAttribute(ttCXMLBranch* pBranch, LONG lGrow)
{
	ttASSERT(pBranch);
	if (!pBranch)
		return NULL;
	XMLATTR* a = NewAttribute();
	if (!a)
		return NULL;
	if (pBranch->cAttributes == pBranch->cAttributeSpace) { // Out of space, so grow.
		XMLATTR** t = (XMLATTR**) ttReAlloc(pBranch->aAttributes, sizeof(ttCXMLBranch*) *(pBranch->cAttributeSpace + lGrow));
		if (t) {
			pBranch->aAttributes = t;
			pBranch->cAttributeSpace += lGrow;
		}
	}
	pBranch->aAttributes[pBranch->cAttributes] = a;
	pBranch->cAttributes++;
	return a;
}

void ttCParseXML::AllocateStringBuffers(ttCXMLBranch* pBranch)
{
	if (!pBranch) {
		pBranch = GetRootBranch();
		ttASSERT(pBranch);
		ttASSERT(pBranch->GetChildrenCount());
		if (pBranch->GetChildrenCount())
			AllocateStringBuffers(pBranch->GetChildAt(0));

		// REVIEW: [ralphw - 02-20-2003] Does the root every actually have any string data?

		if (pBranch->pszName)
			pBranch->pszName = ttStrdup(pBranch->pszName);
		if (pBranch->pszData)
			pBranch->pszData = ttStrdup(pBranch->pszData);
		for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
			XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
			ttASSERT(pAttr);
			if (pAttr) {
				if (pAttr->pszName)
					pAttr->pszName = ttStrdup(pAttr->pszName);
				if (pAttr->pszValue)
					pAttr->pszValue = ttStrdup(pAttr->pszValue);
			}
		}

		return;
	}

	for (size_t iSibling = 1;;iSibling++) {
		if (pBranch->GetChildrenCount()) {
			AllocateStringBuffers(pBranch->GetChildAt(0));
		}
		if (pBranch->pszName)
			pBranch->pszName = ttStrdup(pBranch->pszName);
		if (pBranch->pszData)
			pBranch->pszData = ttStrdup(pBranch->pszData);
		for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
			XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
			ttASSERT(pAttr);
			if (pAttr) {
				if (pAttr->pszName)
					pAttr->pszName = ttStrdup(pAttr->pszName);
				if (pAttr->pszValue)
					pAttr->pszValue = ttStrdup(pAttr->pszValue);
			}
		}

		if (iSibling >= pBranch->GetSiblingsCount())
			break;
		pBranch = pBranch->GetSiblingAt(iSibling);
	}
}

HRESULT ttCParseXML::SaveXmlFile(const char* pszFileName)
{
	ttCFile kf;
	WriteBranch(NULL, kf, 0);
	if (kf.WriteFile(pszFileName))
		return S_OK;
	else
		return E_FAIL;
}

#pragma warning(disable : 4062) // switch doesn't handle all enumerated types

HRESULT ttCParseXML::WriteBranch(ttCXMLBranch* pBranch, ttCFile& kf, size_t iIndent)
{
	if (!pBranch) {
		pBranch = GetRootBranch();
		ttASSERT(pBranch);
		ttASSERT(pBranch->GetChildrenCount());
		ttASSERT(pBranch->type == ENTITY_ROOT);
		HRESULT hr = S_OK;
		if (pBranch->GetChildrenCount())
			hr = WriteBranch(pBranch->GetChildAt(0), kf, 0);

		return hr;
	}

	size_t cbAttrs;
	for (size_t iSibling = 1;;iSibling++) {
		switch (pBranch->type) {
			case ENTITY_ELEMENT:
				for (size_t i = 0; i < iIndent; i++)
					kf.WriteStr("\t");

				kf.WriteStr("<");
				if (pBranch->pszName)
					kf.WriteStr(pBranch->pszName);
				// first find out how long the attribute names will be to see if they will fit on a single line

				cbAttrs = 0;

				for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
					XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
					if (pAttr->pszName) {
						cbAttrs += ttstrlen(pAttr->pszName);
						if (pAttr->pszValue)
							cbAttrs += ttstrlen(pAttr->pszValue);
						cbAttrs += 2;	// include room for spacing
					}
				}

				for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
					XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
					if (pAttr->pszName) {
						if (pBranch->GetAttributesCount() > 1 && cbAttrs > 80) {
							kf.WriteEol("");
							for (size_t i = 0; i < iIndent + 1; i++)
								kf.WriteStr("\t");
						}
						else {
							kf.WriteStr(" ");
						}
						kf.WriteStr(pAttr->pszName);
						kf.WriteStr("=\042");
						if (pAttr->pszValue)
							kf.WriteStr(pAttr->pszValue);
						kf.WriteStr("\042");
					}
				}
				if (pBranch->GetChildrenCount()) {
					kf.WriteStr(">");
					ttCXMLBranch* pFirstChild = pBranch->GetChildAt(0);
					if (pFirstChild->type != ENTITY_PCDATA)
						kf.WriteEol("");

					HRESULT hr = WriteBranch(pFirstChild, kf, iIndent + 1);
					if (FAILED(hr))
						return hr;
					if (pFirstChild->type != ENTITY_PCDATA) {
						for (size_t i = 0; i < iIndent; i++)
							kf.WriteStr("\t");
					}
					kf.WriteStr("</");
					if (pBranch->pszName)
						kf.WriteStr(pBranch->pszName);
					kf.WriteEol(">");
				}
				else {
					kf.WriteEol("/>");
				}
				break;

			case ENTITY_PI:
				for (size_t i = 0; i < iIndent; i++)
					kf.WriteChar('\t');
				kf.WriteStr("<?");
				if (pBranch->pszName)
					kf.WriteStr(pBranch->pszName);
				for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
					XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
					if (pAttr->pszName) {
						kf.WriteChar(' ');
						kf.WriteStr(pAttr->pszName);
						kf.WriteStr("=\042");
						if (pAttr->pszValue)
							kf.WriteStr(pAttr->pszValue);
						kf.WriteStr("\042");
					}
				}
				kf.WriteEol("?>");
				break;

			case ENTITY_PCDATA:
				if (pBranch->pszData)
					kf.WriteStr(pBranch->pszData);
				ttASSERT(!pBranch->GetAttributesCount());
				ttASSERT(!pBranch->GetChildrenCount());
				break;

			default:
				break;	// not handled
		}

		if (iSibling >= pBranch->GetSiblingsCount())
			break;
		pBranch = pBranch->GetSiblingAt(iSibling);
	}
	return S_OK;
}

HRESULT ttCParseXML::SaveHtmlFile(const char* pszFileName)
{
	ttCFile kf;
	WriteHtmlBranch(NULL, kf);
	if (kf.WriteFile(pszFileName))
		return S_OK;
	else
		return E_FAIL;
}

HRESULT ttCParseXML::WriteHtmlBranch(ttCXMLBranch* pBranch, ttCFile& kf)
{
	if (!pBranch) {
		pBranch = GetRootBranch();
		ttASSERT(pBranch);
		ttASSERT(pBranch->GetChildrenCount());
		ttASSERT(pBranch->type == ENTITY_ROOT);
		HRESULT hr = S_OK;

		if (m_cszDocType.IsNonEmpty()) {
			kf.WriteStr(m_cszDocType);
		}

		if (pBranch->GetChildrenCount())
			hr = WriteHtmlBranch(pBranch->GetChildAt(0), kf);

		return hr;
	}

	for (size_t iSibling = 1;;iSibling++) {
		switch (pBranch->type) {
			case ENTITY_ELEMENT:
				kf.WriteStr("<");
				if (pBranch->pszName)
					kf.WriteStr(pBranch->pszName);
				for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
					XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
					if (pAttr->pszName) {
						if (pBranch->GetAttributesCount() > 1) {
							kf.WriteEol("");
						}
						else {
							kf.WriteStr(" ");
						}
						kf.WriteStr(pAttr->pszName);
						kf.WriteStr("=\042");
						if (pAttr->pszValue)
							kf.WriteStr(pAttr->pszValue);
						kf.WriteStr("\042");
					}
				}
				if (pBranch->GetChildrenCount()) {
					kf.WriteStr(">");
					ttCXMLBranch* pFirstChild = pBranch->GetChildAt(0);
					if (pFirstChild->type != ENTITY_PCDATA)
						kf.WriteEol();

					HRESULT hr = WriteHtmlBranch(pFirstChild, kf);
					if (FAILED(hr))
						return hr;
					kf.WriteStr("</");
					if (pBranch->pszName)
						kf.WriteStr(pBranch->pszName);
					kf.WriteEol(">");
				}
				else {
					kf.WriteEol(">");
				}
				break;

			case ENTITY_PI:
				kf.WriteStr("<?");
				if (pBranch->pszName)
					kf.WriteStr(pBranch->pszName);
				for (size_t iAttribute = 0; iAttribute < pBranch->GetAttributesCount(); iAttribute++) {
					XMLATTR* pAttr = pBranch->GetAttributeAt(iAttribute);
					if (pAttr->pszName) {
						kf.WriteChar(' ');
						kf.WriteStr(pAttr->pszName);
						kf.WriteStr("=\042");
						if (pAttr->pszValue)
							kf.WriteStr(pAttr->pszValue);
						kf.WriteStr("\042");
					}
				}
				kf.WriteEol("?>");
				break;

			case ENTITY_PCDATA:
				if (pBranch->pszData)
					kf.WriteStr(pBranch->pszData);
				ttASSERT(!pBranch->GetAttributesCount());
				ttASSERT(!pBranch->GetChildrenCount());
				break;

			default:
				break;
		}

		if (iSibling >= pBranch->GetSiblingsCount())
			break;
		pBranch = pBranch->GetSiblingAt(iSibling);
	}
	return S_OK;
}

ttCXMLBranch* ttCParseXML::AddBranch(ttCXMLBranch* pParent, const char* pszBranchName, XMLENTITY eType)
{
	ttASSERT(pParent);

	ttCXMLBranch* pBranch = GraftBranch(pParent, eType);
	pBranch->pszName = ttStrdup(pszBranchName);
	return pBranch;
}

void ttCParseXML::AddAttribute(ttCXMLBranch* pBranch, const char* pszName, const char* pszValue, size_t iGrow)
{
	ttASSERT(pBranch);
	ttASSERT(pszName);
	ttASSERT(pszValue);
	ttASSERT(*pszName);
	ttASSERT(*pszValue);

	XMLATTR* pAttr = (XMLATTR*) ttMalloc(sizeof(XMLATTR));	// Allocate one attribute.
	pAttr->pszName = ttStrdup(pszName);
	pAttr->pszValue = ttStrdup(pszValue);

	if (pBranch->cAttributes == pBranch->cAttributeSpace) { // Out of space, so grow.
		XMLATTR** t = (XMLATTR**) ttReAlloc(pBranch->aAttributes, sizeof(ttCXMLBranch*) *(pBranch->cAttributeSpace + iGrow));
		if (t) {
			pBranch->aAttributes = t;
			pBranch->cAttributeSpace += iGrow;
		}
		else {	// EXTREMELY unlikely -- means out of system memory
			ttFree(pAttr->pszName);
			ttFree(pAttr->pszValue);
			return;
		}
	}
	pBranch->aAttributes[pBranch->cAttributes] = pAttr;
	pBranch->cAttributes++;
}

ttCXMLBranch* ttCParseXML::AddDataChild(ttCXMLBranch* pParent, const char* pszName, const char* pszData)
{
	ttASSERT(pParent);
	ttASSERT(pszName);
	ttASSERT(*pszName);
	ttASSERT(pszData);
	ttASSERT(*pszData);

	ttCXMLBranch* pChild = AddBranch(pParent, pszName);
	ttCXMLBranch* pSubChild = GraftBranch(pChild, ENTITY_PCDATA);
	pSubChild->pszData = ttStrdup(pszData);
	return pSubChild;
}

// Note that we do NOT FreeAlloc the memory for this node, since we don't have a pointer to CKeyXML that handles memory management
// If the caller needs to FreeAlloc this memory, he should FreeAlloc the pointer to the deleted child and its attributes either before or
// after this call.

bool ttCXMLBranch::RemoveChildAt(size_t i)
{
	ttASSERT(i < cChildren);
	if (i >= cChildren)
		return false;

	for (size_t iChild = i + 1; iChild < cChildren; iChild++) {
		aChildren[iChild - 1] = aChildren[iChild];
	}
	cChildren--;
	return false;
}

bool ttCXMLBranch::ReplaceAttributeValue(ttCParseXML* pxml, const char* pszAttribute, const char* pszNewValue) {
	for (size_t i = 0; i < cAttributes; i++) {
		if (tt::IsSameStrI(pszAttribute, aAttributes[i]->pszName)) {
			if (pxml->isAllocatedStrings())
				pKeyXML->ttFree(aAttributes[i]->pszValue);
			aAttributes[i]->pszValue = pKeyXML->ttStrdup(pszNewValue);
			return true;
		}
	}
	return false;
}

HRESULT ttCParseXML::ParseXmlFile(const char* pszFile)
{
	ttASSERT_NONEMPTY(pszFile);

	if (!pszFile || !*pszFile)
		return STG_E_FILENOTFOUND;

	if (!m_kf.ReadFile(pszFile))
		return STG_E_FILENOTFOUND;

	// CAUTION: ParseXmlString() doesn't allocate strings -- it instead uses pointers into the string buffer it is
	// provided (m_kf in this case). That means the results are only valid while the string buffer remains in memory.

	ParseXmlString(m_kf);

	return S_OK;
}

HRESULT ttCParseXML::ParseHtmlFile(const char* pszFile)
{
	ttASSERT_NONEMPTY(pszFile);

	if (!pszFile || !*pszFile)
		return STG_E_FILENOTFOUND;

	if (!m_kf.ReadFile(pszFile))
		return STG_E_FILENOTFOUND;

	// CAUTION: ParseHtmlString() doesn't allocate strings -- it instead uses pointers into the string buffer it is
	// provided (m_kf in this case). That means the results are only valid while the string buffer remains in memory.

	ParseHtmlString(m_kf);

	return S_OK;
}

size_t ttCXMLBranch::GetSiblingNumber()
{
	if (!parent)
		return 0;

	for (size_t iSibling = 0; iSibling < parent->GetChildrenCount(); iSibling++) {
		if (parent->GetChildAt(iSibling) == this)
			return iSibling;
	}

	return 0;
}

ttCXMLBranch* ttCXMLBranch::FindFirstElement(const char* pszElement)
{
	if (cChildren > 0) {
		for (nextChild = 0; nextChild < cChildren; ++nextChild) {
			if (aChildren[nextChild]->pszName && tt::IsSameStrI(aChildren[nextChild]->pszName, pszElement))
				return aChildren[nextChild];
			else if (aChildren[nextChild]->cChildren) {
				ttCXMLBranch* pBranch = aChildren[nextChild]->FindFirstElement(pszElement);
				if (pBranch)
					return pBranch;
			}
		}
	}
	return nullptr;
}

ttCXMLBranch* ttCXMLBranch::FindNextElement(const char* pszElement)
{
	for (++nextChild; nextChild < cChildren; ++nextChild) {
		if (aChildren[nextChild]->pszName && tt::IsSameStrI(aChildren[nextChild]->pszName, pszElement))
			return aChildren[nextChild];
		else if (aChildren[nextChild]->cChildren) {
			ttCXMLBranch* pBranch = aChildren[nextChild]->FindFirstElement(pszElement);
			if (pBranch)
				return pBranch;
		}
	}
	return nullptr;
}

ttCXMLBranch* ttCXMLBranch::FindFirstElement(HTML_ELEMENT elementSrch)
{
	if (cChildren > 0) {
		for (nextChild = 0; nextChild < cChildren; ++nextChild) {
			if (aChildren[nextChild]->element == elementSrch)
				return aChildren[nextChild];
			else if (aChildren[nextChild]->cChildren) {
				ttCXMLBranch* pBranch = aChildren[nextChild]->FindFirstElement(elementSrch);
				if (pBranch)
					return pBranch;
			}
		}
	}
	return nullptr;
}

ttCXMLBranch* ttCXMLBranch::FindNextElement(HTML_ELEMENT Element)
{
	for (++nextChild; nextChild < cChildren; ++nextChild) {
		if (aChildren[nextChild]->element == Element)
			return aChildren[nextChild];
		else if (aChildren[nextChild]->cChildren) {
			ttCXMLBranch* pBranch = aChildren[nextChild]->FindFirstElement(Element);
			if (pBranch)
				return pBranch;
		}
	}
	return nullptr;
}

// Parse through all the children to see if any of them have the named attribute (pszAttribute). If pszValue is non-null,
// then if the attribute is found it must also have the same value as pszValue.

ttCXMLBranch* ttCXMLBranch::FindFirstAttribute(const char* pszAttribute, const char* pszValue)
{
	ttASSERT_NONEMPTY(pszAttribute);

	if (!pszAttribute || !*pszAttribute)
		return nullptr;

	if (cChildren > 0) {
		for (size_t i = 0; i < cChildren; ++i) {
			if (aChildren[i]->cAttributes) {
				const char* pszAttrVal = aChildren[i]->GetAttribute(pszAttribute);
				if (pszAttrVal) {
					if (!pszValue)
						return aChildren[i];
					else if (tt::IsSameStrI(pszAttrVal, pszValue))
						return aChildren[i];
				}
			}

			if (aChildren[i]->cChildren) {
				ttCXMLBranch* pBranch = aChildren[i]->FindFirstAttribute(pszAttribute, pszValue);
				if (pBranch)
					return pBranch; //Found.
			}
		}
	}
	return nullptr;
}

const char* ttCXMLBranch::GetAttribute(const char* pszAttribute) const
{
	ttASSERT(pszAttribute);
	if (!pszAttribute)
		return NULL;
	XMLATTR* pAttr = MapStringToAttributePtr(pszAttribute);
	if (pAttr)
		return pAttr->pszValue;
	else
		return NULL;
}

void ttCParseXML::SetDocType(size_t type)
{
	switch (type) {
		case DOCTYPE_XHTML_STRICT:
			m_cszDocType = "<!DOCTYPE html PUBLIC \042-//W3C//DTD XHTML 1.0 Strict//EN\042 \042http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\042>";
			break;

		case DOCTYPE_XHTML_TRANSITIONAL:
			m_cszDocType = "<!DOCTYPE html PUBLIC \042-//W3C//DTD XHTML 1.0 Transitional//EN\042 \042http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\042>";
			break;

		case DOCTYPE_HTML_STRICT:
			m_cszDocType = "<!DOCTYPE HTML PUBLIC \042-//W3C//DTD HTML 4.01//EN\042 \042http://www.w3.org/TR/html4/strict.dtd\042>";
			break;

		case DOCTYPE_HTML_TRANSITIONAL:
			m_cszDocType = "<!DOCTYPE HTML PUBLIC \042-//W3C//DTD HTML 4.01 Transitional//EN\042 \042http://www.w3.org/TR/html4/loose.dtd\042>";
			break;

		default:
			ttFAIL("Unsupported DOCTYPE");
			break;
	}
}

#define GROW_SIZE 1 // Default child element & attribute space growth increment.

inline bool IsSymbol(char c) { return (tt::IsAlpha(c) || tt::IsDigit(c) || c=='_' || c==':' || c=='-' || c=='.'); }
inline bool IsEnter(char c) { return (c == '<'); }
inline bool IsLeave(char c) { return (c == '>'); }
inline bool IsDash(char c) { return (c=='-'); }
inline bool IsAttrSymbol(char c) { return (!isSpace(c) && !IsEnter(c) && !IsLeave(c)); }

inline char* SkipSymbol(char* psz) { while (IsSymbol(*psz)) psz++; return psz; }
inline char* SkipAttrSymbol(char* psz) { while (IsAttrSymbol(*psz)) psz++; return psz; }

inline bool	 IsFormatableElement(ttCXMLBranch* pBranch) {
	if (pBranch->element == ELEMENT_PRE || pBranch->element == ELEMENT_SCRIPT)
		return true;
	while (pBranch->parent && pBranch->type != ENTITY_ROOT) {
		pBranch = pBranch->parent;
		if (pBranch->element == ELEMENT_PRE || pBranch->element == ELEMENT_SCRIPT)
			return true;
	}
	return false;
}

#define IsClose(c)			(c=='/')
#define IsConnective(c)		(c=='=')
#define IsSpecial(c)		(c=='!')
#define IsPi(c)				(c=='?')
#define IsQuote(c)			(c=='"' || c=='\'')
#define IsLeftBracket(c)	(c=='[')
#define IsRightBracket(c)	(c==']')
#define SkipWS()			{ while((*psz > 0 && *psz <'!')) ++psz; if (!*psz) return psz; }
#define Push(t)				{ pBranch = GraftBranch(pBranch, t); }
#define Pop()				{ pBranch = pBranch->parent; }
#define ScanUntil(x)		{ while(*psz != 0 && !(x)) ++psz; if (!*psz) return psz; }
#define ScanWhile(x)		{ while((x)) ++psz; if (*psz==0) return psz; }

char* ttCParseXML::ParseXmlString(char* psz, ttCXMLBranch* pRoot)
{
	ttASSERT(psz);
	if (!psz)
		return psz;
	ttCXMLBranch* pBranch;

	if (!pRoot) {
		m_pRoot = NewBranch(ENTITY_ROOT); // Allocate a new root.
		m_pRoot->parent = m_pRoot; // Point to self.
		pBranch = m_pRoot; // Tree branch cursor.
	}
	else {
		pBranch = pRoot; // Tree branch cursor.
	}
	char cChar = 0; // Current char, in cases where we must null-terminate before we test.
	char* pMark = psz; // Marked string position for temporary look-ahead.

	while(*psz != 0) {
LOC_SEARCH: // Obliviously search for next element.
		while (*psz && !IsEnter(*psz))
			psz++;
		if (IsEnter(*psz)) {
			++psz;
LOC_CLASSIFY: // What kind of element?
			if (IsPi(*psz)) {	//'<?...'
				++psz;
				if (IsSymbol(*psz) && (m_uOptions & PARSE_PI)) {
					pMark = psz;
					while (*psz && !IsPi(*psz))
						psz++;
					if (IsPi(*psz))
						*psz ='/'; // Same semantics as for '<.../>', so fudge it.
					psz = pMark;
					Push(ENTITY_PI);  // Graft a new branch on the tree.
					goto LOC_ELEMENT; // Go read the element name.
				}
				else { // Bad PI or PARSE_PI not set.
					while (*psz && !IsLeave(*psz))
						psz++;
					if (!*psz)
						return psz; // unexpected end
					++psz;
					pMark = 0;
					continue;
				}
			}
			else if (IsSpecial(*psz)) { //'<!...'
				++psz;
				if (IsDash(*psz)) { //'<!-...'
					++psz;
					if ((m_uOptions & PARSE_COMMENTS) && IsDash(*psz)) { //'<!--...'
						++psz;
						Push(ENTITY_COMMENT); // Graft a new branch on the tree.
						pBranch->pszData = psz; // Save the offset.
						while(*psz!=0 && *(psz+1) && *(psz+2) && !((IsDash(*psz) && IsDash(*(psz+1))) && IsLeave(*(psz+2))))
							++psz; // Scan for terminating '-->'.
						if (*psz == 0)
							return psz;
						*psz = 0; // Zero-terminate this segment at the first terminating '-'.
						if ((m_uOptions & PARSE_TRIM_COMMENT)) { // Trim whitespace.
							if ((m_uOptions & PARSE_NORMALIZE))
								_StrWnorm(&pBranch->pszData);
							else
								StrWtrim(&pBranch->pszData);
						}
						psz += 2; // Step over the '\0-'.
						Pop(); // Pop since this is a standalone.
						goto LOC_LEAVE; // Look for any following PCDATA.
					}
					else
					{
						while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 && !((IsDash(*psz) && IsDash(*(psz+1))) && IsLeave(*(psz+2)))) ++psz; // Scan for terminating '-->'.
						if (*psz == 0)
							return psz;
						psz += 2;
						goto LOC_LEAVE; // Look for any following PCDATA.
					}
				}
				else if (IsLeftBracket(*psz)) { //'<![...'
					++psz;
					if (*psz =='I') {	 //'<![I...'
						++psz;
						if (*psz =='N') {	 //'<![IN...'
							++psz;
							if (*psz=='C') { //'<![INC...'
								++psz;
								if (*psz=='L') { //'<![INCL...'
									++psz;
									if (*psz=='U') { //'<![INCLU...'
										++psz;
										if (*psz=='D') { //'<![INCLUD...'
											++psz;
											if (*psz=='E') { //'<![INCLUDE...'
												++psz;
												if (IsLeftBracket(*psz)) { //'<![INCLUDE[...'
													++psz;
													if ((m_uOptions & ENTITY_CDATA)) {
														Push(ENTITY_INCLUDE); // Graft a new branch on the tree.
														pBranch->pszData = psz; // Save the offset.
														while(!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) && IsLeave(*(psz+2))))
															++psz; // Scan for terminating ']]>'.
														if (IsRightBracket(*psz)) {
															*psz = 0; // Zero-terminate this segment.
															++psz;
															if ((m_uOptions & PARSE_TRIM_CDATA)) { // Trim whitespace.
																if ((m_uOptions & PARSE_NORMALIZE))
																	_StrWnorm(&pBranch->pszData);
																else
																	StrWtrim(&pBranch->pszData);
															}
														}
														Pop(); // Pop since this is a standalone.
													}
													else // Flagged for discard, but we still have to scan for the terminator.
													{
														while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 &&
																!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) &&
																IsLeave(*(psz+2))))
															++psz; // Scan for terminating ']]>'.
														++psz;
													}
													++psz; // Step over the last ']'.
													goto LOC_LEAVE; // Look for any following PCDATA.
												}
											}
										}
									}
								}
							}
						}
					}
					else if (*psz=='C') { //'<![C...'
						++psz;
						if (*psz=='D') { //'<![CD...'
							++psz;
							if (*psz=='A') { //'<![CDA...'
								++psz;
								if (*psz=='T') { //'<![CDAT...'
									++psz;
									if (*psz=='A') { //'<![CDATA...'
										++psz;
										if (IsLeftBracket(*psz)) { //'<![CDATA[...'
											++psz;
											if ((m_uOptions & PARSE_CDATA)) {
												Push(ENTITY_CDATA); // Graft a new branch on the tree.
												pBranch->pszData = psz; // Save the offset.
												while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 &&
														!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) &&
														IsLeave(*(psz+2))))
													++psz; // Scan for terminating ']]>'.
												if (*(psz+2)==0)
													return psz; // Very badly formed.
												if (IsRightBracket(*psz)) {
													*psz = 0; // Zero-terminate this segment.
													++psz;
													if ((m_uOptions & PARSE_TRIM_CDATA)) { // Trim whitespace.
														if ((m_uOptions & PARSE_NORMALIZE))
															_StrWnorm(&pBranch->pszData);
														else
															StrWtrim(&pBranch->pszData);
													}
												}
												Pop(); // Pop since this is a standalone.
											}
											else // Flagged for discard, but we still have to scan for the terminator.
											{
												while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 &&
														!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) &&
														IsLeave(*(psz+2))))
													++psz; // Scan for terminating ']]>'.
												++psz;
											}
											++psz; // Step over the last ']'.
											goto LOC_LEAVE; // Look for any following PCDATA.
										}
									}
								}
							}
						}
					}
					continue; // Probably a corrupted CDATA section, so just eat it.
				}
				else if (*psz=='D') { //'<!D...'
					++psz;
					if (*psz=='O') { //'<!DO...'
						++psz;
						if (*psz=='C') { //'<!DOC...'
							++psz;
							if (*psz=='T') { //'<!DOCT...'
								++psz;
								if (*psz=='Y') { //'<!DOCTY...'
									++psz;
									if (*psz=='P') { //'<!DOCTYP...'
										++psz;
										if (*psz=='E') { //'<!DOCTYPE...'
											++psz;
											while(isSpace(*psz))
												++psz;
											if (!*psz)
												return psz;
											XMLATTR* a = 0;
											if ((m_uOptions & PARSE_DOCTYPE))
											{
												Push(ENTITY_DOCTYPE); // Graft a new branch on the tree.
												a = AddAttribute(pBranch,3); // Store the DOCTYPE name.
												a->pszValue = a->pszName = psz; // Save the offset.
											}
											psz = SkipSymbol(psz);
											if (!*psz)
												return psz;
											cChar = *psz;
											*psz++ = 0;
											if (!*psz)
												return psz;
											if (isSpace(cChar))
												SkipWS(); // Eat any whitespace.
LOC_DOCTYPE_SYMBOL:
											if (IsSymbol(*psz)) {
												pMark = psz;
												psz = SkipSymbol(psz);
												if (*psz == 0)
													return psz;
												if ((m_uOptions & PARSE_DOCTYPE)) {
													a = AddAttribute(pBranch,1);
													a->pszValue = a->pszName = pMark;
													*psz = 0;
												}
												++psz;
												SkipWS();
											}
											if (IsQuote(*psz)) { //'...SYSTEM "..."'
LOC_DOCTYPE_QUOTE:
												cChar = *psz;
												++psz;
												pMark = psz;
												while(*psz && *psz != cChar)
													++psz;
												if (*psz) {
													if ((m_uOptions & PARSE_DOCTYPE)) {
														a = AddAttribute(pBranch,1);
														a->pszValue = pMark;
														*psz = 0;
													}
													++psz;
													SkipWS(); // Eat whitespace.
													if (IsQuote(*psz))
														goto LOC_DOCTYPE_QUOTE; // Another quoted section to store.
													else if (IsSymbol(*psz))
														goto LOC_DOCTYPE_SYMBOL; // Not wellformed, but just parse it.
												}
											}
											if (IsLeftBracket(*psz)) { //'...[...'
												++psz; // Step over the bracket.
												if ((m_uOptions & PARSE_DOCTYPE))
													pBranch->pszData = psz; // Store the offset.
												int bd = 1; // Bracket depth counter.
												while(*psz!=0) { // Loop till we're out of all brackets.
													if (IsRightBracket(*psz))
														--bd;
													else if (IsLeftBracket(*psz))
														++bd;
													if (bd == 0)
														break;
													++psz;
												}
												if ((m_uOptions & PARSE_DOCTYPE)) {
													*psz = 0; // Zero-terminate.
													if ((m_uOptions & PARSE_DTD)  || (m_uOptions & PARSE_DTD_ONLY)) {
														if ((m_uOptions & PARSE_DTD))
															ParseXmlString(pBranch->pszData, pBranch); // Parse it.
														if ((m_uOptions & PARSE_DTD_ONLY))
															return (psz+1); // Flagged to parse DTD only, so leave here.
													}
													else if ((m_uOptions & PARSE_TRIM_DOCTYPE)) { // Trim whitespace.
														if ((m_uOptions & PARSE_NORMALIZE))
															_StrWnorm(&pBranch->pszData);
														else
															StrWtrim(&pBranch->pszData);
													}
													++psz; // Step over the zero.
													Pop(); // Pop since this is a standalone.
												}
												ScanUntil(IsLeave(*psz));
												continue;
											}
											// Fall-through; make sure we pop.
											Pop(); // Pop since this is a standalone.
											continue;
										}
									}
								}
							}
						}
					}
				}
				else if (IsSymbol(*psz)) { // An inline DTD tag.
					pMark = psz;
					ScanWhile(IsSymbol(*psz));
					cChar = *psz;
					*psz++ = 0;
					if (!*psz)
						return psz;
					XMLENTITY e = ENTITY_DTD_ENTITY;
					if (tt::IsSameStrI(pMark, "ATTLIST"))
						e = ENTITY_DTD_ATTLIST;
					else if (tt::IsSameStrI(pMark, "ELEMENT"))
						e = ENTITY_DTD_ELEMENT;
					else if (tt::IsSameStrI(pMark, "NOTATION"))
						e = ENTITY_DTD_NOTATION;
					Push(e); // Graft a new branch on the tree.
					if (*psz != 0 && isSpace(cChar)) {
						SkipWS(); // Eat whitespace.
						if (IsSymbol(*psz) || *psz =='%') {
							pMark = psz;
							if (*psz =='%')	{ // Could be '<!ENTITY % name' -or- '<!ENTITY %name'
								++psz;
								if (isSpace(*psz)) {
									SkipWS(); // Eat whitespace.
									*(psz-1) ='%';
									pBranch->pszName = (psz-1);
								}
								else
									pBranch->pszName = pMark;
							}
							else
								pBranch->pszName = psz;
							ScanWhile(IsSymbol(*psz));
							cChar = *psz;
							*psz++ = 0;
							if (!*psz)
								return psz;
							if (isSpace(cChar)) {
								SkipWS(); // Eat whitespace.
								if (e == ENTITY_DTD_ENTITY) { // Special case; may have multiple quoted sections w/anything inside.
									pBranch->pszData = psz; // Just store everything here.
									BOOL qq = FALSE; // Quote in/out flag.
									while(*psz!=0) { // Loop till we find the right sequence.
										if (!qq && IsQuote(*psz)) {
											cChar = *psz;
											qq = TRUE;
										}
										else if (qq && *psz == cChar)
											qq = FALSE;
										else if (!qq && IsLeave(*psz)) { // Not in quoted reqion and '>' hit.
											*psz = 0;
											++psz;
											if ((m_uOptions & PARSE_TRIM_ENTITY)) {
												if ((m_uOptions & PARSE_NORMALIZE))
													_StrWnorm(&pBranch->pszData);
												else
													StrWtrim(&pBranch->pszData);
											}
											Pop();
											goto LOC_SEARCH;
										}
										++psz;
									}
									if ((m_uOptions & PARSE_TRIM_ENTITY)) {
										if ((m_uOptions & PARSE_NORMALIZE))
											_StrWnorm(&pBranch->pszData);
										else
											StrWtrim(&pBranch->pszData);
									}
								}
								else {
									pBranch->pszData = psz;
									ScanUntil(IsLeave(*psz)); // Just look for '>'.
									*psz = 0;
									++psz;
									if ((m_uOptions & PARSE_TRIM_ENTITY)) {
										if ((m_uOptions & PARSE_NORMALIZE))
											_StrWnorm(&pBranch->pszData);
										else
											StrWtrim(&pBranch->pszData);
									}
									Pop();
									goto LOC_SEARCH;
								}
							}
						}
					}
					Pop();
				}
			}
			else if (IsSymbol(*psz)) { //'<#...'
				pBranch = GraftBranch(pBranch); // Graft a new branch on the tree.
LOC_ELEMENT: // Scan for & store element name.
				pBranch->pszName = psz;
				while (IsSymbol(*psz))
					psz++;
				if (!*psz)
					return psz;
				cChar = *psz;
				*psz++ = 0;
				if (!*psz)
					return psz;
				if (IsClose(cChar)) {	//'</...'
					while (*psz && !IsLeave(*psz))	// Scan for '>', stepping over the tag name.
						psz++;
					Pop();
					continue;
				}
				else if (!isSpace(cChar))
					goto LOC_PCDATA; // No attributes, so scan for PCDATA.
				else {
					ttASSERT(isSpace(cChar));
					SkipWS(); // Eat any whitespace.
LOC_ATTRIBUTE:
					if (IsSymbol(*psz) || *psz =='%') { //<... #...
						XMLATTR* a = AddAttribute(pBranch, GROW_SIZE);	// Make space for this attribute.
						a->pszName = psz; // Save the offset.
						psz = SkipSymbol(psz);
						if (!*psz)
							return psz;
						cChar = *psz;
						*psz++ = 0;
						if (!*psz)
							return psz;
						if (isSpace(cChar))
							SkipWS(); // Eat any whitespace.
						if ((IsConnective(cChar) || IsConnective(*psz))) { //'<... #=...'
							if (IsConnective(*psz))
								++psz;
							SkipWS(); // Eat any whitespace.
							if (IsQuote(*psz)) { //'<... #="...'
								cChar = *psz; // Save quote char to avoid breaking on "''" -or- '""'.
								++psz; // Step over the quote.
								a->pszValue = psz; // Save the offset.
								psz = tt::FindChar(psz, cChar);
								if (!psz)
									return NULL;
								*psz++ = 0;
								if (!*psz)
									return psz;
								if ((m_uOptions & PARSE_TRIM_ATTRIBUTE)) {	// Trim whitespace.
									if ((m_uOptions & PARSE_NORMALIZE))
										_StrWnorm(&a->pszValue);
									else
										StrWtrim(&a->pszValue);
								}
								if (IsLeave(*psz)) {
									++psz;
									goto LOC_PCDATA;
								}
								else if (IsClose(*psz)) {
									++psz;
									Pop();
									SkipWS(); // Eat any whitespace.
									if (IsLeave(*psz))
										++psz;
									goto LOC_PCDATA;
								}
								if (isSpace(*psz)) { // This may indicate a following attribute.
									SkipWS(); // Eat any whitespace.
									goto LOC_ATTRIBUTE; // Go scan for additional attributes.
								}
							}
						}
						if (IsSymbol(*psz))
							goto LOC_ATTRIBUTE;
						else if (*psz != 0 && pBranch->type == ENTITY_PI) {
							ScanUntil(IsClose(*psz));
							SkipWS(); // Eat any whitespace.
							if (IsClose(*psz))
								++psz;
							SkipWS(); // Eat any whitespace.
							if (IsLeave(*psz))
								++psz;
							Pop();
							goto LOC_PCDATA;
						}
					}
				}
LOC_LEAVE:
				if (IsLeave(*psz)) { //'...>'
					++psz; // Step over the '>'.
LOC_PCDATA: //'>...<'
					pMark = psz; // Save this offset while searching for a terminator.
					SkipWS(); // Eat whitespace if no genuine PCDATA here.
					if (IsEnter(*psz)) { //We hit a '<...', with only whitespace, so don't bother storing anything.
						if (IsClose(*(psz+1))) { //'</...'
							while (*psz && !IsLeave(*psz))
								psz++;
							Pop(); // Pop.
							continue; // Continue scanning.
						}
						else
							goto LOC_SEARCH; // Expect a new element enter, so go scan for it.
					}
					psz = pMark; //We hit something other than whitespace; restore the original offset.
					Push(ENTITY_PCDATA); // Graft a new branch on the tree.
					pBranch->pszData = psz; // Save the offset.
					ScanUntil(IsEnter(*psz)); //'...<'
					cChar = *psz;
					*psz++ = 0;
					if (!*psz)
						return psz;
					if ((m_uOptions & PARSE_TRIM_PCDATA)) { // Trim whitespace.
						if ((m_uOptions & PARSE_NORMALIZE))
							_StrWnorm(&pBranch->pszData);
						else
							StrWtrim(&pBranch->pszData);
					}
					Pop(); // Pop since this is a standalone.
					if (IsEnter(cChar)) { //Did we hit a '<...'?
						if (IsClose(*psz)) { //'</...'
							while (*psz && !IsLeave(*psz))
								psz++;
							Pop(); // Pop.
							goto LOC_LEAVE;
						}

						if (*psz)
							goto LOC_CLASSIFY; //We hit a '<!...'. We must test this here if we want comments intermixed w/PCDATA.
						else
							return psz;
					}
				}
				// Fall-through A.
				else if (IsClose(*psz)) { //'.../'
					++psz;
					if (IsLeave(*psz)) { //'.../>'
						Pop(); // Pop.
						++psz;
						continue;
					}
				}
			}
			// Fall-through B.
			else if (IsClose(*psz)) { //'.../'
				ScanUntil(IsLeave(*psz)); //'.../>'
				Pop(); // Pop.
				continue;
			}
		}
	}
	return psz;
}

bool IsEndTagForbidden(HTML_ELEMENT element)	// returns true of the element does not have a close tag
{
	if (	element == ELEMENT_AREA ||
			element == ELEMENT_BASE ||
			element == ELEMENT_BASEFONT ||
			element == ELEMENT_BGSOUND ||
			element == ELEMENT_BR ||
			element == ELEMENT_COL ||
			element == ELEMENT_FRAME ||
			element == ELEMENT_HR ||
			element == ELEMENT_IMG ||
			element == ELEMENT_INPUT ||
			element == ELEMENT_ISINDEX ||
			element == ELEMENT_LINK ||
			element == ELEMENT_META ||
			element == ELEMENT_PARAM)
		return true;
	else
		return false;
}

bool DoesEndTagCloseChildren(HTML_ELEMENT element) // returns true if the element should force closed any child elements
{
	if (
			element == ELEMENT_BODY ||
			element == ELEMENT_DIR ||
			element == ELEMENT_DL ||
			element == ELEMENT_HEAD ||
			element == ELEMENT_HTML ||
			element == ELEMENT_MENU ||
			element == ELEMENT_OBJECT ||
			element == ELEMENT_OL ||
			element == ELEMENT_P ||
			element == ELEMENT_TABLE ||
			element == ELEMENT_TR ||
			element == ELEMENT_UL)
		return true;
	else
		return false;
}

char* ttCParseXML::ParseHtmlString(char* psz, ttCXMLBranch* pRoot)
{
	ttASSERT(psz);
	if (!psz)
		return psz;
	ttCXMLBranch* pBranch;
	bool bInScriptSection = false;
	HTML_ELEMENT elemNew;

	if (!pRoot) {
		m_pRoot = NewBranch(ENTITY_ROOT); // Allocate a new root.
		m_pRoot->parent = m_pRoot; // Point to self.
		pBranch = m_pRoot; // Tree branch cursor.
	}
	else {
		pBranch = pRoot; // Tree branch cursor.
	}
	char cChar = 0;    // Current char, in cases where we must null-terminate before we test.
	char* pMark = psz; // Marked string position for temporary look-ahead.

	while(*psz != 0) {
LOC_SEARCH: // Obliviously search for next element.
		while (*psz && !IsEnter(*psz))
			psz++;
		if (IsEnter(*psz)) {
			++psz;
LOC_CLASSIFY: // What kind of element?
			if (IsPi(*psz)) {	//'<?...'
				++psz;
				if (IsSymbol(*psz) && (m_uOptions & PARSE_PI)) {
					pMark = psz;
					while (*psz && !IsPi(*psz))
						psz++;
					if (IsPi(*psz))
						*psz = '/';   // Same semantics as for '<.../>', so fudge it.
					psz = pMark;
					Push(ENTITY_PI);  // Graft a new branch on the tree.
					char* pszElement = psz;
					psz = SkipSymbol(psz);
					if (!*psz)
						return psz;
					char cTmpChar = *psz;
					*psz = 0;
					elemNew = ParseElementTag(pszElement, psz + 1);
					*psz = cTmpChar;
					psz = pszElement;
					goto LOC_ELEMENT; // Go read the element name.
				}
				else { // Bad PI or PARSE_PI not set.
					while (*psz && !IsLeave(*psz))
						psz++;
					if (!*psz)
						return psz; // unexpected end
					++psz;
					pMark = 0;
					continue;
				}
			}
			else if (IsSpecial(*psz)) { //'<!...'
				++psz;
				if (IsDash(*psz)) { 	//'<!-...'
					++psz;
					if ((m_uOptions & PARSE_COMMENTS) && IsDash(*psz)) { //'<!--...'
						++psz;
						Push(ENTITY_COMMENT);	// Graft a new branch on the tree.
						pBranch->pszData = psz; // Save the offset.
						while(*psz!=0 && *(psz+1) && *(psz+2) && !((IsDash(*psz) && IsDash(*(psz+1))) && IsLeave(*(psz+2))))
							++psz; // Scan for terminating '-->'.
						if (*psz == 0)
							return psz;
						*psz = 0; // Zero-terminate this segment at the first terminating '-'.
						if (!bInScriptSection)
							_StrWnorm(&pBranch->pszData);
						psz += 2;		// Step over the '\0-'.
						Pop();	  		// Pop since this is a standalone.
						goto LOC_LEAVE; // Look for any following PCDATA.
					}
					else
					{
						while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 && !((IsDash(*psz) && IsDash(*(psz+1))) && IsLeave(*(psz+2)))) ++psz; // Scan for terminating '-->'.
						if (*psz == 0)
							return psz;
						psz += 2;
						goto LOC_LEAVE; // Look for any following PCDATA.
					}
				}
				else if (IsLeftBracket(*psz)) { //'<![...'
					++psz;
					if (*psz == 'I') {	 //'<![I...'
						++psz;
						if (*psz == 'N') {	 //'<![IN...'
							++psz;
							if (*psz=='C') { //'<![INC...'
								++psz;
								if (*psz=='L') { //'<![INCL...'
									++psz;
									if (*psz=='U') { //'<![INCLU...'
										++psz;
										if (*psz=='D') { //'<![INCLUD...'
											++psz;
											if (*psz=='E') { //'<![INCLUDE...'
												++psz;
												if (IsLeftBracket(*psz)) { //'<![INCLUDE[...'
													++psz;
													if ((m_uOptions & ENTITY_CDATA)) {
														Push(ENTITY_INCLUDE); // Graft a new branch on the tree.
														pBranch->pszData = psz; // Save the offset.
														while(!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) && IsLeave(*(psz+2))))
															++psz; // Scan for terminating ']]>'.
														if (IsRightBracket(*psz)) {
															*psz = 0; // Zero-terminate this segment.
															++psz;
															if ((m_uOptions & PARSE_TRIM_CDATA)) { // Trim whitespace.
																_StrWnorm(&pBranch->pszData);
															}
														}
														Pop(); // Pop since this is a standalone.
													}
													else // Flagged for discard, but we still have to scan for the terminator.
													{
														while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 &&
																!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) &&
																IsLeave(*(psz+2))))
															++psz; // Scan for terminating ']]>'.
														++psz;
													}
													++psz; // Step over the last ']'.
													goto LOC_LEAVE; // Look for any following PCDATA.
												}
											}
										}
									}
								}
							}
						}
					}
					else if (*psz=='C') { //'<![C...'
						++psz;
						if (*psz=='D') { //'<![CD...'
							++psz;
							if (*psz=='A') { //'<![CDA...'
								++psz;
								if (*psz=='T') { //'<![CDAT...'
									++psz;
									if (*psz=='A') { //'<![CDATA...'
										++psz;
										if (IsLeftBracket(*psz)) { //'<![CDATA[...'
											++psz;
											if ((m_uOptions & PARSE_CDATA)) {
												Push(ENTITY_CDATA); // Graft a new branch on the tree.
												pBranch->pszData = psz; // Save the offset.
												while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 &&
														!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) &&
														IsLeave(*(psz+2))))
													++psz; // Scan for terminating ']]>'.
												if (*(psz+2)==0)
													return psz; // Very badly formed.
												if (IsRightBracket(*psz)) {
													*psz = 0; // Zero-terminate this segment.
													++psz;
													if ((m_uOptions & PARSE_TRIM_CDATA)) { // Trim whitespace.
														_StrWnorm(&pBranch->pszData);
													}
												}
												Pop(); // Pop since this is a standalone.
											}
											else // Flagged for discard, but we still have to scan for the terminator.
											{
												while(*psz!=0 && *(psz+1)!=0 && *(psz+2)!=0 &&
														!(IsRightBracket(*psz) && IsRightBracket(*(psz+1)) &&
														IsLeave(*(psz+2))))
													++psz; // Scan for terminating ']]>'.
												++psz;
											}
											++psz; // Step over the last ']'.
											goto LOC_LEAVE; // Look for any following PCDATA.
										}
									}
								}
							}
						}
					}
					continue; // Probably a corrupted CDATA section, so just eat it.
				}
				else if (tt::IsSameSubStrI(psz, "DOCTYPE")) {
					psz += sizeof("DOCTYPE");
					while(isSpace(*psz))
						++psz;
					if (!*psz)
						return psz;
					XMLATTR* a = NULL;
					Push(ENTITY_DOCTYPE); // Graft a new branch on the tree.
					a = AddAttribute(pBranch, 3); // Store the DOCTYPE name.
					a->pszValue = a->pszName = psz; // Save the offset.
					psz = SkipSymbol(psz);
					if (!*psz)
						return psz;
					cChar = *psz;
					*psz++ = 0;
					if (!*psz)
						return psz;
					if (isSpace(cChar))
						SkipWS(); // Eat any whitespace.
LOC_DOCTYPE_SYMBOL:
					if (IsSymbol(*psz)) {
						pMark = psz;
						psz = SkipSymbol(psz);
						if (*psz == 0)
							return psz;
						a = AddAttribute(pBranch,1);
						a->pszValue = a->pszName = pMark;
						*psz = 0;
						++psz;
						SkipWS();
					}
					if (IsQuote(*psz)) { //'...SYSTEM "..."'
LOC_DOCTYPE_QUOTE:
						cChar = *psz;
						++psz;
						pMark = psz;
						while(*psz && *psz != cChar)
							++psz;
						if (*psz) {
							a = AddAttribute(pBranch,1);
							a->pszValue = pMark;
							*psz = 0;
							++psz;
							SkipWS(); // Eat whitespace.
							if (IsQuote(*psz))
								goto LOC_DOCTYPE_QUOTE; // Another quoted section to store.
							else if (IsSymbol(*psz))
								goto LOC_DOCTYPE_SYMBOL; // Not wellformed, but just parse it.
						}
						if (IsLeftBracket(*psz)) { //'...[...'
							++psz; // Step over the bracket.
							pBranch->pszData = psz; // Store the offset.
							size_t bd = 1; // Bracket depth counter.
							while (*psz != 0) {	  // Loop till we're out of all brackets.
								if (IsRightBracket(*psz))
									--bd;
								else if (IsLeftBracket(*psz))
									++bd;
								if (bd == 0)
									break;
								++psz;
							}
							*psz = 0; // Zero-terminate.
							if ((m_uOptions & PARSE_DTD)  || (m_uOptions & PARSE_DTD_ONLY)) {
								if ((m_uOptions & PARSE_DTD))
									ParseHtmlString(pBranch->pszData, pBranch); // Parse it.
								if ((m_uOptions & PARSE_DTD_ONLY))
									return (psz+1); // Flagged to parse DTD only, so leave here.
							}
							else if ((m_uOptions & PARSE_TRIM_DOCTYPE)) { // Trim whitespace.
								_StrWnorm(&pBranch->pszData);
							}
							++psz; // Step over the zero.
							Pop(); // Pop since this is a standalone.
							ScanUntil(IsLeave(*psz));
							continue;
						}
						// Fall-through; make sure we pop.
						Pop(); // Pop since this is a standalone.
						continue;
					}
				}
				else if (IsSymbol(*psz)) { // An inline DTD tag.
					pMark = psz;
					ScanWhile(IsSymbol(*psz));
					cChar = *psz;
					*psz++ = 0;
					if (!*psz)
						return psz;
					XMLENTITY e = ENTITY_DTD_ENTITY;
					if (tt::IsSameStrI(pMark,"ATTLIST"))
						e = ENTITY_DTD_ATTLIST;
					else if (tt::IsSameStrI(pMark,"ELEMENT"))
						e = ENTITY_DTD_ELEMENT;
					else if (tt::IsSameStrI(pMark,"NOTATION"))
						e = ENTITY_DTD_NOTATION;
					Push(e); // Graft a new branch on the tree.
					if (*psz != 0 && isSpace(cChar)) {
						SkipWS(); // Eat whitespace.
						if (IsSymbol(*psz) || *psz == '%') {
							pMark = psz;
							if (*psz == '%')	{ // Could be '<!ENTITY % name' -or- '<!ENTITY %name'
								++psz;
								if (isSpace(*psz)) {
									SkipWS(); // Eat whitespace.
									*(psz-1) = '%';
									pBranch->pszName = (psz-1);
								}
								else
									pBranch->pszName = pMark;
							}
							else
								pBranch->pszName = psz;
							ScanWhile(IsSymbol(*psz));
							cChar = *psz;
							*psz++ = 0;
							if (!*psz)
								return psz;
							if (isSpace(cChar)) {
								SkipWS(); // Eat whitespace.
								if (e == ENTITY_DTD_ENTITY) { // Special case; may have multiple quoted sections w/anything inside.
									pBranch->pszData = psz;  // Just store everything here.
									BOOL qq = FALSE;		  // Quote in/out flag.
									while (*psz != 0) { 	  // Loop till we find the right sequence.
										if (!qq && IsQuote(*psz)) {
											cChar = *psz;
											qq = TRUE;
										}
										else if (qq && *psz == cChar)
											qq = FALSE;
										else if (!qq && IsLeave(*psz)) { // Not in quoted reqion and '>' hit.
											*psz = 0;
											++psz;
											if ((m_uOptions & PARSE_TRIM_ENTITY)) {
												_StrWnorm(&pBranch->pszData);
											}
											Pop();
											goto LOC_SEARCH;
										}
										++psz;
									}
									if ((m_uOptions & PARSE_TRIM_ENTITY)) {
										_StrWnorm(&pBranch->pszData);
									}
								}
								else {
									pBranch->pszData = psz;
									ScanUntil(IsLeave(*psz)); // Just look for '>'.
									*psz = 0;
									++psz;
									if ((m_uOptions & PARSE_TRIM_ENTITY)) {
										_StrWnorm(&pBranch->pszData);
									}
									Pop();
									goto LOC_SEARCH;
								}
							}
						}
					}
					Pop();
				}
			}
			else if (IsSymbol(*psz)) { //'<#...'
				{
					char* pszElement = psz;
					psz = SkipSymbol(psz);
					if (!*psz)
						return psz;
					char cTmpChar = *psz;
					*psz = 0;
					elemNew = ParseElementTag(pszElement, psz + 1);
					*psz = cTmpChar;
					psz = pszElement;
				}

				// Some tags will force close previous tags

				if (pBranch->element == ELEMENT_FORM) {
					// We only see table or list elements if the form is part of a table or list -- a nested table
					// or list would not have our FORM as a parent

					if (elemNew == ELEMENT_TD || elemNew == ELEMENT_TR || elemNew == ELEMENT_LI || elemNew == ELEMENT_DT) {
						Pop();
					}
				}

				if (pBranch->element == ELEMENT_TR && elemNew == ELEMENT_TR) {
					Pop();	// Close the current Table Data element
				}
				if (pBranch->element == ELEMENT_TD && (elemNew == ELEMENT_TD || elemNew == ELEMENT_TR)) {
					Pop();	// Close the current Table Data element
				}

				if (pBranch->element == ELEMENT_LI && elemNew == ELEMENT_LI) {
					Pop();	// Close the current list element
				}
				else if (pBranch->element == ELEMENT_LI && elemNew == ELEMENT_DT) {
					Pop();	// Close the current list element
				}


				pBranch = GraftBranch(pBranch); // Graft a new branch on the tree.
LOC_ELEMENT: // Scan for & store element name.
				pBranch->pszName = psz;
				psz = SkipSymbol(psz);
				if (!*psz)
					return psz;
				cChar = *psz;
				*psz++ = 0;
				ttASSERT(elemNew == ParseElementTag(pBranch->pszName, psz + 1));
				pBranch->element = elemNew;
				if (!*psz)
					return psz;

				if (pBranch->element == ELEMENT_BODY)
					m_pBodyBranch = pBranch;
				else if (pBranch->element == ELEMENT_HEAD)
					m_pHeadBranch = pBranch;
				else if (pBranch->element == ELEMENT_MSH_LINK)
					m_aMSHLinks.Add(pBranch);
				else if (pBranch->element == ELEMENT_OBJECT)
					m_aObjectTags.Add(pBranch);
				else if (pBranch->element == ELEMENT_TITLE)
					m_pTitleBranch = pBranch;

				if (IsClose(cChar)) {	//'</...'
					const char* pszStart = tt::FindNonSpace(psz + 1);
					while (*psz && !IsLeave(*psz))	// Scan for '>', stepping over the tag name.
						psz++;
					char chSave = *psz;
					*psz = 0;

					// HTML 4.01 specifies that a number of tags cannot contain end tags. That does not,
					// however, prevent people from authoring those end tags. We need to check for that
					// condition here, and not close the current element if a forbidden end tag was specified
					// (because the element that can't have and end tag is already closed).

					if (!IsEndTagForbidden(ParseElementTag(pszStart, psz + 1)))
						Pop();
					*psz = chSave;
					continue;
				}
				else if (IsLeave(cChar)) {
					if (IsEndTagForbidden(pBranch->element))
						Pop();
					goto LOC_PCDATA;
				}
				else if (!isSpace(cChar))
					goto LOC_PCDATA; // No attributes, so scan for PCDATA.

				else {
					ttASSERT(isSpace(cChar));
					bInScriptSection = (pBranch->element == ELEMENT_SCRIPT);
					SkipWS(); // Eat any whitespace.
LOC_ATTRIBUTE:
					if (IsSymbol(*psz)) { //<... #...
						XMLATTR* a = AddAttribute(pBranch, GROW_SIZE);	// Make space for this attribute.
						a->pszName = psz; // Save the offset.
						psz = SkipSymbol(psz);
						if (!*psz)
							return psz;
						cChar = *psz;
						*psz++ = 0;
						CharLower(a->pszName);	// XHTML DTD requires attributes to be lower case
						if (!*psz)
							return psz;
						if (isSpace(cChar))
							SkipWS(); // Eat any whitespace.
						if ((IsConnective(cChar) || IsConnective(*psz))) { //'<... #=...'
							if (IsConnective(*psz))
								++psz;
							SkipWS(); // Eat any whitespace.
							if (IsQuote(*psz)) { //'<... #="...'
								cChar = *psz; // Save quote char to avoid breaking on "''" -or- '""'.
								++psz; // Step over the quote.
								a->pszValue = psz; // Save the offset.
								psz = tt::FindChar(psz, cChar);
								if (!psz)
									return NULL;
								*psz++ = 0;
								if (!*psz)
									return psz;
								_StrWnorm(&a->pszValue);
								if (IsLeave(*psz)) {
									++psz;
									if (IsEndTagForbidden(pBranch->element))
										Pop();
									goto LOC_PCDATA;
								}
								else if (IsClose(*psz)) {
									++psz;
									Pop();
									SkipWS(); // Eat any whitespace.
									if (IsLeave(*psz))
										++psz;
									goto LOC_PCDATA;
								}
								if (isSpace(*psz)) { // This may indicate a following attribute.
									SkipWS(); // Eat any whitespace.
									goto LOC_ATTRIBUTE; // Go scan for additional attributes.
								}
							}
							else {	// HTML attributes don't have to be quoted if they don't have any spaces
								a->pszValue = psz; // Save the offset.
								psz = SkipAttrSymbol(psz);
								char chSave = *psz;
								*psz = '\0';
								a->pszValue = ttStrdup(a->pszValue);
								*psz = chSave;
								if (IsLeave(*psz)) {
									++psz;
									if (IsEndTagForbidden(pBranch->element))
										Pop();
									goto LOC_PCDATA;
								}
								else if (IsClose(*psz)) {
									++psz;
									Pop();
									SkipWS(); // Eat any whitespace.
									if (IsLeave(*psz))
										++psz;
									goto LOC_PCDATA;
								}
								if (isSpace(*psz)) { // This may indicate a following attribute.
									SkipWS(); // Eat any whitespace.
									goto LOC_ATTRIBUTE; // Go scan for additional attributes.
								}
							}
						}
						if (IsLeave(cChar)) {	// attribute with no value
							a->pszValue = ttStrdup("true");
							if (IsEndTagForbidden(pBranch->element))
								Pop();
							goto LOC_PCDATA;
						}
						if (IsSymbol(*psz))
							goto LOC_ATTRIBUTE;
						else if (*psz != 0 && pBranch->type == ENTITY_PI) {
							ScanUntil(IsClose(*psz));
							SkipWS(); // Eat any whitespace.
							if (IsClose(*psz))
								++psz;
							SkipWS(); // Eat any whitespace.
							if (IsLeave(*psz))
								++psz;
							Pop();
							goto LOC_PCDATA;
						}
					}
				}
LOC_LEAVE:
				if (IsLeave(*psz)) { //'...>'
					++psz; // Step over the '>'.
LOC_PCDATA: //'>...<'
					pMark = psz; // Save this offset while searching for a terminator.
					if (!IsFormatableElement(pBranch))
						SkipWS(); // Eat whitespace if no genuine PCDATA here.
					if (IsEnter(*psz)) { // We hit a '<...', with only whitespace, so don't bother storing anything.
						if (IsClose(psz[1])) { //'</...'
							const char* pszStart = tt::FindNonSpace(psz + 2);
							while (*psz && !IsLeave(*psz))	// Scan for '>', stepping over the tag name.
								psz++;
							*psz = 0;

							// Don't close <P> if there is no open <P>

							if (pBranch->element != ELEMENT_P && ParseElementTag(pszStart, psz + 1, true) == ELEMENT_P) {
								psz++;
								goto LOC_PCDATA;
							}

							// HTML 4.01 specifies that a number of tags cannot contain end tags. That does not,
							// however, prevent people from authoring those end tags. We need to check for that
							// condition here, and not close the current element if a forbidden end tag was specified
							// (because the element that can't have and end tag is already closed).

							HTML_ELEMENT elemClose = ParseElementTag(pszStart, psz + 1, true);
							if (!IsEndTagForbidden(elemClose)) {
								// Some authors add more close tags for things like <span> then are actually needed
								if (elemClose == ELEMENT_SPAN && pBranch->element != ELEMENT_SPAN)	{
									psz++;
									goto LOC_PCDATA;
								}
								if (elemClose == ELEMENT_DIV && pBranch->element != ELEMENT_DIV) {
									psz++;
									goto LOC_PCDATA;
								}

								// The <form> element cannot be nested, so if we are looking at a </form> then only allow it
								// if we are within a <form> section.

								if (elemClose == ELEMENT_FORM && pBranch->element != ELEMENT_FORM) {
									ttCXMLBranch* pParent = pBranch->parent;
									while (pParent->element != ELEMENT_FORM && pParent->type != ENTITY_ROOT)	{
										pParent = pParent->parent;
									}
									if (pParent->element != ELEMENT_FORM) {
										psz++;
										goto LOC_PCDATA;
									}
								}

								// Some end tags automatically close any open child tags. We check for unclosed child tags here
								// and force them closed

								if (pBranch->element != elemClose && pBranch->parent && DoesEndTagCloseChildren(elemClose)) {
									do {
										Pop();
									} while(pBranch->element != elemClose && pBranch->parent->type != ENTITY_ROOT);
								}
								Pop();
							}
							psz++;
							goto LOC_PCDATA;
						}
						else {
							if (pBranch->element == ELEMENT_P) {
								char* pszSave = psz;
								char* pszElement = ++psz;
								ScanWhile(IsSymbol(*psz));
								char chSave = *psz;
								*psz = 0;
								HTML_ELEMENT NewElement = ParseElementTag(pszElement, psz + 1);
								*psz = chSave;
								psz = pszSave;

								if (NewElement == ELEMENT_P)
									Pop();	// can't have nested paragraphcs
							}
							goto LOC_SEARCH; // Expect a new element enter, so go scan for it.
						}
					}
					psz = pMark; //We hit something other than whitespace; restore the original offset.
					Push(ENTITY_PCDATA); // Graft a new branch on the tree.
					pBranch->pszData = psz; // Save the offset.
					ScanUntil(IsEnter(*psz)); //'...<'

					// HTML script can contain '<' and other characters. We don't want to parse each and every one of those, so
					// instead when we encounter a '<' character, we look to see if the next non-space character begins a /script
					// string, and if so, we assume that is the end of the script. Theoretically, a script could have a quoted
					// end script tag, e.g., document.write("</script") -- but that's unlikely enough that we shouldn't have to
					// worry about it.

					while (bInScriptSection) {
						if (!*psz) {
							bInScriptSection = false;
							break;
						}
						if (tt::IsSameSubStrI(tt::FindNonSpace(psz + 1), "/script")) {
							bInScriptSection = false;
							break;
						}
						psz++;
						ScanUntil(IsEnter(*psz)); //'...<'
					}
					cChar = *psz;
					*psz++ = 0;
					if (!*psz)
						return psz;
					// Note that unlike XML, we don't normalize the string
					Pop(); // Pop since this is a standalone.
					if (IsEnter(cChar)) { //Did we hit a '<...'?
						if (IsClose(*psz)) { //'</...'
							psz++;	// step over the '/' character
							SkipWS();	// skip over any whitespite
							char* pszCloseElement = psz;
							while (*psz && *psz != '>')
								psz++;

							// In HTML, some elements don't need close tags. If we encounter a close tag of
							// the parent element, then we need to first Pop out of the current branch since
							// it is effectively closed.

							char chSave = *psz;
							*psz = 0;
							HTML_ELEMENT CloseElement = ParseElementTag(pszCloseElement, psz + 1);
							*psz = chSave;

							// Can't have a close paragraph if there is no open paragraph

							if (CloseElement == ELEMENT_P && pBranch->element != ELEMENT_P)
								goto LOC_LEAVE;

							if (pBranch->GetElementTag() == ELEMENT_LI) {
								if (	CloseElement == ELEMENT_OL ||
										CloseElement == ELEMENT_UL ||
										CloseElement == ELEMENT_DIR ||
										CloseElement == ELEMENT_MENU) {
									Pop();
									// You cannot have a nested LI -- if found, it means the previous nested item auto-close the
									// list item.
									while (pBranch->GetElementTag() == ELEMENT_LI)
										Pop();
								}
							}
							else if (pBranch->GetElementTag() == ELEMENT_BODY && CloseElement == ELEMENT_HTML) {
								Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_DT || pBranch->GetElementTag() == ELEMENT_DD) {
								if (CloseElement == ELEMENT_DL)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_TD || pBranch->GetElementTag() == ELEMENT_TH) {
								if (CloseElement == ELEMENT_TABLE) {
									Pop();
									if (pBranch->GetElementTag() == ELEMENT_TR)
										Pop();
									if (pBranch->GetElementTag() == ELEMENT_COLGROUP)
										Pop();
								}
							}
							else if (pBranch->GetElementTag() == ELEMENT_TR) {
								if (CloseElement == ELEMENT_TABLE)
									Pop();
							}

							Pop(); // Pop.
							goto LOC_LEAVE;
						}
						SkipWS();	// skip over any whitespite
						if (tt::IsAlpha(*psz)) {
							char* pszElement = psz;
							ScanWhile(IsSymbol(*psz));
							char chSave = *psz;
							*psz = 0;
							HTML_ELEMENT NewElement = ParseElementTag(pszElement, psz + 1);
							*psz = chSave;
							psz = pszElement;

							if (pBranch->GetElementTag() == ELEMENT_P) {
								if (NewElement == ELEMENT_P)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_LI) {
								if (NewElement == ELEMENT_LI)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_DT || pBranch->GetElementTag() == ELEMENT_DD) {
								if (NewElement == ELEMENT_DT || NewElement == ELEMENT_DD)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_HEAD) {
								if (NewElement == ELEMENT_BODY)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_THEAD) {
								if (NewElement == ELEMENT_TBODY || NewElement == ELEMENT_TFOOT)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_TBODY) {
								if (NewElement == ELEMENT_TFOOT)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_TR) {
								if (NewElement == ELEMENT_TR)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_TD) {
								if (NewElement == ELEMENT_TR || NewElement == ELEMENT_TH || NewElement == ELEMENT_TD)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_TH) {
								if (NewElement == ELEMENT_TR || NewElement == ELEMENT_TH || NewElement == ELEMENT_TD)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_COLGROUP) {
								if (NewElement == ELEMENT_COLGROUP)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_HEAD) {
								if (NewElement == ELEMENT_BODY)
									Pop();
							}
							else if (pBranch->GetElementTag() == ELEMENT_P) {
								if (NewElement == ELEMENT_P)	// we don't allow nested paragraphs
									Pop();
							}
							goto LOC_CLASSIFY; //We hit a '<!...'. We must test this here if we want comments intermixed w/PCDATA.
						}

						if (*psz)
							goto LOC_CLASSIFY; //We hit a '<!...'. We must test this here if we want comments intermixed w/PCDATA.
						else
							return psz;
					}
				}
				// Fall-through A.
				else if (IsClose(*psz)) { //'.../'
					++psz;
					if (IsLeave(*psz)) { //'.../>'
						Pop(); // Pop.
						++psz;
						continue;
					}
				}
			}
			// Fall-through B.
			else if (IsClose(*psz)) { //'.../'
				ScanUntil(IsLeave(*psz)); //'.../>'
				Pop(); // Pop.
				continue;
			}
		}
	}
	return psz;
}
