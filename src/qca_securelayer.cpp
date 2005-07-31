/*
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

#include "qca_securelayer.h"

#include <QtCore>
#include "qcaprovider.h"

namespace QCA {

Provider::Context *getContext(const QString &type, const QString &provider);

//----------------------------------------------------------------------------
// SecureLayer
//----------------------------------------------------------------------------
SecureLayer::SecureLayer(QObject *parent)
:QObject(parent)
{
}

bool SecureLayer::isClosable() const
{
	return false;
}

void SecureLayer::close()
{
}

QByteArray SecureLayer::readUnprocessed()
{
	return QByteArray();
}

//----------------------------------------------------------------------------
// TLS
//----------------------------------------------------------------------------
enum ResetMode
{
	ResetSession        = 0,
	ResetSessionAndData = 1,
	ResetAll            = 2
};

class TLS::Private : public QObject
{
	Q_OBJECT
public:
	TLS *q;
	TLSContext *c;

	CertificateChain localCert;
	PrivateKey localKey;
	CertificateCollection trusted;
	bool con_ssfMode;
	int con_minSSF, con_maxSSF;
	QStringList con_cipherSuites;
	bool tryCompress;

	QString host;
	CertificateChain peerCert;
	Validity peerValidity;
	bool hostMismatch;
	TLSContext::SessionInfo sessionInfo;

	QByteArray in, out;
	QByteArray to_net, from_net;

	enum { OpStart, OpHandshake, OpShutdown, OpEncode, OpDecode };
	int last_op;

	bool handshaken, closing, closed, error;
	bool tryMore;
	int bytesEncoded;
	Error errorCode;

	Private(TLS *_q) : QObject(_q), q(_q)
	{
		c = 0;

		reset(ResetAll);
	}

	void reset(ResetMode mode)
	{
		if(c)
			c->reset();

		host = QString();
		out.clear();
		handshaken = false;
		closing = false;
		closed = false;
		error = false;
		tryMore = false;
		bytesEncoded = 0;

		if(mode >= ResetSessionAndData)
		{
			peerCert = CertificateChain();
			peerValidity = ErrorValidityUnknown;
			hostMismatch = false;
			in.clear();
			to_net.clear();
			from_net.clear();
		}

		if(mode >= ResetAll)
		{
			localCert = CertificateChain();
			localKey = PrivateKey();
			trusted = CertificateCollection();
			con_ssfMode = true;
			con_minSSF = 128;
			con_maxSSF = -1;
			con_cipherSuites = QStringList();
			tryCompress = false;
		}
	}

	bool start(bool serverMode)
	{
		if(con_ssfMode)
			c->setConstraints(con_minSSF, con_maxSSF);
		else
			c->setConstraints(con_cipherSuites);

		c->setup(trusted, localCert, localKey, serverMode, tryCompress, false);

		bool ok;
		c->start();
		last_op = OpStart;
		c->waitForResultsReady(-1);
		ok = c->result() == TLSContext::Success;
		if(!ok)
			return false;

		update();
		return true;
	}

	void close()
	{
		if(!handshaken || closing)
			return;

		closing = true;
	}

	void update()
	{
		bool wasHandshaken = handshaken;
		//q->layerUpdateBegin();
		int _read = q->bytesAvailable();
		int _readout = q->bytesOutgoingAvailable();
		bool _closed = closed;
		bool _error = error;

		if(closing)
			updateClosing();
		else
			updateMain();

		if(!wasHandshaken && handshaken)
			QMetaObject::invokeMethod(q, "handshaken", Qt::QueuedConnection);

		if(q->bytesAvailable() > _read)
		{
			emit q->readyRead();
			//QMetaObject::invokeMethod(q, "readyRead", Qt::QueuedConnection);
		}
		if(q->bytesOutgoingAvailable() > _readout)
			QMetaObject::invokeMethod(q, "readyReadOutgoing", Qt::QueuedConnection);
		if(!_closed && closed)
			QMetaObject::invokeMethod(q, "closed", Qt::QueuedConnection);
		if(!_error && error)
			QMetaObject::invokeMethod(q, "error", Qt::QueuedConnection);
		//q->layerUpdateEnd();
	}

	void updateClosing()
	{
		QByteArray a;
		TLSContext::Result r;
		c->update(from_net, QByteArray());
		last_op = OpShutdown;
		c->waitForResultsReady(-1);
		a = c->to_net();
		r = c->result();
		from_net.clear();

		if(r == TLSContext::Error)
		{
			reset(ResetSession);
			error = true;
			errorCode = ErrorHandshake;
			return;
		}

		to_net.append(a);

		if(r == TLSContext::Success)
		{
			from_net = c->unprocessed();
			reset(ResetSession);
			closed = true;
			return;
		}
	}

	void updateMain()
	{
		bool force_read = false;

		if(!handshaken)
		{
			QByteArray a;
			TLSContext::Result r;
			c->update(from_net, QByteArray());
			last_op = OpHandshake;
			c->waitForResultsReady(-1);
			a = c->to_net();
			r = c->result();
			from_net.clear();

			if(r == TLSContext::Error)
			{
				reset(ResetSession);
				error = true;
				errorCode = ErrorHandshake;
				return;
			}

			to_net.append(a);

			if(r == TLSContext::Success)
			{
				peerCert = c->peerCertificateChain();
				if(!peerCert.isEmpty())
				{
					peerValidity = c->peerCertificateValidity();
					if(peerValidity == ValidityGood && !host.isEmpty() && !peerCert.primary().matchesHostname(host))
						hostMismatch = true;
				}
				sessionInfo = c->sessionInfo();
				handshaken = true;
				force_read = true;
			}
		}

		if(handshaken)
		{
			bool eof = false;

			if(!out.isEmpty() || tryMore)
			{
				tryMore = false;
				QByteArray a;
				int enc;
				bool more = false;
				c->update(QByteArray(), out);
				c->waitForResultsReady(-1);
				bool ok = c->result() == TLSContext::Success;
				a = c->to_net();
				enc = c->encoded();
				eof = c->eof();
				if(ok && enc < out.size())
					more = true;
				out.clear();
				if(!eof)
				{
					if(!ok)
					{
						reset(ResetSession);
						error = true;
						errorCode = ErrorCrypt;
						return;
					}
					bytesEncoded += enc;
					if(more)
						tryMore = true;
					to_net.append(a);
				}
			}

			if(!from_net.isEmpty() || force_read)
			{
				QByteArray a;
				QByteArray b;
				c->update(from_net, QByteArray());
				c->waitForResultsReady(-1);
				bool ok = c->result() == TLSContext::Success;
				a = c->to_app();
				b = c->to_net();
				eof = c->eof();
				from_net.clear();
				if(!ok)
				{
					reset(ResetSession);
					error = true;
					errorCode = ErrorCrypt;
					return;
				}
				in.append(a);
				to_net.append(b);
			}

			if(eof)
			{
				close();
				updateClosing();
				return;
			}
		}
	}

private slots:
	void tls_resultsReady()
	{
		//printf("results ready\n");
	}
};

TLS::TLS(QObject *parent, const QString &provider)
:SecureLayer(parent), Algorithm("tls", provider)
{
	d = new Private(this);
	d->c = static_cast<TLSContext *>(context());
	d->c->setParent(d);
	connect(d->c, SIGNAL(resultsReady()), d, SLOT(tls_resultsReady()));
}

TLS::TLS(Mode mode, QObject *parent, const QString &provider)
:SecureLayer(parent), Algorithm(mode == Stream ? "tls" : "dtls", provider)
{
	d = new Private(this);
	d->c = static_cast<TLSContext *>(context());
	d->c->setParent(d);
	connect(d->c, SIGNAL(resultsReady()), d, SLOT(tls_resultsReady()));
}

TLS::~TLS()
{
	d->c->setParent(0);
	delete d;
}

void TLS::reset()
{
	d->reset(ResetAll);
}

QStringList TLS::supportedCipherSuites(Mode mode, const QString &provider)
{
	QStringList list;
	const TLSContext *c = static_cast<const TLSContext *>(getContext(mode == Stream ? "tls" : "dtls", provider));
	if(!c)
		return list;
	list = c->supportedCipherSuites();
	delete c;
	return list;
}

void TLS::setCertificate(const CertificateChain &cert, const PrivateKey &key)
{
	d->localCert = cert;
	d->localKey = key;
}

void TLS::setTrustedCertificates(const CertificateCollection &trusted)
{
	d->trusted = trusted;
}

void TLS::setConstraints(SecurityLevel s)
{
	int min;
	switch(s)
	{
		case SL_None:
			min = 0;
			break;
		case SL_Integrity:
			min = 1;
			break;
		case SL_Export:
			min = 40;
			break;
		case SL_Baseline:
			min = 128;
			break;
		case SL_High:
			min = 129;
			break;
		case SL_Highest:
			min = qMax(129, d->c->maxSSF());
			break;
	}
	d->con_ssfMode = true;
	d->con_minSSF = min;
	d->con_maxSSF = -1;
}

void TLS::setConstraints(int minSSF, int maxSSF)
{
	d->con_ssfMode = true;
	d->con_minSSF = minSSF;
	d->con_maxSSF = maxSSF;
}

void TLS::setConstraints(const QStringList &cipherSuiteList)
{
	d->con_ssfMode = false;
	d->con_cipherSuites = cipherSuiteList;
}

bool TLS::canCompress(Mode mode, const QString &provider)
{
	bool ok = false;
	const TLSContext *c = static_cast<const TLSContext *>(getContext(mode == Stream ? "tls" : "dtls", provider));
	if(!c)
		return ok;
	ok = c->canCompress();
	delete c;
	return ok;
}

void TLS::setCompressionEnabled(bool b)
{
	d->tryCompress = b;
}

void TLS::startClient(const QString &host)
{
	d->reset(ResetSessionAndData);
	d->host = host;
	//layerUpdateBegin();
	if(!d->start(false))
	{
		d->reset(ResetSession);
		d->error = true;
		d->errorCode = ErrorInit;
		QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection);
	}
	//layerUpdateEnd();
}

void TLS::startServer()
{
	d->reset(ResetSessionAndData);
	//layerUpdateBegin();
	if(!d->start(true))
	{
		d->reset(ResetSession);
		d->error = true;
		d->errorCode = ErrorInit;
		QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection);
	}
	//layerUpdateEnd();
}

bool TLS::isHandshaken() const
{
	return d->handshaken;
}

bool TLS::isCompressed() const
{
	return d->sessionInfo.isCompressed;
}

TLS::Version TLS::version() const
{
	return d->sessionInfo.version;
}

QString TLS::cipherSuite() const
{
	return d->sessionInfo.cipherSuite;
}

int TLS::cipherBits() const
{
	return d->sessionInfo.cipherBits;
}

int TLS::cipherMaxBits() const
{
	return d->sessionInfo.cipherMaxBits;
}

TLS::Error TLS::errorCode() const
{
	return d->errorCode;
}

TLS::IdentityResult TLS::peerIdentityResult() const
{
	if(d->peerCert.isEmpty())
		return NoCertificate;

	if(d->peerValidity != ValidityGood)
		return InvalidCertificate;

	if(d->hostMismatch)
		return HostMismatch;

	return Valid;
}

Validity TLS::peerCertificateValidity() const
{
	return d->peerValidity;
}

CertificateChain TLS::localCertificateChain() const
{
	return d->localCert;
}

CertificateChain TLS::peerCertificateChain() const
{
	return d->peerCert;
}

bool TLS::isClosable() const
{
	return true;
}

int TLS::bytesAvailable() const
{
	return d->in.size();
}

int TLS::bytesOutgoingAvailable() const
{
	return d->to_net.size();
}

void TLS::close()
{
	d->close();
	d->update();
}

void TLS::write(const QByteArray &a)
{
	d->out.append(a);
	d->update();
}

QByteArray TLS::read()
{
	QByteArray a = d->in;
	d->in.clear();
	return a;
}

void TLS::writeIncoming(const QByteArray &a)
{
	d->from_net.append(a);
	d->update();
}

QByteArray TLS::readOutgoing(int *plainBytes)
{
	QByteArray a = d->to_net;
	d->to_net.clear();
	if(plainBytes)
		*plainBytes = d->bytesEncoded;
	d->bytesEncoded = 0;
	return a;
}

QByteArray TLS::readUnprocessed()
{
	QByteArray a = d->from_net;
	d->from_net.clear();
	return a;
}

int TLS::packetsAvailable() const
{
	// TODO
	return 0;
}

int TLS::packetsOutgoingAvailable() const
{
	// TODO
	return 0;
}

void TLS::setPacketMTU(int size) const
{
	// TODO
	Q_UNUSED(size);
}

//----------------------------------------------------------------------------
// SASL
//----------------------------------------------------------------------------

/*
  These don't map, but I don't think it matters much..
    SASL_TRYAGAIN  (-8)  transient failure (e.g., weak key)
    SASL_BADMAC    (-9)  integrity check failed
      -- client only codes --
    SASL_WRONGMECH (-11) mechanism doesn't support requested feature
    SASL_NEWSECRET (-12) new secret needed
      -- server only codes --
    SASL_TRANS     (-17) One time use of a plaintext password will
                         enable requested mechanism for user
    SASL_PWLOCK    (-21) password locked
    SASL_NOCHANGE  (-22) requested change was not needed
*/

QString *saslappname = 0;
class SASL::Private
{
public:
	void setSecurityProps()
	{
		//c->setSecurityProps(noPlain, noActive, noDict, noAnon, reqForward, reqCreds, reqMutual, ssfmin, ssfmax, ext_authid, ext_ssf);
	}

	// security opts
	bool noPlain, noActive, noDict, noAnon, reqForward, reqCreds, reqMutual;
	int ssfmin, ssfmax;
	QString ext_authid;
	int ext_ssf;

	bool tried;
	SASLContext *c;
	//QHostAddress localAddr, remoteAddr;
	int localPort, remotePort;
	QByteArray stepData;
	bool allowCSF;
	bool first, server;
	Error errorCode;

	QByteArray inbuf, outbuf;
};

SASL::SASL(QObject *parent, const QString &provider)
:SecureLayer(parent), Algorithm("sasl", provider)
{
	d = new Private;
	d->c = (SASLContext *)context();
	reset();
}

SASL::~SASL()
{
	delete d;
}

void SASL::reset()
{
	d->localPort = -1;
	d->remotePort = -1;

	d->noPlain = false;
	d->noActive = false;
	d->noDict = false;
	d->noAnon = false;
	d->reqForward = false;
	d->reqCreds = false;
	d->reqMutual = false;
	d->ssfmin = 0;
	d->ssfmax = 0;
	d->ext_authid = QString();
	d->ext_ssf = 0;

	d->inbuf.resize(0);
	d->outbuf.resize(0);

	d->c->reset();
}

SASL::Error SASL::errorCode() const
{
	return d->errorCode;
}

SASL::AuthCondition SASL::authCondition() const
{
	return AuthFail;
	//return (AuthCondition)d->c->authError();
}

void SASL::setConstraints(AuthFlags f, SecurityLevel s)
{
	Q_UNUSED(f);
	Q_UNUSED(s);

	/*d->noPlain    = (f & SAllowPlain) ? false: true;
	d->noAnon     = (f & SAllowAnonymous) ? false: true;
	//d->noActive   = (f & SAllowActiveVulnerable) ? false: true;
	//d->noDict     = (f & SAllowDictVulnerable) ? false: true;
	d->reqForward = (f & SRequireForwardSecrecy) ? true : false;
	d->reqCreds   = (f & SRequirePassCredentials) ? true : false;
	d->reqMutual  = (f & SRequireMutualAuth) ? true : false;*/

	//d->ssfmin = minSSF;
	//d->ssfmax = maxSSF;
}

void SASL::setConstraints(AuthFlags, int, int)
{
}

void SASL::setExternalAuthId(const QString &authid)
{
	d->ext_authid = authid;
}

void SASL::setExternalSSF(int x)
{
	d->ext_ssf = x;
}

void SASL::setLocalAddr(const QString &addr, quint16 port)
{
	Q_UNUSED(addr);
	//d->localAddr = addr;
	d->localPort = port;
}

void SASL::setRemoteAddr(const QString &addr, quint16 port)
{
	Q_UNUSED(addr);
	//d->remoteAddr = addr;
	d->remotePort = port;
}

void SASL::startClient(const QString &service, const QString &host, const QStringList &mechlist, ClientSendMode mode)
{
	SASLContext::HostPort la, ra;
	/*if(d->localPort != -1) {
		la.addr = d->localAddr;
		la.port = d->localPort;
	}
	if(d->remotePort != -1) {
		ra.addr = d->remoteAddr;
		ra.port = d->remotePort;
	}*/

	//d->allowCSF = allowClientSendFirst;
	//d->c->setCoreProps(service, host, d->localPort != -1 ? &la : 0, d->remotePort != -1 ? &ra : 0);
	d->setSecurityProps();

	Q_UNUSED(service);
	Q_UNUSED(host);
	Q_UNUSED(mechlist);
	Q_UNUSED(mode);

	/*if(!d->c->clientStart(mechlist))
	{
		// TODO: ErrorInit
		return;
	}*/
	d->first = true;
	d->server = false;
	d->tried = false;
	QTimer::singleShot(0, this, SLOT(tryAgain()));
	//return true;
}

void SASL::startServer(const QString &service, const QString &host, const QString &realm, ServerSendMode mode)
{
	//Q_UNUSED(allowServerSendLast);

	SASLContext::HostPort la, ra;
	/*if(d->localPort != -1) {
		la.addr = d->localAddr;
		la.port = d->localPort;
	}
	if(d->remotePort != -1) {
		ra.addr = d->remoteAddr;
		ra.port = d->remotePort;
	}*/

	//d->c->setCoreProps(service, host, d->localPort != -1 ? &la : 0, d->remotePort != -1 ? &ra : 0);
	//d->setSecurityProps();
	Q_UNUSED(service);
	Q_UNUSED(host);
	Q_UNUSED(realm);
	Q_UNUSED(mode);

	QString appname;
	if(saslappname)
		appname = *saslappname;
	else
		appname = "qca";

	/*if(!d->c->serverStart(realm, mechlist, appname))
	{
		// TODO: ErrorInit
		return;
	}*/
	d->first = true;
	d->server = true;
	d->tried = false;
	//return true;
}

void SASL::putServerFirstStep(const QString &mech)
{
	Q_UNUSED(mech);
	/*int r =*/ //d->c->serverFirstStep(mech, 0);
	//handleServerFirstStep(r);
}

void SASL::putServerFirstStep(const QString &mech, const QByteArray &clientInit)
{
	Q_UNUSED(mech);
	Q_UNUSED(clientInit);
	/*int r =*/ //d->c->serverFirstStep(mech, &clientInit);
	//handleServerFirstStep(r);
}

void SASL::putStep(const QByteArray &stepData)
{
	d->stepData = stepData;
	//tryAgain();
}

void SASL::setUsername(const QString &user)
{
	d->c->setClientParams(&user, 0, 0, 0);
}

void SASL::setAuthzid(const QString &authzid)
{
	d->c->setClientParams(0, &authzid, 0, 0);
}

void SASL::setPassword(const QSecureArray &pass)
{
	d->c->setClientParams(0, 0, &pass, 0);
}

void SASL::setRealm(const QString &realm)
{
	d->c->setClientParams(0, 0, 0, &realm);
}

void SASL::continueAfterParams()
{
	//tryAgain();
}

void SASL::continueAfterAuthCheck()
{
	//tryAgain();
}

QString SASL::mechanism() const
{
	return QString();
}

QStringList SASL::mechanismList() const
{
	return QStringList();
}

int SASL::ssf() const
{
	return d->c->ssf();
}

int SASL::bytesAvailable() const
{
	return 0;
}

int SASL::bytesOutgoingAvailable() const
{
	return 0;
}

void SASL::write(const QByteArray &a)
{
	Q_UNUSED(a);
}

QByteArray SASL::read()
{
	return QByteArray();
}

void SASL::writeIncoming(const QByteArray &a)
{
	Q_UNUSED(a);
}

QByteArray SASL::readOutgoing(int *plainBytes)
{
	Q_UNUSED(plainBytes);
	return QByteArray();
}

}

#include "qca_securelayer.moc"
