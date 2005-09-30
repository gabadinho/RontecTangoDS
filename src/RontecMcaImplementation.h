//				RONTEC MCA CHANNEL ANALYSER IMPLEMENTATION CLASS
//
//				file	: RontecMcaImplementation.h
//				provides the hardware interface for an "abstract" MCA device server
//				sends /receveives the real commands
//
//
//
//
//
//
//
//
//

#ifndef  __RONTEC_MCA_IMPLEMENTATION_H__
#define  __RONTEC_MCA_IMPLEMENTATION_H__

#include <tango.h>
#include <DeviceProxyHelper.h>


class RontecMcaImplementation : public Tango::LogAdapter
{
private :
	// class attributes
	Tango::DeviceProxyHelper *_sl;
	long rontec_number_of_channels;
	// for spectrum reading configuration :
	long _start_channel;
	long _end_channel;
	long _step_width;
	long _summation_number;
	long _max_data;



// constructor
public:
	RontecMcaImplementation(Tango::DeviceImpl* dev, 				 // for log adapter
													Tango::DeviceProxyHelper * ds_,  // device proxy on the serial line
													Tango::DevLong start_chan,
													Tango::DevLong end_chan
													);

// destructor
	virtual ~RontecMcaImplementation(void);


// sends a direct command to the RONTEC with its arguments,
// assumuing a perfect knowledge of RCL 2.2
// returns the answer of the RONTEC
	std::string SendRontecMessage( std::string cmd );

// resets the RONTEC controller,	command $##
// Tango::DevFailed exception if RONTEC returns an error
	Tango::ConstDevString reset();

// clears the acquisition memory buffer in the RONTEC command $CC
// Tango::DevFailed exception if RONTEC returns an error
	void clear(void);

// gets the input ( seen by the amplifier ) count rate from the hard	command $BC
// Tango::DevFailed exception if RONTEC returns an error
	long get_input_count_rate(void);

// gets the output ( processed ) count rate from the hard  command $NC
// Tango::DevFailed exception if RONTEC returns an error
	long get_output_count_rate(void);

// returns the dead time ( the % of events not processed )
// dead time = (input count rate - output count rate ) / input count rate
	float get_dead_time(void);

// gets the cycle time for input and output count rate from the hard command $TC
// Tango::DevFailed exception if RONTEC returns an error
	long get_cycle_time(void);

// returns the remaining acquisition time  in real time mode
// Tango::DevFailed exception if RONTEC returns an error command $MR
	long get_remaining_acquisition_real_time(void);

// returns the elapsed acquisition time  in real time mode
// Tango::DevFailed exception if RONTEC returns an error command $MR
	long get_elapsed_acquisition_real_time(void);

// returns the temperature of the sensor in celsius degrees command $DT
// Tango::DevFailed exception if RONTEC returns an error
	float get_detector_temperature(void);

// set the filter setting ( processor )  command $SF
// value can be from 0 to 3 : from max cps to min cps
// Tango::DevFailed exception if value out of range
	void set_filter_setting( long  value );

// get the filter setting ( processor )  command $FF
// value can be from 0 to 3 : from max cps to min cps
// Tango::DevFailed exception if value out of range
	long get_filter_setting(void);

// retreives the energy configured command $FE
// Tango::DevFailed exception if RONTEC returns an error
	long retreive_energy_range();

// sets the energy configured command $SE
// possible input values : 10000, 20000, 40000, 80000 with rounding to lower value
	void set_energy_range(long );

// retreives the offset and gain configured command $FC
// Tango::DevFailed exception if RONTEC returns an error
// arguments offset and gain are passed as reference
	void retreive_offset_gain(long &offs, long &gain);

// ======== SPECTRUM ACQUISITION METHODS ================
// configure acquisition time in real time mode  command $SM
// argin : acquisition time in second
// Tango::DevFailed exception if RONTEC returns an error
	void configure_acquisition_mode(long format, long mode);

// configure acquisition with default values
// format = 4 ( data are returned as long ( 32 bits ) integers
// mode 	= 0 ( accumulate spectrum )
	void configure_acquisition_mode(void);

// configure acquisition reading properties :
// these attributes are the global attributes of the acquisition
// the	get_spectrum_data() method will read a part of the configured spectrum
// start channel		: the 1st channel to be acquired
// step_width 			: read the all the channel modulo step_width
// summation_number : add the channels
// max_data 				: maximum of data to be read
	void configure_spectrum_reading_properties( long start_ch, long step_w, long summation_num, long max_d );

// gets configured acquisition reading properties :
// returns
// start channel		: the 1st channel to be acquired
// step_width 			: read the channel modulo step_width
// summation_number : add the channels
// max_data 				: maximum of data to be read
	void get_spectrum_reading_properties( long &start_ch, long &step_w, long &summation_num, long &max_d );

// configures the starting channel for reading spectrum
	void set_spectrum_reading_first_channel(Tango::DevLong start);

// configures the last channel for reading spectrum
	void set_spectrum_reading_last_channel(Tango::DevLong end);

// Set acquisition time in real time mode and start acquisition command $MT
// argin : acquisition time in second
// Tango::DevFailed exception if RONTEC returns an error
	void start_acquisition(float time);

// read partial spectrum beginning at begin for length channel and write them in buffer
// you must provide a buffer long enough !
	 void get_spectrum_data(long begin, long length, long *buffer);


// read partial spectrum beginning at begin for length channel and write them in buffertac
// Taco DS like code
	 void get_spectrum_data_2(long begin, long length, unsigned long *buffer);


// Pauses the acquisition
	void pause(void);

// resumes (restart the paused ) acquisition
	void resume(void);

// get the pause status
// returns RUNNING/PAUSE
	std::string get_pause_status(void);

// get_partial spectrum beginning at channel, according to the class attibutes for the spectrum readings
// argin begin : the partial spectrum channel beginning
//			 size  : the size to get
// returns : pointer on data read back
	long * get_partial_spectrum(long begin, long size);

// sends the command to the serial line device
// Tango::DevFailed exception if RONTEC returns an error
	std::string ascii_command(std::string cmd);

// controls if response is an error response,
// if error throws exception
// Tango::DevFailed exception if RONTEC returns an error
	void scan_response(std::string resp);

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
												 long &high_channel); 		// argout : high channel configured

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
												 long high_channel);		 // argout : high channel configured

//	roi_get_count(long ttl_num)
// returns the count of pulses on the selected ttl output

	long roi_get_count(long ttl_num);




};

// gestion du mutex automatique : unlock automatique des que sortie de visibilité
class automatic_mutex
{

public :
	automatic_mutex(omni_mutex * m)
	{
		my_mutex_ = m;
		my_mutex_->lock();
	};
	virtual ~automatic_mutex()
	{
		my_mutex_->unlock();
	};
private :
	omni_mutex * my_mutex_;
};


#endif	 // __RONTEC_MCA_IMPLEMENTATION_H__

