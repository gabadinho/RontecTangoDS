static const char *RcsId = "$Header: /users/chaize/newsvn/cvsroot/Instrumentation/Rontec/src/main.cpp,v 1.5 2010-10-06 21:41:17 vince_soleil Exp $";
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
// $Author: vince_soleil $
//
// $Revision: 1.5 $ $
//
// $Log: not supported by cvs2svn $
// Revision 1.4  2007/06/05 14:38:30  dhaussy
// * corrected a bug in energyMode attribute write
//
// Revision 1.3  2006/08/31 15:51:10  tithub
// * Les temps sont exprim�s en seconde au lieu de millisecondes
// * La commande GetPartOfSpectrum renvoie une partie du spectre lu si le thread est running, ou lit une partie du spectre sur le Rontec sinon
// * La commande ClearData arr�te le thread de lecture
// * Attributs StartingChannel et EndingChannel m�moris�s
//
// Revision 1.2  2006/07/24 14:48:18  tithub
// Nouvelle interface Tango
//
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

#if defined(ENABLE_CRASH_REPORT)
# include <crashreporting/crash_report.h>
#else
# define DECLARE_CRASH_HANDLER
# define INSTALL_CRASH_HANDLER
#endif

DECLARE_CRASH_HANDLER;

int main(int argc, char *argv[])
{
  INSTALL_CRASH_HANDLER;


	Tango::Util *tg = 0;
	try
	{
		// Initialise the device server
		//----------------------------------------
		tg = Tango::Util::init(argc,argv);

		//cout << "ds_Rontec.exe version 060831c" << endl;
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
