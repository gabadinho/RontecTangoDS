/*******************************************************************************************************
 *
 * DYNAMIC ATTRIBUTES IMPLEMENTATION
 *
 *******************************************************************************************************/
#include "DynamicAttributes.h"

namespace Rontec_ns
{

void Rontec::create_dynamic_attributes() {

	for(long i=1; i<=NB_MAX_ROI; i++) {
		std::ostringstream oss;
		oss << i;
		std::string name = "roi"+oss.str();
		std::string label = "Roi "+oss.str();
		roiAttrib* attr = new roiAttrib(name,i);
		Tango::UserDefaultAttrProp	prop;
		prop.set_label(label.c_str());
		prop.set_format("%9.3d");
		std::string description = "value of the ROI"+oss.str()+"\n";
		prop.set_description(description.c_str());
		attr->set_default_properties(prop);
		attr->set_disp_level(Tango::EXPERT);
		add_attribute(attr);
	}
}

void Rontec::read_roi(Tango::Attribute &attr,long num_roi)
{
	DEBUG_STREAM << "Rontec::read_roi(Tango::Attribute &attr,num_roi) entering... "<< endl;
	DEBUG_STREAM << "num_roi=" << num_roi << endl;
	if(!_mca) Tango::Except::throw_exception((const char *)"OPERATION_NOT_ALLOWED",(const char *)"The _mca object is not initialized!",(const char *)"_mca check");
	if(!is_ROI_configured(num_roi) )
	{
		ERROR_STREAM << "DATA_OUT_OF_RANGE Rontec::read_roi(): ttl output number not configured in property" << endl;
		Tango::Except::throw_exception (
			(const char *)"DATA_OUT_OF_RANGE",
			(const char *)" ttl output number not configured in property ",
			(const char *)"Rontec::get_roi_count");

	}
	*(attr_roi_read[num_roi]) = _mca->roi_get_count(num_roi);
	attr.set_value(attr_roi_read[num_roi]);
}

bool Rontec::is_roi_allowed(Tango::AttReqType type,long num_roi)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}

} // namespace
