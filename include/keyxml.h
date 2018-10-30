/////////////////////////////////////////////////////////////////////////////
// Original Name: Pug XML Parser
// Author:		Kristen Wegner
// Copyright:	Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Name:		CKeyXML, CKeyXmlBranch
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2003-2018 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_KEYXML_H__
#define __TTLIB_KEYXML_H__

#include "hashpair.h"		// CHashPair
#include "ttarray.h"		// CTTArray
#include "cstr.h"			// CStr
#include "keyxml_defs.h"	// #defines and enums
#include "keyfile.h"		// CKeyFile

class CKeyXML;			// forward definition
class CKeyXmlBranch;	// forward definition

typedef struct
{
	char*	pszName;					// Pointer to attribute name.
	char*	pszValue;					// Pointer to attribute value.
} XMLATTR;

typedef struct
{
	CKeyXmlBranch*	parent;				// Pointer to parent
	char*			pszName;			// Pointer to element name.
	XMLENTITY		type;				// Branch type; see XMLENTITY.
	size_t			cAttributes;		// Count attributes.
	size_t			cAttributeSpace;	// Available pointer space in 'attribute'.
	XMLATTR**		aAttributes;		// Array of pointers to attributes; see XMLATTR.
	size_t			cChildren;			// Count children in member 'child'.
	size_t			cChildSpace;		// Available pointer space in 'child'.
	CKeyXmlBranch**	aChildren;			// Array of pointers to children.
	char*			pszData;			// Pointer to any associated string data.
	HTML_ELEMENT	element;			// HTML Element -- only valid in an HTML file
} XMLBRANCH;

class CKeyXmlBranch : public XMLBRANCH
{
public:
	bool IsNull()					const { return type == ENTITY_NULL; }
	bool IsElement()				const { return type == ENTITY_ELEMENT; }
	bool IsComment()				const { return type == ENTITY_COMMENT; }
	bool IsPCDATA()					const { return type == ENTITY_PCDATA; }
	bool IsCDATA()					const { return type == ENTITY_CDATA; }
	bool IsINCLUDE()				const { return type == ENTITY_INCLUDE; }
	bool IsPI()						const { return type == ENTITY_PI; }
	bool IsDOCTYPE()				const { return type == ENTITY_DOCTYPE; }
	bool IsDTD()					const { return type >  ENTITY_DOCTYPE; }
	bool IsDTD_ATTLIST()			const { return type == ENTITY_DTD_ATTLIST; }
	bool IsDTD_ELEMENT()			const { return type == ENTITY_DTD_ELEMENT; }
	bool IsDTD_ENTITY()				const { return type == ENTITY_DTD_ENTITY; }
	bool IsDTD_NOTATION()			const { return type == ENTITY_DTD_NOTATION; }
	bool IsNamed(const char* pszNamed)	const { return pszName ? IsSameString(pszName, pszNamed) : false; }
	bool IsRoot()					const { return this == parent; }

	// Class functions

	CKeyXmlBranch*	FindFirstElement(HTML_ELEMENT element);
	CKeyXmlBranch*	FindFirstElement(const char* pszName);
	const char*		GetAttribute(const char* pszName) const;
	XMLATTR*		GetAttributeAt(size_t i) { ASSERT(i < cAttributes); return (i < cAttributes) ? aAttributes[i] : NULL; }
	size_t			GetAttributesCount() const { return cAttributes; }
	CKeyXmlBranch*	GetChildAt(size_t i) { ASSERT(i < cChildren); return (i < cChildren) ? aChildren[i] : NULL; }
	size_t			GetChildrenCount() const { return cChildren; }
	const char*		GetData() { return pszData ? pszData : ""; }
	HTML_ELEMENT	GetElementTag() const { return element; }
	const char*		GetName() const { return pszName ? pszName : ""; }
	CKeyXmlBranch*	GetSiblingAt(size_t i) { return (!IsRoot() && i < GetSiblingsCount()) ? parent->aChildren[i] : NULL; }
	size_t			GetSiblingNumber();
	size_t			GetSiblingsCount() const { return (!IsRoot()) ? parent->cChildren : 0; }
	XMLENTITY		GetType() const { return type; }
	bool			RemoveChildAt(size_t i);
	bool			ReplaceAttributeValue(CKeyXML* pxml, const char* pszName, const char* pszNewValue);
	const char*		GetFirstChildData() { if (cChildren) return GetChildAt(0)->GetData(); else return NULL; }

	inline XMLATTR* MapStringToAttributePtr(const char* pszString) const {
		for (size_t i = 0; i < cAttributes; i++) {
			if (IsSameString(pszString, aAttributes[i]->pszName))
				return aAttributes[i];
		}
		return nullptr;
	}

	CKeyXmlBranch* operator[](size_t i){ return (CKeyXmlBranch*) GetChildAt(i); }
};

class CKeyXML
{
public:
	CKeyXML();
	~CKeyXML() { HeapDestroy(m_hHeap); }

	void SetDocType(size_t type = DOCTYPE_XHTML_STRICT);

	HRESULT ParseXmlFile(const char* pszFile);	// returns S_OK if read, STG_E_FILENOTFOUND if not found
	HRESULT ParseHtmlFile(const char* pszFile);
	char*	ParseXmlString(char* pszXmlString, CKeyXmlBranch* pRoot = nullptr);
	char*	ParseHtmlString(char* szXmlString, CKeyXmlBranch* pRoot = nullptr);
	char*	ParseSitemapString(char* szXmlString, CKeyXmlBranch* pRoot = nullptr);

	HRESULT SaveXmlFile(const char* pszFileName);
	void	SaveXmlFile(CKeyFile* pkf) { WriteBranch(nullptr, *pkf, 0); }	// nullptr means no parent
	HRESULT SaveHtmlFile(const char* pszFileName);
	void	SaveHtmlFile(CKeyFile* pkf) { WriteHtmlBranch(nullptr, *pkf); }

	inline char* strdup(const char* psz) const {
		size_t cb = kstrlen(psz) + 1;
		char* pszDst = (char*) malloc(cb);
		memcpy(pszDst, psz ? psz : "", cb);
		return pszDst;
	}

	CKeyXmlBranch* GetRootBranch() { return m_pRoot; }

	CKeyXmlBranch* GetBodyBranch() { return m_pBodyBranch; }
	CKeyXmlBranch* GetHeadBranch() { return m_pHeadBranch; }
	CKeyXmlBranch* GetTitleBranch() { return m_pTitleBranch; }

	// Call AddRoot() when creating XML from scratch (no input file or string)
	CKeyXmlBranch* AddRoot() { m_pRoot = NewBranch(ENTITY_ROOT); m_pRoot->parent = m_pRoot; return m_pRoot; }
	CKeyXmlBranch* AddBranch(CKeyXmlBranch* pParent, PCSTR pszBranchName, XMLENTITY eType = ENTITY_ELEMENT);
	void		   AddAttribute(CKeyXmlBranch* pBranch, PCSTR pszName, PCSTR pszValue, size_t iGrow = 4 /* estimated new attributes */);
	CKeyXmlBranch* AddDataChild(CKeyXmlBranch* pParent, PCSTR pszName, PCSTR pszData);
	CKeyXmlBranch* GraftBranch(CKeyXmlBranch* pParent, XMLENTITY eType = ENTITY_ELEMENT);

	const char* GetTitle() {	// if an HTML/XHTML file was parsed, this will return the title (if any)
		if (m_pTitleBranch && m_pTitleBranch->GetChildrenCount()) {
			CKeyXmlBranch* pData = m_pTitleBranch->GetChildAt(0);
			if (pData && pData->GetType() == ENTITY_PCDATA && pData->pszData)
				return pData->GetData();
		}
		return NULL;
	}

	size_t GetMSHLinkCount() { return m_aMSHLinks.GetCount(); }
	CKeyXmlBranch* GetMSHLink(size_t pos) {	 return m_aMSHLinks[pos]; }

	size_t GetObjectTagCount() { return m_aObjectTags.GetCount(); }
	CKeyXmlBranch* GetObjectTag(size_t pos) {  return m_aObjectTags[pos]; }

	char*	AllocateBuffer(size_t cb) { return (char*) malloc(cb); }
	void	AllocateStringBuffers(CKeyXmlBranch* pBranch = nullptr);	// convert all strings to separately allocated buffers
	void	FreeBuffer(char* pszBuffer) { free(pszBuffer); }
	bool	isAllocatedStrings() { return m_bAllocatedStrings; }

protected:
	// Class functions

	CKeyXmlBranch* NewBranch(XMLENTITY eType = ENTITY_ELEMENT);
	XMLATTR*	   AddAttribute(CKeyXmlBranch* pBranch, LONG lGrow);
	HRESULT		   WriteBranch(CKeyXmlBranch* pBranch, CKeyFile& kf, size_t iIndent);
	HRESULT		   WriteHtmlBranch(CKeyXmlBranch* pBranch, CKeyFile& kf);
	HTML_ELEMENT   ParseElementTag(PCSTR pszName, PCSTR pszCurLoc, bool bEndTag = false);

	XMLATTR* NewAttribute(void) {
		XMLATTR* p = (XMLATTR*) malloc(sizeof(XMLATTR));	// Allocate one attribute.
		p->pszName = p->pszValue = 0; // No name or value.
		return p;
	}

	// REVIEW: [randalphwa - 10/1/2018] This needs to be changed to use CTTHeap since code below won't work on non-Windows platforms

	void* malloc(size_t cb) const {
		void* pv = HeapAlloc(m_hHeap, HEAP_NO_SERIALIZE, cb);
		if (!pv)
			OOM();	// doesn't return
		return pv;
	}
	template <typename T> void free(T pv) const {
		VERIFY(HeapFree(m_hHeap, HEAP_NO_SERIALIZE, (void*) pv));
	}
	template <typename T> T realloc(T pv, size_t cb) const {
		if (!pv) {
			pv = (T) HeapAlloc(m_hHeap, HEAP_NO_SERIALIZE, cb);
			if (!pv)
				OOM();	// doesn't return
			return pv;
		}
		pv = (T) HeapReAlloc(m_hHeap, HEAP_NO_SERIALIZE, (void*) pv, cb);
		if (!pv)
			OOM();	// doesn't return
		return pv;
	}

	// Class members

	HANDLE	m_hHeap;
	CKeyXmlBranch*	m_pRoot;			// Pointer to current XML Document tree root.
	size_t		m_uOptions;				// Parser options.
	bool		m_bAllocatedStrings;	// true if we allocated our own memory to hold strings
	bool		m_bXmlDataIsland;		// true if we encountered one or more data islands

	CKeyXmlBranch* m_pBodyBranch;
	CKeyXmlBranch* m_pHeadBranch;
	CKeyXmlBranch* m_pTitleBranch;

	CTTArray<CKeyXmlBranch*> m_aMSHLinks;	// array of all MSHelp:link elements that appear
	CTTArray<CKeyXmlBranch*> m_aObjectTags;	// array of all <object> elements that appear
	CHashPair m_lstXmlTags;
	CHashPair m_lstUnknownTags;

	CStr m_cszDocType;
};

#endif	// __TTLIB_KEYXML_H__
