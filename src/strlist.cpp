/////////////////////////////////////////////////////////////////////////////
// Name:		CStrList, CDblStrList, CStrIntList
// Purpose:		String arrays based off a private heap
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2005-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under Windows, all the classes here use their own private heap. The advantage of this approach is that
// none of the individual allocations need to be freed -- the entire heap is destroyed when the class
// destructor is called. By default, all the classes use a non-serialized private heap -- which means
// multiple threads need to use a crit section if accessing a shared instance of these classes.

// In a non-Windows build the classes will be serialized if the underlying malloc() routines are serialed,
// and the underlying CTTHeap destructor will free all memory allocated by this class (but because it will be
// calling free() for every item allocated, it will be a lot slower then the _WINDOWS_ version).

#include "precomp.h"

#include "../include/strlist.h"	// CStrList
#include "../include/cstr.h"	// CStr

CStrList::CStrList(bool bSerialize) : CTTHeap(bSerialize)
{
	m_cAllocated = 0;
	m_cItems = 0;
	m_enum = 0;
	m_aptrs = nullptr;
	m_bSerialize = bSerialize;	// save in case Delete() is called (which recreates the heap)
	m_flags = 0;
}

bool CStrList::Enum(const char** ppszResult)
{
	ASSERT_MSG(ppszResult, "NULL pointer!");
	if (!ppszResult)
		return false;

	if (m_cItems == 0 || m_enum == m_cItems)
		return false;
	*ppszResult = m_aptrs[m_enum++];
	return true;
}

void CStrList::SetFlags(size_t flags)
{
	m_flags = flags;
	if (m_flags & FLG_ADD_DUPLICATES)
		m_HashPair.Delete();	// Can't use a hash list if duplicates are allowed
}

size_t CStrList::Add(const char* pszKey)
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	if (!pszKey)
		return (size_t) -1;

	CStr cszKey;
	const char* pszNormalized = NormalizeString(pszKey, cszKey);

	if (isNoDuplicates()) {
		HASH hash = HashFromSz(pszNormalized);
		size_t pos = m_HashPair.GetVal(hash);
		if (pos != (size_t) -1)
			return pos;
		else {
			if (m_cItems + 1 >= m_cAllocated) {	// the +1 is paranoia -- it shouldn't really be necessary
				m_cAllocated += 32;	// add room for 32 strings at a time
				m_aptrs = (const char**) this->realloc(m_aptrs, m_cAllocated * sizeof(char*));
			}
			m_aptrs[m_cItems] = this->strdup(pszNormalized);
			m_HashPair.Add(hash, m_cItems);
			return m_cItems++;
		}
	}

	if (m_cItems + 1 >= m_cAllocated) {	// the +1 is paranoia -- it shouldn't really be necessary
		m_cAllocated += 32;	// add room for 32 strings at a time
		m_aptrs = (const char**) (m_aptrs ? this->realloc(m_aptrs, m_cAllocated * sizeof(char*)) : this->malloc(m_cAllocated * sizeof(char*)));
	}
	m_aptrs[m_cItems] = this->strdup(pszNormalized);
	return m_cItems++;
}

bool CStrList::Find(const char* pszKey) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	if (!pszKey || !*pszKey)
		return false;

	CStr cszKey;
	const char* pszNormalized = NormalizeString(pszKey, cszKey);

	if (isNoDuplicates())
		return m_HashPair.Find(pszNormalized);

	for (size_t i = 0; i < m_cItems; i++)	{
		if (kstrcmp(m_aptrs[i], pszNormalized))
			return true;
	}
	return false;
}

size_t CStrList::GetPos(const char* pszKey) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	if (!pszKey || !*pszKey)
		return (size_t) -1;

	CStr cszKey;
	const char* pszNormalized = NormalizeString(pszKey, cszKey);

	if (isNoDuplicates())
		return m_HashPair.GetVal(pszNormalized);

	for (size_t pos = 0; pos < m_cItems; ++pos)	{
		if (kstrcmp(m_aptrs[pos], pszNormalized))
			return pos;
	}
	return (size_t) -1;
}

const char* CStrList::Get(size_t pos) const
{
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems)
		return "";	// better to return a pointer to an empty string then an invalid pointer
	return m_aptrs[pos];
}

void CStrList::Remove(size_t pos)
{
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems)
		return;

	if (isNoDuplicates())
		m_HashPair.Remove(m_aptrs[pos]);

	this->free((void*) m_aptrs[pos]);
	memmove((void*) (m_aptrs + pos), (void*) (m_aptrs + pos + 1), (m_cItems - (pos + 1)) * sizeof(char*));
	--m_cItems;
}

void CStrList::Remove(const char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	ASSERT_MSG(*psz, "empty string!");
	if (!psz || !*psz)
		return;

	size_t pos = GetPos(psz);
	if (pos != (size_t) -1)
		Remove(pos);
}

void CStrList::Delete()
{
	if (m_cItems == 0)
		return;	// we haven't added anything yet, so nothing to do!
	if (m_bCreated && m_hHeap && m_hHeap != GetProcessHeap())
		HeapDestroy(m_hHeap);
	m_hHeap = HeapCreate(m_bSerialize ? 0 : HEAP_NO_SERIALIZE, 4096, 0);
	m_bCreated = true;
	m_cAllocated = 0;
	m_cItems = 0;
	m_enum = 0;
	m_aptrs = nullptr;
	m_HashPair.Delete();
}

void CStrList::Replace(size_t pos, const char* pszKey)
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems || !pszKey || !*pszKey)
		return;

	if (isNoDuplicates())
		m_HashPair.Remove(m_aptrs[pos]);

	this->free((void*) m_aptrs[pos]);

	CStr cszKey;
	const char* pszNormalized = NormalizeString(pszKey, cszKey);

	m_aptrs[pos] = this->strdup(pszNormalized);

	if (isNoDuplicates())
		m_HashPair.Add(pszNormalized, pos);
}

void CStrList::Swap(size_t posA, size_t posB)
{
	if (posA > m_cItems || posB > m_cItems || posA == posB)
		return;
	const char* pszA = m_aptrs[posA];
	m_aptrs[posA] = m_aptrs[posB];
	m_aptrs[posB] = pszA;

	m_HashPair.SetVal(m_aptrs[posA], posA);
	m_HashPair.SetVal(m_aptrs[posB], posB);
}

void CStrList::InsertAt(size_t pos, const char* pszKey)
{
	if (pos >= m_cItems) {
		Add(pszKey);
		return;
	}

	if (m_cItems + 1 >= m_cAllocated) {		// the +1 is paranoia -- it shouldn't really be necessary
		m_cAllocated += 32;	// add room for 32 strings at a time
		m_aptrs = (const char**) (m_aptrs ? realloc(m_aptrs, m_cAllocated * sizeof(char*)) : malloc(m_cAllocated * sizeof(char*)));
	}
	memmove((void*) (m_aptrs + pos + 1), (void*) (m_aptrs + pos), (m_cItems - pos + 1) * sizeof(char*));

	if (isNoDuplicates()) {
		CHashPair::HASH_PAIR* pHashPair = m_HashPair.GetArray();
		for (size_t posHash = 0; posHash < m_HashPair.GetCount(); ++posHash) {
			if (pHashPair[posHash].val >= pos)
				pHashPair[posHash].val++;
		}
	}

	CStr cszKey;
	const char* pszNormalized = NormalizeString(pszKey, cszKey);

	m_aptrs[pos] = this->strdup(pszNormalized);
	m_cItems++;

	if (isNoDuplicates())
		m_HashPair.Add(pszNormalized);
}

void CStrList::Sort()
{
	qsorti(0, m_cItems - 1);
	if (isNoDuplicates()) {
		for (size_t pos = 0; pos < m_cItems; ++pos)
			m_HashPair.Add(m_aptrs[pos], pos);	// It already exists, this will just update the associated position
	}
}

// It is the caller's responsibility to ensure that all strings have at least iColumn characters

void CStrList::Sort(size_t iColumn)
{
	m_SortColumn = iColumn;
	qsortCol(0, m_cItems - 1);
	if (isNoDuplicates()) {
		for (size_t pos = 0; pos < m_cItems; ++pos)
			m_HashPair.Add(m_aptrs[pos], pos);	// It already exists, this will just update the associated position
	}
}

const char* CStrList::NormalizeString(const char* pszString, CStr& cszKey) const
{
	// BUGBUG: [randalphwa - 9/20/2018] This is not thread safe, which means CStrList is not thread safe

	if (m_flags & FLG_IGNORE_CASE || m_flags & FLG_URL_STRINGS)	{
		cszKey = pszString;
		cszKey.MakeLower();
		if (m_flags & FLG_URL_STRINGS)
			BackslashToForwardslash(cszKey);
		return cszKey;
	}
	cszKey.Delete();
	return pszString;
}

void CStrList::qsorti(ptrdiff_t low, ptrdiff_t high)
{
	if (low >= high)
		return;

	// Do we need to sort?

	ptrdiff_t pos = high - 1;
	while (pos >= low) {
		if (strcmp(m_aptrs[pos], m_aptrs[pos + 1]) > 0)
			break;
		else
			pos--;
	}
	if (pos < low)
		return;	// it's already sorted

	swap(low, (low + high) / 2);

	ptrdiff_t end = low;
	for (pos = low + 1; pos <= high; pos++) {
		if (strcmp(m_aptrs[pos], m_aptrs[low]) < 0) {
			swap(++end, pos);
		}
	}
	swap(low, end);

	if (low < end - 1)
		qsorti(low, end - 1);
	if (end + 1 < high)
		qsorti(end + 1, high);
}

// This is a bit dangerous as we do NOT check to see if each string actually has at least m_SortColumn characters

void CStrList::qsortCol(ptrdiff_t low, ptrdiff_t high)
{
	if (low >= high)
		return;

	// Do we need to sort?

	ptrdiff_t pos = high - 1;
	while (pos >= low) {
		if (strcmp(m_aptrs[pos] + m_SortColumn, m_aptrs[pos + 1] + m_SortColumn) > 0)
			break;
		else
			pos--;
	}
	if (pos < low)
		return;	// it's already sorted

	swap(low, (low + high) / 2);

	ptrdiff_t end = low;
	for (pos = low + 1; pos <= high; pos++) {
		if (strcmp(m_aptrs[pos] + m_SortColumn, m_aptrs[low] + m_SortColumn) < 0) {
			swap(++end, pos);
		}
	}
	swap(low, end);

	if (low < end - 1)
		qsortCol(low, end - 1);
	if (end + 1 < high)
		qsortCol(end + 1, high);
}

const char* CStrList::operator[](size_t pos) const
{
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems)
		return "";		// better to return a pointer to an empty string then an invalid pointer
	return m_aptrs[pos];
}

/////////////////////	CDblStrList		//////////////////////////

CDblStrList::CDblStrList(bool bSerialize) : CTTHeap(bSerialize)
{
	m_cAllocated = 0;
	m_cItems = 0;
	m_aptrs = nullptr;
	m_bSerialize = bSerialize;
	m_pHashLookup = nullptr;
}

CDblStrList::~CDblStrList()
{
	delete m_pHashLookup;
}

void CDblStrList::PreventDuplicateKeys()
{
	if (!m_pHashLookup)
		m_pHashLookup = new CHashPair;
}

void CDblStrList::Add(const char* pszKey, const char* pszVal)
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	ASSERT_MSG(pszVal, "NULL pointer!");	// it's okay to add an empty val string as long as it isn't a null pointer

	if (!pszKey || !*pszKey || !pszVal )
		return;

	if (m_pHashLookup) {
		HASH hash = HashFromSz(pszKey);
		if (m_pHashLookup->Find(hash)) {
			return;
		}
		else {
			m_pHashLookup->Add(hash);
		}
	}

	if (m_cItems >= m_cAllocated) {
		m_cAllocated += 32;	// add room for 32 strings at a time
		m_aptrs = (DBLPTRS*) (m_aptrs ? this->realloc(m_aptrs, m_cAllocated * sizeof(DBLPTRS)) : this->malloc(m_cAllocated * sizeof(DBLPTRS)));
	}
	m_aptrs[m_cItems].pszKey = this->strdup(pszKey);
	m_aptrs[m_cItems++].pszVal = this->strdup(pszVal);
}

bool CDblStrList::FindKey(const char* pszKey, size_t* ppos) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	if (!pszKey || !*pszKey)
		return false;
	for (size_t i = 0; i < m_cItems; i++)	{
		if (kstrcmp(m_aptrs[i].pszKey, pszKey))	{
			if (ppos)
				*ppos = i;
			return true;
		}
	}
	return false;
}

bool CDblStrList::FindVal(const char* pszVal, size_t* ppos) const
{
	ASSERT_MSG(pszVal, "NULL pointer!");
	if (!pszVal)
		return false;
	for (size_t i = 0; i < m_cItems; i++)	{
		if (kstrcmp(m_aptrs[i].pszVal, pszVal)) {
			if (ppos)
				*ppos = i;
			return true;
		}
	}
	return false;
}

const char* CDblStrList::GetKeyAt(size_t pos) const
{
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems)
		return NULL;
	return m_aptrs[pos].pszKey;
}

const char* CDblStrList::GetValAt(size_t pos) const
{
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems)
		return NULL;
	return m_aptrs[pos].pszVal;
}

const char* CDblStrList::GetMatchingVal(const char* pszKey) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	if (!pszKey)
		return nullptr;
	for (size_t pos = 0; pos < m_cItems; ++pos)	{
		if (kstrcmp(m_aptrs[pos].pszKey, pszKey)) {
			return m_aptrs[pos].pszVal;
		}
	}
	return nullptr;
}

void CDblStrList::Replace(size_t pos, const char* pszKey, const char* pszVal)
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	ASSERT_MSG(pszVal, "NULL pointer!");	// okay if pszVal is an empty string, just not a null pointer
	ASSERT(pos < m_cItems);
	if (pos >= m_cItems || !pszKey || !*pszKey || *pszVal)
		return;

	this->free((void*) m_aptrs[pos].pszKey);
	this->free((void*) m_aptrs[pos].pszVal);
	m_aptrs[pos].pszKey = this->strdup(pszKey);
	m_aptrs[pos].pszVal = this->strdup(pszVal);
}

void CDblStrList::SortKeys()
{
	ASSERT(m_cItems > 0);
	if (m_cItems < 2)
		return;
	m_bSortKeys = true;
	qsorti(0, m_cItems - 1);
}

void CDblStrList::SortVals()
{
	ASSERT(m_cItems > 0);
	if (m_cItems < 2)
		return;
	m_bSortKeys = false;
	qsorti(0, m_cItems - 1);
}

void CDblStrList::qsorti(ptrdiff_t low, ptrdiff_t high)
{
	if (low >= high)
		return;

	// First find out if we are already sorted

	ptrdiff_t pos = high - 1;
	while (pos >= low) {
		if (m_bSortKeys) {
			if (strcmp(m_aptrs[pos].pszKey, m_aptrs[pos + 1].pszKey) > 0)
				break;
			else
				pos--;
		}
		else {
			if (strcmp(m_aptrs[pos].pszVal, m_aptrs[pos + 1].pszVal) > 0)
				break;
			else
				pos--;
		}
	}
	if (pos < low)
		return;	// it's already sorted

	swap(low, (low + high) / 2);

	ptrdiff_t end = low;
	for (pos = low + 1; pos <= high; pos++) {
		if (m_bSortKeys) {
			if (strcmp(m_aptrs[pos].pszKey, m_aptrs[low].pszKey) < 0)
				swap(++end, pos);
		}
		else {
			if (strcmp(m_aptrs[pos].pszVal, m_aptrs[low].pszVal) < 0)
				swap(++end, pos);
		}
	}
	swap(low, end);

	if (low < end - 1)
		qsorti(low, end - 1);
	if (end + 1 < high)
		qsorti(end + 1, high);
}

//////////////////////////////// CStrIntList	/////////////////////////////////

CStrIntList::CStrIntList(bool bSerialize) : CTTHeap(bSerialize)
{
	m_cAllocated = 0;
	m_cItems = 0;
	m_aptrs = nullptr;
	m_bSerialize = bSerialize;
	m_posEnumKey = (size_t) -1;
}

void CStrIntList::Add(const char* pszKey, ptrdiff_t newVal)
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	if (!pszKey || !*pszKey )
		return;

	for (size_t pos = 0; pos < m_cItems; ++pos)	{
		if (kstrcmp(m_aptrs[pos].pszKey, pszKey)) {
			ptrdiff_t cItems = m_aptrs[pos].pVal[0];
			for (ptrdiff_t valPos = 1; valPos <= cItems; ++valPos) {
				if (newVal == m_aptrs[pos].pVal[valPos])
					return;	// we've already added this value
			}
			// newVal not found, so add it
			++cItems;
			m_aptrs[pos].pVal = (ptrdiff_t*) this->realloc(m_aptrs[pos].pVal, (cItems + 1) * sizeof(ptrdiff_t));
			m_aptrs[pos].pVal[0] = cItems;
			m_aptrs[pos].pVal[cItems] = newVal;
			return;
		}
	}

	// key not found, so add it

	if (m_cItems >= m_cAllocated) {
		m_cAllocated += 32;	// add room for 32 strings at a time
		m_aptrs = (DBLPTRS*) (m_aptrs ? this->realloc(m_aptrs, m_cAllocated * sizeof(DBLPTRS)) : this->malloc(m_cAllocated * sizeof(DBLPTRS)));
	}
	m_aptrs[m_cItems].pszKey = this->strdup(pszKey);
	m_aptrs[m_cItems].pVal = (ptrdiff_t*) this->malloc(2 * sizeof(ptrdiff_t));
	m_aptrs[m_cItems].pVal[0] = 1;
	m_aptrs[m_cItems].pVal[1] = newVal;
	++m_cItems;
}

bool CStrIntList::Add(size_t posKey, ptrdiff_t newVal)
{
	ASSERT(posKey < m_cItems);
	if (posKey >= m_cItems)
		return false;

	ptrdiff_t cItems = m_aptrs[posKey].pVal[0];
	for (ptrdiff_t valPos = 1; valPos <= cItems; ++valPos) {
		if (newVal == m_aptrs[posKey].pVal[valPos])
			return true;
	}
	// newVal not found, so add it
	++cItems;
	m_aptrs[posKey].pVal = (ptrdiff_t*) this->realloc(m_aptrs[posKey].pVal, (cItems + 1) * sizeof(ptrdiff_t));
	m_aptrs[posKey].pVal[0] = cItems;
	m_aptrs[posKey].pVal[cItems] = newVal;
	return true;
}

bool CStrIntList::FindKey(const char* pszKey, size_t* ppos) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	if (!pszKey || !*pszKey)
		return false;
	for (size_t pos = 0; pos < m_cItems; ++pos)	{
		if (kstrcmp(m_aptrs[pos].pszKey, pszKey))	{
			if (ppos)
				*ppos = pos;
			return true;
		}
	}
	return false;
}

bool CStrIntList::GetValCount(const char* pszKey, ptrdiff_t* pVal) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	ASSERT_MSG(pVal, "NULL pointer!");

	if (!pszKey || !*pszKey || !pVal)
		return false;
	for (size_t posKey = 0; posKey < m_cItems; ++posKey)	{
		if (kstrcmp(m_aptrs[posKey].pszKey, pszKey)) {
			*pVal = m_aptrs[posKey].pVal[0];
			return true;
		}
	}
	return false;
}

ptrdiff_t CStrIntList::GetValCount(size_t posKey) const
{
	ASSERT(posKey < m_cItems);
	if (posKey >= m_cItems)
		return 0;	// there isn't a way to indicate an error other then the ASSERT in _DEBUG builds
	return m_aptrs[posKey].pVal[0];
}

bool CStrIntList::GetVal(const char* pszKey, ptrdiff_t* pVal, size_t posVal) const
{
	ASSERT_MSG(pszKey, "NULL pointer!");
	ASSERT_MSG(*pszKey, "empty string!");
	ASSERT_MSG(pVal, "NULL pointer!");

	if (!pszKey || !*pszKey || !pVal)
		return false;
	for (size_t posKey = 0; posKey < m_cItems; ++posKey)	{
		if (kstrcmp(m_aptrs[posKey].pszKey, pszKey)) {
			if ((ptrdiff_t) posVal > m_aptrs[posKey].pVal[0])
				return false;
			*pVal = m_aptrs[posKey].pVal[posVal];
			return true;
		}
	}
	return false;
}

bool CStrIntList::GetVal(size_t posKey, ptrdiff_t* pVal, size_t posVal) const
{
	ASSERT(posKey < m_cItems);
	if (posKey >= m_cItems)
		return false;

	if ((ptrdiff_t) posVal > m_aptrs[posKey].pVal[0])
		return false;
	*pVal = m_aptrs[posKey].pVal[posVal];
	return true;
}

bool CStrIntList::BeginEnum(const char* pszKey)
{
	if (!FindKey(pszKey, &m_posEnumKey)) {
		m_posEnumKey = (size_t) -1;	// flag it as invalid
		return false;
	}
	m_posEnumVal = 1;
	return true;
}

bool CStrIntList::Enum(ptrdiff_t* pVal)
{
	if (m_posEnumKey >= m_cItems)	// this will also catch -1 as the value (since m_posEnumKey is unsigned)
		return false;
	if (m_posEnumVal > m_aptrs[m_posEnumKey].pVal[0])
		return false;
	*pVal = m_aptrs[m_posEnumKey].pVal[m_posEnumVal++];
	return true;
}

const char* CStrIntList::GetKey(size_t posKey) const
{
	ASSERT(posKey < m_cItems);
	if (posKey >= m_cItems)
		return nullptr;

	return m_aptrs[posKey].pszKey;
}