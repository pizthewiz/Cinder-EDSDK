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

class CameraBrowser;

// expected device handler functions:
//  didRemoveCamera
//  didOpenSessionWithError (possibly not needed)
//  didCloseSessionWithError (possibly not needed)
//  didAddFile
//  didDownloadFile
//  didReadFile

typedef std::shared_ptr<class Camera> CameraRef;

class Camera : public std::enable_shared_from_this<Camera> {
public:
    static CameraRef create(EdsCameraRef camera);
	~Camera();

    std::string getName() const;

    bool hasOpenSession() const;
    void requestOpenSession();
    void requestCloseSession();

    void requestTakePicture(/*options*/);
    void requestDownloadFile();
    void requestReadFile();

private:
    Camera(EdsCameraRef camera);

    static EdsError EDSCALLBACK handleObjectEvent(EdsUInt32 inEvent, EdsBaseRef inRef, EdsVoid* inContext);
    static EdsError EDSCALLBACK handlePropertyEvent(EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext);
    static EdsError EDSCALLBACK handleStateEvent(EdsUInt32 inEvent, EdsUInt32 inParam, EdsVoid* inContext);

    EdsCameraRef mCamera;
    EdsDeviceInfo mDeviceInfo;
    bool mHasOpenSession;

    friend class CameraBrowser;
};

}}
