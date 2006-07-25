static const char *RcsId = "$Header: /users/chaize/newsvn/cvsroot/Instrumentation/Rontec/src/main.cpp,v 1.2 2006-07-24 14:48:18 tithub Exp $";
//+=============================================================================
//
// file :        main.cpp
//
// description : C++ source for a TANGO device server main.
//               The main rule is to initialise (and create) the Tango
//               system and to create the DServerClass singleton.
//               The main should be the same for every Tango device server.
//
// project :     TANGO Device Server
//
// $Author: tithub $
//
// $Revision: 1.2 $ $
//
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2005/09/30 12:13:33  syldup
// initial import
//
// Revision 1.1  2005/09/09 15:38:21  coquet
// initial import, tested with real hardware.
// Raw version, a lot of functions are not implemented ( hard-coded ) but DS is usable
//
//
// copyleft :    European Synchrotron Radiation Facility
//               BP 220, Grenoble 38043
//               FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================

#include <tango.h>


int main(int argc,char *argv[])
{

	Tango::Util *tg;
	try
	{
		// Initialise the device server
		//----------------------------------------
		tg = Tango::Util::init(argc,argv);

		// Create the device server singleton 
		//	which will create everything
		//----------------------------------------
		tg->server_init(false);

		// Run the endless loop
		//----------------------------------------
		cout << "Ready to accept request" << endl;
		tg->server_run();
	}
	catch (bad_alloc)
	{
		cout << "Can't allocate memory to store device object !!!" << endl;
		cout << "Exiting" << endl;
	}
	catch (CORBA::Exception &e)
	{
		Tango::Except::print_exception(e);
		
		cout << "Received a CORBA_Exception" << endl;
		cout << "Exiting" << endl;
	}
	tg->server_cleanup();
	return(0);
}