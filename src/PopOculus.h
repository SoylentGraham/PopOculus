#pragma once
#include <ofxSoylent.h>
#include <SoyApp.h>
#include <TJob.h>
#include <TChannel.h>
#include "SoyHmd.h"



class TPopOculus : public TJobHandler, public TChannelManager
{
public:
	TPopOculus();
	
	virtual void	AddChannel(std::shared_ptr<TChannel> Channel) override;

	void			OnExit(TJobAndChannel& JobAndChannel);
	void			OnListDevices(TJobAndChannel& JobAndChannel);
	void			OnOculusVersion(TJobAndChannel& JobAndChannel);
	
public:
	OvrDeviceManager			mOculus;
	Soy::Platform::TConsoleApp	mConsoleApp;
};



