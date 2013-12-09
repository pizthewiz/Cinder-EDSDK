//
//  Camera.cpp
//  Cinder-EDSDK
//
//  Created by Jean-Pierre Mouilleseaux on 08 Dec 2013.
//  Copyright 2013 Chorded Constructions. All rights reserved.
//

#include "Camera.h"

namespace Cinder { namespace EDSDK {

CameraRef Camera::create() {
    return CameraRef(new Camera())->shared_from_this();
}

Camera::Camera() {
}

Camera::~Camera() {
}

}}
