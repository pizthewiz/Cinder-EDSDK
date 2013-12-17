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
using namespace std;

namespace Cinder { namespace EDSDK {

CameraBrowserRef CameraBrowser::create() {
	return CameraBrowserRef(new CameraBrowser())->shared_from_this();
}

CameraBrowser::CameraBrowser() {
//    mIsBrowsing = false;

    EdsError error = EdsInitializeSDK();
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to initialize SDK" << endl;
        throw Exception();
    }
}

CameraBrowser::~CameraBrowser() {
    // TODO - disconnect handler

    mCameras.clear();

    EdsError error = EdsTerminateSDK();
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to terminate SDK cleanly" << endl;
        throw Exception();
    }
}

//bool CameraBrowser::isBrowsing() const {
//    return mIsBrowsing;
//}

void CameraBrowser::start() {
//    if (mIsBrowsing) {
//        return;
//    }
//
//    mIsBrowsing = true;

    EdsError error = EdsSetCameraAddedHandler(CameraBrowser::handleCameraAdded, this);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to set camera added handler" << endl;
    }

    enumerateCameraList();
    // TODO - didEnumerateCameras
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

const vector<CameraRef>& CameraBrowser::getCameras() const {
    return mCameras;
}

#pragma mark - PRIVATE

void CameraBrowser::enumerateCameraList() {
    EdsCameraListRef cameraList = NULL;
    EdsError error = EdsGetCameraList(&cameraList);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get camera list" << endl;
        EdsRelease(cameraList);
        return;
    }

	EdsUInt32 cameraCount = 0;
    error = EdsGetChildCount(cameraList, &cameraCount);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get camera count" << endl;
        EdsRelease(cameraList);
        return;
    }

    for (uint32_t idx = 0; idx < cameraCount; idx++) {
        EdsCameraRef cam = NULL;
        error = EdsGetChildAtIndex(cameraList, idx, &cam);
        if (error != EDS_ERR_OK) {
            console() << "ERROR - failed to get camera: " << idx << endl;
            continue;
        }

        CameraRef camera = NULL;
        try {
            CameraRef camera = Camera::create(cam);
        } catch (...) {
            EdsRelease(cam);
            continue;
        }
        EdsRelease(cam);

        // add if previously unknown
        if (any_of(mCameras.begin(), mCameras.end(), [camera](CameraRef c) { return strcmp(c->mDeviceInfo.szPortName, camera->mDeviceInfo.szPortName) == 0; })) {
            mCameras.push_back(camera);

            // TODO - didAddCamera
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
