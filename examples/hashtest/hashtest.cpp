#include"qca.h"
#include<stdio.h>

int main(int argc, char **argv)
{
	QCA::Initializer init;
	QCString cs = (argc >= 2) ? argv[1] : "hello";

	if( !QCA::isSupported("sha1") )
		printf("SHA1 not supported!\n");
	else {
		QCA::SHA1 sha1Hash;
		QString result = sha1Hash.hashToString(cs);
		printf("sha1(\"%s\") = [%s]\n", cs.data(), result.latin1());
	}

	if( !QCA::isSupported("md5") )
		printf("MD5 not supported!\n");
	else {
		QCA::MD5 md5Hash;
		QString result = md5Hash.hashToString(cs);
		printf("md5(\"%s\") = [%s]\n", cs.data(), result.latin1());
	}

	return 0;
}

