/*
 * qca_tools.cpp - Qt Cryptographic Architecture
 * Copyright (C) 2004  Justin Karneges
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

#include "qca.h"

#include "botantools/botantools.h"

namespace QCA {

// Botan shouldn't throw any exceptions in our init/deinit.

void botan_init()
{
	Botan::Init::set_mutex_type(new Botan::Qt_Mutex);
	Botan::Init::startup_memory_subsystem();

	Botan::set_default_allocator("locking");
#ifdef Q_OS_UNIX
	// botan prefers mmap over locking, so we will too
	Botan::add_allocator_type("mmap", new Botan::MemoryMapping_Allocator);
	Botan::set_default_allocator("mmap");
#endif
}

void botan_deinit()
{
	Botan::Init::shutdown_memory_subsystem();
	Botan::Init::set_mutex_type(0);
}

}

// TODO: catch exceptions for SecureVector and BigInt

//----------------------------------------------------------------------------
// QSecureArray
//----------------------------------------------------------------------------
class QSecureArray::Private
{
public:
	Private(uint size) : buf((Botan::u32bit)size) {}
	Private(const Botan::SecureVector<Botan::byte> &a) : buf(a) {}

	Botan::SecureVector<Botan::byte> buf;
};

QSecureArray::QSecureArray()
{
	d = 0;
}

QSecureArray::QSecureArray(int size)
{
	if(size > 0)
		d = new Private(size);
	else
		d = 0;
}

QSecureArray::QSecureArray(const QByteArray &a)
{
	d = 0;
	*this = a;
}

QSecureArray::QSecureArray(const QSecureArray &from)
{
	d = 0;
	*this = from;
}

QSecureArray::~QSecureArray()
{
	delete d;
}

QSecureArray & QSecureArray::operator=(const QSecureArray &from)
{
	delete d;
	d = 0;

	if(from.d)
		d = new Private(from.d->buf);

	return *this;
}

QSecureArray & QSecureArray::operator=(const QByteArray &from)
{
	delete d;
	d = 0;

	if(!from.isEmpty()) {
		d = new Private(from.size());
		Botan::byte *p = (Botan::byte *)d->buf;
		memcpy(p, from.data(), from.size());
	}

	return *this;
}

char & QSecureArray::operator[](int index) const
{
	return (char &)(*((Botan::byte *)d->buf + index));
}

char *QSecureArray::data() const
{
	if(!d)
		return 0;
	Botan::byte *p = (Botan::byte *)d->buf;
	return ((char *)p);
}

uint QSecureArray::size() const
{
	return (d ? d->buf.size() : 0);
}

bool QSecureArray::isEmpty() const
{
	return (size() == 0);
}

bool QSecureArray::resize(uint size)
{
	int cur_size = (d ? d->buf.size() : 0);
	if(cur_size == (int)size)
		return true;

	if(size > 0) {
		Private *d2 = new Private(size);
		Botan::byte *p2 = (Botan::byte *)d2->buf;
		if(d) {
			Botan::byte *p = (Botan::byte *)d->buf;
			memcpy(p2, p, cur_size);
			delete d;
		}
		d = d2;
	}
	else {
		delete d;
		d = 0;
	}
	return true;
}

QSecureArray QSecureArray::copy() const
{
	return *this;
}

QByteArray QSecureArray::toByteArray() const
{
	if(isEmpty())
		return QByteArray();

	QByteArray buf(size());
	memcpy(buf.data(), data(), size());
	return buf;
}

//----------------------------------------------------------------------------
// QBigInteger
//----------------------------------------------------------------------------
class QBigInteger::Private
{
public:
	Botan::BigInt n;
};

QBigInteger::QBigInteger()
{
	d = new Private;
}

QBigInteger::QBigInteger(int i)
{
	d = new Private;
	if(i < 0) {
		d->n = Botan::BigInt(i * (-1));
		d->n.set_sign(Botan::BigInt::Negative);
	}
	else {
		d->n = Botan::BigInt(i);
		d->n.set_sign(Botan::BigInt::Positive);
	}
}

QBigInteger::QBigInteger(const QString &s)
{
	d = new Private;
	fromString(s);
}

QBigInteger::QBigInteger(const QSecureArray &a)
{
	d = new Private;
	fromArray(a);
}

QBigInteger::QBigInteger(const QBigInteger &from)
{
	d = new Private;
	*this = from;
}

QBigInteger::~QBigInteger()
{
	delete d;
}

QBigInteger & QBigInteger::operator=(const QBigInteger &from)
{
	d->n = from.d->n;
	return *this;
}

QBigInteger & QBigInteger::operator+=(const QBigInteger &i)
{
	d->n += i.d->n;
	return *this;
}

QBigInteger & QBigInteger::operator-=(const QBigInteger &i)
{
	d->n -= i.d->n;
	return *this;
}

QBigInteger & QBigInteger::operator=(const QString &s)
{
	fromString(s);
	return *this;
}

QSecureArray QBigInteger::toArray() const
{
	QSecureArray a(d->n.encoded_size(Botan::BigInt::Binary) + 1);
	if(d->n.is_negative())
		a[0] = 1;
	else
		a[0] = 0;
	Botan::BigInt::encode((Botan::byte *)a.data() + 1, d->n, Botan::BigInt::Binary);
	return a;
}

void QBigInteger::fromArray(const QSecureArray &a)
{
	if(a.isEmpty()) {
		d->n = Botan::BigInt(0);
		return;
	}
	Botan::BigInt::Sign sign = Botan::BigInt::Positive;
	if(a[0] != 0)
		sign = Botan::BigInt::Negative;
	d->n = Botan::BigInt::decode((const Botan::byte *)a.data() + 1, a.size() - 1, Botan::BigInt::Binary);
	d->n.set_sign(sign);
}

QString QBigInteger::toString() const
{
	QCString cs;
	try {
		QByteArray a(d->n.encoded_size(Botan::BigInt::Decimal));
		Botan::BigInt::encode((Botan::byte *)a.data(), d->n, Botan::BigInt::Decimal);
		cs = QCString(a.data(), a.size() + 1);
	}
	catch(std::exception &e) {
		//std::cout << "toString error: " << e.what() << std::endl;
	}
	return QString::fromLatin1(cs);
}

bool QBigInteger::fromString(const QString &s)
{
	//try {
		QCString cs = s.latin1();
		d->n = Botan::BigInt::decode((const Botan::byte *)cs.data(), cs.length(), Botan::BigInt::Decimal);
	//}
	return true;
}
