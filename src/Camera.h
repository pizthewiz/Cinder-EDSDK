//
//  Camera.h
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
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

namespace Cinder { namespace EDSDK {

class CameraBrowser;

// expected device handler functions:
//  didRemoveCamera
//  didOpenSessionWithError
//  didBecomeReady
//  didCloseSessionWithError
//  didAddFile
//  didDownloadFile

typedef std::shared_ptr<class Camera> CameraRef;

class Camera : public std::enable_shared_from_this<Camera> {
public:
    static CameraRef create(EdsCameraRef camera);
	~Camera();

    bool hasOpenSession() const;
    void requestOpenSession();
    void requestCloseSession();

    void requestTakePicture();
    void requestDownloadFile();

private:
    Camera(EdsCameraRef camera);

    EdsCameraRef mCamera;
    EdsDeviceInfo mDeviceInfo;

    friend class CameraBrowser;
};

}}
