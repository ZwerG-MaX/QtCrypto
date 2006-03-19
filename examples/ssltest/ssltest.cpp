/*
 Copyright (C) 2003-2005 Justin Karneges

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QtCore>
#include <QtNetwork>
#include <QtCrypto>

void showCertInfo(const QCA::Certificate &cert)
{
	printf("-- Cert --\n");
	printf(" CN: %s\n", qPrintable(cert.commonName()));
	printf(" Valid from: %s, until %s\n",
		qPrintable(cert.notValidBefore().toString()),
		qPrintable(cert.notValidAfter().toString()));
	printf(" PEM:\n%s\n", qPrintable(cert.toPEM()));
}

static QString validityToString(QCA::Validity v)
{
	QString s;
	switch(v)
	{
		case QCA::ValidityGood:
			s = "Validated";
			break;
		case QCA::ErrorRejected:
			s = "Root CA is marked to reject the specified purpose";
			break;
		case QCA::ErrorUntrusted:
			s = "Certificate not trusted for the required purpose";
			break;
		case QCA::ErrorSignatureFailed:
			s = "Invalid signature";
			break;
		case QCA::ErrorInvalidCA:
			s = "Invalid CA certificate";
			break;
		case QCA::ErrorInvalidPurpose:
			s = "Invalid certificate purpose";
			break;
		case QCA::ErrorSelfSigned:
			s = "Certificate is self-signed";
			break;
		case QCA::ErrorRevoked:
			s = "Certificate has been revoked";
			break;
		case QCA::ErrorPathLengthExceeded:
			s = "Maximum certificate chain length exceeded";
			break;
		case QCA::ErrorExpired:
			s = "Certificate has expired";
			break;
		case QCA::ErrorExpiredCA:
			s = "CA has expired";
			break;
		case QCA::ErrorValidityUnknown:
		default:
			s = "General certificate validation error";
			break;
	}
	return s;
}

class SecureTest : public QObject
{
	Q_OBJECT
public:
	SecureTest()
	{
		sock = new QTcpSocket;
		connect(sock, SIGNAL(connected()), SLOT(sock_connected()));
		connect(sock, SIGNAL(readyRead()), SLOT(sock_readyRead()));
		connect(sock, SIGNAL(error(QAbstractSocket::SocketError)),
			SLOT(sock_error(QAbstractSocket::SocketError)));

		ssl = new QCA::TLS;
		connect(ssl, SIGNAL(handshaken()), SLOT(ssl_handshaken()));
		connect(ssl, SIGNAL(readyRead()), SLOT(ssl_readyRead()));
		connect(ssl, SIGNAL(readyReadOutgoing()),
			SLOT(ssl_readyReadOutgoing()));
		connect(ssl, SIGNAL(closed()), SLOT(ssl_closed()));
		connect(ssl, SIGNAL(error()), SLOT(ssl_error()));
	}

	~SecureTest()
	{
		delete ssl;
		delete sock;
	}

	void start(const QString &_host)
	{
		int n = _host.indexOf(':');
		int port;
		if(n != -1)
		{
			host = _host.mid(0, n);
			port = _host.mid(n+1).toInt();
		}
		else
		{
			host = _host;
			port = 443;
		}

		printf("Trying %s:%d...\n", qPrintable(host), port);
		sock->connectToHost(host, port);
	}

signals:
	void quit();

private slots:
	void sock_connected()
	{
		// We just do this to help doxygen...
		QCA::TLS *ssl = SecureTest::ssl;

		printf("Connected, starting TLS handshake...\n");

		if(!QCA::haveSystemStore())
			printf("Warning: no root certs\n");
		else 
			ssl->setTrustedCertificates(QCA::systemStore());

		ssl->startClient(host);
	}

	void sock_readyRead()
	{
		// We just do this to help doxygen...
		QCA::TLS *ssl = SecureTest::ssl;

		ssl->writeIncoming(sock->readAll());
	}

	void sock_connectionClosed()
	{
		printf("\nConnection closed.\n");
		quit();
	}

	void sock_error(QAbstractSocket::SocketError x)
	{
		if(x == QAbstractSocket::RemoteHostClosedError)
		{
			sock_connectionClosed();
			return;
		}

		printf("\nSocket error.\n");
		quit();
	}

	void ssl_handshaken()
	{
		// We just do this to help doxygen...
		QCA::TLS *ssl = SecureTest::ssl;

		QCA::TLS::IdentityResult r = ssl->peerIdentityResult();

		printf("Successful SSL handshake using %s (%i of %i bits)\n",
		       qPrintable(ssl->cipherSuite()),
		       ssl->cipherBits(),
		       ssl->cipherMaxBits() );
		if(r != QCA::TLS::NoCertificate)
		{
			cert = ssl->peerCertificateChain().primary();
			if(!cert.isNull())
				showCertInfo(cert);
		}

		QString str = "Peer Identity: ";
		if(r == QCA::TLS::Valid)
			str += "Valid";
		else if(r == QCA::TLS::HostMismatch)
			str += "Error: Wrong certificate";
		else if(r == QCA::TLS::InvalidCertificate)
			str += "Error: Invalid certificate.\n -> Reason: " +
				validityToString(ssl->peerCertificateValidity());
		else
			str += "Error: No certificate";
		printf("%s\n", qPrintable(str));

		printf("Let's try a GET request now.\n");
		QString req = "GET / HTTP/1.0\nHost: " + host + "\n\n";
		ssl->write(req.toLatin1());
	}

	void ssl_readyRead()
	{
		// We just do this to help doxygen...
		QCA::TLS *ssl = SecureTest::ssl;

		QByteArray a = ssl->read();
		printf("%s", a.data());
	}

	void ssl_readyReadOutgoing()
	{
		// We just do this to help doxygen...
		QCA::TLS *ssl = SecureTest::ssl;

		sock->write(ssl->readOutgoing());
	}

	void ssl_closed()
	{
		printf("SSL session closed.\n");
	}

	void ssl_error()
	{
		// We just do this to help doxygen...
		QCA::TLS *ssl = SecureTest::ssl;

		int x = ssl->errorCode();
		if(x == QCA::TLS::ErrorHandshake)
		{
			printf("SSL Handshake Error!\n");
			quit();
		}
		else
		{
			printf("SSL Error!\n");
			quit();
		}
	}

private:
	QString host;
	QTcpSocket *sock;
	QCA::TLS *ssl;
	QCA::Certificate cert;
};

#include "ssltest.moc"

int main(int argc, char **argv)
{
	QCA::Initializer init;

	QCoreApplication app(argc, argv);
	QString host = argc > 1 ? argv[1] : "andbit.net";

	if(!QCA::isSupported("tls"))
	{
		printf("TLS not supported!\n");
		return 1;
	}

	SecureTest *s = new SecureTest;
	QObject::connect(s, SIGNAL(quit()), &app, SLOT(quit()));
	s->start(host);
	app.exec();
	delete s;

	return 0;
}
