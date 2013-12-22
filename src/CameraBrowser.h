//
//  CameraBrowser.h
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#pragma once

#include "Camera.h"

namespace Cinder { namespace EDSDK {

typedef std::shared_ptr<class CameraBrowser> CameraBrowserRef;

class CameraBrowserHandler {
public:
    virtual void didAddCamera(CameraBrowser* browser, CameraRef camera) = 0;
    virtual void didRemoveCamera(CameraBrowser* browser, CameraRef camera) = 0;
    virtual void didEnumerateCameras(CameraBrowser* browser) = 0;
};

class CameraBrowser : public std::enable_shared_from_this<CameraBrowser> {
public:
    static CameraBrowserRef create(CameraBrowserHandler* handler);
	~CameraBrowser();

//    bool isBrowsing() const;
    void start();
//    void stop();

    const std::vector<CameraRef>& getCameras() const;

private:
    CameraBrowser(CameraBrowserHandler* handler);
    void enumerateCameraList();

    static EdsError EDSCALLBACK handleCameraAdded(EdsVoid* inContext);

    CameraBrowserHandler* mHandler;
    std::vector<CameraRef> mCameras;
    bool mIsBrowsing;
};

}}
