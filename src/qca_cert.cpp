/*
 * qca_cert.cpp - Qt Cryptographic Architecture
 * Copyright (C) 2003-2005  Justin Karneges <justin@affinix.com>
 * Copyright (C) 2004,2005  Brad Hards <bradh@frogmouth.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "qca_cert.h"

#include <qdatetime.h>
#include <qregexp.h>
#include "qca_publickey.h"
#include "qcaprovider.h"

namespace QCA {

Provider::Context *getContext(const QString &type, const QString &provider);

//----------------------------------------------------------------------------
// CertificateOptions
//----------------------------------------------------------------------------
CertificateOptions::CertificateOptions(CertificateRequestFormat f)
{
	Q_UNUSED(f);
}

CertificateRequestFormat CertificateOptions::format() const
{
	return CSR_PKCS10;
}

void CertificateOptions::setFormat(CertificateRequestFormat f)
{
	Q_UNUSED(f);
}

bool CertificateOptions::isValid() const
{
	return false;
}

QString CertificateOptions::commonName() const
{
	return QString();
}

QString CertificateOptions::country() const
{
	return QString();
}

QString CertificateOptions::organization() const
{
	return QString();
}

QString CertificateOptions::organizationalUnit() const
{
	return QString();
}

QString CertificateOptions::locality() const
{
	return QString();
}

QString CertificateOptions::state() const
{
	return QString();
}

QBigInteger CertificateOptions::serialNumber() const
{
	return QBigInteger();
}

QString CertificateOptions::email() const
{
	return QString();
}

QString CertificateOptions::uri() const
{
	return QString();
}

QString CertificateOptions::dns() const
{
	return QString();
}

QString CertificateOptions::challenge() const
{
	return QString();
}

QDateTime CertificateOptions::notValidBefore() const
{
	return QDateTime();
}

QDateTime CertificateOptions::notValidAfter() const
{
	return QDateTime();
}

bool CertificateOptions::isCA() const
{
	return false;
}

int CertificateOptions::pathLimit() const
{
	return 0;
}

void CertificateOptions::setCommonName(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setCountry(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setOrganization(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setOrganizationalUnit(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setLocality(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setState(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setSerialNumber(const QBigInteger &i)
{
	Q_UNUSED(i);
}

void CertificateOptions::setEmail(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setURI(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setDNS(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setChallenge(const QString &s)
{
	Q_UNUSED(s);
}

void CertificateOptions::setValidityPeriod(const QDateTime &start, const QDateTime &end)
{
	Q_UNUSED(start);
	Q_UNUSED(end);
}

void CertificateOptions::setAsCA(int pathLimit)
{
	Q_UNUSED(pathLimit);
}

//----------------------------------------------------------------------------
// Certificate
//----------------------------------------------------------------------------
// (adapted from kdelibs) -- Justin
static bool cnMatchesAddress(const QString &_cn, const QString &peerHost)
{
	QString cn = _cn.stripWhiteSpace().lower();
	QRegExp rx;

	// Check for invalid characters
	if(QRegExp("[^a-zA-Z0-9\\.\\*\\-]").search(cn) >= 0)
		return false;

	// Domains can legally end with '.'s.  We don't need them though.
	while(cn.endsWith("."))
		cn.truncate(cn.length()-1);

	// Do not let empty CN's get by!!
	if(cn.isEmpty())
		return false;

	// Check for IPv4 address
	rx.setPattern("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
	if(rx.exactMatch(peerHost))
		return peerHost == cn;

	// Check for IPv6 address here...
	rx.setPattern("^\\[.*\\]$");
	if(rx.exactMatch(peerHost))
		return peerHost == cn;

	if(cn.contains('*')) {
		// First make sure that there are at least two valid parts
		// after the wildcard (*).
		QStringList parts = QStringList::split('.', cn, false);

		while(parts.count() > 2)
			parts.remove(parts.begin());

		if(parts.count() != 2) {
			return false;  // we don't allow *.root - that's bad
		}

		if(parts[0].contains('*') || parts[1].contains('*')) {
			return false;
		}

		// RFC2818 says that *.example.com should match against
		// foo.example.com but not bar.foo.example.com
		// (ie. they must have the same number of parts)
		if(QRegExp(cn, false, true).exactMatch(peerHost) &&
			QStringList::split('.', cn, false).count() ==
			QStringList::split('.', peerHost, false).count())
			return true;

		return false;
	}

	// We must have an exact match in this case (insensitive though)
	// (note we already did .lower())
	if(cn == peerHost)
		return true;
	return false;
}

Certificate::Certificate()
{
}

Certificate::Certificate(const CertificateOptions &opts, const PrivateKey &key, const QString &provider)
{
	Q_UNUSED(opts);
	Q_UNUSED(key);
	Q_UNUSED(provider);
}

bool Certificate::isNull() const
{
	return (!context() ? true : false);
}

QDateTime Certificate::notValidBefore() const
{
	return ((CertContext *)context())->notValidBefore();
}

QDateTime Certificate::notValidAfter() const
{
	return ((CertContext *)context())->notValidAfter();
}

Certificate::Info Certificate::subjectInfo() const
{
	return ((CertContext *)context())->subjectInfo();
}

Certificate::Info Certificate::issuerInfo() const
{
	return ((CertContext *)context())->issuerInfo();
}

QString Certificate::commonName() const
{
	return subjectInfo()["CN"];
}

QBigInteger Certificate::serialNumber() const
{
	return ((CertContext *)context())->serialNumber();
}

PublicKey Certificate::subjectPublicKey() const
{
	PKeyContext *c = ((CertContext *)context())->subjectPublicKey();
	PublicKey key;
	key.change(c);
	return key;
}

SignAlgo Certificate::signatureAlgorithm() const
{
	return SignUnknown;
}

bool Certificate::isCA() const
{
	return false;
}

bool Certificate::isSelfSigned() const
{
	return false;
}

QSecureArray Certificate::toDER() const
{
	return ((CertContext *)context())->toDER();
}

QString Certificate::toPEM() const
{
	return ((CertContext *)context())->toPEM();
}

Certificate Certificate::fromDER(const QSecureArray &a, const QString &provider)
{
	Certificate c;
	CertContext *cc = (CertContext *)getContext("cert", provider);
	if(cc->fromDER(a) == CertContext::Good)
		c.change(cc);
	return c;
}

Certificate Certificate::fromPEM(const QString &s, const QString &provider)
{
	Certificate c;
	CertContext *cc = (CertContext *)getContext("cert", provider);
	if(cc->fromPEM(s) == CertContext::Good)
		c.change(cc);
	return c;
}

bool Certificate::matchesHostname(const QString &realHost) const
{
	QString peerHost = realHost.stripWhiteSpace();
	while(peerHost.endsWith("."))
		peerHost.truncate(peerHost.length()-1);
	peerHost = peerHost.lower();

	if(cnMatchesAddress(commonName(), peerHost))
		return true;
	return false;
}

bool Certificate::operator==(const Certificate &) const
{
	return false;
}

bool Certificate::operator!=(const Certificate &a) const
{
	return !(*this == a);
}

//----------------------------------------------------------------------------
// CertificateChain
//----------------------------------------------------------------------------
CertificateChain::CertificateChain()
{
}

CertificateChain::CertificateChain(const Certificate &primary)
{
	append(primary);
}

const Certificate & CertificateChain::primary() const
{
	return first();
}

//----------------------------------------------------------------------------
// CertificateRequest
//----------------------------------------------------------------------------
CertificateRequest::CertificateRequest()
{
}

CertificateRequest::CertificateRequest(const CertificateOptions &opts, const PrivateKey &key, const QString &provider)
{
	Q_UNUSED(opts);
	Q_UNUSED(key);
	Q_UNUSED(provider);
}

bool CertificateRequest::isNull() const
{
	return false;
}

CertificateRequestFormat CertificateRequest::format() const
{
	return CSR_PKCS10;
}

PublicKey CertificateRequest::subjectPublicKey() const
{
	return PublicKey();
}

bool CertificateRequest::isCA() const
{
	return false;
}

int CertificateRequest::pathLimit() const
{
	return 0;
}

QString CertificateRequest::challenge() const
{
	return QString();
}

SignAlgo CertificateRequest::signatureAlgorithm() const
{
	return SignUnknown;
}

QSecureArray CertificateRequest::toDER() const
{
	return QSecureArray();
}

QString CertificateRequest::toPEM() const
{
	return QString();
}

CertificateRequest CertificateRequest::fromDER(const QSecureArray &a, const QString &provider)
{
	Q_UNUSED(a);
	Q_UNUSED(provider);
	return CertificateRequest();
}

CertificateRequest CertificateRequest::fromPEM(const QString &s, const QString &provider)
{
	Q_UNUSED(s);
	Q_UNUSED(provider);
	return CertificateRequest();
}

QString CertificateRequest::toString() const
{
	return QString();
}

CertificateRequest CertificateRequest::fromString(const QString &s, const QString &provider)
{
	Q_UNUSED(s);
	Q_UNUSED(provider);
	return CertificateRequest();
}

//----------------------------------------------------------------------------
// CRLEntry
//----------------------------------------------------------------------------
CRLEntry::CRLEntry()
{
}

CRLEntry::CRLEntry(const Certificate &c, Reason r)
{
	Q_UNUSED(c);
	Q_UNUSED(r);
}

QBigInteger CRLEntry::serialNumber() const
{
	return QBigInteger();
}

QDateTime CRLEntry::time() const
{
	return QDateTime();
}

CRLEntry::Reason CRLEntry::reason() const
{
	return Unspecified;
}

//----------------------------------------------------------------------------
// CRL
//----------------------------------------------------------------------------
CRL::CRL()
{
}

bool CRL::isNull() const
{
	return (!context() ? true : false);
}

int CRL::number() const
{
	return -1;
}

QDateTime CRL::thisUpdate() const
{
	return QDateTime();
}

QDateTime CRL::nextUpdate() const
{
	return QDateTime();
}

QValueList<CRLEntry> CRL::revoked() const
{
	return QValueList<CRLEntry>();
}

SignAlgo CRL::signatureAlgorithm() const
{
	return SignUnknown;
}

QSecureArray CRL::toDER() const
{
	return ((CRLContext *)context())->toDER();
}

QString CRL::toPEM() const
{
	return ((CRLContext *)context())->toPEM();
}

CRL CRL::fromDER(const QSecureArray &a, const QString &provider)
{
	CRL c;
	CRLContext *cc = (CRLContext *)getContext("crl", provider);
	if(cc->fromDER(a) == CRLContext::Good)
		c.change(cc);
	return c;
}

CRL CRL::fromPEM(const QString &s, const QString &provider)
{
	CRL c;
	CRLContext *cc = (CRLContext *)getContext("crl", provider);
	if(cc->fromPEM(s) == CRLContext::Good)
		c.change(cc);
	return c;
}

//----------------------------------------------------------------------------
// CertificateAuthority
//----------------------------------------------------------------------------
CertificateAuthority::CertificateAuthority(const Certificate &cert, const PrivateKey &key, const QString &provider)
{
	Q_UNUSED(cert);
	Q_UNUSED(key);
	Q_UNUSED(provider);
}

Certificate CertificateAuthority::certificate() const
{
	return Certificate();
}

Certificate CertificateAuthority::signRequest(const CertificateRequest &req, const QDateTime &notValidAfter) const
{
	Q_UNUSED(req);
	Q_UNUSED(notValidAfter);
	return Certificate();
}

CRL CertificateAuthority::createCRL(const QDateTime &nextUpdate) const
{
	Q_UNUSED(nextUpdate);
	return CRL();
}

CRL CertificateAuthority::updateCRL(const CRL &crl, const QValueList<CRLEntry> &entries, const QDateTime &nextUpdate) const
{
	Q_UNUSED(crl);
	Q_UNUSED(entries);
	Q_UNUSED(nextUpdate);
	return CRL();
}

//----------------------------------------------------------------------------
// Store
//----------------------------------------------------------------------------
Store::Store(const QString &provider)
:Algorithm("store", provider)
{
}

void Store::addCertificate(const Certificate &cert, bool trusted)
{
	((StoreContext *)context())->addCertificate(*((CertContext *)cert.context()), trusted);
}

void Store::addCRL(const CRL &crl)
{
	((StoreContext *)context())->addCRL(*((CRLContext *)crl.context()));
}

CertValidity Store::validate(const Certificate &cert, CertUsage u) const
{
	return ((StoreContext *)context())->validate(*((CertContext *)cert.context()), u);
}

QValueList<Certificate> Store::certificates() const
{
	return QValueList<Certificate>();
}

QValueList<CRL> Store::crls() const
{
	return QValueList<CRL>();
}

QByteArray Store::toPKCS7() const
{
	return QByteArray();
}

QString Store::toFlatText() const
{
	return QString();
}

bool Store::fromPKCS7(const QByteArray &a)
{
	Q_UNUSED(a);
	return false;
}

bool Store::fromFlatText(const QString &s)
{
	Q_UNUSED(s);
	return false;
}

void Store::append(const Store &a)
{
	Q_UNUSED(a);
}

Store Store::operator+(const Store &a) const
{
	Store s = *this;
	s.append(a);
	return s;
}

Store & Store::operator+=(const Store &a)
{
	append(a);
	return *this;
}

//----------------------------------------------------------------------------
// PersonalBundle
//----------------------------------------------------------------------------
PersonalBundle::PersonalBundle(const QString &provider)
{
	Q_UNUSED(provider);
}

bool PersonalBundle::isNull() const
{
	return false;
}

CertificateChain PersonalBundle::certificateChain() const
{
	return CertificateChain();
}

PrivateKey PersonalBundle::privateKey() const
{
	return PrivateKey();
}

void PersonalBundle::setCertificateChainAndKey(const CertificateChain &c, const PrivateKey &key)
{
	Q_UNUSED(c);
	Q_UNUSED(key);
}

QSecureArray PersonalBundle::toArray(const QString &name, const QSecureArray &passphrase) const
{
	Q_UNUSED(name);
	Q_UNUSED(passphrase);
	return QSecureArray();
}

PersonalBundle PersonalBundle::fromArray(const QSecureArray &a, const QSecureArray &passphrase, const QString &provider)
{
	Q_UNUSED(a);
	Q_UNUSED(passphrase);
	Q_UNUSED(provider);
	return PersonalBundle();
}

}
