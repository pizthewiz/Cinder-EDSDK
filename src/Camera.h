//
//  Camera.h
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#pragma once

#if defined(CINDER_MAC)
    #define __MACOS__
#elif defined(CINDER_MSW) || defined(CINDER_WINRT)
    #error Target platform unsupported by Cinder-EDSDK
#else
    #error Target platform unsupported by EDSDK
#endif

#include "EDSDK.h"

namespace Cinder { namespace EDSDK {

typedef std::shared_ptr<class Camera> CameraRef;

class CameraHandler {
public:
//    virtual void didRemoveCamera(CameraRef camera) = 0;
//    virtual void didAddFile(CameraRef camera, void* file) = 0;
//    virtual void didDownloadFile(CameraRef camera, void* file, void* something) = 0;
//    virtual void didReadFile(CameraRef camera, void* file, void* something) = 0;
};

class Camera : public std::enable_shared_from_this<Camera> {
public:
    static CameraRef create(EdsCameraRef camera);
	~Camera();

    CameraHandler* getHandler() const;
    void setHandler(CameraHandler* handler);

    std::string getName() const;
    std::string getPortName() const;

    bool hasOpenSession() const;
    EdsError requestOpenSession();
    EdsError requestCloseSession();

    EdsError requestTakePicture(/*Options* options*/);
    EdsError requestDownloadFile();
    EdsError requestReadFile();

private:
    Camera(EdsCameraRef camera);

    static EdsError EDSCALLBACK handleObjectEvent(EdsUInt32 inEvent, EdsBaseRef inRef, EdsVoid* inContext);
    static EdsError EDSCALLBACK handlePropertyEvent(EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext);
    static EdsError EDSCALLBACK handleStateEvent(EdsUInt32 inEvent, EdsUInt32 inParam, EdsVoid* inContext);

    CameraHandler* mHandler;
    EdsCameraRef mCamera;
    EdsDeviceInfo mDeviceInfo;
    bool mHasOpenSession;
};

}}
