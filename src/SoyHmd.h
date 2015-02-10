#pragma once




class THmdMeta
{
public:
	THmdMeta() :
		mHasPositional	( false ),
		mResolution		( 0,0 ),
		mWindowPosition	( 0,0 ),
		mConnected		( false ),
		mHasOrientation	( false ),
		mHasDriftCorrection	( false )
	{
	}
	
	inline bool		IsValid() const	{	return !mSerial.empty();	}
	
public:
	std::string		mSerial;
	std::string		mProductName;	//	DK1, DK2 etc - make this an emum?
	vec2f			mResolution;
	vec2f			mWindowPosition;
	bool			mConnected;
	bool			mHasPositional;
	bool			mHasOrientation;
	bool			mHasDriftCorrection;
};
std::ostream& operator<< (std::ostream &out,const THmdMeta &in);


class THmdDevice
{
public:
	virtual THmdMeta	GetMeta() const=0;
};

class OvrHmdDevice : public THmdDevice
{
public:
	virtual THmdMeta	GetMeta() const	{	return mMeta;	}
	
public:
	THmdMeta	mMeta;
};



class OvrDeviceManager
{
public:
	OvrDeviceManager();
	~OvrDeviceManager();
	
	bool							IsLoaded(std::stringstream& Error) const	{	Error << mInitError;	return IsLoaded();	}
	bool							IsLoaded() const							{	return mInitError.empty();	}
	
	void							GetDevices(ArrayBridge<THmdMeta>&& Metas,std::stringstream& Error);
	std::shared_ptr<OvrHmdDevice>	GetDevice(const std::string& Serial);
	std::string						GetVersion() const;
	
public:
	std::string								mInitError;
	Array<std::shared_ptr<OvrHmdDevice>>	mDevices;
};
