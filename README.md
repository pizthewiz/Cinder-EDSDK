# Cinder-EDSDK
`Cinder-EDSDK` is a [CinderBlock](http://libcinder.org/) to interact with Canon cameras. See Canon's [compatibility chart](http://www.usa.canon.com/cusa/consumer/standard_display/sdk_homepage#SDKQ7) for a list of supported cameras.

### FEATURES
Image capture to memory or on-disk, camera keep-alive, simultaneous control of multiple cameras, camera connect and disconnect handling. It does *not* offer Live View support.

### EXAMPLE
```C++
void CaptureApp::setup() {
    CameraBrowser::instance()->connectAddedHandler(&CaptureApp::browserDidAddCamera, this);
    CameraBrowser::instance()->start();
}

void CaptureApp::browserDidAddCamera(CameraRef camera) {
    mCamera = camera;
    mCamera->connectFileAddedHandler(&CaptureApp::didAddFile, this);
    EdsError error = mCamera->requestOpenSession();
    if (error == EDS_ERR_OK) {
        mCamera->requestTakePicture();
    }
}

void CaptureApp::didAddFile(CameraRef camera, CameraFileRef file) {
    fs::path destinationFolderPath = expandPath(fs::path("~/Desktop/Captures"));
    camera->requestDownloadFile(file, destinationFolderPath, [this](EdsError error, fs::path outputFilePath) {
        if (error == EDS_ERR_OK) {
            console() << "downloaded image to: " << outputFilePath << endl;
        }
    });
}
```

### INTEGRATION
- Apply for EDSDK access from [Canon USA](http://www.usa.canon.com/cusa/consumer/standard_display/sdk_homepage) or [Canon Europa](https://www.didp.canon-europa.com) (serving Europe, Africa and The Middle East)
- Wait days / weeks / months
- Download
- Unarchive
- Move the _EDSDK_ folder into _Cinder-EDSDK/lib_

Cinder-EDSDK has only been tested with the master branch of the [Cinder repository](https://github.com/Cinder/Cinder) on OS X 10.10.2 with EDSDK 2.15. The application target must be built `i386` as the EDSDK-provided frameworks are not yet both built `x86_64`. Please see the [TODO](TODO.md) for the project status.

*NOTE* - Due to a bug introduced in EDSDK 2.10, the `CameraBrowser` will only call the camera removed handler for cameras with an open session.

### GREETZ
- [Red Paper Heart](http://www.redpaperheart.com)'s [Cinder-CanonSdk](https://github.com/redpaperheart/Cinder-CanonSdk)
- [v002-Camera-Live](https://github.com/v002/v002-Camera-Live)
