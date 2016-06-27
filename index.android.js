import React, { Component, PropTypes } from 'react';
import {
    NativeAppEventEmitter,
    NativeModules,
    Platform,
    requireNativeComponent,
    View,
} from 'react-native';

const Constants = NativeModules.UIManager.RCTLive.Constants;

function convertNativeProps(props) {
    const newProps = { ...props };
    if (typeof props.aspect === 'string') {
        newProps.aspect = Live.constants.Aspect[props.aspect];
    }

    if (typeof props.flashMode === 'string') {
        newProps.flashMode = Live.constants.FlashMode[props.flashMode];
    }

    if (typeof props.orientation === 'string') {
        newProps.orientation = Live.constants.Orientation[props.orientation];
    }

    if (typeof props.torchMode === 'string') {
        newProps.torchMode = Live.constants.TorchMode[props.torchMode];
    }

    if (typeof props.type === 'string') {
        newProps.type = Live.constants.Type[props.type];
    }

    return newProps;
}

export default class Live extends Component {
    static constants = {
        Aspect: Constants.Aspect,
        Type: Constants.Type,
        CaptureQuality: Constants.CaptureQuality,
        Orientation: Constants.Orientation,
        FlashMode: Constants.FlashMode,
        TorchMode: Constants.TorchMode
    };

    static propTypes = {
        ...View.propTypes,
        aspect: PropTypes.oneOfType([
            PropTypes.string,
            PropTypes.number
        ]),
        defaultOnFocusComponent: PropTypes.bool,
        flashMode: PropTypes.oneOfType([
            PropTypes.string,
            PropTypes.number
        ]),
        keepAwake: PropTypes.bool,
        onFocusChanged: PropTypes.func,
        onZoomChanged: PropTypes.func,
        mirrorImage: PropTypes.bool,
        orientation: PropTypes.oneOfType([
            PropTypes.string,
            PropTypes.number
        ]),
        torchMode: PropTypes.oneOfType([
            PropTypes.string,
            PropTypes.number
        ]),
        type: PropTypes.oneOfType([
            PropTypes.string,
            PropTypes.number
        ])
    };

    static defaultProps = {
        aspect: Constants.Aspect.fill,
        type: Constants.Type.back,
        orientation: Constants.Orientation.auto,
        defaultOnFocusComponent: true,
        flashMode: Constants.FlashMode.off,
        torchMode: Constants.TorchMode.off,
        mirrorImage: false,
    };

    constructor() {
        super();
    }

    render() {
        const nativeProps = convertNativeProps(this.props);
        return <RCTLive {...nativeProps} />;
    }
}

export const constants = Live.constants;

const RCTLive = requireNativeComponent('RCTLive', Live);
