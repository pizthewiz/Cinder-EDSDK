# Cinder-EDSDK
`Cinder-EDSDK` is a [CinderBlock](http://libcinder.org/) to interact with Canon cameras. See Canon's [compatibility chart](http://www.usa.canon.com/cusa/consumer/standard_display/sdk_homepage#SDKQ7) for a list of supported cameras.

### FEATURES
Image capture to memory or on-disk, camera keep-alive, simultaneous control of multiple cameras, camera connect and disconnect callbacks…

### EXAMPLE
```C++
void CaptureApp::setup() {
    mCameraBrowser = CameraBrowser::instance();
    mCamera.setHandler(this);
    mCameraBrowser->start();
}

void CaptureApp::didAddCamera(CameraBrowser* cameraBrowser, CameraRef camera) {
    mCamera = camera;
    mCamera.setHandler(this);
    EdsError error = mCamera->requestOpenSession();
    if (error == EDS_ERR_OK) {
        mCamera->requestTakePicture();
    }
}

void CaptureApp::didAddFile(Cinder::EDSDK::Camera* camera, CameraFileRef file) {
    fs::path destinationFolderPath = expandPath(fs::path("~/Desktop/Captures"));
    camera->requestDownloadFile(file, destinationFolderPath, [this](EdsError error, fs::path outputFilePath) {
        if (error == EDS_ERR_OK) {
            console() << "downloaded image to: " << outputFilePath << endl;
        }
    });
}
```

### INTEGRATION
- Apply for EDSDK access from [Canon USA](http://www.usa.canon.com/cusa/consumer/standard_display/sdk_homepage) or [Canon Europa](https://www.didp.canon-europa.com) (Serving Europe, Africa and The Middle East)
- Wait days / weeks / months
- Download
- Unarchive
- Move the _EDSDK_ folder into _Cinder-EDSDK/lib_

Cinder-EDSDK has only been tested with the dev branch of the [Cinder repository](https://github.com/Cinder/Cinder) on OS X 10.9.1 with EDSDK 2.13.2. The application target must be built i386 as the EDSDK provided frameworks are not yet both built x86_64. Please see the [TODO](TODO.md) for the project status.

### GREETZ
- [Red Paper Heart](http://www.redpaperheart.com)'s [Cinder-CanonSdk](https://github.com/redpaperheart/Cinder-CanonSdk)
- [v002-Camera-Live](https://github.com/v002/v002-Camera-Live)
