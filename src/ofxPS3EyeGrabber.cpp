// =============================================================================
//
// Copyright (c) 2014-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofxPS3EyeGrabber.h"
#include "ofConstants.h"


const int ofxPS3EyeGrabber::ITUR_BT_601_CY = 1220542;
const int ofxPS3EyeGrabber::ITUR_BT_601_CUB = 2116026;
const int ofxPS3EyeGrabber::ITUR_BT_601_CUG = -409993;
const int ofxPS3EyeGrabber::ITUR_BT_601_CVG = -852492;
const int ofxPS3EyeGrabber::ITUR_BT_601_CVR = 1673527;
const int ofxPS3EyeGrabber::ITUR_BT_601_SHIFT = 20;


void ofxPS3EyeGrabber::yuv422_to_rgba8888(const uint8_t* yuv_src,
                                          const int stride,
                                          uint8_t* dst,
                                          const int width,
                                          const int height)
{
    const int bIdx = 2;
    const int uIdx = 0;
    const int yIdx = 0;

    const int uidx = 1 - yIdx + uIdx * 2;
    const int vidx = (2 + uidx) % 4;

#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)

    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        uint8_t* row = dst + (width * 4) * j; // 4 channels

        for (int i = 0; i < 2 * width; i += 4, row += 8)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;

            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
            row[3]      = (0xff);

            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[6-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[5]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[4+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
            row[7]      = (0xff);
        }
    }

#undef _max
#undef _saturate

}


void ofxPS3EyeGrabber::yuv422_to_rgb888(const uint8_t* yuv_src,
                                        const int stride,
                                        uint8_t* dst,
                                        const int width,
                                        const int height)
{
    const int bIdx = 2;
    const int uIdx = 0;
    const int yIdx = 0;

    const int uidx = 1 - yIdx + uIdx * 2;
    const int vidx = (2 + uidx) % 4;

#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)

    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        uint8_t* row = dst + (width * 3) * j; // 3 channels

        for (int i = 0; i < 2 * width; i += 4, row += 6)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;


            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;

            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);

            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[5-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[4]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[3+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
        }
    }

#undef _max
#undef _saturate

}


ofxPS3EyeGrabber::ofxPS3EyeGrabber()
{
    ofAddListener(ofEvents().exit, this, &ofxPS3EyeGrabber::exit);
}


ofxPS3EyeGrabber::~ofxPS3EyeGrabber()
{
    ofRemoveListener(ofEvents().exit, this, &ofxPS3EyeGrabber::exit);
    stop();
    _cam.reset();
}


void ofxPS3EyeGrabber::exit(ofEventArgs& args)
{
    stop();
}


void ofxPS3EyeGrabber::start()
{
    if (_cam)
    {
        _cam->start();
        startThread();
    }
}


void ofxPS3EyeGrabber::stop()
{
    stopThread();

    if (_cam)
    {
        _cam->stop();
    }
}


void ofxPS3EyeGrabber::threadedFunction()
{
    while (isThreadRunning())
    {
        if (_cam)
        {
            bool res = _cam->updateDevices();

            if (!res)
            {
                break;
            }
        }
    }
}


std::vector<ofVideoDevice> ofxPS3EyeGrabber::listDevices() const
{
    std::vector<ofVideoDevice> devices;

    const auto& eyeDevices = ps3eye::PS3EYECam::getDevices();

    auto iter = eyeDevices.begin();

    int id = 0;

    while (iter != eyeDevices.end())
    {
        ofVideoDevice device;
        device.id = id++;
        device.deviceName = "PS3-Eye";
        device.hardwareName = "None"; // TODO: get from libusb.
        int bus = libusb_get_bus_number((*iter)->device_);
        int port = libusb_get_port_number((*iter)->device_);
        int address = libusb_get_device_address((*iter)->device_);
        device.serialID = "BUS:" + ofToString(bus)+ ", PORT:"  + ofToString(port)  + ", DADDRESS:"  + ofToString(address)  ; // TODO: get from libusb.
        // device.formats.push_back(...) // Formats ... we could list all of them ... but ...
        device.bAvailable = !(*iter)->isStreaming();;
        devices.push_back(device);
        ++iter;
    }

    return devices;
}

void ofxPS3EyeGrabber::setBusNumber(int busNumber){
    this->_bus_number = busNumber;
}
void ofxPS3EyeGrabber::setPortNumber(int portNumber){
    this->_port_number = portNumber;
}

void ofxPS3EyeGrabber::setDeviceAddress(int deviceAddress){
    this->_device_address = deviceAddress;
}

bool ofxPS3EyeGrabber::setup(int w, int h)
{
    if (!_cam)
    {
        const auto& eyeDevices = ps3eye::PS3EYECam::getDevices();

        if(this->_bus_number==0) {
        
            if (_deviceId < eyeDevices.size())
            {
                _cam = eyeDevices[_deviceId];

                bool success = _cam->init(w, h, _requestedFrameRate);

                if (success)
                {
                    start();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                ofLogWarning("ofxPS3EyeGrabber::setup") << "Device id is out of range: " << _deviceId;
                return false;
            }
        }
        else{
            bool cameraFound = false;
            int cameraIndex = 0;
            
            while (cameraIndex<eyeDevices.size() && !cameraFound){
                std::shared_ptr<ps3eye::PS3EYECam> auxCam = eyeDevices[cameraIndex];
                uint8_t busNumber=0;
                busNumber = auxCam->getBusNumber();
                int busNumberInt = busNumber;
                
                uint8_t portNumber = 0;
                portNumber = auxCam->getPortNumber();
                int portNumberInt = portNumber;

                if(this->_bus_number == busNumberInt && this->_port_number == portNumberInt){
                    
                    cameraFound = true;
                    
                }
                else{
                    //continue here
                    cameraIndex += 1;
                }
                
            }
            
            if(cameraFound){
                _cam = eyeDevices[cameraIndex];
                
                bool success = _cam->init(w, h, _requestedFrameRate);
                
                if (success)
                {
                    start();
                    return true;
                }
                else
                {
                    return false;
                }


            }
            else{
                ofLogWarning("ofxPS3EyeGrabber::setup") << "Device bus, port, and address not found ";
                return false;
            }
        }
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setup") << "Camera is already initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::update()
{
    _isFrameNew = false;

    if (_cam)
    {
        if (_cam->isNewFrame())
        {
            if (_pixelFormat == OF_PIXELS_RGB)
            {
                if (!_pixels.isAllocated() || _pixels.getPixelFormat() != OF_PIXELS_RGB)
                {
                    _pixels.allocate(_cam->getWidth(),
                                     _cam->getHeight(),
                                     OF_PIXELS_RGB);
                }

                yuv422_to_rgb888(_cam->getLastFramePointer(),
                                 _cam->getRowBytes(),
                                 _pixels.getData(),
                                 _cam->getWidth(),
                                 _cam->getHeight());
            }
            else if (_pixelFormat == OF_PIXELS_RGBA)
            {
                if (!_pixels.isAllocated() || _pixels.getPixelFormat() != OF_PIXELS_RGBA)
                {
                    _pixels.allocate(_cam->getWidth(),
                                     _cam->getHeight(),
                                     OF_PIXELS_RGBA);
                }

                yuv422_to_rgba8888(_cam->getLastFramePointer(),
                                   _cam->getRowBytes(),
                                   _pixels.getData(),
                                   _cam->getWidth(),
                                   _cam->getHeight());
            }
            else
            {
                // Set from native pixels requires no extra copying or
                // colorspace conversion.
                _pixels.setFromExternalPixels(_cam->getLastFramePointer(),
                                              _cam->getWidth(),
                                              _cam->getHeight(),
                                              OF_PIXELS_YUY2);
            }

            _isFrameNew = true;

        }
    }
}


bool ofxPS3EyeGrabber::isFrameNew() const
{
    return _isFrameNew;
}


bool ofxPS3EyeGrabber::isInitialized() const
{
  return _cam != nullptr;
}


ofPixels& ofxPS3EyeGrabber::getPixels()
{
    return _pixels;
}


const ofPixels& ofxPS3EyeGrabber::getPixels() const
{
    return _pixels;
}


void ofxPS3EyeGrabber::close()
{
    stopThread();
}


float ofxPS3EyeGrabber::getHeight() const
{
    if (_cam)
    {
        return _cam->getHeight();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getHeight") << "Camera is not initialized.";
        return 0;
    }
}


float ofxPS3EyeGrabber::getWidth() const
{
    if (_cam)
    {
        return _cam->getWidth();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getWidth") << "Camera is not initialized.";
        return 0;
    }
}


bool ofxPS3EyeGrabber::setPixelFormat(ofPixelFormat pixelFormat)
{
    if (pixelFormat == OF_PIXELS_NATIVE)
    {
        _pixelFormat = OF_PIXELS_YUY2;
        return true;
    }
    else if (pixelFormat == OF_PIXELS_RGB
          || pixelFormat == OF_PIXELS_RGBA
          || pixelFormat == OF_PIXELS_YUY2)
    {
        _pixelFormat = pixelFormat;
        return true;
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setPixelFormat") << "setPixelFormat(): requested pixel format " << pixelFormat << " not supported";
        return false;
    }
}


ofPixelFormat ofxPS3EyeGrabber::getPixelFormat() const
{
    return _pixelFormat;
}


void ofxPS3EyeGrabber::setVerbose(bool verbose)
{
    ofLogWarning("ofxPS3EyeGrabber::setVerbose") << "Not implemented.";
}


void ofxPS3EyeGrabber::setDeviceID(int deviceId)
{
    _deviceId = deviceId;
}


void ofxPS3EyeGrabber::setDesiredFrameRate(int framerate)
{
    _requestedFrameRate = framerate;
}


void ofxPS3EyeGrabber::videoSettings()
{
    ofLogWarning("ofxPS3EyeGrabber::setDeviceID") << "Not implemented.  Call settings (e.g. setAutogain()) directly.";
}


bool ofxPS3EyeGrabber::getAutogain() const
{
    if (_cam)
    {
        return _cam->getAutogain();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getAutogain") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setAutogain(bool val)
{
    if (_cam)
    {
        _cam->setAutogain(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setAutogain") << "Camera is not initialized.";
    }
}


bool ofxPS3EyeGrabber::getAutoWhiteBalance() const
{
    if (_cam)
    {
        return _cam->getAutoWhiteBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getAutoWhiteBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setAutoWhiteBalance(bool val)
{
    if (_cam)
    {
        _cam->setAutoWhiteBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setAutoWhiteBalance") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getGain() const
{
    if (_cam)
    {
        return _cam->getGain();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getGain") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setGain(uint8_t val)
{
    if (_cam)
    {
        _cam->setGain(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setGain") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getExposure() const
{
    if (_cam)
    {
        return _cam->getExposure();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getExposure") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setExposure(uint8_t val)
{
    if (_cam)
    {
        _cam->setExposure(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setExposure") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getSharpness() const
{
    if (_cam)
    {
        return _cam->getSharpness();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getSharpness") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setSharpness(uint8_t val)
{
    if (_cam)
    {
        _cam->setSharpness(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setSharpness") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getContrast() const
{
    if (_cam)
    {
        return _cam->getContrast();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getContrast") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setContrast(uint8_t val)
{
    if (_cam)
    {
        _cam->setContrast(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setContrast") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getBrightness() const
{
    if (_cam)
    {
        return _cam->getBrightness();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getBrightness") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setBrightness(uint8_t val)
{
    if (_cam)
    {
        _cam->setBrightness(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setBrightness") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getHue() const
{
    if (_cam)
    {
        return _cam->getHue();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getHue") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setHue(uint8_t val)
{
    if (_cam)
    {
        _cam->setHue(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setHue") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getRedBalance() const
{
    if (_cam)
    {
        return _cam->getRedBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getRedBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setRedBalance(uint8_t val)
{
    if (_cam)
    {
        _cam->setRedBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setRedBalance") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getBlueBalance() const
{
    if (_cam)
    {
        return _cam->getBlueBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getBlueBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setBlueBalance(uint8_t val)
{
    if (_cam)
    {
        _cam->setBlueBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setBlueBalance") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getGreenBalance() const
{
    if (_cam)
    {
        return _cam->getGreenBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getGreenBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setGreenBalance(uint8_t val)
{
    if (_cam)
    {
        _cam->setGreenBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setGreenBalance") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setVerticalFlip(bool enable)
{
    if (_cam)
    {
        _cam->setVerticalFlip(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setVerticalFlip") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setHorizontalFlip(bool enable)
{
    if (_cam)
    {
        _cam->setHorizontalFlip(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setHorizontalFlip") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setTestPattern(bool enable)
{
    if (_cam)
    {
        _cam->setTestPattern(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setTestPattern") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setLED(bool enable)
{
    if (_cam)
    {
        _cam->setLED(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setFlip") << "Camera is not initialized.";
    }
}



float ofxPS3EyeGrabber::getFPS() const
{
    if (_cam)
    {
        return _cam->getFrameRate();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getFPS") << "Camera is not initialized.";
        return 0;
    }
}


float ofxPS3EyeGrabber::getActualFPS() const
{
    if (_cam)
    {
        return _cam->getActualFrameRate();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getActualFPS") << "Camera is not initialized.";
        return 0;
    }
}
