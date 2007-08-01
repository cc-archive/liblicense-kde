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

#include "licenseprops.h"
#include "licenseprops.moc"

#include <kvbox.h>
#include <QtCore/QSignalMapper>
#include <QVBoxLayout>

#include <kgenericfactory.h>

#include <liblicense.h>

typedef KGenericFactory<LicensePropsPlugin, KPropertiesDialog> LicensePropsFactory;
K_EXPORT_COMPONENT_FACTORY( licenseprops, LicensePropsFactory( "licenseprops" ) );

LicensePropsPlugin::LicensePropsPlugin(KPropertiesDialog *_props, const QStringList &) : KPropertiesDialogPlugin(_props)
{
	m_vBox = new KVBox();
	
	m_widget = new QWidget( m_vBox );
	QVBoxLayout * vbox = new QVBoxLayout( m_widget );
	
	QWidget *main = new QWidget(m_widget);
	vbox->addWidget( main );
	
	licenseChooser = new LicenseChooser(main);
	connect( licenseChooser, SIGNAL(licenseChanged()), this, SLOT(setDirty()) );

	m_widget->show(); // In case the dialog was shown already.

	if ( properties->items().count() == 1 ) {
		KFileItem *item = properties->item();
		if (item->url().isLocalFile()) {
			_props->addPage( m_vBox, i18n("&License") );

			QByteArray ba = item->localPath().toUtf8();
			char *license = ll_read(ba.data());

			licenseChooser->setLicenseURI(QString::fromUtf8(license));
		}
	}
}

LicensePropsPlugin::~LicensePropsPlugin()
{
	delete m_vBox;
	delete licenseChooser;
}

bool LicensePropsPlugin::supports( const KFileItemList& items )
{
    KFileItemList::const_iterator kit = items.begin();
    const KFileItemList::const_iterator kend = items.end();
    for ( ; kit != kend; ++kit )
    {
        bool isLocal = (*kit)->isLocalFile();
        // We only support local dirs
        if ( !(*kit)->isDir() || !isLocal )
            return false;
    }
    return true;
}

void LicensePropsPlugin::applyChanges()
{
	kDebug() << "LicensePropsPlugin::applyChanges" << endl;

	KFileItem *item = properties->item();
	if (!licenseChooser->licenseURI().isEmpty()) {
		QByteArray byteArray = licenseChooser->licenseURI().toUtf8();
		
		//watch out: we can't do uriEdit->text().toUtf8().data() because the result is 
		//only valid as long as the QByteArray is around
		char *license = byteArray.data();
		kDebug() << "writing license: " << license << endl;
		ll_write(item->localPath().toUtf8().data(),license);
	}
}

QWidget* LicensePropsPlugin::page() const
{
	return m_vBox;
}

