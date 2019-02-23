/////////////////////////////////////////////////////////////////////////////
// Name:		ttCHashPair
// Purpose:		Class utilizing an array of HASH numbers and an associated value
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/tthashpair.h"	// ttCHashPair
#include "../include/ttheap.h"		// memory allocation routines

#define GROWTH_MALLOC  16

ttCHashPair::ttCHashPair(size_t EstimatedMembers)
{
	if (!EstimatedMembers) {
		m_cMalloced = 0;
		m_pahash = nullptr;
	}
	else {
		m_pahash = (HASH_PAIR*) tt::malloc(EstimatedMembers * sizeof(HASH_PAIR));
		m_cMalloced = EstimatedMembers;
	}
	m_cMembers = 0;
}

ttCHashPair::~ttCHashPair()
{
	Delete();
}

void ttCHashPair::Delete()
{
	if (m_pahash) {
		tt::free(m_pahash);
		m_pahash = nullptr;
		m_cMembers = m_cMalloced = 0;
	}
}

void ttCHashPair::Add(size_t hash, size_t val)
{
	// Note that we are adding the full HASH_PAIR structure, not a pointer to the structure

	if (m_cMembers + 1 > m_cMalloced) {
		m_cMalloced += GROWTH_MALLOC;
		m_pahash = (HASH_PAIR*) tt::realloc(m_pahash, m_cMalloced * sizeof(HASH_PAIR));
	}

	if (!m_cMembers) {
		m_pahash[0].hash = hash;
		m_pahash[0].val = val;
		++m_cMembers;
		return;
	}

	HASH_PAIR* pInsert = FindInsertionPoint(hash);
	if (pInsert->hash == hash) {	// If hash is already added, update the value
		pInsert->val = val;
		return;
	}

	// The array of pairs is larger then the actual number of members in use, so if the insertion point is at the end of the array,
	// then all we have to do is set the hash/val pair.

	size_t cbMove = (m_pahash + m_cMembers) - pInsert;
	if (cbMove)
		memmove(pInsert + 1, pInsert, cbMove * sizeof(HASH_PAIR));
	pInsert->hash = hash;
	pInsert->val = val;
	++m_cMembers;
}

void ttCHashPair::SetVal(size_t hash, size_t val)
{
	HASH_PAIR* pPair = GetHashPair(hash);
	if (pPair)
		pPair->val = val;
}

void ttCHashPair::Remove(size_t hashDel)
{
	for (size_t pos = 0; pos < m_cMembers; pos++) {
		if (m_pahash[pos].hash == hashDel) {
			if (pos == m_cMembers - 1) {
				m_cMembers--;
				return;
			}

			memmove(m_pahash + pos, m_pahash + pos + 1, ((m_cMembers - 1) - pos) * sizeof(HASH_PAIR));
			m_cMembers--;
			return;
		}
	}
}

bool ttCHashPair::Find(size_t hash) const
{
	if (!m_pahash)
		return false;

	HASH_PAIR* pLow = m_pahash;
	HASH_PAIR* pHigh = m_pahash + (m_cMembers - 1);

	size_t num = m_cMembers;
	while (pLow->hash <= pHigh->hash) {
		size_t half = num / 2;
		if (half) {
			HASH_PAIR* pMid = pLow + (num & 1 ? half : (half - 1));
			if (pMid->hash == hash)
				return true;
			else if (pMid->hash > hash) {
				pHigh = pMid - 1;
				num = num & 1 ? half : half - 1;
			}
			else {
				pLow = pMid + 1;
				num = half;
			}
		}
		else if (num)
			return (pLow->hash == hash);
		else
			return false;
	}
	return false;
}

size_t ttCHashPair::GetVal(size_t hash) const
{
	if (!m_pahash)
		return (size_t) -1;

	HASH_PAIR* pLow = m_pahash;
	HASH_PAIR* pHigh = m_pahash + (m_cMembers - 1);

	size_t num = m_cMembers;
	while (pLow->hash <= pHigh->hash) {
		size_t half = num / 2;
		if (half) {
			HASH_PAIR* pMid = pLow + (num & 1 ? half : (half - 1));
			if (pMid->hash == hash)
				return pMid->val;
			else if (pMid->hash > hash) {
				pHigh = pMid - 1;
				num = num & 1 ? half : half - 1;
			}
			else {
				pLow = pMid + 1;
				num = half;
			}
		}
		else if (num)
			return (pLow->hash == hash ? pLow->val : (size_t) -1);
		else
			return (size_t) -1;
	}
	return (size_t) -1;
}

ttCHashPair::HASH_PAIR* ttCHashPair::FindInsertionPoint(size_t hash) const
{
	if (m_pahash[0].hash > hash)
		return &m_pahash[0];	// insert at beginning

	HASH_PAIR* pHigh = m_pahash + (m_cMembers - 1);
	if (pHigh->hash < hash)
		return pHigh + 1;	// insert at end

	HASH_PAIR* pLow = m_pahash;
	HASH_PAIR* pMid;

	size_t num = m_cMembers;
	while (pLow <= pHigh) {
		size_t half = num / 2;
		if (half) {
			pMid = pLow + (num & 1 ? half : (half - 1));
			if (pMid->hash == hash)
				return pMid;	// already added
			if (pMid->hash > hash) {
				pHigh = pMid - 1;
				if (pHigh->hash < hash)
					return pMid;	// insert at end
				num = num & 1 ? half : half - 1;
			}
			else {
				pLow = pMid + 1;
				num = half;
			}
		}
		else
			return pLow;
	}
	return pLow;
}

ttCHashPair::HASH_PAIR* ttCHashPair::GetHashPair(size_t hash) const
{
	if (!m_pahash)
		return nullptr;

	HASH_PAIR* pLow = m_pahash;
	HASH_PAIR* pHigh = m_pahash + (m_cMembers - 1);

	size_t num = m_cMembers;
	while (pLow->hash <= pHigh->hash) {
		size_t half = num / 2;
		if (half) {
			HASH_PAIR* pMid = pLow + (num & 1 ? half : (half - 1));
			if (pMid->hash == hash)
				return pMid;
			else if (pMid->hash > hash) {
				pHigh = pMid - 1;
				num = num & 1 ? half : half - 1;
			}
			else {
				pLow = pMid + 1;
				num = half;
			}
		}
		else if (num && pLow->hash == hash)
			return pLow;
		else
			return nullptr;
	}
	return nullptr;
}
