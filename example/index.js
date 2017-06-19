'use strict';

var React = require('react');
var ReactNative = require('react-native');
var {
    StyleSheet,
    View,
    TouchableOpacity,
    Text,
    Image,
    TextInput,
    ScrollView,
} = ReactNative;

import Live from '@remobile/react-native-live';
var SplashScreen = require('@remobile/react-native-splashscreen');

module.exports = React.createClass({
    componentWillMount() {
        SplashScreen.hide();
    },
    render() {
        return (
            <View style={styles.container}>
                <Live style={styles.marqueeLabel}  />
            </View>
        );
    }
});

const styles = StyleSheet.create({
    container: {
        flex: 1
    },
    marqueeLabel: {
        backgroundColor: 'red',
        width:sr.w,
        height:sr.h,
    }
});
