/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from komposetaskmgrdcopiface.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "./komposetaskmgrdcopiface.h"

#include <kdatastream.h>


static const char* const KomposeTaskMgrDcopIface_ftable[5][3] = {
    { "void", "createDefaultView()", "createDefaultView()" },
    { "void", "createVirtualDesktopView()", "createVirtualDesktopView()" },
    { "void", "createWorldView()", "createWorldView()" },
    { "void", "closeCurrentView()", "closeCurrentView()" },
    { 0, 0, 0 }
};
static const int KomposeTaskMgrDcopIface_ftable_hiddens[4] = {
    0,
    0,
    0,
    0,
};

bool KomposeTaskMgrDcopIface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    if ( fun == KomposeTaskMgrDcopIface_ftable[0][1] ) { // void createDefaultView()
	replyType = KomposeTaskMgrDcopIface_ftable[0][0]; 
	createDefaultView( );
    } else if ( fun == KomposeTaskMgrDcopIface_ftable[1][1] ) { // void createVirtualDesktopView()
	replyType = KomposeTaskMgrDcopIface_ftable[1][0]; 
	createVirtualDesktopView( );
    } else if ( fun == KomposeTaskMgrDcopIface_ftable[2][1] ) { // void createWorldView()
	replyType = KomposeTaskMgrDcopIface_ftable[2][0]; 
	createWorldView( );
    } else if ( fun == KomposeTaskMgrDcopIface_ftable[3][1] ) { // void closeCurrentView()
	replyType = KomposeTaskMgrDcopIface_ftable[3][0]; 
	closeCurrentView( );
    } else {
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return true;
}

QCStringList KomposeTaskMgrDcopIface::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "KomposeTaskMgrDcopIface";
    return ifaces;
}

QCStringList KomposeTaskMgrDcopIface::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; KomposeTaskMgrDcopIface_ftable[i][2]; i++ ) {
	if (KomposeTaskMgrDcopIface_ftable_hiddens[i])
	    continue;
	QCString func = KomposeTaskMgrDcopIface_ftable[i][0];
	func += ' ';
	func += KomposeTaskMgrDcopIface_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


