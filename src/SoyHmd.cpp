#include "PopOculus.h"
#include <OVR.h>



THmdMeta GetMeta(ovrHmd Hmd,std::stringstream&& DebugSerial)
{
	THmdMeta Meta;
	
	//	use debug serial in case it doesn't exist
	Meta.mSerial = DebugSerial.str();
	Meta.mConnected = (Hmd != nullptr);
	
	//	no more info to supply
	if ( !Hmd )
		return Meta;
	
	auto& hmd = *Hmd;
	Meta.mSerial = std::string( hmd.SerialNumber );
	Meta.mResolution = vec2f( hmd.Resolution.w, hmd.Resolution.h );
	Meta.mWindowPosition = vec2f( hmd.WindowsPos.x, hmd.WindowsPos.y );
	Meta.mProductName = hmd.ProductName;

	Meta.mHasOrientation = (hmd.HmdCaps & ovrTrackingCap_Orientation);
	Meta.mHasPositional = (hmd.HmdCaps & ovrTrackingCap_Position);
	Meta.mHasDriftCorrection = (hmd.HmdCaps & ovrTrackingCap_MagYawCorrection);

	return Meta;
}


std::ostream& operator<< (std::ostream &out,const THmdMeta &in)
{
	out << in.mSerial << "/" << in.mProductName;
 
	//	caps
	out << "(";
	out << in.mResolution.x << "x" << in.mResolution.y << "@" << in.mWindowPosition.x << "x" << in.mWindowPosition.y;
	out << ",";

	if ( in.mHasOrientation )
		out << "Orientation,";
	if ( in.mHasPositional )
		out << "Position,";
	if ( in.mHasDriftCorrection )
		out << "DriftCorrection,";
	
	out << ")";

	return out;
}


OvrDeviceManager::OvrDeviceManager()
{
	if ( !ovr_Initialize() )
	{
		mInitError = "Failed to initialised OVR Library";
	}
}

OvrDeviceManager::~OvrDeviceManager()
{
	if ( IsLoaded() )
		ovr_Shutdown();
}

std::string OvrDeviceManager::GetVersion() const
{
	return ovr_GetVersionString();
}

void OvrDeviceManager::GetDevices(ArrayBridge<THmdMeta>&& Metas,std::stringstream& Error)
{
	if ( !IsLoaded( Error ) )
		return;
	
	//	list real devices first
	auto HmdCount = ovrHmd_Detect();
	for ( int i=0;	i<HmdCount;	i++ )
	{
		//	create a meta for each one it said it had, even if it's disapeared now
		auto Hmd = ovrHmd_Create(i);
		auto Meta = GetMeta( Hmd, std::stringstream() << "ovr" << i );
		
		if ( Hmd )
			ovrHmd_Destroy( Hmd );
	}
	
	//	always add the debug one
	BufferArray<ovrHmdType,10> DebugHmdTypes;
	DebugHmdTypes.PushBack( ovrHmd_None );
	DebugHmdTypes.PushBack( ovrHmd_DK1 );
	DebugHmdTypes.PushBack( ovrHmd_DKHD );
	DebugHmdTypes.PushBack( ovrHmd_DK2 );
	DebugHmdTypes.PushBack( ovrHmd_Other );
	for ( int i=0;	i<DebugHmdTypes.GetSize();	i++ )
	{
		auto Hmd = ovrHmd_CreateDebug(DebugHmdTypes[i]);
		auto Meta = GetMeta( Hmd, std::stringstream() << "ovrdebug" << i );
		
		if ( Hmd )
			ovrHmd_Destroy( Hmd );
	}
}

std::shared_ptr<OvrHmdDevice> OvrDeviceManager::GetDevice(const std::string& Serial)
{
	return nullptr;
}
