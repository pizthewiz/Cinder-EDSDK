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
using namespace std;

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
        console() << "ERROR - failed to get device info" << endl;
    }
}

Camera::~Camera() {
    // TODO - disconnect handler

    EdsRelease(mCamera);
    mCamera = NULL;
}

}}
