# Cinder-EDSDK
`Cinder-EDSDK` is a [CinderBlock](http://libcinder.org/) to interact with a Canon DSLR.

## FEATURES
Image capture to memory or on-disk, camera keep-alive, simultaneous control of multiple cameras, camera connect and disconnect callbacks…

## EXAMPLE
```C++
void CaptureApp::setup() {
    mCameraBrowser = Cinder::EDSDK::CameraBrowser::create(this);
    mCameraBrowser->start();
}

void CaptureApp::didAddCamera(Cinder::EDSDK::CameraBrowser* cameraBrowser, Cinder::EDSDK::CameraRef camera) {
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

Download the EDSDK from Canon, unarchive and move the _EDSDK_ folder into _Cinder-EDSDK/lib_.

Only tested with Cinder 0.8.6-dev on OS X 10.9 with EDSDK 2.13.2. The application target must be built i386, Canon's libraries are not yet built 64-bit.

## GREETZ
- [Red Paper Heart](http://www.redpaperheart.com)'s [Cinder-CanonSdk](https://github.com/redpaperheart/Cinder-CanonSdk)
- [v002-Camera-Live](https://github.com/v002/v002-Camera-Live)
