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
    // TODO - disconnect handler

    if (mHasOpenSession) {
        requestCloseSession();
    }

    EdsRelease(mCamera);
    mCamera = NULL;
}

#pragma mark -

std::string Camera::getName() const {
    return std::string(mDeviceInfo.szPortName);
}

bool Camera::hasOpenSession() const {
    return mHasOpenSession;
}

void Camera::requestOpenSession() {
    if (mHasOpenSession) {
        return;
    }

    EdsError error = EdsOpenSession(mCamera);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to open camera session" << std::endl;
    }

    mHasOpenSession = (error == EDS_ERR_OK);
    // TODO - didOpenSessionWithError

    // TODO - could return error instead of didOpenSessionWithError
}

void Camera::requestCloseSession() {
    if (!mHasOpenSession) {
        return;
    }

    EdsError error = EdsCloseSession(mCamera);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to close camera session" << std::endl;
    }

    mHasOpenSession = !(error == EDS_ERR_OK);
    // TODO - didCloseSessionWithError

    // TODO - could return error instead of didCloseSessionWithError
}

#pragma mark - CALLBACKS

EdsError EDSCALLBACK Camera::handleObjectEvent(EdsUInt32 inEvent, EdsBaseRef inRef, EdsVoid* inContext) {
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK Camera::handlePropertyEvent(EdsUInt32 inEvent, EdsUInt32 inPropertyID, EdsUInt32 inParam, EdsVoid* inContext) {
    return EDS_ERR_OK;
}

EdsError EDSCALLBACK Camera::handleStateEvent(EdsUInt32 inEvent, EdsUInt32 inParam, EdsVoid* inContext) {
    return EDS_ERR_OK;
}

}}
