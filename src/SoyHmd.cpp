#include "PopOculus.h"
#include <OVR.h>



THmdMeta GetMeta(ovrHmd Hmd,const std::string& DebugSerial)
{
	THmdMeta Meta;
	
	//	use debug serial in case it doesn't exist
	Meta.mSerial = DebugSerial;
	Meta.mConnected = (Hmd != nullptr);
	
	//	no more info to supply
	if ( !Hmd )
		return Meta;
	
	auto& hmd = *Hmd;

	//	use serial number if supplied
	if ( hmd.SerialNumber[0] != '\0' )
		Meta.mSerial = std::string( hmd.SerialNumber );

	Meta.mResolution = vec2f( hmd.Resolution.w, hmd.Resolution.h );
	Meta.mWindowPosition = vec2f( hmd.WindowsPos.x, hmd.WindowsPos.y );
	Meta.mProductName = hmd.ProductName;

	Meta.mHasOrientation = (hmd.HmdCaps & ovrTrackingCap_Orientation);
	Meta.mHasPositional = (hmd.HmdCaps & ovrTrackingCap_Position);
	Meta.mHasDriftCorrection = (hmd.HmdCaps & ovrTrackingCap_MagYawCorrection);

	return Meta;
}

THmdMeta GetMeta(ovrHmd Hmd,std::stringstream&& DebugSerial)
{
	return GetMeta( Hmd, DebugSerial.str() );
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

		if ( Meta.IsValid() )
			Metas.PushBack( Meta );
		
		if ( Hmd )
			ovrHmd_Destroy( Hmd );
	}
	
	//	always add the debug one
	BufferArray<std::tuple<std::string,ovrHmdType>,10> DebugHmdTypes;
//	DebugHmdTypes.PushBack( std::make_tuple("ovrHmd_None",ovrHmd_None) );
	DebugHmdTypes.PushBack( std::make_tuple("ovrHmd_DK1",ovrHmd_DK1) );
	DebugHmdTypes.PushBack( std::make_tuple("ovrHmd_DKHD",ovrHmd_DKHD) );
	DebugHmdTypes.PushBack( std::make_tuple("ovrHmd_DK2",ovrHmd_DK2) );
	DebugHmdTypes.PushBack( std::make_tuple("ovrHmd_Other",ovrHmd_Other) );
	for ( int i=0;	i<DebugHmdTypes.GetSize();	i++ )
	{
		auto& HmdTypeName = std::get<0>( DebugHmdTypes[i] );
		auto& HmdType = std::get<1>( DebugHmdTypes[i] );
		auto Hmd = ovrHmd_CreateDebug( HmdType );
		auto Meta = GetMeta( Hmd, HmdTypeName );
		
		if ( Meta.IsValid() )
			Metas.PushBack( Meta );
	
		if ( Hmd )
			ovrHmd_Destroy( Hmd );
	}
}

std::shared_ptr<OvrHmdDevice> OvrDeviceManager::GetDevice(const std::string& Serial)
{
	return nullptr;
}
