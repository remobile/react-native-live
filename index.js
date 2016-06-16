'use strict';

var React = require('react');
var ReactNative = require('react-native');
var {
    PropTypes,
} = React;
var {
    requireNativeComponent
} = ReactNative;

var Live = React.createClass({
    propTypes: {
        text : PropTypes.string.isRequired,
        marqueeType : PropTypes.number,
        scrollDuration : PropTypes.number,
        fadeLength : PropTypes.number,
        leadingBuffer : PropTypes.number,
        trailingBuffer : PropTypes.number,
        textColor : PropTypes.string,
        font: PropTypes.string
    },
    render: function() {
        return (
            <RCTLive  {...this.props}/>
        );
    }
});

var RCTLive = requireNativeComponent('RCTLive', Live);
module.exports = Live;
