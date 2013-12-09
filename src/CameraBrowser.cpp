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
}

CameraBrowser::~CameraBrowser() {
}

}}
