//
//  Camera.cpp
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#include "Camera.h"

using namespace ci;
using namespace ci::app;

namespace Cinder { namespace EDSDK {

CameraRef Camera::create(EdsCameraRef camera) {
    return CameraRef(new Camera(camera))->shared_from_this();
}

Camera::Camera(EdsCameraRef camera) {
    if (camera == NULL) {
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
    mHandler = NULL;

    if (mHasOpenSession) {
        requestCloseSession();
    }

    EdsRelease(mCamera);
    mCamera = NULL;
}

#pragma mark -

CameraHandler* Camera::getHandler() const {
    return mHandler;
}

void Camera::setHandler(CameraHandler* handler) {
    mHandler = handler;
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

EdsError Camera::requestOpenSession(SessionSettings* settings) {
    if (mHasOpenSession) {
        return EDS_ERR_SESSION_ALREADY_OPEN;
    }

    EdsError error = EdsOpenSession(mCamera);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to open camera session" << std::endl;
        return error;
    }
    mHasOpenSession = true;

    EdsUInt32 saveTo = (settings != NULL) ? settings->getPictureSaveLocation() : kEdsSaveTo_Host;
    error = EdsSetPropertyData(mCamera, kEdsPropID_SaveTo, 0, sizeof(saveTo) , &saveTo);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set save destination host/device" << std::endl;
        return error;
    }

    if (saveTo == kEdsSaveTo_Host || saveTo == kEdsSaveTo_Camera) {
        // TODO - EdsSetCapacity
    }

    mShouldKeepAlive = (settings != NULL) ? settings->getShouldKeepAlive() : mShouldKeepAlive;

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

EdsError Camera::requestTakePicture(/*Options* options*/) {
    return EDS_ERR_UNIMPLEMENTED;
}

EdsError Camera::requestDownloadFile() {
    return EDS_ERR_UNIMPLEMENTED;
}

EdsError Camera::requestReadFile() {
    return EDS_ERR_UNIMPLEMENTED;
}

#pragma mark - CALLBACKS

EdsError EDSCALLBACK Camera::handleObjectEvent(EdsUInt32 inEvent, EdsBaseRef inRef, EdsVoid* inContext) {
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK Camera::handlePropertyEvent(EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext) {
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK Camera::handleStateEvent(EdsUInt32 inEvent, EdsUInt32 inParam, EdsVoid* inContext) {
    Camera* camera = (Camera*)inContext;
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
            camera->mHasOpenSession = false;
            camera->mHandler->didRemoveCamera(camera);
            // TODO - how do we get this to the CameraBrowser, singleton?
            break;
        default:
            break;
    }

    return EDS_ERR_OK;
}

}}
