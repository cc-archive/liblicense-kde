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

	juris_t *jurisdictions = ll_get_jurisdictions();
	int i;
	int len = ll_list_length(jurisdictions);
	for (i=0; i<len; ++i) {
		char *juris_name = ll_jurisdiction_name(jurisdictions[i]);
		chooserWidget->jurisdictionComboBox->addItem(
			QIcon(QString(LICENSE_ICON_DIR "/%1.png").arg(jurisdictions[i])),
			QString::fromUtf8(juris_name),
			QVariant(jurisdictions[i])
		);
		free(juris_name);
	}
	ll_free_list(jurisdictions);

	// CLEANUP: icon paths
	chooserWidget->attributionCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/by.svg") );
	chooserWidget->sharingCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/ash.svg") );
	chooserWidget->remixingCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/ar.svg") );
	chooserWidget->noCommercialCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/pcw.svg") );
	chooserWidget->shareAlikeCheckBox->setIcon( QIcon(LICENSE_ICON_DIR "/sa.svg") );

	connect(chooserWidget->uriCombo,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(licenseChanged(const QString &)));

	signalMapper = new QSignalMapper(this);

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
	return chooserWidget->uriCombo->currentText();
}

void LicenseChooser::setLicenseURI( const QString &uriString )
{
	char **attr;
	char **attrs;

	const QByteArray uri_ba = uriString.toUtf8();
	const uri_t uri = (const uri_t)uri_ba.data();

	signalMapper->blockSignals(true);
	chooserWidget->sharingCheckBox->setChecked(false);
	chooserWidget->remixingCheckBox->setChecked(false);
	chooserWidget->attributionCheckBox->setChecked(false);
	chooserWidget->shareAlikeCheckBox->setChecked(false);
	chooserWidget->noCommercialCheckBox->setChecked(false);
	permits_flags = requires_flags = prohibits_flags = LL_UNSPECIFIED;
	signalMapper->blockSignals(false);

	if ( !ll_verify_uri(uri) ) {
		kDebug() << "Setting combo to: " << uriString << endl;
		chooserWidget->uriCombo->setEditText(uriString);
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

	kDebug() << "Setting combo to: " << uriString << endl;
	chooserWidget->uriCombo->setEditText(uriString);

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
	chooser = ll_new_license_chooser(juris,attributes);
}

void LicenseChooser::updateLicense()
{
	print_flags(attributes,permits_flags,requires_flags,prohibits_flags);

	chooserWidget->uriCombo->clear();

	const ll_license_list_t *licenses = ll_get_licenses_from_flags(chooser,permits_flags,requires_flags,prohibits_flags);
	if (licenses) {
		const uri_t uri = (const uri_t)licenses->license;
		updateLicense(uri);

		const ll_license_list_t *license = licenses;
		while (license) {
			kDebug() << "adding combo item: " << license->license << endl;
			chooserWidget->uriCombo->addItem(QString::fromUtf8(license->license));
			license = license->next;
		}
	} else {
		kDebug() << "No license matches" << endl;
		chooserWidget->uriCombo->setEditText(QString::null);
		chooserWidget->licenseEdit->setText(i18n("None"));
		emit licenseChanged();
	}
}

void LicenseChooser::updateLicense(const uri_t uri)
{
		QString version;
		int *v = ll_get_version(uri);
		if (v) {
			QStringList versionList;
			int i;
			for (i=1; i<=v[0]; ++i) {
				versionList << QString::number(v[i]);
			}
			version = QString(" - %1").arg(versionList.join("."));
		}

		chooserWidget->licenseEdit->setText(
			QString("%1%2")
				.arg(QString::fromUtf8(ll_get_name(uri)))
				.arg(version)
		);
		free(v);
}

void LicenseChooser::checkBoxClicked(QWidget *checkBox)
{
	if ( checkBox == chooserWidget->attributionCheckBox ) {
		requires_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#Attribution");
	} else if ( checkBox == chooserWidget->sharingCheckBox ) {
		permits_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#Distribution");
	} else if ( checkBox == chooserWidget->remixingCheckBox ) {
		permits_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#DerivativeWorks");

		chooserWidget->shareAlikeCheckBox->setEnabled( chooserWidget->remixingCheckBox->isChecked() );
		if (!chooserWidget->remixingCheckBox->isChecked() && chooserWidget->shareAlikeCheckBox->isChecked())
			chooserWidget->shareAlikeCheckBox->setChecked(false);

	} else if ( checkBox == chooserWidget->noCommercialCheckBox ) {
		prohibits_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#CommercialUse");
	} else if ( checkBox == chooserWidget->shareAlikeCheckBox ) {
		requires_flags ^= ll_attribute_flag(chooser,"http://creativecommons.org/ns#ShareAlike");
	}

	updateLicense();
}

void LicenseChooser::licenseChanged(const QString &uri)
{
	QByteArray data = uri.toUtf8();
	updateLicense(data.data());
	
	emit licenseChanged();
}
