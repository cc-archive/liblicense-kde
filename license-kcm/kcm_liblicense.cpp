// Creative Commons has made the contents of this file
// available under a CC-GNU-LGPL license:
//
// http://creativecommons.org/licenses/LGPL/2.1/
//
// A copy of the full license can be found as part of this
// distribution in the file COPYING.
// 
// You may use the liblicense software in accordance with the
// terms of that license. You agree that you are solely 
// responsible for your use of the liblicense software and you
// represent and warrant to Creative Commons that your use
// of the liblicense software will comply with the CC-GNU-LGPL.
//
// Copyright 2007, Creative Commons, www.creativecommons.org.
// Copyright 2007, Jason Kivlighn.

#include "kcm_liblicense.h"
#include "kcm_liblicense.moc"

#include <liblicense.h>

#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <kaboutdata.h>

typedef KGenericFactory<KCMLiblicense, QWidget> LibLicenseFactory;
K_EXPORT_COMPONENT_FACTORY( liblicense, LibLicenseFactory( "liblicense" ) );

KCMLiblicense::KCMLiblicense(QWidget *parent, const QStringList &) :
	  KCModule(LibLicenseFactory::componentData(), parent/*, name*/)
{
	KAboutData * about = new KAboutData("kcmliblicense", "liblicense",
	                                    ki18n("Default Content License"),
	                                    "0.4",
	                                    ki18n("Default Content License Control Panel Module"),
	                                    KAboutData::License_LGPL_V2,
	                                    ki18n("(c) 2007 Jason Kivlighn"));
	about->addAuthor(ki18n("Jason Kivlighn"), KLocalizedString(), "jkivlighn@gmail.com");

	setAboutData( about );

	licenseChooser = new LicenseChooser(this);
	connect( licenseChooser, SIGNAL(licenseChanged()), this, SLOT(changed()) );

	load();
}

KCMLiblicense::~KCMLiblicense()
{
	delete licenseChooser;
}

void KCMLiblicense::load()
{
	licenseChooser->restoreDefault();
}

void KCMLiblicense::save()
{
	const QByteArray uri_ba = licenseChooser->licenseURI().toUtf8();
	ll_set_default((const ll_uri_t)uri_ba.data());
}

void KCMLiblicense::defaults()
{
	printf("defaults\n");
}
