//				RONTEC MCA CHANNEL ANALYSER IMPLEMENTATION CLASS
//
//				file	: RontecMcaImplementation.cpp
//				provides the hardware interface for an "abstract" MCA device server
//				to a real RONTEC MCA analyser
//
//
//				sends /receveives the real commands
//
//
//
//
//
//


#include <RontecMcaImplementation.h>
#include <XString.h>
#include <math.h>


// constructor
RontecMcaImplementation::RontecMcaImplementation(
											Tango::DeviceImpl* dev, 				 // for log adapter
											Tango::DeviceProxyHelper * sl,	 // device proxy on the serial line

											Tango::DevLong start_chan,
											Tango::DevLong end_chan
											) : Tango::LogAdapter(dev)
{

	DEBUG_STREAM << "RontecMcaImplementation::RontecMcaImplementation(): entering... !" << endl;
	std::cout << "RontecMcaImplementation::RontecMcaImplementation(): entering... !" << endl;

// initialise internal variables with call values
	_sl = sl; 																		 // the serial line device proxy helper class
	this->_start_channel = start_chan;
	if( _start_channel < 0 )
		_start_channel = 0;

	this->_end_channel		 = end_chan;
	this->rontec_number_of_channels = end_chan;
	this->_step_width 		 = 1;
	this->_summation_number = 1;
	this->_max_data 			 = this->_end_channel - this->_start_channel;
}

// Destructor
RontecMcaImplementation::~RontecMcaImplementation()
{
}


//+------------------------------------------------------------------
/**
 *	method: RontecMcaImplementation::reset
 *
 *	description:	method to execute "Reset"
 *
 * @return	string returend by the rontec
 *
 */
//+------------------------------------------------------------------
Tango::ConstDevString RontecMcaImplementation::reset()
{
	//	POGO has generated a method core with argout allocation.
	//	If you would like to use a static reference without copying,
	//	See "TANGO Device Server Programmer's Manual"
	//		(chapter : Writing a TANGO DS / Exchanging data)
	//------------------------------------------------------------
	DEBUG_STREAM << "RontecMcaImplementation::reset(): entering... !" << endl;
	//	Add your own code to control device here
	std::string cmd("$##");
	std::string response =	this->ascii_command(cmd);
	// retreive the real response (startup of the program )
	DEBUG_STREAM << "RontecMcaImplementation::reset(): before DevSerReadLine" << endl;
	Tango::DevString resp;
	_sl->command_out("DevSerReadLine",resp);
	Tango::ConstDevString argout = CORBA::string_dup(resp);
	return argout;

}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::clear
 *
 *	description:	method to execute the command CC
 *	clears the acquisition memory
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::clear()
{
	DEBUG_STREAM << "RontecMcaImplementation::clear(): entering... !" << endl;
	//	Add your own code to control device here
	// we do not wait for any response
	ascii_command(std::string("$CC"));
}


//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_input_count_rate
 *
 *	description:	method to execute the command BC
 *	returns the input count rate
 *
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::get_input_count_rate(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_input_count_rate() entering... "<< endl;
	std::string resp = ascii_command(std::string("$BC"));
	long val = XString<long>::convertFromString(resp.substr(4));
	return val;
}
//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_output_count_rate
 *
 *	description:	method to execute the command NC
 *	returns the output count rate
 *
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::get_output_count_rate(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_output_count_rate() entering... "<< endl;
	std::string resp = ascii_command(std::string("$NC"));
	long val = XString<long>::convertFromString(resp.substr(4));
	return val;
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_dead_time
 *
 *	description:	returns the input count rate - output count rate / input count rate
 *	returns the % of non-processed input counts
 *
 *
 */
//+------------------------------------------------------------------
float RontecMcaImplementation::get_dead_time(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_dead_time() entering... "<< endl;
	long icr = get_input_count_rate();
	if( icr == 0)
	{
		return 0.0;
	}
	long ocr = get_output_count_rate();
	// MODIF PATRICK LE 28/06/2005
//	float val = ( (static_cast<float>(icr) - static_cast<float>(ocr)) / static_cast<float>(icr) );
	float val = (1.0 - ((static_cast<float>(ocr)) / static_cast<float>(icr) )) * 100.0;
	return val;
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_cycle_time
 *
 *	description:	method to execute the command NC
 *	returns the output count rate
 *
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::get_cycle_time(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_cycle_time() entering... "<< endl;
	std::string resp = ascii_command(std::string("$TC"));
	long val = XString<long>::convertFromString(resp.substr(4));
	return val;
}



//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_remaining_acquisition_real_time
 *
 *	description:	method to execute the command MR
 *	returns the remainig acquisition time ( real time mode ) in ms
 *
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::get_remaining_acquisition_real_time(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_remaining_acquisition_real_time() entering... "<< endl;
	std::string resp = ascii_command(std::string("$MR"));
	long val = XString<long>::convertFromString(resp.substr(4));
	return val;
}
//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_elapsed_acquisition_real_time
 *
 *	description:	method to execute the command MR
 *	returns the remainig acquisition time ( real time mode ) in ms
 *
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::get_elapsed_acquisition_real_time(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_elapsed_acquisition_real_time() entering... "<< endl;
	std::string resp = ascii_command(std::string("$MS"));
	long val = XString<long>::convertFromString(resp.substr(4));
	return val;
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_detector_temperature
 *
 *	description:	method to execute the command DT
 *	returns the detector temperature in Celsius degrees
 *
 *
 */
//+------------------------------------------------------------------
float RontecMcaImplementation::get_detector_temperature(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_output_count_rate() entering... "<< endl;
	std::string resp = ascii_command(std::string("$DT"));
	long val = 0;
	float f = 0.0;
	val = XString<long>::convertFromString(resp.substr(4));
	f = static_cast<float>(val);
	f/=10.0;
	return f;
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_filter_setting
 *
 *	description:	method to execute the command FF
 *	returns filter setting ( processor ) configured
 *	value can be from 0 to 3 : from max cps to min cps
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::get_filter_setting(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_filter_setting() entering... "<< endl;
	std::string resp = ascii_command(std::string("$FF"));
	long val = XString<long>::convertFromString(resp.substr(4));
	return val;
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::set_filter_setting
 *
 *	description:	method to execute the command SF
 *	returns filter setting ( processor ) configured
 *	value can be from 0 to 3 : from max cps to min cps
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::set_filter_setting(long value)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_filter_setting() entering... "<< endl;
	// value must be in 0 1 2 3
	if( (value < 0) || (value > 3) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::set_filter_setting() processor number must be in [0..3] " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" processor number must be in [0..3]",
				(const char *)"RontecMcaImplementation::set_filter_setting()");
	}
	std::stringstream cmd;
	cmd << "$SF " << value;
	std::string resp = ascii_command( cmd.str() );
	// exception raised in scan_response if processor number is invalid
}




//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::retreive_energy_range()
 *
 *	description:	retreives fron the RONTEC the maximum energy configured
 *	writes the energy in internal variable hw_max_energy
 *
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::retreive_energy_range()
{
	DEBUG_STREAM << "RontecMcaImplementation::retreive_energy_range(): entering... !" << endl;
	//	Add your own code to control device here
	// query energy range
	std::string energy_str = this->ascii_command(std::string("$FE"));
	long energy_number = XString<long>::convertFromString(energy_str.substr(4));
	long hw_max_energy = 0;

	switch ( energy_number )
	{
		case 0 :
			hw_max_energy = 10000;		 // 10 KeV ( raw 10000)
		break;
		case 1 :
			hw_max_energy = 20000;
		break;
		case 2 :
			hw_max_energy = 40000;
		break;
		case 3 :
			hw_max_energy = 80000;
		break;
		default :
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::retreive_energy_range() energy range not documented " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" energy range not documented",
				(const char *)"RontecMcaImplementation::retreive_energy_range()");
	}
	return hw_max_energy;
}
//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::retreive_energy_range()
 *
 *	description:	sets the energy configured command $SE
 *	possible input values : 10000, 20000, 40000, 80000 with rounding to lower value
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::set_energy_range(long energy)
{
	DEBUG_STREAM << "RontecMcaImplementation::set_energy_range(): entering... !" << endl;
	std::stringstream o;
	o << "$SE";
	if(energy < 20000)
	{ o <<" 0";}
	else if(energy < 40000)
	{o <<" 1";}
	else if(energy < 80000)
	{o <<" 2";}
	else
	{o <<" 3";}
	 this->ascii_command(o.str());


}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::retreive_offset_gain()
 *
 *	description:	retreives fron the RONTEC the maximum energy configured
 *	writes the energy in internal variable hw_offset and hw_gain
 *	the argouts offset and gain are passed by reference
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::retreive_offset_gain(long &offs, long &gain)
{
	DEBUG_STREAM << "RontecMcaImplementation::retreive_offset_gain(): entering... !" << endl;
	//	Add your own code to control device here
	// query offset gain on the hw
	std::string og_str = ascii_command(std::string("$FC"));

	// parse the string
	// response is	: !FC offset gain<CR>
	std::string::size_type begidx, endidx;
	begidx = og_str.find_first_of(" ");
	endidx = og_str.find_last_of(" ");
//	std::cout << "Rontec::retreive_offset_gain return: " << og_str << endl << " 		begidx " <<  begidx << "	endidx " << endidx << "  npos " << std::string::npos << endl;

	if( (begidx == std::string::npos) || (endidx == std::string::npos) || (begidx == endidx) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::retreive_offset_gain() invalid formed response " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid formed response ",
				(const char *)"RontecMcaImplementation::retreive_offset_gain()");
	}
	offs = XString<long>::convertFromString(og_str.substr(begidx,endidx));
	gain = XString<long>::convertFromString(og_str.substr(endidx));
}



//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::configure_acquisition
 *
 *	description:	method to execute the command SM
 *	set the format and the mode for spectrum acquisition
 *	format : [2..4] see documentation
 *	mode 0..1 see documentation
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::configure_acquisition_mode(long format, long mode)
{
	DEBUG_STREAM << "RontecMcaImplementation::configure_acquisition() entering... "<< endl;
	// value must be in 0 1 2 3
	if( (format < 2) || (format > 4) || (mode < 0) || (mode > 1) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::configure_acquisition() invalid arguments " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid arguments",
				(const char *)"RontecMcaImplementation::configure_acquisition()");
	}
	std::stringstream cmd;
	cmd << "$SM " << format << " " << mode ;
	ascii_command(cmd.str());
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::configure_acquisition
 *
 *	description:	method to execute the command SM
 *	set the format and the mode for spectrum acquisition with default values
 *	format = 4 ( 4 bytes/channel, as long int ( 32 bits )
 *	mode	 = 0 ( accumulate spectrum )
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::configure_acquisition_mode(void)
{
	this->configure_acquisition_mode(4,0);
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::configure_spectrum_reading_properties
 *
 *	description:	configurations for the command get_spectrum, command SS
 *	set the parameters for reading spectrum
 * start_channel		: the 1st channel to be acquired
 * step_width 			: read the all the channel modulo step_width
 * summation_number : add the channels
 * max_data 				: maximum of data to be read
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::configure_spectrum_reading_properties( long start_ch, long step_w, long summation_num, long max_d )
{
	DEBUG_STREAM << "RontecMcaImplementation::configure_spectrum_reading_properties() entering... "<< endl;
	if(  (start_ch > 4096) || (start_ch < 0) || (step_w > 4096) || (step_w < 0) || (summation_num > 4096) || (summation_num < 0) || (max_d < 0) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::configure_spectrum_reading_properties() invalid arguments " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid arguments",
				(const char *)"RontecMcaImplementation::configure_spectrum_reading_properties()");
	}
	this->_start_channel		=  start_ch;
	this->_step_width 			=  step_w;
	this->_summation_number =  summation_num;
	this->_max_data 				=  max_d;
}


//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::set_spectrum_reading_first_channel
 *
 *	description:	configurations for the command get_spectrum, command SS
 *	set the parameters for reading spectrum
 * start		: the 1st channel to be acquired
 *
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::set_spectrum_reading_first_channel(Tango::DevLong start)
{
	DEBUG_STREAM << "RontecMcaImplementation::set_spectrum_reading_first_channel() entering... "<< endl;
	if( start < 1 )
		start = 1;
	if(start > this->_end_channel)
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::set_spectrum_reading_first_channel() invalid argument " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid argument",
				(const char *)"RontecMcaImplementation::set_spectrum_reading_first_channel()");
	}
	this->_start_channel = start;
//	std::cout << "		_start_channel set to " << this->_start_channel << endl;
}
//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::set_spectrum_reading_last_channel
 *
 *	description:	configurations for the command get_spectrum, command SS
 *	set the parameters for reading spectrum
 * end		: the 1st channel to be acquired
 *
 *
 *
 */
//+------------------------------------------------------------------
	void RontecMcaImplementation::set_spectrum_reading_last_channel(Tango::DevLong end)
{
	DEBUG_STREAM << "RontecMcaImplementation::set_spectrum_reading_last_channel() entering... "<< endl;
	if(end > rontec_number_of_channels)
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::set_spectrum_reading_last_channel() invalid argument " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid argument",
				(const char *)"RontecMcaImplementation::set_spectrum_reading_last_channel()");
	}

	else if( end < this->_start_channel )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::set_spectrum_reading_last_channel() invalid argument " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid argument",
				(const char *)"RontecMcaImplementation::set_spectrum_reading_last_channel()");
	}
	this->_end_channel = end;
//	std::cout << "		_end_channel set to " << this->_end_channel << endl;

}


//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_spectrum_reading_properties
 *
 *	description:	returns configurations for the command get_spectrum, command SS
 *	get back	the parameters for reading spectrum
 * start_chan 	 : the 1st channel to be acquired
 * step_w 			 : read the all the channel modulo step_width
 * summation_num : add the channels
 * max_d				 : maximum of data to be read
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::get_spectrum_reading_properties( long &start_ch, long &step_w, long &summation_num, long &end_chan )
{
	DEBUG_STREAM << "RontecMcaImplementation::get_spectrum_reading_properties() entering... "<< endl;
	DEBUG_STREAM << "_start_channel=" << this->_start_channel << "		end_channel=" << this->_end_channel << endl;

	 start_ch 		 = this->_start_channel ;
	 step_w 			 = this->_step_width ;
	 summation_num = this->_summation_number ;
	 end_chan 		 = this->_end_channel ;
}


//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::start_acquisition
 *
 *	description:	method to execute the command MT
 *	uses class attributes : start_channel, step_width, summation_number , maxdata
 *	starts acquisition for the time specified in seconds
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::start_acquisition(float time)
{
	DEBUG_STREAM << "RontecMcaImplementation::start_acquisition() entering... "<< endl;
	long t ;
	if(time <= 0.001)
	{
		t = 0;
	}
	else
	{
		t = static_cast<long>( (time * 1000.0) );
	}
	std::stringstream cmd;
	cmd << "$MT " << t ;
	this->ascii_command(cmd.str());
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::pause
 *
 *	description:	method to execute the command MP
 *	pauses the acquisition
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::pause(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::pause() entering... "<< endl;
	ascii_command(std::string("$MP ON"));
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::resume
 *
 *	description:	method to execute the command MP
 *	resumes previously paused acquisition
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::resume(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::pause() entering... "<< endl;
	ascii_command(std::string("$MP NO"));
}

//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::get_pause_status
 *
 *	description:	method to execute the command FP
 *	returns the pause state RUNNING/PAUSE
 *
 *
 */
//+------------------------------------------------------------------
std::string RontecMcaImplementation::get_pause_status(void)
{
	DEBUG_STREAM << "RontecMcaImplementation::pause() entering... "<< endl;
	std::string resp = ascii_command(std::string("$FP"));
	if(resp[4] == '+')
		resp = "PAUSE";
	else
		resp = "RUNNING";
	return resp;
}

//+------------------------------------------------------------------
/**
 *	method: RontecMcaImplementation::get_spectrum_data_2
 *
 *	description:	method to execute read partial spectrum on RONTEC
 *	Taco like code
 *	argin : the channel number beginning of spectrum to read
 *					the length to read
 *					reference on the long buffer for data
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::get_spectrum_data_2(long begin, long length, unsigned long *buffer)
{
	 #define SL_NCHAR 		 1		 /* character read/write mode */
	DEBUG_STREAM << "RontecMcaImplementation::get_spectrum_data_2(): entering... !" << endl;
	if ( (begin < _start_channel) || (begin > _end_channel) )
	{
	 ERROR_STREAM << "RontecMcaImplementation::get_spectrum_data_2(): error on start_channel" << endl;
		begin = _start_channel;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" start channel not in start/end channel	",
				(const char *)"RontecMcaImplementation::get_spectrum_data_2()");
	}
	if (begin + length > _end_channel)
	{
	 ERROR_STREAM << "RontecMcaImplementation::get_spectrum_data_2(): error on length" << endl;
		length = _end_channel - begin;
	}

	std::stringstream cmd;
	cmd << "$SS " << begin << " " << this->_step_width << " " << this->_summation_number << " " << length << '\r';

// on envoie la commande $SS et on lit le retour
	try
	{
		std::string resp = ascii_command(cmd.str());
//		std::cout << " get_spectrum_data_2 : retour $SS " << resp << endl;
	}
	catch(...)
	{
		FATAL_STREAM	<< "SOFTWARE_ERROR RontecMcaImplementation::get_spectrum_data_2() : error on Serial::DevSerWriteString call 	" << endl;
		Tango::Except::throw_exception (
				(const char *)"UNKNOWN_ERROR",
				(const char *)" error on ascii_command call",
				(const char *)"RontecMcaImplementation::get_spectrum_data_2()");
	}
	// on recupere les data!
	Tango::DevVarCharArray dvca;
	dvca.length(length * 4 );
//	std::cout << " taille dvca : " << dvca.length() << endl;


	// pointeur de reception des char
	Tango::DevVarCharArray * data;
	int j = 0;
	int nboctetslus= 0;
	for(int i = 0; i < ((int)length * 4) ; i += nboctetslus)
	{
		unsigned long nb_to_read = 4 * length - i;
		// Limitation à la lecture de 2048 octets en une fois. 
		//Sinon DevSerReadChar sort en timeout ( si nb octets > 3600)
		if(nb_to_read > 2048 ) nb_to_read = 2048;
		long l_arg = (SL_NCHAR + (nb_to_read << 8));
		_sl->command_inout("DevSerReadChar",l_arg,data);
		nboctetslus = data->length();

//		std::cout << "lecture reussie de " << data->length() << " chars sur le RONTEC i = " << i << endl;
		// copier les data recues dans dvca
		for(int k = 0; k < data->length(); k++, j++)
		{
//			std::cout << "k="<<k<<" 		j=" << j << endl;
			dvca[j] = (*data)[k];
		}
	}
//	std::cout << " taille dvca finale : " << dvca.length() << endl;

	try
	{
		int max = dvca.length() ;
		j = 0;
		for(int i = 0; i < max && j < length; i += 4, j++)
		{
//			std::cout << j << std::endl;

			buffer[j]  = (unsigned long)dvca[i+3];
			buffer[j] += (unsigned long)dvca[i+2]* 0x100;
			buffer[j] += (unsigned long)dvca[i+1]* 0x10000;
			buffer[j] += (unsigned long)dvca[i]  * 0x1000000;
				}
	}
	catch(...)
	{
		ERROR_STREAM << "OUT_OF_MEMORY RontecMcaImplementation::get_spectrum_data_2() error copying data " << endl;
		Tango::Except::throw_exception (
				(const char *)"OUT_OF_MEMORY",
				(const char *)" error copying data ",
				(const char *)"RontecMcaImplementation::get_spectrum_data_2()");
	}


}
//+------------------------------------------------------------------
/**
 *	method: RontecMcaImplementation::get_spectrum_data
 *
 *	description:	method to execute read partial spectrum on
 *	argin : the channel number beginning of spectrum to read
 *					the length to read
 *					reference on the long buffer for data
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::get_spectrum_data(long begin, long length, long *buffer)
{
	DEBUG_STREAM << "RontecMcaImplementation::get_spectrum_data(): entering... !" << endl;
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	// throw exception?
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	if ( (begin < _start_channel) || (begin > _end_channel) )
	{
	 ERROR_STREAM << "RontecMcaImplementation::get_spectrum_data(): error on start_channel" << endl;
		begin = _start_channel;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" start channel not in start/end channel	",
				(const char *)"RontecMcaImplementation::get_spectrum_data()");
	}
	if (begin + length > _end_channel)
	{
	 ERROR_STREAM << "RontecMcaImplementation::get_spectrum_data(): error on length" << endl;
		length = _end_channel - begin;
	}

	_sl->command_in("DevSerFlush", static_cast<Tango::DevLong>(2) );

	std::stringstream cmd;
	cmd << "$SS " << begin << " " << this->_step_width << " " << this->_summation_number << " " << length << '\r';
//	std::cout << " command :	" << cmd.str() << endl;

//	std::string response =	this->ascii_command(cmd.str());

// essai autrement
	Tango::DevLong lenwritten = 0;
	try
	{
		_sl->command_inout("DevSerWriteString", cmd.str().c_str(), lenwritten );
	}
	catch(...)
	{
		FATAL_STREAM	<< "SOFTWARE_ERROR RontecMcaImplementation::get_spectrum_data() : error on Serial::DevSerWriteString call 	" << endl;
		Tango::Except::throw_exception (
				(const char *)"UNKNOWN_ERROR",
				(const char *)" error on Serial::DevSerWriteString call",
				(const char *)"RontecMcaImplementation::get_spectrum_data()");
	}

//	std::cout << " get_spectrum_data : number of chars written : " << lenwritten << endl;
// sleep 200 ms to let RONTEC answer
	// compute the sleep time
	long time_ms = 200 + ( length / 4 );
	long time_sec = time_ms / 1000;
	long time_nanosec = ( ( time_ms % 1000) * 1000000);
	omni_thread::sleep(time_sec,time_nanosec);

	// how much chars in buffer?
	Tango::DevLong number = 0;
	_sl->command_out("DevSerGetNChar", number);
//	std::cout << " get_spectrum_data : number of chars in buffer : " << number << endl;

Tango::DevVarCharArray *dvca;
Tango::DeviceData dd_in, dd_out;
	try
	{

		dd_in <<	 number;
//		std::cout << " before  _sl->command_inout ->DevSerReadCharArray "  << endl;
		dd_out = _sl->get_device_proxy()->command_inout("DevSerReadNBinData",dd_in );
//	_sl->command_inout("DevSerReadCharArray",static_cast<Tango::DevLong>( len_in_chars ), dvca );
//		std::cout << " AFTER	_sl->command_inout ->DevSerReadCharArray "	<< endl;
		dd_out >> dvca;
	}
	catch(Tango::DevFailed &e)
	{
//		std::cout << " in catch Tango::DevFailed _sl->command_inout ->DevSerReadCharArray "  << endl;
		FATAL_STREAM	<< "SOFTWARE_ERROR RontecMcaImplementation::get_spectrum_data() : error on Serial::DevSerReadCharArray call 	" << endl;
		Tango::Except::re_throw_exception (e,
				(const char *)"SOFTWARE_ERROR",
				(const char *)" error on Serial::DevSerReadCharArray call",
				(const char *)"RontecMcaImplementation::get_spectrum_data()");
	}

	catch(...)
	{
//		std::cout << " in catch  _sl->command_inout ->DevSerReadCharArray "  << endl;
		FATAL_STREAM	<< "SOFTWARE_ERROR RontecMcaImplementation::get_spectrum_data() : error on Serial::DevSerReadCharArray call 	" << endl;
		Tango::Except::throw_exception (
				(const char *)"UNKNOWN_ERROR",
				(const char *)" error on Serial::DevSerReadCharArray call",
				(const char *)"RontecMcaImplementation::get_spectrum_data()");
	}

//	std::cout << " get_spectrum_data : after DevSerReadCharArray : "	<< endl;
	// debug : print out the char values
//	std::cout << " get_spectrum_data : data size " << dvca->length() << endl;

//	for(int j = 0; j < dvca->length() ; j++)
//	{
//		if((j %16 ) == 0) printf("\n");
//		printf(" %2.2X", (unsigned char)(*dvca)[j]);
//	}
//	printf("\n");

	// retreive the real response : the data following the response header
	// TODO : copy data in the buffer

	try
	{
//		unsigned char * base_addr = dvca->get_buffer();
//std::cout << " after dvca->get_buffer " << std::endl;
		int max = dvca->length() - 4;
		int j = 0;
		for(int i = 4; i < max && j < length; i += 4, j++)
		{
//			std::cout << j << std::endl;

			buffer[j]  = (unsigned long)(*dvca)[i+3];
			buffer[j] += (unsigned long)((*dvca)[i+2]* 0x100);
			buffer[j] += (unsigned long)((*dvca)[i+1]* 0x10000);
			buffer[j] += (unsigned long)((*dvca)[i] * 0x1000000);

/*			buffer[j]  = *(base_addr+i+3);
			buffer[j] += *(base_addr+i+2) * 0x100;
			buffer[j] += *(base_addr+i+1) * 0x10000;
			buffer[j] += *(base_addr+i) * 0x1000000;			*/
		}
//		std::cout << " fin copie " << std::endl;
	}
	catch(...)
	{
		ERROR_STREAM	<< "OUT_OF_MEMORY RontecMcaImplementation::get_spectrum_data() error copying data " << endl;
		Tango::Except::throw_exception (
				(const char *)"OUT_OF_MEMORY",
				(const char *)" error copying data ",
				(const char *)"RontecMcaImplementation::get_spectrum_data()");
	}
}


//+------------------------------------------------------------------
/**
 *	method: Rontec::roi_get_parameters
 *
 *	description:	method to execute "GetROIParameters"
 *	returns the ROI output channel number parameters
 *	returns
 *	the atomic number configured in the RONTEC
 *	atomic name as configured in the RONTEC
 *	low channel configured
 *	high channel configured
 *	according to the hardware parameters : max channel energy, offset an gain calibration
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::roi_get_parameters(long & ttl_num,					// argin/argout : TTL output number
																								long &atomic_number,		 // argout : the atomic number configured in the RONTEC
																								std::string &name,			 // argout : atomic name as configured in the RONTEC
																								long &low_channel,			 // argout : low channel configured
																								long &high_channel) 		 // argout : high channel configured
{
	DEBUG_STREAM << "RontecMcaImplementation::roi_get_parameters(): entering... !" << endl;

	//	Add your own code to control device here

	if(( ttl_num < 1 ) || ( ttl_num > 8 ) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::roi_get_parameters() TTL output out of range 1..8 " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" TTL output out of range 1..8",
				(const char *)"RontecMcaImplementation::roi_get_parameters()");
	}

	// retreive max energy in hw units ( in eV )
	long energy = this->retreive_energy_range();
	// query offset/gain parameters in hw units
	long hw_offset, hw_gain;
	this->retreive_offset_gain(hw_offset,hw_gain);
	if(hw_gain <= 0)
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::roi_get_parameters() gain <= 0 " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" gain <= 0",
				(const char *)"RontecMcaImplementation::roi_get_parameters()");
	}

	double gain = static_cast<double>(hw_gain);
	double offset = static_cast<double>(hw_offset);

	std::stringstream cmd;
	cmd << "$GK " << ttl_num ;

	std::string resp = ascii_command(cmd.str());
	INFO_STREAM << "RontecMcaImplementation::roi_get_parameters() RONTEC raw return : " << resp << endl;
	// response form : !GK channel number atomic number atomic name low_energy high_energy
	// example !GK 1 33 As 8000 8500

	// the TTL output number is not returned by RONTEC
	// ttl_num = XString<Tango::DevLong>::convertFromString(resp.substr(4,1));

	std::string::size_type begidx, endidx;
	// find the name
	begidx = resp.find_first_of(" ", 6);
	endidx = resp.find_first_of(" ", (begidx +1));
	name = resp.substr(begidx, endidx);

	// the high energy
	endidx = resp.find_last_of(" ");
	double high_energy = XString<double>::convertFromString(resp.substr(endidx));
	double chan =  ( ((high_energy * 10000)/gain) + (offset * 0.01) );
	// MODIF PATRICK G. Le 20/06/2005
	// Permet un arrondi correct
	chan += 0.5;
	high_channel = static_cast<long>(chan);
	// the low energy
	begidx = resp.find_last_of(" ", (endidx-1));
	double low_energy = XString<double>::convertFromString( resp.substr(begidx, endidx ));
	chan =	( ((low_energy * 10000)/hw_gain) + (hw_offset * 0.01) );
	// MODIF PATRICK G. Le 20/06/2005
	// Permet un arrondi correct
	chan += 0.5;
	low_channel = static_cast<Tango::DevLong>(chan);
}

//+------------------------------------------------------------------
/**
 *	method: Rontec::roi_set_parameters
 *
 *	description:	method to execute the command $SK
 *	sets the ROI ttl output channel number parameters
 *	argin :
 *	the atomic number configured in the RONTEC
 *	atomic name as configured in the RONTEC
 *	low channel configured
 *	high channel configured
 *	according to the hardware parameters : max channel energy, offset an gain calibration
 *
 *
 */
//+------------------------------------------------------------------
	void RontecMcaImplementation::roi_set_parameters(long ttl_num,					// argin/argout : TTL output number
												 long atomic_number,		 // argout : the atomic number configured in the RONTEC
												 std::string name,			 // argout : atomic name as configured in the RONTEC
												 long low_channel,			 // argout : low channel configured
												 long high_channel) 		 // argout : high channel configured

{
	DEBUG_STREAM << "RontecMcaImplementation::roi_set_parameters(): entering... !" << endl;
	if(( ttl_num < 1 ) || ( ttl_num > 8 ) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecMcaImplementation::roi_get_parameters() TTL output out of range 1..8 " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" TTL output out of range 1..8",
				(const char *)"RontecMcaImplementation::roi_get_parameters()");
	}

	// compute low_energy and high_energy from channel
	long energy = this->retreive_energy_range();
	long hw_offset, hw_gain;
	this->retreive_offset_gain(hw_offset,hw_gain);
	double gain 	= static_cast<double>(hw_gain);
	double offset = static_cast<double>(hw_offset);
	double low_energy = (0.0001 * ( static_cast<double>(low_channel) - (0.01 * offset)) * gain);
	double high_energy = (0.0001 * ( static_cast<double>(high_channel) - (0.01 * offset)) * gain);

	// MODIF PATRICK G. Le 20/06/2005
	// Permet un arrondi correct
	low_energy += 0.5;
	high_energy += 0.5;

	std::stringstream cmd;
//	cmd << "$SK " << ttl_num << " " << atomic_number << " " << name << " " <<  static_cast<long>(low_energy) << " " << static_cast<long>(high_energy) ;
	cmd << "$SK " << ttl_num << " " << atomic_number << " " << name << " " <<  static_cast<long>(low_energy) << " " << static_cast<long>(high_energy) ;
	INFO_STREAM << "RontecMcaImplementation::roi_set_parameters() command sent to RONTEC : " << cmd.str() << endl;
	ascii_command(cmd.str());
}

//+------------------------------------------------------------------
/**
 *	method: Rontec::roi_get_count
 *
 *	description:	method to get counts on the ttl output
 *	returns the ROI output channel number counts
 *
 */
//+------------------------------------------------------------------
long RontecMcaImplementation::roi_get_count(long ttl_num)
{
	DEBUG_STREAM << "RontecMcaImplementation::roi_get_count(): entering... !" << endl;

	std::stringstream cmd;
	cmd << "$GR " << ttl_num ;
	std::string count_str = ascii_command(cmd.str());
	long count_num = XString<long>::convertFromString(count_str.substr(4));
	return count_num;

}


//+------------------------------------------------------------------
/**
 *	internal method: RontecMcaImplementation::ascii_command
 *
 *	description:	send/reseive an ascii command
 *	argin : the command to be sent
 *	argout : the response if response is correct
 *	exception if RONTEC returned an error
 */
//+------------------------------------------------------------------
std::string RontecMcaImplementation::ascii_command(std::string cmd)
{
	DEBUG_STREAM << "RontecMcaImplementation::ascii_command(): entering... !" << endl;
	DEBUG_STREAM << "RontecMcaImplementation::ascii_command(): for command : " << cmd << endl;

	//	Add your own code to control device here
	// communication with RONTEC through serial line device server
	_sl->command_in("DevSerFlush", static_cast<Tango::DevLong>(2));
	#define SL_LINE 			2 		/* line read mode */
	// serial line is hard to code, see serial line device server!
	cmd = cmd + '\r';
	Tango::DevVarLongStringArray dvlsa ;
	dvlsa.svalue.length(1);
	dvlsa.svalue[0] = CORBA::string_dup(cmd.c_str());
	dvlsa.lvalue.length(1);
	dvlsa.lvalue[0] = SL_LINE;

	Tango::DevString resp;
	_sl->command_inout("WriteRead",dvlsa,resp);
	std::string response(resp);
	if(response.size() <= 0)
	{
		response = "!ERROR RONTEC NE REPOND PAS A LA COMMANDE ";
	}
	scan_response(response);

	return response;
}

//+------------------------------------------------------------------
/**
 *	internal method: McaDataReader::scan_response
 *
 *	description:	scans the RONTEC response
 *	throws exception if response is an error
 *
 *
 */
//+------------------------------------------------------------------
void RontecMcaImplementation::scan_response(std::string resp)
{
	DEBUG_STREAM << "RontecMcaImplementation::scan_response(): entering... !" << endl;

	//	Add your own code to control device here
	// first letters are !ERR
	if ( (resp[1] == 69) && (resp[2] == 82) && (resp[3] == 82) )
	{
		int ret_value = XString<int>::convertFromString(resp.substr(8));
		std::string err;
		switch(ret_value)
		{
			case 0:
				err = "General error or buffer overflow";
			break;
			case 1:
				err = "Unknown command";
			break;
			case 2:
				err = "Numeric Parameter expected";
			break;
			case 4:
				err = "Boolean parameter expected";
			break;
			case 5:
				err = "Additional parameter expected";
			break;
			case 6:
				err = "Unexpected parameter or character";
			break;
			case 7:
				err = "Illegal numeric value";
			break;
			case 8:
				err = "Unknown subcommand";
			break;
			case 9:
				err = "Function not implemented or no hardware support";
			break;
			case 10:
				err = "Flash EPROM programming fault";
			break;
			case 11:
				err = "Error clearing Flash EPROM";
			break;
			case 12:
				err = "Flash EPROM read error";
			break;
			case 13:
				err = "Hardware error";
			break;
			case 16:
				err = "Illegal baud rate";
			break;
			default:
				err = "non documented error";
			break;
		}
		ERROR_STREAM	<< "OPERATION_NOT_ALLOWED RontecMcaImplementation::scan_response(): error returned : " << err << endl;
		Tango::Except::throw_exception (
				(const char *)"OPERATION_NOT_ALLOWED",
				(const char *)err.c_str(),
				(const char *)"RontecMcaImplementation::scan_response()");

	}
}

//+------------------------------------------------------------------
/**
 *	internal method: McaDataReader::SendRontecMessage
 *
 *	sends a direct command to the RONTEC with its arguments,
 *	assuming a perfect knowledge of RCL 2.2
 *	returns the answer of the RONTEC
 *
 */
//+------------------------------------------------------------------
// sends a direct command to the RONTEC with its arguments,
// assuming a perfect knowledge of RCL 2.2
// returns the answer of the RONTEC
std::string RontecMcaImplementation::SendRontecMessage( std::string cmd )
{
	std::string argout = ascii_command(cmd);
	return argout ;
}

