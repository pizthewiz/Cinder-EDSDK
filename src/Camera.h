//
//  Camera.h
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#pragma once

#ifdef CINDER_MAC
    #define __MACOS__
#else
    #error Target platform unsupported by Cinder-EDSDK
#endif

#include "EDSDK.h"

namespace Cinder { namespace EDSDK {

typedef std::shared_ptr<class Camera> CameraRef;

class Camera : public std::enable_shared_from_this<Camera> {
public:
    static CameraRef create();
	~Camera();

protected:
    Camera();

    EdsCameraRef mCamera;
};

}}
