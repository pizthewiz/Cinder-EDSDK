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

class CameraBrowser : public std::enable_shared_from_this<CameraBrowser> {
public:
    static CameraBrowserRef create();
	~CameraBrowser();

protected:
    CameraBrowser();

    std::vector<CameraRef> mCameras;
};

}}
