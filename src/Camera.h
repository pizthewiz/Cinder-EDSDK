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
#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"

namespace Cinder { namespace EDSDK {

typedef std::shared_ptr<class Camera> CameraRef;
typedef std::shared_ptr<class CameraFile> CameraFileRef;

class CameraHandler {
public:
    virtual void didRemoveCamera(Camera* camera) = 0;
    virtual void didAddFile(Camera* camera, CameraFileRef file) = 0;
};

class CameraFile : public std::enable_shared_from_this<CameraFile> {
public:
    static CameraFileRef create(EdsDirectoryItemRef directoryItem);
	~CameraFile();

    std::string getName() const;
    uint32_t getSize() const;

private:
    CameraFile(EdsDirectoryItemRef directoryItem);

    EdsDirectoryItemRef mDirectoryItem;
    EdsDirectoryItemInfo mDirectoryItemInfo;

    friend class Camera;
};

class Camera : public std::enable_shared_from_this<Camera> {
public:
    struct Settings {
        Settings() : mShouldKeepAlive(true), mPictureSaveLocation(kEdsSaveTo_Host) {}

        Settings& setShouldKeepAlive(bool flag) {
            mShouldKeepAlive = flag; return *this;
        }
        bool getShouldKeepAlive() const {
            return mShouldKeepAlive;
        }
        Settings& setPictureSaveLocation(EdsUInt32 location)  {
            mPictureSaveLocation = location; return *this;
        }
        EdsUInt32 getPictureSaveLocation() const {
            return mPictureSaveLocation;
        }

    private:
        bool mShouldKeepAlive;
        EdsUInt32 mPictureSaveLocation;
    };

    static CameraRef create(EdsCameraRef camera);
	~Camera();

    CameraHandler* getHandler() const;
    void setHandler(CameraHandler* handler);

    std::string getName() const;
    std::string getPortName() const;

    bool hasOpenSession() const;
    EdsError requestOpenSession(const Settings &settings = Settings());
    EdsError requestCloseSession();

    EdsError requestTakePicture();
    void requestDownloadFile(const CameraFileRef file, const ci::fs::path destinationFolderPath, std::function<void(EdsError error, ci::fs::path outputFilePath)> callback);
    void requestReadFile(const CameraFileRef file, std::function<void(EdsError error, ci::Surface surface)> callback);

private:
    Camera(EdsCameraRef camera);

    static EdsError EDSCALLBACK handleObjectEvent(EdsUInt32 inEvent, EdsBaseRef inRef, EdsVoid* inContext);
    static EdsError EDSCALLBACK handlePropertyEvent(EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext);
    static EdsError EDSCALLBACK handleStateEvent(EdsUInt32 inEvent, EdsUInt32 inParam, EdsVoid* inContext);

    CameraHandler* mHandler;
    EdsCameraRef mCamera;
    EdsDeviceInfo mDeviceInfo;
    bool mHasOpenSession;
    bool mShouldKeepAlive;
};

}}
