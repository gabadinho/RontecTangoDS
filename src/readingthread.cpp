//////////////////////////////////////////////////////////////////////
//
// Projet : RONTEC device server
// readingthread.cpp: implementation of the readingthread class.
// Ce thread fait la lecture de spectre
// protection contre les acces concurrents a la liaison serie par un mutex
//
//////////////////////////////////////////////////////////////////////

#include <readingthread.h>


namespace Rontec_ns
{

//--------------------------------------------------------------------
// Construction/Destruction
//--------------------------------------------------------------------

	
readingthread::readingthread ( Tango::DeviceImpl* host_device,
															RontecMcaImplementation * mca,
															omni_mutex * ser_access,
															unsigned long t_sec,
															unsigned long t_nanosec,
															long read_size	) :
																				omni_thread(),
																				Tango::LogAdapter(host_device),
																				serial_access_( ser_access ),
																				timeout_sec_( t_sec ),
																				timeout_nanosec_( t_nanosec ),
																				read_size_(read_size),
																				mca_(mca),
																				has_error_(false)

{
	DEBUG_STREAM << "readingthread::readingthread <-" << std::endl;
	::memset(&data_, 0,( sizeof(unsigned long) * 4096));
	this->counter_ = 0;
	this->first_channel_index_ = 0;
	this->index_ = first_channel_index_;
	this->last_channel_index_ = 4095;
	this->thread_is_locking_ = false;

	DEBUG_STREAM << "readingthread::readingthread ->" << std::endl;
}

// threads should not be deleted : you must use abort()!
// Destructor
readingthread::~readingthread()
{
	DEBUG_STREAM << "readingthread::~readingthread <-" << std::endl;
	DEBUG_STREAM << "readingthread::~readingthread ->" << std::endl;

}





//--------------------------------------------------------------------
// readingthread::abort
//--------------------------------------------------------------------
void readingthread::abort (void)
{
	std::cout << "RontecReadingThread::abort <-" << std::endl;
	this->go_on_ = false;

	if (this->read_cond_)
	{
		this->read_cond_->signal();
	}

	try
	{
		void * dummy = 0;
		DEBUG_STREAM << "PLCVarsReadingThread::abort ->" << std::endl;
		this->join(&dummy);
	}
	catch (...)
	{
	 //- ignore any exception, if thread does not exists as well
	}

	std::cout << "RontecReadingThread::abort ->" << std::endl;
}


//--------------------------------------------------------------------
// readingthread::go
//--------------------------------------------------------------------
void readingthread ::go (void) throw (Tango::DevFailed)
{
	if(this->go_on_)
		return;

	std::cout << "RontecReadingThread::go <-" << endl;
	try
	{
		this->read_cond_ = new omni_condition(&this->read_lock_);
		if (this->read_cond_ == 0)
			throw std::bad_alloc();
 }
	catch (const std::bad_alloc&)
	{
		std::cout << " --------CATCHED BAD ALLOC EXCEPTION ---------" << std::endl;
		has_error_ = true;
		ERROR_STREAM << "readingthread::go OUT_OF_MEMORY catched bad_alloc exception " << std::endl;

		error_list_.length(1);
		error_list_[0].reason = CORBA::string_dup("OUT_OF_MEMORY");
		error_list_[0].desc = CORBA::string_dup("catched bad_alloc exception ");
		error_list_[0].origin = CORBA::string_dup("readingthread::go ");
		error_list_[0].severity = Tango::ERR;

	}
	catch (...)
	{
		std::cout << " --------CATCHED UNKNOWN EXCEPTION ---------" << std::endl;
		has_error_ = true;
		ERROR_STREAM << "readingthread::go OUT_OF_MEMORY catched (...) exception " << std::endl;

		error_list_.length(1);
		error_list_[0].reason = CORBA::string_dup("OUT_OF_MEMORY");
		error_list_[0].desc = CORBA::string_dup("catched (...) exception ");
		error_list_[0].origin = CORBA::string_dup("readingthread::go ");
		error_list_[0].severity = Tango::ERR;
	}
	start_undetached();

	std::cout << "readingthread::go ->" << endl;
}



//--------------------------------------------------------------------
// readingthread::run_undetached
//--------------------------------------------------------------------
void* readingthread::run_undetached (void *)
{
	//- !!!! PAS DE RETHROW DANS CETTE METHODE !!!!!!
	// le thread mourrait salement
	// catcher les erreurs
	// les ranger dans error_list ( une sequence )
	// throw exception dans la methode de consommation des donnees
	INFO_STREAM << "readingthread::run_undetached ENTERING <-" << std::endl;

	go_on_ = true;

	unsigned long abs_sec = 0;
	unsigned long abs_nsec = 0;

	long size = this->read_size_ ;
	while (go_on_)
	{

		try
		{
			// DEBUG visualisation du progres de 'acquisition dans la fenetre dos
			std::cout << "*" ;
			// compteur ayant servi pour la mise au point, plus utilise
			++counter_;
			// parcourir tous les canaux entre debut et fin
			size = this->read_size_ ;
			if( (this->index_ + this->read_size_) >= this->last_channel_index_)
			{
				size = this->last_channel_index_ - this->index_;
				size++;
			}
			// RAZ index parcours
			if( this->index_	>= this->last_channel_index_)
			{
				this->index_ = first_channel_index_;
			}
			// lock the resource
			serial_access_->lock();
			this->thread_is_locking_ = true;


			// lire les data via le rontecmcaimplementation
			mca_->get_spectrum_data_2(this->index_, this->read_size_, &this->data_[this->index_]);

			// UNlock the ressource
			this->thread_is_locking_ = false;
			serial_access_->unlock();


			// atteint le dernier channel? repartir au debut
			if(this->index_ >=	this->last_channel_index_ )
			{
				this->index_ = this->first_channel_index_;
			}
			else
			{
				this->index_ += this->read_size_ ;
			}
			//- get absolute from relative time

			omni_thread::get_time(&abs_sec, &abs_nsec, this->timeout_sec_,this->timeout_nanosec_);
		 // omni_thread::sleep(this->timeout_sec_, timeout_nanosec_);

#ifdef WIN32
			this->read_lock_.lock();
#endif
		 // use of the condition variable as a temporisation for the thread to sleep
		 // condition.timedwait waits for a signal or end-of-time
			//- wait for the cond. var. to be signaled
			this->read_cond_->timedwait(abs_sec, abs_nsec);


			// paused?
			if(this->pause_)
			{
				this->read_cond_->wait();
			}
		}
		catch (Tango::DevFailed& df)
		{
			serial_access_->unlock();
			ERROR_STREAM << "readingthread::run_undetached catched DevFailed exception " << df << std::endl;
			this->has_error_ = true;
			store_last_error(df);

			::memset(this->data_, 0,( sizeof(unsigned long)* 4096));
			this->index_ = first_channel_index_;
		}
		catch (...)
		{
			serial_access_->unlock();
			has_error_ = true;
			ERROR_STREAM << "readingthread::run_undetached catched (...) exception " << std::endl;

			error_list_.length(1);
			error_list_[0].reason = CORBA::string_dup("UNKNOWN_ERROR");
			error_list_[0].desc = CORBA::string_dup("unknown error trying to read PLCServer ");
			error_list_[0].origin = CORBA::string_dup("readingthread::run_undetached ");
			error_list_[0].severity = Tango::ERR;

//			store_last_error(error_list_);
			::memset(&data_, 0, ( sizeof(unsigned long)* 4096));
			this->index_ = first_channel_index_;
		}


	} //	while (go_on_)

	INFO_STREAM << "readingthread::run_undetached -> EXIT" << endl;
	this->go_on_ = false;
	return 0;
}


//--------------------------------------------------------------------
// readingthread::resume
//--------------------------------------------------------------------
void readingthread::resume (void)
{
	DEBUG_STREAM << "readingthread::resume <-" << std::endl;
	this->read_cond_->signal();
	// this->read_cond_->broadcast();
	this->pause_ = false;

	DEBUG_STREAM << "readingthread::resume ->" << std::endl;
}

//--------------------------------------------------------------------
// readingthread::store_last_error
//--------------------------------------------------------------------
void readingthread::store_last_error (Tango::DevFailed& df)
{
	DEBUG_STREAM << "readingthread::store_last_error <-" << std::endl;
	error_list_.length(df.errors.length());

	for(int i = 0; i < df.errors.length(); i++)
	{
		error_list_[i].reason = CORBA::string_dup(df.errors[i].reason);
		error_list_[i].desc = CORBA::string_dup(df.errors[i].desc);
		error_list_[i].origin = CORBA::string_dup(df.errors[i].origin);
		error_list_[i].severity = df.errors[i].severity;
	}
	DEBUG_STREAM << "readingthread::store_last_error ->" << std::endl;
}

// ============================================================================
// readingthread::get_spectrum
// returns data read by the thread
// if exception, throws las exception catched in the polling thread
// ============================================================================
unsigned long * readingthread::get_spectrum (void) const
		throw (Tango::DevFailed)
{
//	DEBUG_STREAM << "readingthread::get_spectrum <-" << std::endl;
	if (has_error_)
	{
		// previously catched and stores in error_list internal private variable
		// use this way to catch errors in the thread loop and to let it know
		// to the data reader :
		// the thread has catched errors and did not rethrow it
		// this method can throw exception without killing the thread
		throw Tango::DevFailed(error_list_);
	}
//	DEBUG_STREAM << "readingthread::get_spectrum <-" << std::endl;

	return (unsigned long *)(&this->data_);
}



} // namespace Rontec_ns
