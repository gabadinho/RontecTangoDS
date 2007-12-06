#include "RontecImpl.h"

//////////////////////////////////////////////////////////////////
// RontecImpl class implementation

#define SL_LINE 		2 		/* line read mode */
#define SL_NCHAR 		1		/* character read/write mode */

RontecImpl::RontecImpl(Tango::DeviceImpl* dev) 
: Tango::LogAdapter(dev) {
	_dev = dev;
	_proxy = 0;
	_reading_thread = 0;

	_proxy_name = "UNDEFINED PROXY NAME";
//	_baud = 38400;
//	_timeout = 1000;

	_min_index = 0;
	_max_index = 4095;
	_length = _max_index - _min_index + 1;
	_read_first_index = _min_index;
	_read_last_index = _max_index;
	_read_size = 256;
	_summation_number = 1;
	_step_width = 1;
}

RontecImpl::~RontecImpl() {
	if(_proxy) {
		delete _proxy;
		_proxy = 0;
	}
	if(_reading_thread) {
		_reading_thread->abort();
		_reading_thread = 0;
	}
}

void RontecImpl::init(string proxy_name,/*unsigned long baud,short timeout,*/long read_size) throw(Tango::DevFailed) {
	_read_size = read_size;
	if(proxy_name!="") {
		_proxy_name = proxy_name;
//		_baud = baud;
//		_timeout = timeout;
	}
	//- critical section
	{
		omni_mutex_lock proxy_lock(_proxy_mutex);
		if( _proxy )
		{
			delete _proxy;
			_proxy = 0;
		}
		_proxy = new Tango::DeviceProxyHelper(_proxy_name,_dev);
		/*
		_proxy->command_in("DevSerSetBaudrate", static_cast<Tango::DevULong>(_baud));
		*/
		_proxy->command_in("DevSerSetNewline", static_cast<Tango::DevShort>(13));
		/*
		_proxy->command_in("DevSerSetTimeout", static_cast<Tango::DevShort>(_timeout));
		*/
	} //- end critical section 

	//- caution pause() locks the _proxy_mutex!
	//- deadlock under linux, strangely, it worked under W32
	pause();
}



Tango::ConstDevString RontecImpl::reset() throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::reset(): entering... !" << endl;
	if(_proxy) init();
	std::string resp;
	{
		omni_mutex_lock proxy_lock(_proxy_mutex);
		// communication with RONTEC through serial line device server
		ascii_command("$##");
		_proxy->command_out("DevSerReadLine",resp);
	}
	return CORBA::string_dup(resp.c_str());
}

void RontecImpl::clear() throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::clear(): entering... !" << endl;
	// stop reading thread
	if(_reading_thread) {
		_reading_thread->abort();
		_reading_thread = 0;
	}
	// Rontec clear
	ascii_command("$CC");
}

double RontecImpl::get_input_count_rate(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_input_count_rate() entering... "<< endl;
	long cycle_time = get_cycle_time();
	double val = long_command("$BC");
	if(cycle_time>0)
		return val/cycle_time;
	else
		return 0;
}

double RontecImpl::get_output_count_rate(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_output_count_rate() entering... "<< endl;
	long cycle_time = get_cycle_time();
	double val = long_command("$NC");
	if(cycle_time>0)
		return val/cycle_time;
	else
		return 0;
}

double RontecImpl::get_dead_time(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_dead_time() entering... "<< endl;
	long icr = get_input_count_rate();
	if( icr == 0) {
		return 0.0;
	}
	long ocr = get_output_count_rate();
	// MODIF PATRICK LE 28/06/2005
//	float val = ( (static_cast<float>(icr) - static_cast<float>(ocr)) / static_cast<float>(icr) );
	double val = (1.0 - ((static_cast<float>(ocr)) / static_cast<float>(icr) )) * 100.0;
	return val;
}

double RontecImpl::get_cycle_time(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_cycle_time() entering... "<< endl;
	double cycle = long_command("$TC");
	if(cycle<=0) {
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::set_cycle_time() cycle time must be >0" << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" cycle time must be >0",
				(const char *)"RontecImpl::set_cycle_time()");
	}
	return cycle/1000.0;
}

void RontecImpl::set_cycle_time(double cycle) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_cycle_time() entering... "<< endl;
	if(cycle<=0) {
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::set_cycle_time() cycle time must be >0" << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" cycle time must be >0",
				(const char *)"RontecImpl::set_cycle_time()");
	}
	std::ostringstream cmd;
	cmd << "$CT " << (long) (cycle*1000.0);
	std::string resp = ascii_command(cmd.str());
	// exception raised in scan_response if processor number is invalid
}

double RontecImpl::get_remaining_acquisition_real_time(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_remaining_acquisition_real_time() entering... "<< endl;
	double val = long_command("$MR");
	return val/1000.0;
}

double RontecImpl::get_elapsed_acquisition_real_time(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_elapsed_acquisition_real_time() entering... "<< endl;
	if(!is_reading_thread_running()) {
		// forces time update when the spectrum is not read
		ascii_command("$SS 0 1 1 0");
	}
	double val = long_command("$MS");
	return val/1000.0;
}

double RontecImpl::get_elapsed_acquisition_live_time(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_elapsed_acquisition_live_time() entering... "<< endl;
	if(!is_reading_thread_running()) {
		// forces time update when the spectrum is not read
		ascii_command("$SS 0 1 1 0");
	}
	double val = long_command("$LS");
	return val/1000.0;
}

double RontecImpl::get_detector_temperature(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_output_count_rate() entering... "<< endl;
	double val = long_command("$DT");
	return val/10.0;
}

long RontecImpl::get_filter_setting(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_filter_setting() entering... "<< endl;
	long val = long_command("$FF");
	return val;
}

void RontecImpl::set_filter_setting(long value) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_filter_setting() entering... "<< endl;
	// value must be in 0 1 2 3
	if( (value < 0) || (value > 3) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::set_filter_setting() processor number must be in [0..3] " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" processor number must be in [0..3]",
				(const char *)"RontecImpl::set_filter_setting()");
	}
	std::ostringstream cmd;
	cmd << "$SF " << value;
	ascii_command(cmd.str());
	// exception raised in scan_response if processor number is invalid
}

long RontecImpl::retreive_energy_range() throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::retreive_energy_range(): entering... !" << endl;
	//	Add your own code to control device here
	// query energy range
	long energy_number = long_command("$FE");
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
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::retreive_energy_range() energy range not documented " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" energy range not documented",
				(const char *)"RontecImpl::retreive_energy_range()");
	}
	return hw_max_energy;
}

void RontecImpl::set_energy_range(long energy) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::set_energy_range(): entering... !" << endl;
	std::ostringstream cmd;
	cmd << "$SE ";
	if(energy < 20000) { 
		cmd <<"0";
	} 
	else if(energy < 40000) {
		cmd <<"1";
	} 
	else if(energy < 80000) {
		cmd <<"2";
	}
	else {
		cmd <<"3";
	}
	ascii_command(cmd.str());
}

void RontecImpl::retreive_offset_gain(long &offs, long &gain) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::retreive_offset_gain(): entering... !" << endl;
	//	Add your own code to control device here
	// query offset gain on the hw
	std::string og_str = ascii_command("$FC");

	std::string dummy;
	istringstream parser(og_str);
	parser >> dummy >> offs >> gain;
}

void RontecImpl::configure_acquisition_mode(long format, long mode) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::configure_acquisition() entering... "<< endl;
	// value must be in 0 1 2 3
	if( (format < 2) || (format > 4) || (mode < 0) || (mode > 1) ) {
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::configure_acquisition() invalid arguments " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid arguments",
				(const char *)"RontecImpl::configure_acquisition()");
	}
	std::ostringstream cmd;
	cmd << "$SM " << format << " " << mode ;
	ascii_command(cmd.str());
}

void RontecImpl::configure_spectrum_reading_properties( long start_ch, long step_w, long summation_num, long max_d ) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::configure_spectrum_reading_properties() entering... "<< endl;
	if(  (start_ch > _max_index) || (start_ch < _min_index) || (step_w > _max_index) || (step_w < 0) || (summation_num > _max_index) || (summation_num < 0) || (max_d < 0) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::configure_spectrum_reading_properties() invalid arguments " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid arguments",
				(const char *)"RontecImpl::configure_spectrum_reading_properties()");
	}
	_read_first_index		=  start_ch;
	_step_width 			=  step_w;
	_summation_number		=  summation_num;
	_read_last_index		=  max_d;
}

void RontecImpl::set_spectrum_reading_first_channel(Tango::DevLong start) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::set_spectrum_reading_first_channel() entering... "<< endl;
	if( start < 0 )
		start = 0;
	if(start > _max_index || start > _read_last_index)
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::set_spectrum_reading_first_channel() invalid argument " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid argument",
				(const char *)"RontecImpl::set_spectrum_reading_first_channel()");
	}
	_read_first_index = start;
//	std::cout << "		_start_channel set to " << this->_start_channel << endl;
}

void RontecImpl::set_spectrum_reading_last_channel(Tango::DevLong end) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::set_spectrum_reading_last_channel() entering... "<< endl;
	//if(end > rontec_number_of_channels)
	if(end > _max_index || end < _read_first_index)
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::set_spectrum_reading_last_channel() invalid argument " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" invalid argument",
				(const char *)"RontecImpl::set_spectrum_reading_last_channel()");
	}
	_read_last_index = end;
//	std::cout << "		_end_channel set to " << this->_end_channel << endl;
}

void RontecImpl::get_spectrum_reading_properties( long &start_ch, long &step_w, long &summation_num, long &end_chan ) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::get_spectrum_reading_properties() entering... "<< endl;
	//DEBUG_STREAM << "_start_channel=" << this->_start_channel << "		end_channel=" << this->_end_channel << endl;

	 start_ch		= _read_first_index;
	 step_w			= _step_width ;
	 summation_num	= _summation_number ;
	 end_chan		= _read_last_index;
}

long RontecImpl::get_spectrum(unsigned long* dest,long begin, long length) throw(Tango::DevFailed) {
	if(!_reading_thread) {
		ERROR_STREAM << "RontecImpl::get_spectrum(): reading thread not started." << endl;
		Tango::Except::throw_exception (
				(const char *)"OPERATION_NOT_ALLOWED",
				(const char *)" reading thread not started.",
				(const char *)"RontecImpl::get_spectrum()");

	} 
	return _reading_thread->get_spectrum(dest,begin,length);
}

void RontecImpl::start_acquisition(double time,bool live,bool start_reading_thread) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::start_acquisition() entering... "<< endl;

	// configure RONTEC :
	// format : data will be returned as 4-bytes integers
	// mode 	: rontec sends accumulated spectrum ( do not clear after transmit )
	configure_acquisition_mode(4,0);

	// start acquisition
	long t ;
	if(time <= 0.001) {
		t = 0;
	}
	else {
		//t = static_cast<long>( (time * 1000.0) );
		t = static_cast<long>( time * 1000.0 );
	}
	std::ostringstream cmd;
	if(!live) {
		// start acquisition, real time
		cmd << "$MT " << t ;
	} else {
		// start acquisition, live time
		cmd << "$LT " << t ;
	}
	this->ascii_command(cmd.str());
	
	// start reading thread
	if(_reading_thread) {
		_reading_thread->abort();
		_reading_thread = 0;
	}

	if(start_reading_thread) {
		_reading_thread = new RontecThread(this);
		_reading_thread->go();
	}
}

void RontecImpl::pause(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::pause() entering... "<< endl;
	ascii_command("$MP ON");

if(_reading_thread &&_reading_thread->is_running())
   _reading_thread->pause();


}

void RontecImpl::resume(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::pause() entering... "<< endl;
	ascii_command("$MP NO");
}

std::string RontecImpl::get_pause_status(void) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::pause() entering... "<< endl;
	std::string resp = ascii_command("$FP");
	if(resp[4] == '+')
		resp = "PAUSE";
	else
		resp = "RUNNING";
	return resp;
}

void RontecImpl::roi_get_parameters(	long & ttl_num,		// argin/argout : TTL output number
										long &atomic_number,// argout : the atomic number configured in the RONTEC
										std::string &name,	// argout : atomic name as configured in the RONTEC
										long &low_channel,	// argout : low channel configured
										long &high_channel) // argout : high channel configured
throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::roi_get_parameters(): entering... !" << endl;

	//	Add your own code to control device here

	if(( ttl_num < 1 ) || ( ttl_num > 8 ) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::roi_get_parameters() TTL output out of range 1..8 " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" TTL output out of range 1..8",
				(const char *)"RontecImpl::roi_get_parameters()");
	}

	// retreive max energy in hw units ( in eV )
	long energy = retreive_energy_range();
	// query offset/gain parameters in hw units
	long hw_offset, hw_gain;
	retreive_offset_gain(hw_offset,hw_gain);
	if(hw_gain <= 0)
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::roi_get_parameters() gain <= 0 " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" gain <= 0",
				(const char *)"RontecImpl::roi_get_parameters()");
	}

	double gain = static_cast<double>(hw_gain);
	double offset = static_cast<double>(hw_offset);

	std::ostringstream cmd;
	cmd << "$GK " << ttl_num ;

	std::string resp = ascii_command(cmd.str());
	INFO_STREAM << "RontecImpl::roi_get_parameters() RONTEC raw return : " << resp << endl;
	// response form : !GK atomic number atomic name low_energy high_energy
	// example !GK 33 As 8000 8500

	// the TTL output number is not returned by RONTEC
	// ttl_num = XString<Tango::DevLong>::convertFromString(resp.substr(4,1));
	std::string mnemo;
	long high_energy = 0.0;
	long low_energy = 0.0;
	std::string dummy;
	istringstream parser(resp);
	parser >> mnemo >> atomic_number >> name >> low_energy >> high_energy;
	double chan = 0.0;

	// the low energy
	if(low_energy!=65535) {
		chan =	( ((low_energy * 10000)/hw_gain) + (hw_offset * 0.01) );
		// MODIF PATRICK G. Le 20/06/2005
		// Permet un arrondi correct
		chan += 0.5;
		low_channel = static_cast<Tango::DevLong>(chan);
	} 
	else {
		low_channel = -1;
	}
	// the high energy
	if(high_energy!=65535) {
		chan =  ( ((high_energy * 10000)/gain) + (offset * 0.01) );
		// MODIF PATRICK G. Le 20/06/2005
		// Permet un arrondi correct
		chan += 0.5;
		high_channel = static_cast<long>(chan);
	} 
	else {
		high_channel = -1;
	}

}

void RontecImpl::roi_set_parameters(	long ttl_num,		// argin/argout : TTL output number
										long atomic_number,	// argout : the atomic number configured in the RONTEC
										std::string name,	// argout : atomic name as configured in the RONTEC
										long low_channel,	// argout : low channel configured
										long high_channel) 	// argout : high channel configured

throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::roi_set_parameters(): entering... !" << endl;
	if(( ttl_num < 1 ) || ( ttl_num > 8 ) )
	{
		ERROR_STREAM	<< "DATA_OUT_OF_RANGE RontecImpl::roi_get_parameters() TTL output out of range 1..8 " << endl;
		Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)" TTL output out of range 1..8",
				(const char *)"RontecImpl::roi_get_parameters()");
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

	std::ostringstream cmd;
//	cmd << "$SK " << ttl_num << " " << atomic_number << " " << name << " " <<  static_cast<long>(low_energy) << " " << static_cast<long>(high_energy) ;
	cmd << "$SK " << ttl_num << " " << atomic_number << " " << name << " " <<  static_cast<long>(low_energy) << " " << static_cast<long>(high_energy) ;
	INFO_STREAM << "RontecImpl::roi_set_parameters() command sent to RONTEC : " << cmd.str() << endl;
	ascii_command(cmd.str());
}

double RontecImpl::roi_get_count(long ttl_num) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::roi_get_count(): entering... !" << endl;
	double cycle_time = get_cycle_time();
	std::ostringstream cmd;
	cmd << "$GR " << ttl_num ;
	double count_num = long_command(cmd.str());
	if(cycle_time>0)
		return count_num / cycle_time;
	else
		return 0;
}

std::string RontecImpl::ascii_command(std::string cmd) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::ascii_command() : " << cmd << endl;
	if(!_proxy) init();

	//if(!_proxy) Tango::Except::throw_exception ((const char *)"OPERATION_NOT_ALLOWED",(const char *)"Serial line device proxy not initialized !",(const char *)"RontecImpl::ascii_command()");
	// serial line is hard to code, see serial line device server!
	cmd = cmd + '\r';
	Tango::DevVarLongStringArray dvlsa ;
	dvlsa.svalue.length(1);
	dvlsa.svalue[0] = CORBA::string_dup(cmd.c_str());
	dvlsa.lvalue.length(1);
	dvlsa.lvalue[0] = SL_LINE;
	std::string resp;
	{
		omni_mutex_lock proxy_lock(_proxy_mutex);
		try {
			// communication with RONTEC through serial line device server
			_proxy->command_in("DevSerFlush", static_cast<Tango::DevLong>(2));
			//std::cout << "write : " << cmd << std::endl;
			_proxy->command_inout("WriteRead",dvlsa,resp);
			//std::cout << "read : " << resp << std::endl;
		} 
		catch(Tango::DevFailed &df) {
			// force proxy init
			delete _proxy;
			_proxy = 0;
			Tango::Except::re_throw_exception(df,
				(const char *)"COMMUNICATION_BROKEN",
				(const char *)"Error while sending command.",
				(const char *)"RontecImpl::ascii_command()"
				);
		}
	}
	std::string response(resp);
	if(response.size() <= 0) {
		Tango::Except::throw_exception ((const char *)"COMMUNICATION_BROKEN",(const char *)"The Rontec device did not respond !",(const char *)"RontecImpl::ascii_command()");
	}
	scan_response(response);
	return response;
}

long RontecImpl::long_command(std::string cmd) throw(Tango::DevFailed) {
	long val = 0;
	std::string resp = ascii_command(cmd);
	std::string dummy;
	std::istringstream parser(resp);
	parser >> dummy >> val;
	return val;
}

void RontecImpl::scan_response(std::string resp) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::scan_response(): entering... !" << endl;

	//	Add your own code to control device here
	// first letters are !ERR
	if ( (resp[0] == '!') && (resp[1] == 'E') && (resp[2] == 'R') && (resp[3] == 'R') )
	{
		int ret_value = 0;
		std::istringstream(resp.substr(8)) >> ret_value;
		std::string err;
		switch(ret_value)
		{
			case 0:
				err = "Rontec error 0 : General error or buffer overflow";
				break;
			case 1:
				err = "Rontec error 1 : Unknown command";
				break;
			case 2:
				err = "Rontec error 2 : Numeric Parameter expected";
				break;
			case 4:
				err = "Rontec error 4 : Boolean parameter expected";
				break;
			case 5:
				err = "Rontec error 5 : Additional parameter expected";
				break;
			case 6:
				err = "Rontec error 6 : Unexpected parameter or character";
				break;
			case 7:
				err = "Rontec error 7 : Illegal numeric value";
				break;
			case 8:
				err = "Rontec error 8 : Unknown subcommand";
				break;
			case 9:
				err = "Rontec error 9 : Function not implemented or no hardware support";
				break;
			case 10:
				err = "Rontec error 10 : Flash EPROM programming fault";
				break;
			case 11:
				err = "Rontec error 11 : Error clearing Flash EPROM";
				break;
			case 12:
				err = "Rontec error 12 : Flash EPROM read error";
				break;
			case 13:
				err = "Rontec error 13 : Hardware error";
				break;
			case 16:
				err = "Rontec error 16 : Illegal baud rate";
				break;
			default:
				err = "Rontec error "+resp.substr(8)+" : non documented error";
		}
		ERROR_STREAM	<< "HARDWARE_FAILURE RontecImpl::scan_response(): error returned : " << err << endl;
		Tango::Except::throw_exception (
				(const char *)"HARDWARE_FAILURE",
				(const char *)err.c_str(),
				(const char *)"RontecImpl::scan_response()");
	}
}

void RontecImpl::read_spectrum(unsigned long* data,long begin, long length) throw(Tango::DevFailed)
{
	DEBUG_STREAM << "RontecImpl::read_spectrum(): entering... !" << endl;
	if(!_proxy) init();
	//if(!_proxy) Tango::Except::throw_exception ((const char *)"OPERATION_NOT_ALLOWED",(const char *)"Serial line device proxy not initialized !",(const char *)"RontecImpl::ascii_command()");

	omni_mutex_lock proxy_lock(_proxy_mutex);
	//Tango::DevVarCharArray dvca;
	//dvca.length(length * 4 );
	std::ostringstream cmd;
	cmd << "$SS " << begin << " " << this->_step_width << " " << this->_summation_number << " " << length << '\r';
	// on envoie la commande $SS et on lit le retour
	std::string resp = ascii_command(cmd.str());
	// on recupere les buff!
	const Tango::DevVarCharArray* buff;
	long i=0;
	while(i<length) {
		long nboctetslus= 0;
		long nb_to_read = 4 * (length - i);
		// Limitation à la lecture de 2048 octets en une fois. 
		//Sinon DevSerReadChar sort en timeout ( si nb octets > 3600)
		if(nb_to_read > 2048 ) nb_to_read = 2048;
		long l_arg = (SL_NCHAR + (nb_to_read << 8));
		_proxy->command_inout("DevSerReadChar",l_arg,buff);
		nboctetslus = buff->length();
		{
			for(long j=0; j<nboctetslus && i<length; j+=4,i++) {
				data[i]  = (unsigned long)(*buff)[j]   << 24;
				data[i] += (unsigned long)(*buff)[j+1] << 16;
				data[i] += (unsigned long)(*buff)[j+2] << 8;
				data[i] += (unsigned long)(*buff)[j+3];
			}
		}
	}
}

bool RontecImpl::is_reading_thread_running() {
	return _reading_thread && _reading_thread->is_running();
}

long RontecImpl::get_read_spectrum_first_channel() {
	long first = 0;
	if(_reading_thread) {
		first = _reading_thread->_first_index;
	}
	return first;
}

long RontecImpl::get_read_spectrum_length() {
	long len = 0;
	if(_reading_thread) {
		len = _reading_thread->_last_index - _reading_thread->_first_index;
	}
	return len;
}

//////////////////////////////////////////////////////////////////
// RontecThread class implementation

RontecThread::RontecThread(RontecImpl* impl) 
: omni_thread((void*) impl,omni_thread::PRIORITY_NORMAL),
Tango::LogAdapter(impl->_dev),
_wait_cond(&_wait_mutex) {
	_impl = impl;
	_go_on = false;
	_running = false;
	_force_pause_thread = false;
	_first_index = _impl->_read_first_index;
	_last_index = _impl->_read_last_index;
	_length = _last_index - _first_index + 1;
	_data = new unsigned long[_length];
	for(long i=0; i<_length; i++) {
		_data[i] = 0;
	}
}

void RontecThread::go() {
	_go_on = true;
	_force_pause_thread = false;
	start_undetached();
}

void RontecThread::abort() {
	_go_on = false;
	_wait_cond.signal();
	unsigned long* data = _data;
	join(0);
	// !!! here the RontecThread object is deleted
	delete [] data;
}


long RontecThread::get_spectrum(unsigned long* dest,long begin, long length) {
	long i = 0;
	if(_error_list.length()>0) {
		throw Tango::DevFailed(_error_list);
	}
	else {
		omni_mutex_lock spectrum_lock(_spectrum_mutex);
		if(begin<_first_index && begin+length>_last_index) {
			Tango::Except::throw_exception (
				(const char *)"DATA_OUT_OF_RANGE",
				(const char *)"begin or length not valid",
				(const char *)"RontecThread::get_spectrum()");
		} 
		else {
			for(i=0; i<length; i++) {
				dest[i] = _data[(begin - _first_index)+i];
			}
		}
	}
	return i;
}

void* RontecThread::run_undetached(void* arg) {
	_running = true;
	long index = _first_index;
	long size = 0;
	long wait_ms = 125;
	long stop_index = -1;
	while(_go_on) {
		try {
			// try to stop immediately the thread 
			if (_force_pause_thread)
				break;

			if(stop_index<0 && _impl->get_pause_status()=="PAUSE") {
				stop_index = index;
				INFO_STREAM << "Last spectrum read..." << endl;
			}
			size = _last_index - index + 1;
			if(size > _impl->_read_size)
				size = _impl->_read_size;

			INFO_STREAM << "Read spectrum : " << index << " " << index + size - 1 << endl;

			/*
			unsigned long* tmp_data = new unsigned long[size];
			_impl->read_spectrum(tmp_data, index, size);
			{
				omni_mutex_lock spectrum_lock(_spectrum_mutex);
				for(long i=0; i<size; i++) {
					_data[i+(index - _first_index)] = tmp_data[i];
				}
			}
			delete [] tmp_data;
			*/
			{
				omni_mutex_lock spectrum_lock(_spectrum_mutex);
				_impl->read_spectrum(_data + index - _first_index, index, size);
			}

			index += size;
			if(index > _last_index) {
				index = _first_index;
			}
			if(stop_index>=0 && stop_index==index) {
				break;
			}
			unsigned long next_s,next_ns;
			omni_thread::get_time(&next_s,&next_ns,wait_ms / 1000,(wait_ms % 1000) * 1000000);
			_wait_mutex.lock();
			_wait_cond.timedwait(next_s,next_ns);

		} catch(Tango::DevFailed& df) {
			_go_on = false;
			_error_list.length(df.errors.length());
			for(long i = 0; i < df.errors.length(); i++)
			{
				_error_list[i].reason = CORBA::string_dup(df.errors[i].reason);
				_error_list[i].desc = CORBA::string_dup(df.errors[i].desc);
				_error_list[i].origin = CORBA::string_dup(df.errors[i].origin);
				_error_list[i].severity = df.errors[i].severity;
			}
		} catch(...) {
			_go_on = false;
			_error_list.length(1);
			_error_list[0].reason = CORBA::string_dup("UNKNOWN_ERROR");
			_error_list[0].desc = CORBA::string_dup("unknown error trying to read PLCServer ");
			_error_list[0].origin = CORBA::string_dup("readingthread::run_undetached ");
			_error_list[0].severity = Tango::ERR;
		}
	}
	_running = false;
	return 0;
}
