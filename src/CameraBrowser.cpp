//
//  CameraBrowser.cpp
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#include "CameraBrowser.h"

namespace Cinder { namespace EDSDK {

CameraBrowserRef CameraBrowser::create() {
	return CameraBrowserRef(new CameraBrowser())->shared_from_this();
}

CameraBrowser::CameraBrowser() {
    mIsBrowsing = false;
}

CameraBrowser::~CameraBrowser() {
    // TODO - disconnect handler

    mCameras.clear();
}

bool CameraBrowser::isBrowsing() const {
    return mIsBrowsing;
}

void CameraBrowser::start() {
    if (mIsBrowsing) {
        return;
    }

    // TODO - start

    mIsBrowsing = true;
}

void CameraBrowser::stop() {
    if (!mIsBrowsing) {
        return;
    }

    // TODO - stop

    mIsBrowsing = false;
}

const std::vector<CameraRef> CameraBrowser::getCameras() const {
    return mCameras;
}

}}
