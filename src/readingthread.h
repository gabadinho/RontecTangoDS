//////////////////////////////////////////////////////////////////////
//
// Projet : RONTEC device server
// readingthread.h: interface for the readingthread class.
// Ce thread fait la lecture de spectre
// protection contre les acces concurrents a la liaison serie par un mutex
//
//////////////////////////////////////////////////////////////////////

#ifndef __READINGTHREAD_H__
#define __READINGTHREAD_H__
#include <tango.h>
#include <RontecMcaImplementation.h>


namespace Rontec_ns
{

	class readingthread : public omni_thread, public Tango::LogAdapter 
	{
	public:
		readingthread(Tango::DeviceImpl* host_device,
									RontecMcaImplementation * mca,
									omni_mutex * ser_access,
									unsigned long t_sec = 0,
									unsigned long t_nanosec = 100000000,
									long read_size = 512
									);
		virtual ~readingthread(void);

		//- go ------------------------------------------------------------
		void go (void) throw (Tango::DevFailed);


		//- abort ---------------------------------------------------------
		void abort (void);

			//- pauses the spectrum acquisition thread ----------------------
		void pause (void)
		{pause_ = true;} ;

		//- resumes the spectrum acquisition thread -----------------------
		void resume (void);

		//- gets the pause status of the spectrum acquisition thread ------
		bool is_paused (void)
		{return pause_;} ;

		//- gets the started status of the spectrum acquisition thread ------
		bool is_started (void)
		{return go_on_;} ;

		//- gets the counter value of the acquisition thread --------------
		long get_counter (void)
		{return counter_;} ;

		//- stores last exception catched ---------------------------------
	void store_last_error (Tango::DevFailed& df);

		//- the way to get back the data ----------------------------------
	unsigned long * get_spectrum (void) const
											throw (Tango::DevFailed);

	//- run_undetached ---------------------------------------
  virtual void* run_undetached (void *);


	private:
		//- 4096 channels data buffer ( fixed length )
		unsigned long data_[4096];
		// index de parcours des channels
		long index_;
		// nombre de channels a lire d'1 coup
		long read_size_;
		// numero du dernier channel a lire
		long last_channel_index_;
		// numero du premier channel a lire
		long first_channel_index_;

		//- thread runs
		bool go_on_;

		// error/exception catched
		bool has_error_;
		//- last_error
		Tango::DevErrorList error_list_;

		 //- timeout
		unsigned long timeout_sec_;
		unsigned long timeout_nanosec_;

		//- mca implementation class pointer
		RontecMcaImplementation *mca_;
		//- mutex for data
		omni_mutex *serial_access_;
		//- mutex for condition variable
		omni_mutex read_lock_;
		//- condition variable
		omni_condition * read_cond_;
		//- thread run/stop control
		bool pause_;

		//- indicates that serial ressource is locked by the thread, to unlock 
		// the serial acces in case of exception in the catch case
		bool thread_is_locking_;

		// counter for visualisation of thread life
		long counter_;
	};
} //namespace Threadtest_ns

#endif // !defined __READINGTHREAD_H__

