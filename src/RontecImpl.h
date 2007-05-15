#ifndef _INCLUDE_RONTEC_IMPL_H
#define _INCLUDE_RONTEC_IMPL_H

#include <tango.h>
#include <DeviceProxyHelper.h>

class RontecImpl : public Tango::LogAdapter {
	friend class RontecThread;
public:
	RontecImpl(Tango::DeviceImpl* dev);
	~RontecImpl();

	void init(string proxy_name = "",/*unsigned long baud = 38400,short timeout = 1000,*/long read_size = 256) throw (Tango::DevFailed);

// resets the RONTEC controller,	command $##
// Tango::DevFailed exception if RONTEC returns an error
	Tango::ConstDevString reset() throw (Tango::DevFailed);

// clears the acquisition memory buffer in the RONTEC command $CC
// Tango::DevFailed exception if RONTEC returns an error
	void clear(void) throw (Tango::DevFailed);

// gets the input ( seen by the amplifier ) count rate from the hard	command $BC
// Tango::DevFailed exception if RONTEC returns an error
	double get_input_count_rate(void) throw (Tango::DevFailed);

// gets the output ( processed ) count rate from the hard  command $NC
// Tango::DevFailed exception if RONTEC returns an error
	double get_output_count_rate(void) throw (Tango::DevFailed);

// returns the dead time ( the % of events not processed )
// dead time = (input count rate - output count rate ) / input count rate
	double get_dead_time(void) throw (Tango::DevFailed);

// gets the cycle time for input and output count rate from the hard command $TC
// Tango::DevFailed exception if RONTEC returns an error
	double get_cycle_time(void) throw (Tango::DevFailed);

// sets the cycle time for input and output count rate from the hard command $CT
// Tango::DevFailed exception if RONTEC returns an error
	void set_cycle_time(double time) throw (Tango::DevFailed);

// returns the remaining acquisition time  in real time mode
// Tango::DevFailed exception if RONTEC returns an error command $MR
	double get_remaining_acquisition_real_time(void) throw (Tango::DevFailed);

// returns the elapsed acquisition time  in real time mode
// Tango::DevFailed exception if RONTEC returns an error command $MR
	double get_elapsed_acquisition_real_time(void) throw (Tango::DevFailed);

// returns the elapsed acquisition time  in live time mode
// Tango::DevFailed exception if RONTEC returns an error command $MR
	double get_elapsed_acquisition_live_time(void) throw (Tango::DevFailed);

// returns the temperature of the sensor in celsius degrees command $DT
// Tango::DevFailed exception if RONTEC returns an error
	double get_detector_temperature(void) throw (Tango::DevFailed);

// set the filter setting ( processor )  command $SF
// value can be from 0 to 3 : from max cps to min cps
// Tango::DevFailed exception if value out of range
	void set_filter_setting( long  value ) throw (Tango::DevFailed);

// get the filter setting ( processor )  command $FF
// value can be from 0 to 3 : from max cps to min cps
// Tango::DevFailed exception if value out of range
	long get_filter_setting(void) throw (Tango::DevFailed);

// retreives the energy configured command $FE
// Tango::DevFailed exception if RONTEC returns an error
	long retreive_energy_range() throw (Tango::DevFailed);

// sets the energy configured command $SE
// possible input values : 10000, 20000, 40000, 80000 with rounding to lower value
	void set_energy_range(long ) throw (Tango::DevFailed);

// retreives the offset and gain configured command $FC
// Tango::DevFailed exception if RONTEC returns an error
// arguments offset and gain are passed as reference
	void retreive_offset_gain(long &offs, long &gain) throw (Tango::DevFailed);

// ======== SPECTRUM ACQUISITION METHODS ================
// configure acquisition time in real time mode  command $SM
// argin : acquisition time in second
// Tango::DevFailed exception if RONTEC returns an error
	void configure_acquisition_mode(long format, long mode) throw (Tango::DevFailed);

// configure acquisition reading properties :
// these attributes are the global attributes of the acquisition
// the	get_spectrum_data() method will read a part of the configured spectrum
// start channel		: the 1st channel to be acquired
// step_width 			: read the all the channel modulo step_width
// summation_number : add the channels
// max_data 				: maximum of data to be read
	void configure_spectrum_reading_properties( long start_ch, long step_w, long summation_num, long max_d ) throw (Tango::DevFailed);

// gets configured acquisition reading properties :
// returns
// start channel		: the 1st channel to be acquired
// step_width 			: read the channel modulo step_width
// summation_number : add the channels
// max_data 				: maximum of data to be read
	void get_spectrum_reading_properties( long &start_ch, long &step_w, long &summation_num, long &max_d ) throw (Tango::DevFailed);


// configures the starting channel for reading spectrum
	void set_spectrum_reading_first_channel(Tango::DevLong start) throw (Tango::DevFailed);

// configures the last channel for reading spectrum
	void set_spectrum_reading_last_channel(Tango::DevLong end) throw (Tango::DevFailed);

// Set acquisition time in real time mode and start acquisition command $MT
// time : acquisition time in second
// live : the acquisition is in live time (true) or in real time (false)
// start_reading_thread : start the reading (true) or not (false)
// Tango::DevFailed exception if RONTEC returns an error
	void start_acquisition(double time,bool live = false,bool start_reading_thread = true) throw (Tango::DevFailed);

// read partial spectrum beginning at begin for length channel and write them in buffertac
// Taco DS like code
	 void read_spectrum(unsigned long* data,long begin, long length) throw (Tango::DevFailed);

// Pauses the acquisition
	void pause(void) throw (Tango::DevFailed);

// resumes (restart the paused ) acquisition
	void resume(void) throw (Tango::DevFailed);

// get the pause status
// returns RUNNING/PAUSE
	std::string get_pause_status(void) throw (Tango::DevFailed);

// get_partial spectrum 
// returns : pointer on data read back
	long get_spectrum(unsigned long* dest,long begin, long length) throw (Tango::DevFailed);

	long get_read_spectrum_first_channel();
	long get_read_spectrum_length();

// sends the command to the serial line device
// Tango::DevFailed exception if RONTEC returns an error
	std::string ascii_command(std::string cmd) throw (Tango::DevFailed);

// sends the command to the serial line device and return the reponse values as long
// Tango::DevFailed exception if RONTEC returns an error
	long long_command(std::string cmd) throw (Tango::DevFailed);

// controls if response is an error response,
// if error throws exception
// Tango::DevFailed exception if RONTEC returns an error
	void scan_response(std::string resp) throw (Tango::DevFailed);

// ===================== ROI METHODS ========================
//	 roi_get_parameters( long ttl_num,					TTL output number
//											long &atomic_number 	 the atomic number configured in the RONTEC
//											std::string &name 		 atomic name as configured in the RONTEC
//											long &low_channel 		 low channel configured
//											long &high_channel		 high channel configured
// reads the ROI of the TTL output ttl_num,
// returns the values read in the RONTEC in the references
	void roi_get_parameters(long &ttl_num,					 // argin/argout : TTL output number
												 long &atomic_number, 		// argout : the atomic number configured in the RONTEC
												 std::string &name, 			// argout : atomic name as configured in the RONTEC
												 long &low_channel, 			// argout : low channel configured
												 long &high_channel) throw (Tango::DevFailed); 		// argout : high channel configured

//	 roi_set_parameters( long ttl_num,					TTL output number
//											long atomic_number		the atomic number configured in the RONTEC
//											std::string name			atomic name as configured in the RONTEC
//											long low_channel			low channel configured
//											long high_channel 		high channel configured
// writes the ROI of the TTL output ttl_num,
	void roi_set_parameters(long ttl_num, 					// argin/argout : TTL output number
												 long atomic_number,		 // argout : the atomic number configured in the RONTEC
												 std::string name,			 // argout : atomic name as configured in the RONTEC
												 long low_channel,			 // argout : low channel configured
												 long high_channel) throw (Tango::DevFailed);		 // argout : high channel configured

//	roi_get_count(long ttl_num)
// returns the count of pulses on the selected ttl output

	double roi_get_count(long ttl_num) throw (Tango::DevFailed);

//  return true if the reading thread is running
	bool is_reading_thread_running();

protected:
	Tango::DeviceImpl* _dev;
	Tango::DeviceProxyHelper* _proxy;
	RontecThread* _reading_thread;
	omni_mutex _proxy_mutex;

	std::string _proxy_name;
//	unsigned long _baud;
//	short _timeout;

	long _min_index;
	long _max_index;
	long _length;
	long _read_first_index;
	long _read_last_index;
	long _read_size;
	long _summation_number;
	long _step_width;
};

class RontecThread : public omni_thread, public Tango::LogAdapter {
	friend class RontecImpl;
public:
	RontecThread(RontecImpl* impl);
	void go();
	void abort();
	bool is_running() { return _running; }
	//long get_spectrum(unsigned long* &dest);
	long get_spectrum(unsigned long* dest,long begin, long length);
protected:
	RontecImpl* _impl;
	bool _go_on;
	bool _running;
	omni_mutex _spectrum_mutex;
	omni_mutex _wait_mutex;
	omni_condition _wait_cond;
	Tango::DevErrorList _error_list;
	unsigned long* _data;
	long _first_index;
	long _last_index;
	long _length;
private:
	void* run_undetached(void* arg);
};

#endif //_INCLUDE_RONTEC_IMPL_H
