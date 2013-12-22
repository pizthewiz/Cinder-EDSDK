//
//  CameraBrowser.cpp
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#include "CameraBrowser.h"

using namespace ci;
using namespace ci::app;

namespace Cinder { namespace EDSDK {

CameraBrowserRef CameraBrowser::create(CameraBrowserHandler* handler) {
	return CameraBrowserRef(new CameraBrowser(handler))->shared_from_this();
}

CameraBrowser::CameraBrowser(CameraBrowserHandler* handler) {
    mIsBrowsing = false;

    mHandler = handler;

    EdsError error = EdsInitializeSDK();
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to initialize SDK" << std::endl;
        throw Exception();
    }
}

CameraBrowser::~CameraBrowser() {
    // TODO - disconnect handler

    mCameras.clear();

    EdsError error = EdsTerminateSDK();
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to terminate SDK cleanly" << std::endl;
        throw Exception();
    }
}

//bool CameraBrowser::isBrowsing() const {
//    return mIsBrowsing;
//}

void CameraBrowser::start() {
    if (mIsBrowsing) {
        return;
    }

    mIsBrowsing = true;

    EdsError error = EdsSetCameraAddedHandler(CameraBrowser::handleCameraAdded, this);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set camera added handler" << std::endl;
    }

    enumerateCameraList();
    mHandler->didEnumerateCameras((CameraBrowserRef)this);
}

//void CameraBrowser::stop() {
//    if (!mIsBrowsing) {
//        return;
//    }
//
//    // TODO - stop
//
//    mIsBrowsing = false;
//}

const std::vector<CameraRef>& CameraBrowser::getCameras() const {
    return mCameras;
}

#pragma mark - PRIVATE

void CameraBrowser::enumerateCameraList() {
    EdsCameraListRef cameraList = NULL;
    EdsError error = EdsGetCameraList(&cameraList);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get camera list" << std::endl;
        EdsRelease(cameraList);
        return;
    }

	EdsUInt32 cameraCount = 0;
    error = EdsGetChildCount(cameraList, &cameraCount);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get camera count" << std::endl;
        EdsRelease(cameraList);
        return;
    }

    for (uint32_t idx = 0; idx < cameraCount; idx++) {
        EdsCameraRef cam = NULL;
        error = EdsGetChildAtIndex(cameraList, idx, &cam);
        if (error != EDS_ERR_OK) {
            console() << "ERROR - failed to get camera: " << idx << std::endl;
            continue;
        }

        CameraRef camera = NULL;
        try {
            camera = Camera::create(cam);
        } catch (...) {
            EdsRelease(cam);
            continue;
        }
        EdsRelease(cam);

        // add if previously unknown
        if (std::none_of(mCameras.begin(), mCameras.end(), [camera](CameraRef c) { return c->getPortName().compare(camera->getPortName()) == 0; })) {
            mCameras.push_back(camera);
            mHandler->didAddCamera((CameraBrowserRef)this, camera);
        }
    }

    EdsRelease(cameraList);
}

#pragma mark - CALLBACKS

EdsError EDSCALLBACK CameraBrowser::handleCameraAdded(EdsVoid* inContext) {
    // we are left to our own devices to determine which camera was added
    ((CameraBrowser*)inContext)->enumerateCameraList();
    return EDS_ERR_OK;
}

}}
