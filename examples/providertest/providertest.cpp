/*
 Copyright (C) 2004 Brad Hards <bradh@frogmouth.net>

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

// QtCrypto/QtCrypto has the declarations for all of QCA
#include <QtCrypto>
#include <iostream>
#include <qstringlist.h>

int main(int argc, char **argv)
{
    Q_UNUSED( argc );
    Q_UNUSED( argv );

    // the Initializer object sets things up, and 
    // also does cleanup when it goes out of scope
    QCA::Initializer init;

    // this is a hack to get all the available providers loaded.
    QCA::isSupported("foo");

    // this gives us all the plugin providers as a list
    QCA::ProviderList qcaProviders = QCA::providers();
    // which we can iterate over...
    QCA::ProviderListIterator it( qcaProviders );
    QCA::Provider *provider;
    while ( 0 != (provider = it.current() ) ) {
	++it;
	// each provider has a name, which we can display
	std::cout << provider->name() << ": ";
	// ... and also a list of features
	QStringList capabilities = provider->features();
	// we turn the string list back into a single string,
	// and display it as well
	std::cout << capabilities.join(", ") << std::endl;
    }

    // Note that the default provider isn't included in
    // the result of QCA::providers()
    std::cout << "default: ";
    // However it is still possible to get the features
    // supported by the default provider
    QStringList capabilities = QCA::defaultFeatures();
    std::cout << capabilities.join(", ") << std::endl;
    return 0;
}

