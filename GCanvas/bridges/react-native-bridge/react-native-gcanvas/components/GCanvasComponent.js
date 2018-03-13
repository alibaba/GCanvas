import React, { Component, PropTypes } from 'react';
import { requireNativeComponent, View } from 'react-native';

var RCTGCanvasView = requireNativeComponent('RCTGCanvasView', GCanvasView);

export default class GCanvasView extends Component {
	static propTypes = {
		// isOffscreen: PropTypes.bool,
		...View.propTypes
	};

	render() {
		return ( <RCTGCanvasView {...this.props} /> );
	};
}