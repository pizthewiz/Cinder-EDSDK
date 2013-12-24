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
    virtual void didRemoveCamera(Camera* camera) = 0;
//    virtual void didAddFile(Camera* camera, void* file) = 0;
//    virtual void didDownloadFile(Camera* camera, void* file, void* something) = 0;
//    virtual void didReadFile(Camera* camera, void* file, void* something) = 0;
};

class CameraSettings {
public:
    bool getShouldKeepAlive() const {
        return mShouldKeepAlive;
    }
    void setShouldKeepAlive(bool flag) {
        mShouldKeepAlive = flag;
    }
    EdsUInt32 getPictureSaveLocation() const {
        return mPictureSaveLocation;
    }
    void setPictureSaveLocation(EdsUInt32 saveLocation) {
        mPictureSaveLocation = saveLocation;
    }

private:
    bool mShouldKeepAlive = true;
    EdsUInt32 mPictureSaveLocation = kEdsSaveTo_Host;
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
    EdsError requestOpenSession(CameraSettings* settings);
    EdsError requestCloseSession();

    EdsError requestTakePicture();
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
    bool mShouldKeepAlive = true;
};

}}
