/**
 * Class roiAttrib 
 * Dynamic attribute class
 * Defined here because it is deleted by Pogo when in RontecClass.h file
 */
#include "Rontec.h"

namespace Rontec_ns
{

class roiAttrib: public Tango::Attr
{
public:
	roiAttrib(const string& name,long num_roi):Attr(name.c_str(), Tango::DEV_LONG, Tango::READ) { m_num_roi = num_roi; };
	~roiAttrib() {};
	
	virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att)
	{(static_cast<Rontec *>(dev))->read_roi(att,m_num_roi);}
	virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty)
	{return (static_cast<Rontec *>(dev))->is_roi_allowed(ty,m_num_roi);}

protected:
	long m_num_roi;
};

} // namespace