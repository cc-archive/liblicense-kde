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

#ifndef LICENSEPROPS_H
#define LICENSEPROPS_H

#include <kpropertiesdialog.h>
#include <kdebug.h>

#include "licensechooser.h"

class KVBox;

class KIO_EXPORT LicensePropsPlugin : public KPropertiesDialogPlugin
{
	Q_OBJECT

public:
    LicensePropsPlugin(KPropertiesDialog *, const QStringList &);
    virtual ~LicensePropsPlugin();

    /**
     * Apply all changes to the file.
     * This function is called when the user presses 'Ok'.
     */
    virtual void applyChanges();

    static bool supports( const KFileItemList& items );

    QWidget* page() const;

private:
    KVBox *m_vBox;
    QWidget *m_widget;
    LicenseChooser *licenseChooser;
};

#endif
