package com.remobile.live;

import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.util.Log;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class RCTCamera {
    private  PreviewCallback _previewCallback = null;
    private final Map<Number, Camera> _cameras;
    private Camera _camera;
    private boolean _isStarting;
    private boolean _isStopping;
    private int _width, _height;
    private long _nativeObject = 0;

    public static native void captureVideoStream(long target, byte[] data, int width, int height);

    public RCTCamera() {
        _cameras = new HashMap<>();
        _previewCallback = new PreviewCallback();
    }

    public void attachNativeObject(long nativeObject) {
        _nativeObject = nativeObject;
    }

    public Camera acquireCameraInstance(int type) {
        if (null == _cameras.get(type)) {
            try {
                Camera camera = Camera.open(type);
                _cameras.put(type, camera);
            } catch (Exception e) {
                System.console().printf("acquireCameraInstance: %s", e.getLocalizedMessage());
            }
        }
        return _cameras.get(type);
    }

    public void releaseCameraInstance(int type) {
        if (null != _cameras.get(type)) {
            _cameras.get(type).release();
            _cameras.remove(type);
        }
    }

    synchronized private void openCamera(int type) {
        if (!_isStarting) {
            _isStarting = true;
            try {
                _camera = acquireCameraInstance(type);
                Camera.Parameters parameters = _camera.getParameters();
                // set autofocus
                List<String> focusModes = parameters.getSupportedFocusModes();
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                    parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                }
                _camera.setParameters(parameters);
                _camera.startPreview();
            } catch (NullPointerException e) {
                e.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
                closeCamera(type);
            } finally {
                _isStarting = false;
            }
        }
    }

    synchronized private void closeCamera(int type) {
        if (!_isStopping) {
            _isStopping = true;
            try {
                if (_camera != null) {
                    _camera.stopPreview();
                    releaseCameraInstance(type);
                    _camera = null;
                }

            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                _isStopping = false;
            }
        }
    }

    private Camera.Size getCaptureQuality(Camera camera, String quality) {
        Camera.Size previewSize = null;
        Camera.Parameters parameters = camera.getParameters();
        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
        switch (quality) {
            case "low":
                for (Camera.Size size : sizes) {
                    if (previewSize == null) {
                        previewSize = size;
                    } else {
                        int resultArea = previewSize.width * previewSize.height;
                        int newArea = size.width * size.height;

                        if (newArea < resultArea) {
                            previewSize = size;
                        }
                    }
                }
                break;
            case "medium":
                previewSize = sizes.get(sizes.size() / 2);
                break;
            case "high":
                int width = Integer.MAX_VALUE, height = Integer.MAX_VALUE;
                for (Camera.Size size : sizes) {
                    if (size.width <= width && size.height <= height) {
                        if (previewSize == null) {
                            previewSize = size;
                        } else {
                            int resultArea = previewSize.width * previewSize.height;
                            int newArea = size.width * size.height;

                            if (newArea > resultArea) {
                                previewSize = size;
                            }
                        }
                    }
                }
                break;
        }
        return previewSize;
    }

    public void startCamera(int fps, boolean useFront, String presetType) {
        openCamera(useFront ? Camera.CameraInfo.CAMERA_FACING_FRONT : Camera.CameraInfo.CAMERA_FACING_BACK);
        Camera.Size size = getCaptureQuality(_camera, presetType);
        _width = size.width;
        _height = size.height;
        int frameSize = (_width * _height * 3) / 2;

        Camera.Parameters parameter = _camera.getParameters();
        parameter.setPreviewFpsRange(fps, fps);
        parameter.setPreviewSize(_width, _height);
        parameter.setPreviewFormat(PixelFormat.YCbCr_420_SP);
        _camera.setParameters(parameter);

        _camera.setPreviewCallbackWithBuffer(_previewCallback);
        _camera.addCallbackBuffer(new byte[frameSize]);
        _camera.addCallbackBuffer(new byte[frameSize]);
    }


    public void stopCamera() {
        closeCamera(Camera.CameraInfo.CAMERA_FACING_FRONT);
    }

    private class PreviewCallback implements Camera.PreviewCallback {
        private long lastCaptureTime = 0;
        private long inter = 66;

        PreviewCallback() {
        }

        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            if (data != null) {
                if (lastCaptureTime == 0) {
                    lastCaptureTime = System.currentTimeMillis();
                } else {
                    long interval = (System.currentTimeMillis() - lastCaptureTime);
                    if (System.currentTimeMillis() < lastCaptureTime) {
                        camera.addCallbackBuffer(data);
                        return;
                    } else {
                        lastCaptureTime += inter;
                    }

                    if (interval > 4 * inter) {
                        lastCaptureTime = System.currentTimeMillis();
                    }
                }
                captureVideoStream(_nativeObject, data, _height, _height);
            }
            camera.addCallbackBuffer(data);
        }
    }
}
