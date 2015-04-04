//
//  Camera.h
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013-2015 Chorded Constructions. All rights reserved.
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

class CameraFile : public std::enable_shared_from_this<CameraFile> {
public:
    static CameraFileRef create(const EdsDirectoryItemRef& directoryItem);
    ~CameraFile();

    inline std::string getName() const {
        return std::string(mDirectoryItemInfo.szFileName);
    }
    inline uint32_t getSize() const {
        return mDirectoryItemInfo.size;
    }

private:
    CameraFile(const EdsDirectoryItemRef& directoryItem);

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
        inline bool getShouldKeepAlive() const {
            return mShouldKeepAlive;
        }
        Settings& setPictureSaveLocation(EdsUInt32 location)  {
            mPictureSaveLocation = location; return *this;
        }
        inline EdsUInt32 getPictureSaveLocation() const {
            return mPictureSaveLocation;
        }

    private:
        bool mShouldKeepAlive;
        EdsUInt32 mPictureSaveLocation;
    };

    static CameraRef create(const EdsCameraRef& camera);
    ~Camera();

    template<typename T, typename Y>
    inline void connectRemovedHandler(T handler, Y* obj) { connectRemovedHandler(std::bind(handler, obj, std::placeholders::_1)); }
    void connectRemovedHandler(const std::function<void(CameraRef)>& handler);
    template<typename T, typename Y>
    inline void connectFileAddedHandler(T handler, Y* obj) { connectFileAddedHandler(std::bind(handler, obj, std::placeholders::_1, std::placeholders::_2)); }
    void connectFileAddedHandler(const std::function<void(CameraRef, CameraFileRef)>& handler);

    inline std::string getName() const {
        return std::string(mDeviceInfo.szDeviceDescription);
    }
    inline std::string getPortName() const {
        return std::string(mDeviceInfo.szPortName);
    }

    inline bool hasOpenSession() const {
        return mHasOpenSession;
    }
    EdsError requestOpenSession(const Settings& settings = Settings());
    EdsError requestCloseSession();

    EdsError requestTakePicture();
    void requestDownloadFile(const CameraFileRef& file, const ci::fs::path& destinationFolderPath, const std::function<void(EdsError error, ci::fs::path outputFilePath)>& callback);
    void requestReadFile(const CameraFileRef& file, const std::function<void(EdsError error, ci::SurfaceRef surface)>& callback);

    inline bool isLiveViewActive() const {
        return mIsLiveViewActive;
    };
    EdsError requestStartLiveView();
    EdsError requestStopLiveView();
    void toggleLiveView();
    void requestLiveViewImage(const std::function<void(EdsError error, ci::SurfaceRef surface)>& callback);

private:
    Camera(const EdsCameraRef& camera);

    static EdsError EDSCALLBACK handleObjectEvent(EdsUInt32 event, EdsBaseRef ref, EdsVoid* context);
    static EdsError EDSCALLBACK handlePropertyEvent(EdsUInt32 event, EdsUInt32 propertyID, EdsUInt32 param, EdsVoid* context);
    static EdsError EDSCALLBACK handleStateEvent(EdsUInt32 event, EdsUInt32 param, EdsVoid* context);

    std::function<void (CameraRef)> mRemovedHandler;
    std::function<void (CameraRef, CameraFileRef)> mFileAddedHandler;
    EdsCameraRef mCamera;
    EdsDeviceInfo mDeviceInfo;
    bool mHasOpenSession;
    bool mShouldKeepAlive;
    bool mIsLiveViewActive;
};

}}
