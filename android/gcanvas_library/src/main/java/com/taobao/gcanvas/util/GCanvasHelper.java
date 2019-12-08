/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas.util;

import android.util.Log;

import com.taobao.gcanvas.GCanvas;
import com.taobao.gcanvas.GCanvas.ViewMode;
import com.taobao.gcanvas.util.GLog;

import org.json.JSONArray;
import org.json.JSONException;

public class GCanvasHelper {

	/**
	 * Parse the js command args to a JsonArray Object
	 *
	 * @param command
	 * @param params
     * @return
     */
	public static JSONArray argsToJsonArray(String command, String params) {
		JSONArray args = null;
		try {
			if (params.equals("{}")) {
				args = new JSONArray();
			}
			else if (params.startsWith("\"") && params.endsWith("\"")) {
				args = new JSONArray();
				args.put(params.substring(1, params.length() - 1));
			}
			// this is hard code to optimize the performance .
			// parsing json costs much time.
			// TODO: need to find a graceful solution later.
			else if (command.equals("render") && params.startsWith("[\"")) {
				args = new JSONArray();
				args.put(params.substring(2, params.length() - 2));
			}
			else {
				args = new JSONArray(params);
			}
		} catch (JSONException e) {
			Log.w("GCANVAS", "fail to parse params:" + params);
			args = new JSONArray();
		}

		return args;
	}

	/**
	 * Try to parse a string content to GCanvas.ViewMode
	 *
	 * @param mode
	 * @return
     */
	public static ViewMode parseViewModeString(String mode) {
		if (mode == null) {
			return GCanvas.DEFAULT_VIEW_MODE;
		} else if (mode.equals("hybrid")) {
			return ViewMode.HYBRID_MODE;
		} else if (mode.equals("default")) {
			return GCanvas.DEFAULT_VIEW_MODE;
		} else if (mode.equals("canvas")) {
			return ViewMode.SINGLE_CANVAS_MODE;
		} else if (mode.equals("switch")) {
			return ViewMode.SWITCH_MODE;
		} else if (mode.equals("float")) {
			return ViewMode.FLOAT_HYBRID_MODE;
		} else {
			GLog.w("CANVAS", "cannot parse the view mode, mode:" + mode);
			return GCanvas.DEFAULT_VIEW_MODE;
		}
	}

}
