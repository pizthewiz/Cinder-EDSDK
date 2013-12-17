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

static EdsError handleCameraAdded(EdsVoid* inContext) {
    // TODO - which camera?
//    CameraBrowser* cameraBrowser = (CameraBrowser*)inContext;
    return EDS_ERR_OK;
}

CameraBrowserRef CameraBrowser::create() {
	return CameraBrowserRef(new CameraBrowser())->shared_from_this();
}

CameraBrowser::CameraBrowser() {
//    mIsBrowsing = false;

    EdsError error = EdsInitializeSDK();
    if (error != EDS_ERR_OK) {
        throw Exception();
    }
}

CameraBrowser::~CameraBrowser() {
    // TODO - disconnect handler

    mCameras.clear();

    EdsError error = EdsTerminateSDK();
    if (error != EDS_ERR_OK) {
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

    EdsCameraListRef cameraList = NULL;
    EdsError error = EdsGetCameraList(&cameraList);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get camera list" << endl;
        EdsRelease(cameraList);
        return;
    }

	EdsUInt32 count = 0;
    error = EdsGetChildCount(cameraList, &count);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed to get camera count" << endl;
        EdsRelease(cameraList);
        return;
    }

    for (uint32_t idx = 0; idx <count; idx++) {
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

        mCameras.push_back(camera);

        // TODO - didAddCamera
    }

    EdsRelease(cameraList);

    // TODO - didEnumerateCameras

    error = EdsSetCameraAddedHandler(handleCameraAdded, this);
    if (error != EDS_ERR_OK) {
        console() << "ERROR - failed set camera added handler" << endl;
    }
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

}}
