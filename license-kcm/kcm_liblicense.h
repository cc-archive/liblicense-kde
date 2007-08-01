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

#ifndef KCMLIBLICENSE_H
#define KCMLIBLICENSE_H

#include <kcmodule.h>
#include <kdebug.h>
#include <kparts/part.h>

#include "licensechooser.h"

class KCMLiblicense : public KCModule
{
	Q_OBJECT

public:
	KCMLiblicense(QWidget *parent, const QStringList &);
	~KCMLiblicense();

	void load();
	void save();
	void defaults();

private:
	LicenseChooser *licenseChooser;
};

#endif
