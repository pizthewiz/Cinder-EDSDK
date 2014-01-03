//
//  Camera.cpp
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013-2014 Chorded Constructions. All rights reserved.
//

#include "Camera.h"
#include "CameraBrowser.h"

using namespace ci;
using namespace ci::app;

namespace Cinder { namespace EDSDK {

#pragma mark CAMERA FILE

CameraFileRef CameraFile::create(EdsDirectoryItemRef directoryItem) {
    return CameraFileRef(new CameraFile(directoryItem))->shared_from_this();
}

CameraFile::CameraFile(EdsDirectoryItemRef directoryItem) {
    if (!directoryItem) {
        throw Exception();
    }

    EdsRetain(directoryItem);
    mDirectoryItem = directoryItem;

    EdsError error = EdsGetDirectoryItemInfo(mDirectoryItem, &mDirectoryItemInfo);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get directory item info" << std::endl;
        throw Exception();
    }
}

CameraFile::~CameraFile() {
    EdsRelease(mDirectoryItem);
    mDirectoryItem = NULL;
}

std::string CameraFile::getName() const {
    return std::string(mDirectoryItemInfo.szFileName);
}

uint32_t CameraFile::getSize() const {
    return mDirectoryItemInfo.size;
}

#pragma mark - CAMERA

CameraRef Camera::create(EdsCameraRef camera) {
    return CameraRef(new Camera(camera))->shared_from_this();
}

Camera::Camera(EdsCameraRef camera) {
    if (!camera) {
        throw Exception();
    }

    EdsRetain(camera);
    mCamera = camera;

    EdsError error = EdsGetDeviceInfo(mCamera, &mDeviceInfo);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get device info" << std::endl;
        // TODO - NULL out mDeviceInfo
    }

    mHasOpenSession = false;

    // set event handlers
    error = EdsSetObjectEventHandler(mCamera, kEdsObjectEvent_All, Camera::handleObjectEvent, this);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set object event handler" << std::endl;
    }
    error = EdsSetPropertyEventHandler(mCamera, kEdsPropertyEvent_All, Camera::handlePropertyEvent, this);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set property event handler" << std::endl;
    }
    error = EdsSetCameraStateEventHandler(mCamera, kEdsStateEvent_All, Camera::handleStateEvent, this);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set object event handler" << std::endl;
    }
}

Camera::~Camera() {
    mRemovedHandler = NULL;
    mFileAddedHandler = NULL;

    if (mHasOpenSession) {
        requestCloseSession();
    }

    // NB - after EDSDK 2.10 releasing the EdsCameraRef will cause an EXC_BAD_ACCESS (code=EXC_I386_GPFLT)
//    EdsRelease(mCamera);
    mCamera = NULL;
}

#pragma mark -

void Camera::connectRemovedHandler(const std::function<void(CameraRef)>& handler) {
    mRemovedHandler = handler;
}

void Camera::connectFileAddedHandler(const std::function<void(CameraRef, CameraFileRef)>& handler) {
    mFileAddedHandler = handler;
}

std::string Camera::getName() const {
    return std::string(mDeviceInfo.szDeviceDescription);
}


std::string Camera::getPortName() const {
    return std::string(mDeviceInfo.szPortName);
}

bool Camera::hasOpenSession() const {
    return mHasOpenSession;
}

EdsError Camera::requestOpenSession(const Settings &settings) {
    if (mHasOpenSession) {
        return EDS_ERR_SESSION_ALREADY_OPEN;
    }

    EdsError error = EdsOpenSession(mCamera);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to open camera session" << std::endl;
        return error;
    }
    mHasOpenSession = true;

    mShouldKeepAlive = settings.getShouldKeepAlive();
    EdsUInt32 saveTo = settings.getPictureSaveLocation();
    error = EdsSetPropertyData(mCamera, kEdsPropID_SaveTo, 0, sizeof(saveTo) , &saveTo);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set save destination host/device" << std::endl;
        return error;
    }

    if (saveTo == kEdsSaveTo_Host) {
        // ??? - requires UI lock?
        EdsCapacity capacity = {0x7FFFFFFF, 0x1000, 1};
        error = EdsSetCapacity(mCamera, capacity);
        if (error != EDS_ERR_OK) {
            console() << "ERROR - failed to set capacity of host" << std::endl;
            return error;
        }
    }

    return EDS_ERR_OK;
}

EdsError Camera::requestCloseSession() {
    if (!mHasOpenSession) {
        return EDS_ERR_SESSION_NOT_OPEN;
    }

    EdsError error = EdsCloseSession(mCamera);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to close camera session" << std::endl;
        return error;
    }

    mHasOpenSession = false;
    return EDS_ERR_OK;
}

EdsError Camera::requestTakePicture() {
    if (!mHasOpenSession) {
        return EDS_ERR_SESSION_NOT_OPEN;
    }

    EdsError error = EdsSendCommand(mCamera, kEdsCameraCommand_TakePicture, 0);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to take picture" << std::endl;
    }
    return error;
}

void Camera::requestDownloadFile(const CameraFileRef file, const fs::path destinationFolderPath, std::function<void(EdsError error, ci::fs::path outputFilePath)> callback) {
    // check if destination exists and create if not
    if (!fs::exists(destinationFolderPath)) {
        bool status = fs::create_directories(destinationFolderPath);
        if (!status) {
            console() << "ERROR - failed to create destination folder path '" << destinationFolderPath << "'" << std::endl;
            return callback(EDS_ERR_INTERNAL_ERROR, NULL);
        }
    }

    fs::path filePath = destinationFolderPath / file->getName();

    EdsStreamRef stream = NULL;
    EdsError error = EdsCreateFileStream(filePath.generic_string().c_str(), kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to create file stream" << std::endl;
        goto download_cleanup;
    }

    error = EdsDownload(file->mDirectoryItem, file->getSize(), stream);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to download" << std::endl;
        goto download_cleanup;
    }

    error = EdsDownloadComplete(file->mDirectoryItem);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to mark download as complete" << std::endl;
        goto download_cleanup;
    }

download_cleanup:
    if (stream) {
        EdsRelease(stream);
    }

    callback(error, filePath);
}

void Camera::requestReadFile(const CameraFileRef file, std::function<void(EdsError error, ci::Surface8u surface)> callback) {
    Buffer buffer = NULL;
    ci::Surface surface;

    EdsStreamRef stream = NULL;
    EdsError error = EdsCreateMemoryStream(0, &stream);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to create memory stream" << std::endl;
        goto read_cleanup;
    }

    error = EdsDownload(file->mDirectoryItem, file->getSize(), stream);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to download" << std::endl;
        goto read_cleanup;
    }

    error = EdsDownloadComplete(file->mDirectoryItem);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to mark download as complete" << std::endl;
        goto read_cleanup;
    }

    void* data;
    error = EdsGetPointer(stream, (EdsVoid**)&data);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get pointer from stream" << std::endl;
        goto read_cleanup;
    }

    EdsUInt32 length;
    error = EdsGetLength(stream, &length);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get stream length" << std::endl;
        goto read_cleanup;
    }

    buffer = Buffer(data, length);
    surface = Surface8u(loadImage(DataSourceBuffer::create(buffer), ImageSource::Options(), "jpg"));

read_cleanup:
    if (stream) {
        EdsRelease(stream);
    }

    callback(error, surface);
}

#pragma mark - CALLBACKS

EdsError EDSCALLBACK Camera::handleObjectEvent(EdsUInt32 inEvent, EdsBaseRef inRef, EdsVoid* inContext) {
    Camera* c = (Camera*)inContext;
    CameraRef camera = CameraBrowser::instance()->cameraForPortName(c->getPortName());
    switch (inEvent) {
        case kEdsObjectEvent_DirItemRequestTransfer: {
            EdsDirectoryItemRef directoryItem = (EdsDirectoryItemRef)inRef;
            CameraFileRef file = NULL;
            try {
                file = CameraFile::create(directoryItem);
            } catch (...) {
                EdsRelease(directoryItem);
                break;
            }
            EdsRelease(directoryItem);
            directoryItem = NULL;
            if (camera->mFileAddedHandler) {
                camera->mFileAddedHandler(camera, file);
            }
            break;
        }
        default:
            if (inRef) {
                EdsRelease(inRef);
                inRef = NULL;
            }
            break;
    }
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK Camera::handlePropertyEvent(EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext) {
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK Camera::handleStateEvent(EdsUInt32 inEvent, EdsUInt32 inParam, EdsVoid* inContext) {
    Camera* c = (Camera*)inContext;
    CameraRef camera = CameraBrowser::instance()->cameraForPortName(c->getPortName());
    switch (inEvent) {
        case kEdsStateEvent_WillSoonShutDown:
            if (camera->mHasOpenSession && camera->mShouldKeepAlive) {
                EdsError error = EdsSendCommand(camera->mCamera, kEdsCameraCommand_ExtendShutDownTimer, 0);
                if (error != EDS_ERR_OK) {
                    console() << "ERROR - failed to extend shut down timer" << std::endl;
                }
            }
            break;
        case kEdsStateEvent_Shutdown:
            camera->requestCloseSession();
            // send handler and browser removal notices
            if (camera->mRemovedHandler) {
                camera->mRemovedHandler(camera);
            }
            CameraBrowser::instance()->removeCamera(camera);
            break;
        default:
            break;
    }
    return EDS_ERR_OK;
}

}}
