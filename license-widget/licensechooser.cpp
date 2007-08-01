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

#include "licensechooser.h"
#include "licensechooser.moc"

#include "ui_license_chooser.h"

#include <QtCore/QSignalMapper>
#include <kdebug.h>
#include <klocale.h>

char *attributes[] = {
		"http://creativecommons.org/ns#Distribution",
		"http://creativecommons.org/ns#CommercialUse",
		"http://creativecommons.org/ns#DerivativeWorks",
		"http://creativecommons.org/ns#ShareAlike",
		"http://creativecommons.org/ns#Attribution",
		NULL
};

void print_flags( char **attributes, int p, int r, int pr )
{
	kDebug() << "Selected flags:" << endl;
	int i = 0;
	while (attributes[i]) {
		if (p & (1<<i)) {
			kDebug() << "\tPERMITS   : " << attributes[i] << endl;;
		} else if (r & (1<<i)) {
			kDebug() << "\tREQUIRES  : " << attributes[i] << endl;;
		} else if (pr & (1<<i)) {
			kDebug() << "\tPROHIBITS : " << attributes[i] << endl;;
		}
		++i;
	}
	kDebug() << endl;
}


LicenseChooser::LicenseChooser( QWidget *parent )
{
	ll_init();

	chooserWidget = new Ui_LicenseChooserWidget;
	chooserWidget->setupUi(parent);

	permits_flags = requires_flags = prohibits_flags = LL_UNSPECIFIED;

	chooserWidget->jurisdictionComboBox->addItem(QIcon(LICENSE_ICON_DIR "/unported.png"), i18n("Unported"), QVariant(QString::null));
	chooserWidget->jurisdictionComboBox->addItem(QIcon(LICENSE_ICON_DIR "/uk.png"), i18n("United Kingdom"), QVariant("uk"));
	chooserWidget->jurisdictionComboBox->addItem(QIcon(LICENSE_ICON_DIR "/us.png"), i18n("United States"), QVariant("us"));

	// CLEANUP: icon paths
	chooserWidget->attributionCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/by.svg") );
	chooserWidget->sharingCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/ash.svg") );
	chooserWidget->remixingCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/ar.svg") );
	chooserWidget->noCommercialCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/pcw.svg") );
	chooserWidget->shareAlikeCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/sa.svg") );

	connect(chooserWidget->uriEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(licenseChanged()));

	QSignalMapper *signalMapper = new QSignalMapper(this);

	connect(chooserWidget->attributionCheckBox, SIGNAL(stateChanged(int)), signalMapper, SLOT(map()));
	connect(chooserWidget->sharingCheckBox, SIGNAL(stateChanged(int)), signalMapper, SLOT(map()));
	connect(chooserWidget->remixingCheckBox, SIGNAL(stateChanged(int)), signalMapper, SLOT(map()));
	connect(chooserWidget->noCommercialCheckBox, SIGNAL(stateChanged(int)), signalMapper, SLOT(map()));
	connect(chooserWidget->shareAlikeCheckBox, SIGNAL(stateChanged(int)), signalMapper, SLOT(map()));

	signalMapper->setMapping(chooserWidget->attributionCheckBox, chooserWidget->attributionCheckBox);
	signalMapper->setMapping(chooserWidget->sharingCheckBox, chooserWidget->sharingCheckBox);
	signalMapper->setMapping(chooserWidget->remixingCheckBox, chooserWidget->remixingCheckBox);
	signalMapper->setMapping(chooserWidget->noCommercialCheckBox, chooserWidget->noCommercialCheckBox);
	signalMapper->setMapping(chooserWidget->shareAlikeCheckBox, chooserWidget->shareAlikeCheckBox);

	connect(signalMapper, SIGNAL(mapped(QWidget*)),
		this, SLOT(checkBoxClicked(QWidget*)));

	connect(chooserWidget->jurisdictionComboBox, SIGNAL(activated(int)), this, SLOT(updateJurisdiction()));

	updateChooser();
}

LicenseChooser::~LicenseChooser()
{
	ll_free_license_chooser(chooser);
	ll_stop();
}

QString LicenseChooser::licenseURI()
{
	return chooserWidget->uriEdit->text();
}

void LicenseChooser::setLicenseURI( const QString &uriString )
{
	char **attr;
	char **attrs;

	const QByteArray uri_ba = uriString.toUtf8();
	const uri_t uri = (const uri_t)uri_ba.data();

	if ( !ll_verify_uri(uri) ) {
		chooserWidget->uriEdit->setText(QString::fromUtf8(uri));
		return;
	}

	attrs = ll_get_permits(uri);
	for (attr=attrs; *attr; ++attr) {
		if (strcmp(*attr,"http://creativecommons.org/ns#Distribution")==0) {
			chooserWidget->sharingCheckBox->setChecked(true);
		} else if (strcmp(*attr,"http://creativecommons.org/ns#DerivativeWorks")==0) {
			chooserWidget->remixingCheckBox->setChecked(true);
		}
	}

	attrs = ll_get_requires(uri);
	for (attr=attrs; *attr; ++attr) {
		if (strcmp(*attr,"http://creativecommons.org/ns#Attribution")==0) {
			chooserWidget->attributionCheckBox->setChecked(true);
		} else if (strcmp(*attr,"http://creativecommons.org/ns#ShareAlike")==0) {
			chooserWidget->shareAlikeCheckBox->setChecked(true);
		}
	}

	attrs = ll_get_prohibits(uri);
	for (attr=attrs; *attr; ++attr) {
		if (strcmp(*attr,"http://creativecommons.org/ns#CommercialUse")==0) {
			chooserWidget->noCommercialCheckBox->setChecked(true);
		}
	}

	char *j = ll_get_jurisdiction(uri);
	QString juris = QString::fromLatin1(j);
	int i;
	for (i=0; i<chooserWidget->jurisdictionComboBox->count(); ++i) {
		if (juris == chooserWidget->jurisdictionComboBox->itemData(i).toString()) {
			chooserWidget->jurisdictionComboBox->setCurrentIndex(i);
			updateChooser();
			goto juris_found;
		}
	}
	//no match found, set to unported
	if ( chooserWidget->jurisdictionComboBox->currentIndex() != 0 ) {
		chooserWidget->jurisdictionComboBox->setCurrentIndex(0);
		updateChooser();
	}

	juris_found:
	free(j);

	updateLicense(uri);
}

void LicenseChooser::updateJurisdiction()
{
	updateChooser();
	updateLicense();
}

void LicenseChooser::updateChooser()
{
	QByteArray juris_ba = chooserWidget->jurisdictionComboBox->itemData( chooserWidget->jurisdictionComboBox->currentIndex() ).toString().toLatin1();
	char *juris = juris_ba.data();
	if ( juris[0] == '\0' ) juris = NULL;

	kDebug() << "juris: " << juris << endl;
	chooser = ll_new_license_chooser(juris,attributes);
}

void LicenseChooser::updateLicense()
{
	print_flags(attributes,permits_flags,requires_flags,prohibits_flags);
	const ll_license_list_t *licenses = ll_get_licenses_from_flags(chooser,permits_flags,requires_flags,prohibits_flags);
	if (licenses) {
		const uri_t uri = (const uri_t)licenses->license;
		updateLicense(uri);
	} else {
		kDebug() << "No license matches" << endl;
		chooserWidget->uriEdit->setText(QString::null);
		chooserWidget->licenseEdit->setText(i18n("none"));
		emit licenseChanged();
	}
}

void LicenseChooser::updateLicense(const uri_t uri)
{
		chooserWidget->uriEdit->setText(QString::fromLatin1(uri));

		int *v = ll_get_version(uri);

		chooserWidget->licenseEdit->setText(
			QString("%1 - %2.%3.%4")
				.arg(QString::fromLatin1(ll_get_name(uri)))
				.arg(v[0]).arg(v[1]).arg(v[2])
		);
		free(v);
}

void LicenseChooser::checkBoxClicked(QWidget *checkBox)
{
	kDebug() << checkBox->objectName() << " clicked" << endl;
	if ( checkBox == chooserWidget->attributionCheckBox ) {
		requires_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#Attribution");
	} else if ( checkBox == chooserWidget->sharingCheckBox ) {
		permits_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#Distribution");
	} else if ( checkBox == chooserWidget->remixingCheckBox ) {
		permits_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#DerivativeWorks");

		chooserWidget->shareAlikeCheckBox->setEnabled( chooserWidget->remixingCheckBox->isChecked() );
		if (!chooserWidget->remixingCheckBox->isChecked())
			chooserWidget->shareAlikeCheckBox->setChecked(false);

	} else if ( checkBox == chooserWidget->noCommercialCheckBox ) {
		prohibits_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#CommercialUse");
	} else if ( checkBox == chooserWidget->shareAlikeCheckBox ) {
		requires_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#ShareAlike");
	}

	updateLicense();
}
