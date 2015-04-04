# Cinder-EDSDK
`Cinder-EDSDK` is a [CinderBlock](http://libcinder.org/docs/welcome/CinderBlocks.html) to interact with Canon cameras. See Canon's [compatibility chart](http://www.usa.canon.com/cusa/consumer/standard_display/sdk_homepage#SDKQ7) for a list of cameras supported in their EDSDK.

`Cinder-EDSDK` is configured for Cinder's [glNext branch](https://github.com/Cinder/Cinder/tree/glNext), please use rev [0cfeedc4](https://github.com/pizthewiz/Cinder-EDSDK/commit/0cfeedc4aa0d3dc97c3c4208b0f7a973c188677c) for Cinder 0.8.6 support.

### FEATURES
Image capture to memory or on-disk, camera keep-alive, simultaneous control of multiple cameras, Live View and camera connect and disconnect handling.

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
1. Apply for EDSDK access from [Canon USA](http://www.usa.canon.com/cusa/consumer/standard_display/sdk_homepage) or [Canon Europa](https://www.didp.canon-europa.com) (serving Europe, Africa and The Middle East)
1. Wait hours / days / weeks / months
1. Download the EDSDK
1. Unarchive
1. Move the _EDSDK_ folder into _Cinder-EDSDK/lib_
1. Replace _Cinder-EDSDK/lib/EDSDK/Framework/EDSDK.framework_ with _EDSDK vX.Y.Z/EDSDK_64/EDSDK.framework_ to allow applications to be built `x86_64`

*NOTE* - Due to a bug introduced in EDSDK v2.10, the `CameraBrowser` will only call the camera removed handler for cameras with an open session.

### GREETZ
- [@hezhao](https://github.com/hezhao) for contributing Live View support
- [Red Paper Heart](http://www.redpaperheart.com)'s [Cinder-CanonSdk](https://github.com/redpaperheart/Cinder-CanonSdk)
- [v002-Camera-Live](https://github.com/v002/v002-Camera-Live)
