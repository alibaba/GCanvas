import React, { Component, PropTypes } from 'react';
import { requireNativeComponent, View, Platform, findNodeHandle } from 'react-native';

var CanvasView = Platform.select({
  ios: requireNativeComponent('RCTGCanvasView', null),
  android: requireNativeComponent('GCanvasView', null)
});

export default class GCanvasView extends Component {
  static propTypes = {
    // isOffscreen: PropTypes.bool,
    ...View.propTypes
  };

  render() {
    return ( <CanvasView {...this.props} /> );
  };
}
