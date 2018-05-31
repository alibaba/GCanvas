(function(){
      
  var createPageHandler = function() {
    return /******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, {
/******/ 				configurable: false,
/******/ 				enumerable: true,
/******/ 				get: getter
/******/ 			});
/******/ 		}
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = 12);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */,
/* 1 */,
/* 2 */,
/* 3 */,
/* 4 */,
/* 5 */,
/* 6 */,
/* 7 */,
/* 8 */,
/* 9 */,
/* 10 */,
/* 11 */,
/* 12 */
/***/ (function(module, exports, __webpack_require__) {

var $app_template$ = __webpack_require__(13)
var $app_script$ = __webpack_require__(14)

$app_define$('@app-component/2d', [], function($app_require$, $app_exports$, $app_module$){
     $app_script$($app_module$, $app_exports$, $app_require$)
     if ($app_exports$.__esModule && $app_exports$.default) {
            $app_module$.exports = $app_exports$.default
        }
     $app_module$.exports.template = $app_template$
})

$app_bootstrap$('@app-component/2d',{ packagerVersion: '0.0.5'})


/***/ }),
/* 13 */
/***/ (function(module, exports) {

module.exports = {
  "type": "div",
  "attr": {
    "id": "test"
  },
  "id": "test",
  "children": [
    {
      "type": "canvas",
      "attr": {
        "id": "canvas_holder"
      },
      "id": "canvas_holder",
      "style": {
        "width": "750px",
        "height": "750px"
      }
    }
  ]
}

/***/ }),
/* 14 */
/***/ (function(module, exports, __webpack_require__) {

module.exports = function(module, exports, $app_require$){"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});

var _typeof = typeof Symbol === "function" && typeof Symbol.iterator === "symbol" ? function (obj) { return typeof obj; } : function (obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj; };

var _gcanvasMin = __webpack_require__(15);

exports.default = {
  start: function start(ref) {
    var ctx = ref.getContext("2d");
    setInterval(function () {
      var startTime = new Date().getTime();
      ctx.fillStyle = "blue";
      ctx.fillRect(0, 0, 100, 100);
      var img = new _gcanvasMin.Image();
      img.src = "https://www.baidu.com/img/bd_logo1.png";
      img.onload = function () {
        ctx.drawImage(img, 100, 100, 400, 400);
      };
      var endTime = new Date().getTime();
      console.log("draw image costing:" + (endTime - startTime) * 0.001);
    }, 16);
  },
  onReady: function onReady() {
    var ref = this.$element('canvas_holder');
    ref = (0, _gcanvasMin.enable)(ref, { bridge: _gcanvasMin.HapBridge, disableAutoSwap: true });
    if (ref) {
      this.start(ref);
    }
  }
};


var moduleOwn = exports.default || module.exports;
var accessors = ['public', 'protected', 'private'];

if (moduleOwn.data && accessors.some(function (acc) {
  return moduleOwn[acc];
})) {
  throw new Error('页面VM对象中的属性data不可与"' + accessors.join(',') + '"同时存在，请使用private替换data名称');
} else if (!moduleOwn.data) {
  moduleOwn.data = {};
  moduleOwn._descriptor = {};
  accessors.forEach(function (acc) {
    var accType = _typeof(moduleOwn[acc]);
    if (accType === 'object') {
      moduleOwn.data = Object.assign(moduleOwn.data, moduleOwn[acc]);
      for (var name in moduleOwn[acc]) {
        moduleOwn._descriptor[name] = { access: acc };
      }
    } else if (accType === 'function') {
      console.warn('页面VM对象中的属性' + acc + '的值不能是函数，请使用对象');
    }
  });
}}

/***/ }),
/* 15 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(module) {var __WEBPACK_AMD_DEFINE_FACTORY__, __WEBPACK_AMD_DEFINE_ARRAY__, __WEBPACK_AMD_DEFINE_RESULT__;

var _typeof = typeof Symbol === "function" && typeof Symbol.iterator === "symbol" ? function (obj) { return typeof obj; } : function (obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj; };

!function (e, t) {
  "object" == ( false ? "undefined" : _typeof(exports)) && "object" == ( false ? "undefined" : _typeof(module)) ? module.exports = t() :  true ? !(__WEBPACK_AMD_DEFINE_ARRAY__ = [], __WEBPACK_AMD_DEFINE_FACTORY__ = (t),
				__WEBPACK_AMD_DEFINE_RESULT__ = (typeof __WEBPACK_AMD_DEFINE_FACTORY__ === 'function' ?
				(__WEBPACK_AMD_DEFINE_FACTORY__.apply(exports, __WEBPACK_AMD_DEFINE_ARRAY__)) : __WEBPACK_AMD_DEFINE_FACTORY__),
				__WEBPACK_AMD_DEFINE_RESULT__ !== undefined && (module.exports = __WEBPACK_AMD_DEFINE_RESULT__)) : "object" == (typeof exports === "undefined" ? "undefined" : _typeof(exports)) ? exports.GCanvas = t() : e.GCanvas = t();
}("undefined" != typeof self ? self : undefined, function () {
  return function (e) {
    function t(a) {
      if (i[a]) return i[a].exports;var n = i[a] = { i: a, l: !1, exports: {} };return e[a].call(n.exports, n, n.exports, t), n.l = !0, n.exports;
    }var i = {};return t.m = e, t.c = i, t.d = function (e, i, a) {
      t.o(e, i) || Object.defineProperty(e, i, { configurable: !1, enumerable: !0, get: a });
    }, t.n = function (e) {
      var i = e && e.__esModule ? function () {
        return e.default;
      } : function () {
        return e;
      };return t.d(i, "a", i), i;
    }, t.o = function (e, t) {
      return Object.prototype.hasOwnProperty.call(e, t);
    }, t.p = "/", t(t.s = 4);
  }([function (e, t, i) {
    "use strict";
    function a(e, t) {
      return e.toLowerCase() + "-" + t;
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.getTransferedObjectUUID = a;
  }, function (e, t, i) {
    "use strict";
    function a(e) {
      return e && e.__esModule ? e : { default: e };
    }function n(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(7),
        s = a(o),
        u = i(8),
        d = a(u),
        c = i(9),
        l = a(c),
        f = function () {
      function e() {
        n(this, e), this._drawCommands = "", this._globalAlpha = 1, this._fillStyle = "rgb(0,0,0)", this._strokeStyle = "rgb(0,0,0)", this._lineWidth = 1, this._lineCap = "butt", this._lineJoin = "miter", this._miterLimit = 10, this._globalCompositeOperation = "source-over", this._textAlign = "start", this._textBaseline = "alphabetic", this._font = "10px sans-serif", this._savedGlobalAlpha = [], this.timer = null, this.componentId = null, this.createRadialGradient = function (e, t, i, a, n, r) {
          return new l.default(e, t, i, a, n, r);
        }, this.quadraticCurveTo = function (e, t, i, a) {
          this._drawCommands = this._drawCommands.concat("u" + e + "," + t + "," + i + "," + a + ";");
        }, this.strokeText = function (e, t, i) {
          var a = e.replace(/!/g, "!!");a = a.replace(/,/g, "!,"), a = a.replace(/;/g, "!;"), this._drawCommands = this._drawCommands.concat("U" + a + "," + t + "," + i + ",0.0;");
        }, this.measureText = function (e) {
          throw new Error("GCanvas not supported yet");
        }, this.isPointInPath = function (e, t) {
          throw new Error("GCanvas not supported yet");
        }, this.className = "CanvasRenderingContext2D";
      }return r(e, [{ key: "setTransform", value: function value(e, t, i, a, n, r) {
          this._drawCommands = this._drawCommands.concat("t" + (1 === e ? "1" : e.toFixed(2)) + "," + (0 === t ? "0" : t.toFixed(2)) + "," + (0 === i ? "0" : i.toFixed(2)) + "," + (1 === a ? "1" : a.toFixed(2)) + "," + n.toFixed(2) + "," + r.toFixed(2) + ";");
        } }, { key: "transform", value: function value(e, t, i, a, n, r) {
          this._drawCommands = this._drawCommands.concat("f" + (1 === e ? "1" : e.toFixed(2)) + "," + (0 === t ? "0" : t.toFixed(2)) + "," + (0 === i ? "0" : i.toFixed(2)) + "," + (1 === a ? "1" : a.toFixed(2)) + "," + n + "," + r + ";");
        } }, { key: "resetTransform", value: function value() {
          this._drawCommands = this._drawCommands.concat("m;");
        } }, { key: "scale", value: function value(e, t) {
          this._drawCommands = this._drawCommands.concat("k" + e.toFixed(2) + "," + t.toFixed(2) + ";");
        } }, { key: "rotate", value: function value(e) {
          this._drawCommands = this._drawCommands.concat("r" + e.toFixed(6) + ";");
        } }, { key: "translate", value: function value(e, t) {
          this._drawCommands = this._drawCommands.concat("l" + e.toFixed(2) + "," + t.toFixed(2) + ";");
        } }, { key: "save", value: function value() {
          this._savedGlobalAlpha.push(this._globalAlpha), this._drawCommands = this._drawCommands.concat("v;");
        } }, { key: "restore", value: function value() {
          this._drawCommands = this._drawCommands.concat("e;"), this._globalAlpha = this._savedGlobalAlpha.pop();
        } }, { key: "createPattern", value: function value(e, t) {
          return new s.default(e, t);
        } }, { key: "createLinearGradient", value: function value(e, t, i, a) {
          return new d.default(e, t, i, a);
        } }, { key: "strokeRect", value: function value(e, t, i, a) {
          this._drawCommands = this._drawCommands.concat("s" + e + "," + t + "," + i + "," + a + ";");
        } }, { key: "clearRect", value: function value(e, t, i, a) {
          this._drawCommands = this._drawCommands.concat("c" + e + "," + t + "," + i + "," + a + ";");
        } }, { key: "clip", value: function value() {
          this._drawCommands = this._drawCommands.concat("p;");
        } }, { key: "resetClip", value: function value() {
          this._drawCommands = this._drawCommands.concat("q;");
        } }, { key: "closePath", value: function value() {
          this._drawCommands = this._drawCommands.concat("o;");
        } }, { key: "moveTo", value: function value(e, t) {
          this._drawCommands = this._drawCommands.concat("g" + e.toFixed(2) + "," + t.toFixed(2) + ";");
        } }, { key: "lineTo", value: function value(e, t) {
          this._drawCommands = this._drawCommands.concat("i" + e.toFixed(2) + "," + t.toFixed(2) + ";");
        } }, { key: "bezierCurveTo", value: function value(e, t, i, a, n, r) {
          this._drawCommands = this._drawCommands.concat("z" + e.toFixed(2) + "," + t.toFixed(2) + "," + i.toFixed(2) + "," + a.toFixed(2) + "," + n.toFixed(2) + "," + r.toFixed(2) + ";");
        } }, { key: "arcTo", value: function value(e, t, i, a, n) {
          this._drawCommands = this._drawCommands.concat("h" + e + "," + t + "," + i + "," + a + "," + n + ";");
        } }, { key: "beginPath", value: function value() {
          this._drawCommands = this._drawCommands.concat("b;");
        } }, { key: "fillRect", value: function value(e, t, i, a) {
          this._drawCommands = this._drawCommands.concat("n" + e + "," + t + "," + i + "," + a + ";");
        } }, { key: "rect", value: function value(e, t, i, a) {
          this._drawCommands = this._drawCommands.concat("w" + e + "," + t + "," + i + "," + a + ";");
        } }, { key: "fill", value: function value() {
          this._drawCommands = this._drawCommands.concat("L;");
        } }, { key: "stroke", value: function value(e) {
          this._drawCommands = this._drawCommands.concat("x;");
        } }, { key: "arc", value: function value(e, t, i, a, n, r) {
          var o = 0;r && (o = 1), this._drawCommands = this._drawCommands.concat("y" + e.toFixed(2) + "," + t.toFixed(2) + "," + i.toFixed(2) + "," + a + "," + n + "," + o + ";");
        } }, { key: "fillText", value: function value(e, t, i) {
          var a = e.replace(/!/g, "!!");a = a.replace(/,/g, "!,"), a = a.replace(/;/g, "!;"), this._drawCommands = this._drawCommands.concat("T" + a + "," + t + "," + i + ",0.0;");
        } }, { key: "drawImage", value: function value(t, i, a, n, r, o, s, u, d) {
          var c = arguments.length;e.GBridge.bindImageTexture(this.componentId, t.src, t._id), this._drawCommands += function () {
            if (3 === c) {
              var e = parseFloat(i) || 0,
                  l = parseFloat(a) || 0;return "d" + t._id + ",0,0," + t.width + "," + t.height + "," + e + "," + l + "," + t.width + "," + t.height + ";";
            }if (5 === c) {
              var f = parseFloat(i) || 0,
                  _ = parseFloat(a) || 0,
                  h = parseInt(n) || t.width,
                  E = parseInt(r) || t.height;return "d" + t._id + ",0,0," + t.width + "," + t.height + "," + f + "," + _ + "," + h + "," + E + ";";
            }if (9 === c) return i = parseFloat(i) || 0, a = parseFloat(a) || 0, n = parseInt(n) || t.width, r = parseInt(r) || t.height, o = parseFloat(o) || 0, s = parseFloat(s) || 0, u = parseInt(u) || t.width, d = parseInt(d) || t.height, "d" + t._id + "," + i + "," + a + "," + n + "," + r + "," + o + "," + s + "," + u + "," + d + ";";
          }();
        } }, { key: "fillStyle", set: function set(t) {
          if (this._fillStyle = t, "string" == typeof t) this._drawCommands = this._drawCommands.concat("F" + t + ";");else if (t instanceof s.default) {
            var i = t._img;e.GBridge.bindImageTexture(this.componentId, i.src, i._id), this._drawCommands = this._drawCommands.concat("G" + i._id + "," + t._style + ";");
          } else if (t instanceof d.default) {
            for (var a = "D" + t._start_pos._x.toFixed(2) + "," + t._start_pos._y.toFixed(2) + "," + t._end_pos._x.toFixed(2) + "," + t._end_pos._y.toFixed(2) + "," + t._stop_count, n = 0; n < t._stop_count; ++n) {
              a += "," + t._stops[n]._pos + "," + t._stops[n]._color;
            }this._drawCommands = this._drawCommands.concat(a + ";");
          } else if (t instanceof l.default) {
            for (var a = "H" + t._start_pos._x.toFixed(2) + "," + t._start_pos._y.toFixed(2) + "," + t._start_pos._r.toFixed(2) + "," + t._end_pos._x.toFixed(2) + "," + t._end_pos._y.toFixed(2) + "," + t._end_pos._r.toFixed(2) + "," + t._stop_count, n = 0; n < t._stop_count; ++n) {
              a += "," + t._stops[n]._pos + "," + t._stops[n]._color;
            }this._drawCommands = this._drawCommands.concat(a + ";");
          }
        }, get: function get() {
          return this._fillStyle;
        } }, { key: "globalAlpha", get: function get() {
          return this._globalAlpha;
        }, set: function set(e) {
          this._globalAlpha = e, this._drawCommands = this._drawCommands.concat("a" + e.toFixed(2) + ";");
        } }, { key: "strokeStyle", get: function get() {
          return this._strokeStyle;
        }, set: function set(t) {
          if (this._strokeStyle = t, "string" == typeof t) this._drawCommands = this._drawCommands.concat("S" + t + ";");else if (t instanceof s.default) {
            var i = t._img;e.GBridge.bindImageTexture(this.componentId, i.src, i._id), this._drawCommands = this._drawCommands.concat("G" + i._id + "," + t._style + ";");
          } else if (t instanceof d.default) {
            for (var a = "D" + t._start_pos._x.toFixed(2) + "," + t._start_pos._y.toFixed(2) + "," + t._end_pos._x.toFixed(2) + "," + t._end_pos._y.toFixed(2) + "," + t._stop_count, n = 0; n < t._stop_count; ++n) {
              a += "," + t._stops[n]._pos + "," + t._stops[n]._color;
            }this._drawCommands = this._drawCommands.concat(a + ";");
          } else if (t instanceof l.default) {
            for (var a = "H" + t._start_pos._x.toFixed(2) + "," + t._start_pos._y.toFixed(2) + "," + t._start_pos._r.toFixed(2) + "," + t._end_pos._x.toFixed(2) + "," + t._end_pos._y + ",".toFixed(2) + t._end_pos._r.toFixed(2) + "," + t._stop_count, n = 0; n < t._stop_count; ++n) {
              a += "," + t._stops[n]._pos + "," + t._stops[n]._color;
            }this._drawCommands = this._drawCommands.concat(a + ";");
          }
        } }, { key: "lineWidth", get: function get() {
          return this._lineWidth;
        }, set: function set(e) {
          this._lineWidth = e, this._drawCommands = this._drawCommands.concat("W" + e + ";");
        } }, { key: "lineCap", get: function get() {
          return this._lineCap;
        }, set: function set(e) {
          this._lineCap = e, this._drawCommands = this._drawCommands.concat("C" + e + ";");
        } }, { key: "lineJoin", get: function get() {
          return this._lineJoin;
        }, set: function set(e) {
          this._lineJoin = e, this._drawCommands = this._drawCommands.concat("J" + e + ";");
        } }, { key: "miterLimit", get: function get() {
          return this._miterLimit;
        }, set: function set(e) {
          this._miterLimit = e, this._drawCommands = this._drawCommands.concat("M" + e + ";");
        } }, { key: "globalCompositeOperation", get: function get() {
          return this._globalCompositeOperation;
        }, set: function set(e) {
          this._globalCompositeOperation = e;var t = 0;switch (e) {case "source-over":
              t = 0;break;case "source-atop":
              t = 5;break;case "source-in":
              t = 0;break;case "source-out":
              t = 2;break;case "destination-over":case "destination-atop":case "destination-in":
              t = 4;break;case "destination-out":
              t = 3;break;case "lighter":
              t = 1;break;case "copy":
              t = 2;break;case "xor":
              t = 6;break;default:
              t = 0;}this._drawCommands = this._drawCommands.concat("B" + t + ";");
        } }, { key: "textAlign", get: function get() {
          return this._textAlign;
        }, set: function set(e) {
          this._textAlign = e;var t = 0;switch (e) {case "start":
              t = 0;break;case "end":
              t = 1;break;case "left":
              t = 2;break;case "center":
              t = 3;break;case "right":
              t = 4;break;default:
              t = 0;}this._drawCommands = this._drawCommands.concat("A" + t + ";");
        } }, { key: "textBaseline", get: function get() {
          return this._textBaseline;
        }, set: function set(e) {
          this._textBaseline = e;var t = 0;switch (e) {case "alphabetic":
              t = 0;break;case "middle":
              t = 1;break;case "top":
              t = 2;break;case "hanging":
              t = 3;break;case "bottom":
              t = 4;break;case "ideographic":
              t = 5;break;default:
              t = 0;}this._drawCommands = this._drawCommands.concat("E" + t + ";");
        } }, { key: "font", get: function get() {
          return this._font;
        }, set: function set(e) {
          this._font = e, this._drawCommands = this._drawCommands.concat("j" + e + ";");
        } }]), e;
    }();t.default = f, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e) {
      return e && e.__esModule ? e : { default: e };
    }function n(e) {
      return Array.isArray(e) ? e : Array.from(e);
    }function r(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var o = function () {
      function e(e, t) {
        var i = [],
            a = !0,
            n = !1,
            r = void 0;try {
          for (var o, s = e[Symbol.iterator](); !(a = (o = s.next()).done) && (i.push(o.value), !t || i.length !== t); a = !0) {}
        } catch (e) {
          n = !0, r = e;
        } finally {
          try {
            !a && s.return && s.return();
          } finally {
            if (n) throw r;
          }
        }return i;
      }return function (t, i) {
        if (Array.isArray(t)) return t;if (Symbol.iterator in Object(t)) return e(t, i);throw new TypeError("Invalid attempt to destructure non-iterable instance");
      };
    }(),
        s = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        u = "function" == typeof Symbol && "symbol" == _typeof(Symbol.iterator) ? function (e) {
      return typeof e === "undefined" ? "undefined" : _typeof(e);
    } : function (e) {
      return e && "function" == typeof Symbol && e.constructor === Symbol && e !== Symbol.prototype ? "symbol" : typeof e === "undefined" ? "undefined" : _typeof(e);
    },
        d = i(10),
        c = a(d),
        l = i(11),
        f = a(l),
        _ = i(12),
        h = a(_),
        E = i(13),
        v = a(E),
        m = i(14),
        T = a(m),
        A = i(15),
        g = a(A),
        N = i(16),
        R = a(N),
        p = i(17),
        b = a(p),
        C = i(18),
        B = a(C),
        I = i(19),
        S = a(I),
        y = i(3),
        F = a(y),
        G = function G(e) {
      var t = arguments.length > 1 && void 0 !== arguments[1] && arguments[1],
          i = "Float32Array";if (t) if (e instanceof Uint8Array) i = "Uint8Array";else if (e instanceof Uint16Array) i = "Uint16Array";else if (e instanceof Uint32Array) i = "Uint32Array";else {
        if (!(e instanceof Float32Array)) throw new Error("Check array type failed. Array type is " + (void 0 === e ? "undefined" : u(e)));i = "Float32Array";
      }return { Uint8Array: 1, Uint16Array: 2, Uint32Array: 4, Float32Array: 14 }[i] + "," + btoa(function (e, t) {
        for (var i = "", a = 0; a < e.length; a++) {
          0 !== a && (i += t), i += e[a];
        }return i;
      }(e, ","));
    },
        x = function () {
      function e(t, i, a) {
        var n = this;r(this, e), O.call(this), this._canvas = t, this._type = i, this._version = "WebGL 1.0", this._attrs = a, this._map = new Map(), Object.keys(c.default).forEach(function (e) {
          return Object.defineProperty(n, e, { value: c.default[e] });
        });
      }return s(e, [{ key: "canvas", get: function get() {
          return this._canvas;
        } }]), e;
    }(),
        O = function O() {
      this.className = "WebGLRenderingContext", this.activeTexture = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.activeTexture + "," + e, !0);
      }, this.attachShader = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.attachShader + "," + e.id + "," + t.id, !0);
      }, this.bindAttribLocation = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.bindAttribLocation + "," + e.id + "," + t + "," + i, !0);
      }, this.bindBuffer = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.bindBuffer + "," + e + "," + (t ? t.id : 0), !0);
      }, this.bindFramebuffer = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.bindFramebuffer + "," + e + "," + (t ? t.id : 0), !0);
      }, this.bindRenderbuffer = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.bindRenderbuffer + "," + e + "," + (t ? t.id : 0), !0);
      }, this.bindTexture = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.bindTexture + "," + e + "," + (t ? t.id : 0), !0);
      }, this.blendColor = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.blendColor + "," + target + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.blendEquation = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.blendEquation + "," + e, !0);
      }, this.blendEquationSeparate = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.blendEquationSeparate + "," + e + "," + t, !0);
      }, this.blendFunc = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.blendFunc + "," + e + "," + t, !0);
      }, this.blendFuncSeparate = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.blendFuncSeparate + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.bufferData = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.bufferData + "," + e + "," + G(t, !0) + "," + i, !0);
      }, this.bufferSubData = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.bufferSubData + "," + e + "," + t + "," + G(i, !0), !0);
      }, this.checkFramebufferStatus = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.checkFramebufferStatus + "," + e);return Number(t);
      }, this.clear = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.clear + "," + e), this._canvas._needRender = !0;
      }, this.clearColor = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.clearColor + "," + e + "," + t + "," + i, !0);
      }, this.clearDepth = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.clearDepth + "," + e, !0);
      }, this.clearStencil = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.clearStencil + "," + e);
      }, this.colorMask = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.colorMask + "," + e + "," + t + "," + i + "," + a);
      }, this.compileShader = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.compileShader + "," + e.id, !0);
      }, this.compressedTexImage2D = function (e, t, i, a, n, r, o) {
        x.GBridge.callNative(this._canvas.id, F.default.compressedTexImage2D + "," + e + "," + t + "," + i + "," + a + "," + n + "," + r + "," + G(o), !0);
      }, this.compressedTexSubImage2D = function (e, t, i, a, n, r, o, s) {
        x.GBridge.callNative(this._canvas.id, F.default.compressedTexSubImage2D + "," + e + "," + t + "," + i + "," + a + "," + n + "," + r + "," + o + "," + G(s), !0);
      }, this.copyTexImage2D = function (e, t, i, a, n, r, o, s) {
        x.GBridge.callNative(this._canvas.id, F.default.copyTexImage2D + "," + e + "," + t + "," + i + "," + a + "," + n + "," + r + "," + o + "," + s, !0);
      }, this.copyTexSubImage2D = function (e, t, i, a, n, r, o, s) {
        x.GBridge.callNative(this._canvas.id, F.default.copyTexSubImage2D + "," + e + "," + t + "," + i + "," + a + "," + n + "," + r + "," + o + "," + s);
      }, this.createBuffer = function () {
        var e = x.GBridge.callNative(this._canvas.id, F.default.createBuffer + ""),
            t = new h.default(e);return this._map.set(t.uuid(), t), t;
      }, this.createFramebuffer = function () {
        var e = x.GBridge.callNative(this._canvas.id, F.default.createFramebuffer + ""),
            t = new v.default(e);return this._map.set(t.uuid(), t), t;
      }, this.createProgram = function () {
        var e = x.GBridge.callNative(this._canvas.id, F.default.createProgram + ""),
            t = new R.default(e);return this._map.set(t.uuid(), t), t;
      }, this.createRenderbuffer = function () {
        var e = x.GBridge.callNative(this._canvas.id, F.default.createRenderbuffer + ""),
            t = new T.default(e);return this._map.set(t.uuid(), t), t;
      }, this.createShader = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.createShader + "," + e),
            i = new b.default(t, e);return this._map.set(i.uuid(), i), i;
      }, this.createTexture = function () {
        var e = x.GBridge.callNative(this._canvas.id, F.default.createTexture + ""),
            t = new g.default(e);return this._map.set(t.uuid(), t), t;
      }, this.cullFace = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.cullFace + "," + e, !0);
      }, this.deleteBuffer = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.deleteBuffer + "," + e.id, !0);
      }, this.deleteFramebuffer = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.deleteFramebuffer + "," + e.id, !0);
      }, this.deleteProgram = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.deleteProgram + "," + e.id, !0);
      }, this.deleteRenderbuffer = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.deleteRenderbuffer + "," + e.id, !0);
      }, this.deleteShader = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.deleteShader + "," + e.id, !0);
      }, this.deleteTexture = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.deleteTexture + "," + e.id, !0);
      }, this.depthFunc = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.depthFunc + "," + e);
      }, this.depthMask = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.depthMask + "," + Number(e), !0);
      }, this.depthRange = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.depthRange + "," + e + "," + t, !0);
      }, this.detachShader = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.detachShader + "," + e.id + "," + t.id, !0);
      }, this.disable = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.disable + "," + e, !0);
      }, this.disableVertexAttribArray = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.disableVertexAttribArray + "," + e, !0);
      }, this.drawArrays = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.drawArrays + "," + e + "," + t + "," + i), this._canvas._needRender = !0;
      }, this.drawElements = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.drawElements + "," + e + "," + t + "," + i + "," + a + ";"), this._canvas._needRender = !0;
      }, this.enable = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.enable + "," + e, !0);
      }, this.enableVertexAttribArray = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.enableVertexAttribArray + "," + e, !0);
      }, this.flush = function () {
        x.GBridge.callNative(this._canvas.id, F.default.flush + "");
      }, this.framebufferRenderbuffer = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.framebufferRenderbuffer + "," + e + "," + t + "," + i + "," + (a ? a.id : 0), !0);
      }, this.framebufferTexture2D = function (e, t, i, a, n) {
        x.GBridge.callNative(this._canvas.id, F.default.framebufferTexture2D + "," + e + "," + t + "," + i + "," + (a ? a.id : 0) + "," + n, !0);
      }, this.frontFace = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.frontFace + "," + e, !0);
      }, this.generateMipmap = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.generateMipmap + "," + e, !0);
      }, this.getActiveAttrib = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getActiveAttrib + "," + e.id + "," + t),
            a = i.split(","),
            n = o(a, 3),
            r = n[0],
            s = n[1],
            u = n[2];return new f.default({ type: Number(r), size: Number(s), name: u });
      }, this.getActiveUniform = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getActiveUniform + "," + e.id + "," + t),
            a = i.split(","),
            n = o(a, 3),
            r = n[0],
            s = n[1],
            u = n[2];return new f.default({ type: Number(r), size: Number(s), name: u });
      }, this.getAttachedShaders = function (e) {
        var t = this,
            i = x.GBridge.callNative(this._canvas.id, F.default.getAttachedShaders + "," + e.id),
            a = n(i);a[0];return a.slice(1).map(function (e) {
          return t._map.get(b.default.uuid(e));
        });
      }, this.getAttribLocation = function (e, t) {
        return x.GBridge.callNative(this._canvas.id, F.default.getAttribLocation + "," + e.id + "," + t);
      }, this.getBufferParameter = function (e, t) {
        var i = (x.GBridge.callNative(this._canvas.id, F.default.getBufferParameter + "," + e + "," + t), getBufferParameter),
            a = o(i, 2);a[0];return a[1];
      }, this.getError = function () {
        return x.GBridge.callNative(this._canvas.id, F.default.getError + "");
      }, this.getExtension = function (e) {
        return null;
      }, this.getFramebufferAttachmentParameter = function (e, t, i) {
        var a = x.GBridge.callNative(this._canvas.id, F.default.getFramebufferAttachmentParameter + "," + e + "," + t + "," + i);switch (i) {case c.default.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
            return this._map.get(T.default.uuid(a)) || this._map.get(g.default.uuid(a)) || null;default:
            return a;}
      }, this.getParameter = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.getParameter + "," + e);switch (e) {case c.default.VERSION:
            return this._version;case c.default.ARRAY_BUFFER_BINDING:case c.default.ELEMENT_ARRAY_BUFFER_BINDING:
            return this._map.get(h.default.uuid(t)) || null;case c.default.CURRENT_PROGRAM:
            return this._map.get(R.default.uuid(t)) || null;case c.default.FRAMEBUFFER_BINDING:
            return this._map.get(v.default.uuid(t)) || null;case c.default.RENDERBUFFER_BINDING:
            return this._map.get(T.default.uuid(t)) || null;case c.default.TEXTURE_BINDING_2D:case c.default.TEXTURE_BINDING_CUBE_MAP:
            return this._map.get(g.default.uuid(t)) || null;case c.default.ALIASED_LINE_WIDTH_RANGE:case c.default.ALIASED_POINT_SIZE_RANGE:case c.default.BLEND_COLOR:case c.default.COLOR_CLEAR_VALUE:case c.default.DEPTH_RANGE:case c.default.MAX_VIEWPORT_DIMS:case c.default.SCISSOR_BOX:case c.default.VIEWPORT:case c.default.COMPRESSED_TEXTURE_FORMATS:default:
            var i = t.split(","),
                a = n(i),
                r = (a[0], a.slice(1));return 1 === r.length ? Number(r[0]) : r.map(Number);}
      }, this.getProgramInfoLog = function (e) {
        return x.GBridge.callNative(this._canvas.id, F.default.getProgramInfoLog + "," + e.id);
      }, this.getProgramParameter = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getProgramParameter + "," + e.id + "," + t),
            a = i.split(",").map(function (e) {
          return parseInt(e);
        }),
            n = o(a, 2),
            r = n[0],
            s = n[1];if (1 === r) return Boolean(s);if (2 === r) return s;throw new Error("Unrecongized program paramater " + i + ", type: " + (void 0 === i ? "undefined" : u(i)));
      }, this.getRenderbufferParameter = function (e, t) {
        return x.GBridge.callNative(this._canvas.id, F.default.getRenderbufferParameter + "," + e + "," + t);
      }, this.getShaderInfoLog = function (e) {
        return x.GBridge.callNative(this._canvas.id, F.default.getShaderInfoLog + "," + e.id);
      }, this.getShaderParameter = function (e, t) {
        return x.GBridge.callNative(this._canvas.id, F.default.getShaderParameter + "," + e.id + "," + t);
      }, this.getShaderPrecisionFormat = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getShaderPrecisionFormat + "," + e + "," + t),
            a = o(i, 3),
            n = a[0],
            r = a[1],
            s = a[2];return new B.default({ rangeMin: Number(n), rangeMax: Number(r), precision: Number(s) });
      }, this.getShaderSource = function (e) {
        return x.GBridge.callNative(this._canvas.id, F.default.getShaderSource + "," + e.id);
      }, this.getSupportedExtensions = function () {
        return Object.keys({});
      }, this.getTexParameter = function (e, t) {
        return x.GBridge.callNative(this._canvas.id, F.default.getTexParameter + "," + e + "," + t);
      }, this.getUniformLocation = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getUniformLocation + "," + e.id + "," + t);return -1 === i ? null : new S.default(Number(i));
      }, this.getVertexAttrib = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getVertexAttrib + "," + e + "," + t);switch (t) {case c.default.VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
            return this._map.get(h.default.uuid(i)) || null;case c.default.CURRENT_VERTEX_ATTRIB:default:
            return i;}
      }, this.getVertexAttribOffset = function (e, t) {
        var i = x.GBridge.callNative(this._canvas.id, F.default.getVertexAttribOffset + "," + e + "," + t);return Number(i);
      }, this.isBuffer = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isBuffer + "," + e.id);return Boolean(t);
      }, this.isContextLost = function () {
        return !1;
      }, this.isEnabled = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isEnabled + "," + e);return Boolean(t);
      }, this.isFramebuffer = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isFramebuffer + "," + e.id);return Boolean(t);
      }, this.isProgram = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isProgram + "," + e.id);return Boolean(t);
      }, this.isRenderbuffer = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isRenderbuffer + "," + renderbuffer.id);return Boolean(t);
      }, this.isShader = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isShader + "," + e.id);return Boolean(t);
      }, this.isTexture = function (e) {
        var t = x.GBridge.callNative(this._canvas.id, F.default.isTexture + "," + e.id);return Boolean(t);
      }, this.lineWidth = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.lineWidth + "," + e, !0);
      }, this.linkProgram = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.linkProgram + "," + e.id, !0);
      }, this.pixelStorei = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.pixelStorei + "," + e + "," + Number(t));
      }, this.polygonOffset = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.polygonOffset + "," + e + "," + t);
      }, this.readPixels = function (e, t, i, a, r, o, s) {
        for (var u = x.GBridge.callNative(this._canvas.id, F.default.readPixels + "," + e + "," + t + "," + i + "," + a + "," + r + "," + o), d = u.split(","), c = n(d), l = (c[0], c.slice(1)), f = 0; f < l.length; f++) {
          s[f] = parseInt(l[f]);
        }
      }, this.renderbufferStorage = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.renderbufferStorage + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.sampleCoverage = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.sampleCoverage + "," + e + "," + Number(t), !0);
      }, this.scissor = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.scissor + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.shaderSource = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.shaderSource + "," + e.id + "," + t);
      }, this.stencilFunc = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.stencilFunc + "," + e + "," + t + "," + i, !0);
      }, this.stencilFuncSeparate = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.stencilFuncSeparate + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.stencilMask = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.stencilMask + "," + e, !0);
      }, this.stencilMaskSeparate = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.stencilMaskSeparate + "," + e + "," + t, !0);
      }, this.stencilOp = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.stencilOp + "," + e + "," + t + "," + i);
      }, this.stencilOpSeparate = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.stencilOp + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.texImage2D = function () {
        for (var e, t = arguments.length, i = Array(t), a = 0; a < t; a++) {
          i[a] = arguments[a];
        }(e = x.GBridge).texImage2D.apply(e, [this._canvas.id].concat(i));
      }, this.texParameterf = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.texParameterf + "," + e + "," + t + "," + i, !0);
      }, this.texParameteri = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.texParameteri + "," + e + "," + t + "," + i);
      }, this.texSubImage2D = function () {
        for (var e, t = arguments.length, i = Array(t), a = 0; a < t; a++) {
          i[a] = arguments[a];
        }(e = x.GBridge).texSubImage2D.apply(e, [this._canvas.id].concat(i));
      }, this.uniform1f = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform1f + "," + e.id + "," + t);
      }, this.uniform1fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform1fv + "," + e.id + "," + G(t), !0);
      }, this.uniform1i = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform1i + "," + e.id + "," + t);
      }, this.uniform1iv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform1iv + "," + e.id + "," + G(t), !0);
      }, this.uniform2f = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform2f + "," + e.id + "," + t + "," + i, !0);
      }, this.uniform2fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform2fv + "," + e.id + "," + G(t), !0);
      }, this.uniform2i = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform2i + "," + e.id + "," + t + "," + i, !0);
      }, this.uniform2iv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform2iv + "," + e.id + "," + G(t), !0);
      }, this.uniform3f = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform3f + "," + e.id + "," + t + "," + i + "," + a, !0);
      }, this.uniform3fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform3fv + "," + e.id + "," + G(t), !0);
      }, this.uniform3i = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform3i + "," + e.id + "," + t + "," + i + "," + a, !0);
      }, this.uniform3iv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform3iv + "," + e.id + "," + G(t), !0);
      }, this.uniform4f = function (e, t, i, a, n) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform4f + "," + e.id + "," + t + "," + i + "," + a + "," + n, !0);
      }, this.uniform4fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform4fv + "," + e.id + "," + G(t), !0);
      }, this.uniform4i = function (e, t, i, a, n) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform4i + "," + e.id + "," + t + "," + i + "," + a + "," + n, !0);
      }, this.uniform4iv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.uniform4iv + "," + e.id + "," + G(t, !0), !0);
      }, this.uniformMatrix2fv = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.uniformMatrix2fv + "," + e.id + "," + Number(t) + "," + G(i), !0);
      }, this.uniformMatrix3fv = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.uniformMatrix3fv + "," + e.id + "," + Number(t) + "," + G(i), !0);
      }, this.uniformMatrix4fv = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.uniformMatrix4fv + "," + e.id + "," + Number(t) + "," + G(i), !0);
      }, this.useProgram = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.useProgram + "," + e.id, !0);
      }, this.validateProgram = function (e) {
        x.GBridge.callNative(this._canvas.id, F.default.validateProgram + "," + e.id, !0);
      }, this.vertexAttrib1f = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib1f + "," + e + "," + t, !0);
      }, this.vertexAttrib2f = function (e, t, i) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib2f + "," + e + "," + t + "," + i, !0);
      }, this.vertexAttrib3f = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib3f + "," + e + "," + t + "," + i + "," + a, !0);
      }, this.vertexAttrib4f = function (e, t, i, a, n) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib4f + "," + e + "," + t + "," + i + "," + a + "," + n, !0);
      }, this.vertexAttrib1fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib1fv + "," + e + "," + G(t), !0);
      }, this.vertexAttrib2fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib2fv + "," + e + "," + G(t), !0);
      }, this.vertexAttrib3fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib3fv + "," + e + "," + G(t), !0);
      }, this.vertexAttrib4fv = function (e, t) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttrib4fv + "," + e + "," + G(t), !0);
      }, this.vertexAttribPointer = function (e, t, i, a, n, r) {
        x.GBridge.callNative(this._canvas.id, F.default.vertexAttribPointer + "," + e + "," + t + "," + i + "," + Number(a) + "," + n + "," + r, !0);
      }, this.viewport = function (e, t, i, a) {
        x.GBridge.callNative(this._canvas.id, F.default.viewport + "," + e + "," + t + "," + i + "," + a, !0);
      };
    };t.default = x, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    Object.defineProperty(t, "__esModule", { value: !0 });var a = 1,
        n = {};n.activeTexture = a++, n.attachShader = a++, n.bindAttribLocation = a++, n.bindBuffer = a++, n.bindFramebuffer = a++, n.bindRenderbuffer = a++, n.bindTexture = a++, n.blendColor = a++, n.blendEquation = a++, n.blendEquationSeparate = a++, n.blendFunc = a++, n.blendFuncSeparate = a++, n.bufferData = a++, n.bufferSubData = a++, n.checkFramebufferStatus = a++, n.clear = a++, n.clearColor = a++, n.clearDepth = a++, n.clearStencil = a++, n.colorMask = a++, n.compileShader = a++, n.compressedTexImage2D = a++, n.compressedTexSubImage2D = a++, n.copyTexImage2D = a++, n.copyTexSubImage2D = a++, n.createBuffer = a++, n.createFramebuffer = a++, n.createProgram = a++, n.createRenderbuffer = a++, n.createShader = a++, n.createTexture = a++, n.cullFace = a++, n.deleteBuffer = a++, n.deleteFramebuffer = a++, n.deleteProgram = a++, n.deleteRenderbuffer = a++, n.deleteShader = a++, n.deleteTexture = a++, n.depthFunc = a++, n.depthMask = a++, n.depthRange = a++, n.detachShader = a++, n.disable = a++, n.disableVertexAttribArray = a++, n.drawArrays = a++, n.drawArraysInstancedANGLE = a++, n.drawElements = a++, n.drawElementsInstancedANGLE = a++, n.enable = a++, n.enableVertexAttribArray = a++, n.flush = a++, n.framebufferRenderbuffer = a++, n.framebufferTexture2D = a++, n.frontFace = a++, n.generateMipmap = a++, n.getActiveAttrib = a++, n.getActiveUniform = a++, n.getAttachedShaders = a++, n.getAttribLocation = a++, n.getBufferParameter = a++, n.getContextAttributes = a++, n.getError = a++, n.getExtension = a++, n.getFramebufferAttachmentParameter = a++, n.getParameter = a++, n.getProgramInfoLog = a++, n.getProgramParameter = a++, n.getRenderbufferParameter = a++, n.getShaderInfoLog = a++, n.getShaderParameter = a++, n.getShaderPrecisionFormat = a++, n.getShaderSource = a++, n.getSupportedExtensions = a++, n.getTexParameter = a++, n.getUniform = a++, n.getUniformLocation = a++, n.getVertexAttrib = a++, n.getVertexAttribOffset = a++, n.isBuffer = a++, n.isContextLost = a++, n.isEnabled = a++, n.isFramebuffer = a++, n.isProgram = a++, n.isRenderbuffer = a++, n.isShader = a++, n.isTexture = a++, n.lineWidth = a++, n.linkProgram = a++, n.pixelStorei = a++, n.polygonOffset = a++, n.readPixels = a++, n.renderbufferStorage = a++, n.sampleCoverage = a++, n.scissor = a++, n.shaderSource = a++, n.stencilFunc = a++, n.stencilFuncSeparate = a++, n.stencilMask = a++, n.stencilMaskSeparate = a++, n.stencilOp = a++, n.stencilOpSeparate = a++, n.texImage2D = a++, n.texParameterf = a++, n.texParameteri = a++, n.texSubImage2D = a++, n.uniform1f = a++, n.uniform1fv = a++, n.uniform1i = a++, n.uniform1iv = a++, n.uniform2f = a++, n.uniform2fv = a++, n.uniform2i = a++, n.uniform2iv = a++, n.uniform3f = a++, n.uniform3fv = a++, n.uniform3i = a++, n.uniform3iv = a++, n.uniform4f = a++, n.uniform4fv = a++, n.uniform4i = a++, n.uniform4iv = a++, n.uniformMatrix2fv = a++, n.uniformMatrix3fv = a++, n.uniformMatrix4fv = a++, n.useProgram = a++, n.validateProgram = a++, n.vertexAttrib1f = a++, n.vertexAttrib2f = a++, n.vertexAttrib3f = a++, n.vertexAttrib4f = a++, n.vertexAttrib1fv = a++, n.vertexAttrib2fv = a++, n.vertexAttrib3fv = a++, n.vertexAttrib4fv = a++, n.vertexAttribPointer = a++, n.viewport = a++, t.default = n, e.exports = t.default;
  }, function (e, t, i) {
    e.exports = i(5);
  }, function (e, t, i) {
    "use strict";
    function a(e) {
      return e && e.__esModule ? e : { default: e };
    }function n(e) {
      var t = arguments.length > 1 && void 0 !== arguments[1] ? arguments[1] : {},
          i = t.bridge,
          a = t.debug,
          n = t.disableAutoSwap,
          r = t.disableComboCommands,
          s = u.default.GBridge = o.default.GBridge = c.default.GBridge = f.default.GBridge = i;s.callEnable(e.ref, [0, -1, !1, !1, 1, "white", !1]), !0 === a && s.callEnableDebug(), r && s.callEnableDisableCombo();var d = new o.default(e.ref, { disableAutoSwap: n });return d.width = e._style.width, d.height = e._style.height, d;
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.HapBridge = t.Image = void 0, t.enable = n;var r = i(6),
        o = a(r),
        s = i(20),
        u = a(s),
        d = i(2),
        c = a(d),
        l = i(1),
        f = a(l),
        _ = i(21),
        h = a(_);t.Image = u.default, t.HapBridge = h.default;
  }, function (e, t, i) {
    "use strict";
    function a(e) {
      return e && e.__esModule ? e : { default: e };
    }function n(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(1),
        s = a(o),
        u = i(2),
        d = a(u),
        c = function () {
      function e(t, i) {
        var a = this,
            r = i.disableAutoSwap;n(this, e), this.id = null, this._needRender = !0, this.id = t, this._disableAutoSwap = r, r && (this._swapBuffers = function () {
          e.GBridge.render(a.id);
        });
      }return r(e, [{ key: "getContext", value: function value(t) {
          var i = this,
              a = null;if (t.match(/webgl/i)) {
            if (a = new d.default(this), a.componentId = this.id, !this._disableAutoSwap) {
              var n = function n() {
                i._needRender && (e.GBridge.render(i.id), i._needRender = !1);
              };setInterval(n, 16);
            }e.GBridge.callSetContextType(this.id, 1);
          } else {
            if (!t.match(/2d/i)) throw new Error("not supported context " + t);a = new s.default(this), a.componentId = this.id;var r = function r() {
              var t = a._drawCommands;a._drawCommands = "", e.GBridge.render2d(i.id, t), i._needRender = !1;
            };setInterval(r, 16), e.GBridge.callSetContextType(this.id, 0);
          }return a;
        } }, { key: "reset", value: function value() {
          e.GBridge.callReset(this.id);
        } }]), e;
    }();t.default = c, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function e(t, i) {
      a(this, e), this._style = i, this._img = t;
    };t.default = n, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function e(t, i, n, r) {
      a(this, e), this.addColorStop = function (e, t) {
        this._stop_count < 5 && 0 <= e && e <= 1 && (this._stops[this._stop_count] = { _pos: e, _color: t }, this._stop_count++);
      }, this._start_pos = { _x: t, _y: i }, this._end_pos = { _x: n, _y: r }, this._stop_count = 0, this._stops = [0, 0, 0, 0, 0];
    };t.default = n, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        r = function () {
      function e(t, i, n, r, o, s) {
        a(this, e), this._start_pos = { _x: t, _y: i, _r: n }, this._end_pos = { _x: r, _y: o, _r: s }, this._stop_count = 0, this._stops = [0, 0, 0, 0, 0];
      }return n(e, [{ key: "addColorStop", value: function value(e, t) {
          this._stop_count < 5 && 0 <= e && e <= 1 && (this._stops[this._stop_count] = { _pos: e, _color: t }, this._stop_count++);
        } }]), e;
    }();t.default = r, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    Object.defineProperty(t, "__esModule", { value: !0 }), t.default = { DEPTH_BUFFER_BIT: 256, STENCIL_BUFFER_BIT: 1024, COLOR_BUFFER_BIT: 16384, POINTS: 0, LINES: 1, LINE_LOOP: 2, LINE_STRIP: 3, TRIANGLES: 4, TRIANGLE_STRIP: 5, TRIANGLE_FAN: 6, ZERO: 0, ONE: 1, SRC_COLOR: 768, ONE_MINUS_SRC_COLOR: 769, SRC_ALPHA: 770, ONE_MINUS_SRC_ALPHA: 771, DST_ALPHA: 772, ONE_MINUS_DST_ALPHA: 773, DST_COLOR: 774, ONE_MINUS_DST_COLOR: 775, SRC_ALPHA_SATURATE: 776, FUNC_ADD: 32774, BLEND_EQUATION: 32777, BLEND_EQUATION_RGB: 32777, BLEND_EQUATION_ALPHA: 34877, FUNC_SUBTRACT: 32778, FUNC_REVERSE_SUBTRACT: 32779, BLEND_DST_RGB: 32968, BLEND_SRC_RGB: 32969, BLEND_DST_ALPHA: 32970, BLEND_SRC_ALPHA: 32971, CONSTANT_COLOR: 32769, ONE_MINUS_CONSTANT_COLOR: 32770, CONSTANT_ALPHA: 32771, ONE_MINUS_CONSTANT_ALPHA: 32772, BLEND_COLOR: 32773, ARRAY_BUFFER: 34962, ELEMENT_ARRAY_BUFFER: 34963, ARRAY_BUFFER_BINDING: 34964, ELEMENT_ARRAY_BUFFER_BINDING: 34965, STREAM_DRAW: 35040, STATIC_DRAW: 35044, DYNAMIC_DRAW: 35048, BUFFER_SIZE: 34660, BUFFER_USAGE: 34661, CURRENT_VERTEX_ATTRIB: 34342, FRONT: 1028, BACK: 1029, FRONT_AND_BACK: 1032, TEXTURE_2D: 3553, CULL_FACE: 2884, BLEND: 3042, DITHER: 3024, STENCIL_TEST: 2960, DEPTH_TEST: 2929, SCISSOR_TEST: 3089, POLYGON_OFFSET_FILL: 32823, SAMPLE_ALPHA_TO_COVERAGE: 32926, SAMPLE_COVERAGE: 32928, NO_ERROR: 0, INVALID_ENUM: 1280, INVALID_VALUE: 1281, INVALID_OPERATION: 1282, OUT_OF_MEMORY: 1285, CW: 2304, CCW: 2305, LINE_WIDTH: 2849, ALIASED_POINT_SIZE_RANGE: 33901, ALIASED_LINE_WIDTH_RANGE: 33902, CULL_FACE_MODE: 2885, FRONT_FACE: 2886, DEPTH_RANGE: 2928, DEPTH_WRITEMASK: 2930, DEPTH_CLEAR_VALUE: 2931, DEPTH_FUNC: 2932, STENCIL_CLEAR_VALUE: 2961, STENCIL_FUNC: 2962, STENCIL_FAIL: 2964, STENCIL_PASS_DEPTH_FAIL: 2965, STENCIL_PASS_DEPTH_PASS: 2966, STENCIL_REF: 2967, STENCIL_VALUE_MASK: 2963, STENCIL_WRITEMASK: 2968, STENCIL_BACK_FUNC: 34816, STENCIL_BACK_FAIL: 34817, STENCIL_BACK_PASS_DEPTH_FAIL: 34818, STENCIL_BACK_PASS_DEPTH_PASS: 34819, STENCIL_BACK_REF: 36003, STENCIL_BACK_VALUE_MASK: 36004, STENCIL_BACK_WRITEMASK: 36005, VIEWPORT: 2978, SCISSOR_BOX: 3088, COLOR_CLEAR_VALUE: 3106, COLOR_WRITEMASK: 3107, UNPACK_ALIGNMENT: 3317, PACK_ALIGNMENT: 3333, MAX_TEXTURE_SIZE: 3379, MAX_VIEWPORT_DIMS: 3386, SUBPIXEL_BITS: 3408, RED_BITS: 3410, GREEN_BITS: 3411, BLUE_BITS: 3412, ALPHA_BITS: 3413, DEPTH_BITS: 3414, STENCIL_BITS: 3415, POLYGON_OFFSET_UNITS: 10752, POLYGON_OFFSET_FACTOR: 32824, TEXTURE_BINDING_2D: 32873, SAMPLE_BUFFERS: 32936, SAMPLES: 32937, SAMPLE_COVERAGE_VALUE: 32938, SAMPLE_COVERAGE_INVERT: 32939, COMPRESSED_TEXTURE_FORMATS: 34467, DONT_CARE: 4352, FASTEST: 4353, NICEST: 4354, GENERATE_MIPMAP_HINT: 33170, BYTE: 5120, UNSIGNED_BYTE: 5121, SHORT: 5122, UNSIGNED_SHORT: 5123, INT: 5124, UNSIGNED_INT: 5125, FLOAT: 5126, DEPTH_COMPONENT: 6402, ALPHA: 6406, RGB: 6407, RGBA: 6408, LUMINANCE: 6409, LUMINANCE_ALPHA: 6410, UNSIGNED_SHORT_4_4_4_4: 32819, UNSIGNED_SHORT_5_5_5_1: 32820, UNSIGNED_SHORT_5_6_5: 33635, FRAGMENT_SHADER: 35632, VERTEX_SHADER: 35633, MAX_VERTEX_ATTRIBS: 34921, MAX_VERTEX_UNIFORM_VECTORS: 36347, MAX_VARYING_VECTORS: 36348, MAX_COMBINED_TEXTURE_IMAGE_UNITS: 35661, MAX_VERTEX_TEXTURE_IMAGE_UNITS: 35660, MAX_TEXTURE_IMAGE_UNITS: 34930, MAX_FRAGMENT_UNIFORM_VECTORS: 36349, SHADER_TYPE: 35663, DELETE_STATUS: 35712, LINK_STATUS: 35714, VALIDATE_STATUS: 35715, ATTACHED_SHADERS: 35717, ACTIVE_UNIFORMS: 35718, ACTIVE_ATTRIBUTES: 35721, SHADING_LANGUAGE_VERSION: 35724, CURRENT_PROGRAM: 35725, NEVER: 512, LESS: 513, EQUAL: 514, LEQUAL: 515, GREATER: 516, NOTEQUAL: 517, GEQUAL: 518, ALWAYS: 519, KEEP: 7680, REPLACE: 7681, INCR: 7682, DECR: 7683, INVERT: 5386, INCR_WRAP: 34055, DECR_WRAP: 34056, VENDOR: 7936, RENDERER: 7937, VERSION: 7938, NEAREST: 9728, LINEAR: 9729, NEAREST_MIPMAP_NEAREST: 9984, LINEAR_MIPMAP_NEAREST: 9985, NEAREST_MIPMAP_LINEAR: 9986, LINEAR_MIPMAP_LINEAR: 9987, TEXTURE_MAG_FILTER: 10240, TEXTURE_MIN_FILTER: 10241, TEXTURE_WRAP_S: 10242, TEXTURE_WRAP_T: 10243, TEXTURE: 5890, TEXTURE_CUBE_MAP: 34067, TEXTURE_BINDING_CUBE_MAP: 34068, TEXTURE_CUBE_MAP_POSITIVE_X: 34069, TEXTURE_CUBE_MAP_NEGATIVE_X: 34070, TEXTURE_CUBE_MAP_POSITIVE_Y: 34071, TEXTURE_CUBE_MAP_NEGATIVE_Y: 34072, TEXTURE_CUBE_MAP_POSITIVE_Z: 34073, TEXTURE_CUBE_MAP_NEGATIVE_Z: 34074, MAX_CUBE_MAP_TEXTURE_SIZE: 34076, TEXTURE0: 33984, TEXTURE1: 33985, TEXTURE2: 33986, TEXTURE3: 33987, TEXTURE4: 33988, TEXTURE5: 33989, TEXTURE6: 33990, TEXTURE7: 33991, TEXTURE8: 33992, TEXTURE9: 33993, TEXTURE10: 33994, TEXTURE11: 33995, TEXTURE12: 33996, TEXTURE13: 33997, TEXTURE14: 33998, TEXTURE15: 33999, TEXTURE16: 34e3, TEXTURE17: 34001, TEXTURE18: 34002, TEXTURE19: 34003, TEXTURE20: 34004, TEXTURE21: 34005, TEXTURE22: 34006, TEXTURE23: 34007, TEXTURE24: 34008, TEXTURE25: 34009, TEXTURE26: 34010, TEXTURE27: 34011, TEXTURE28: 34012, TEXTURE29: 34013, TEXTURE30: 34014, TEXTURE31: 34015, ACTIVE_TEXTURE: 34016, REPEAT: 10497, CLAMP_TO_EDGE: 33071, MIRRORED_REPEAT: 33648, FLOAT_VEC2: 35664, FLOAT_VEC3: 35665, FLOAT_VEC4: 35666, INT_VEC2: 35667, INT_VEC3: 35668, INT_VEC4: 35669, BOOL: 35670, BOOL_VEC2: 35671, BOOL_VEC3: 35672, BOOL_VEC4: 35673, FLOAT_MAT2: 35674, FLOAT_MAT3: 35675, FLOAT_MAT4: 35676, SAMPLER_2D: 35678, SAMPLER_CUBE: 35680, VERTEX_ATTRIB_ARRAY_ENABLED: 34338, VERTEX_ATTRIB_ARRAY_SIZE: 34339, VERTEX_ATTRIB_ARRAY_STRIDE: 34340, VERTEX_ATTRIB_ARRAY_TYPE: 34341, VERTEX_ATTRIB_ARRAY_NORMALIZED: 34922, VERTEX_ATTRIB_ARRAY_POINTER: 34373, VERTEX_ATTRIB_ARRAY_BUFFER_BINDING: 34975, IMPLEMENTATION_COLOR_READ_TYPE: 35738, IMPLEMENTATION_COLOR_READ_FORMAT: 35739, COMPILE_STATUS: 35713, LOW_FLOAT: 36336, MEDIUM_FLOAT: 36337, HIGH_FLOAT: 36338, LOW_INT: 36339, MEDIUM_INT: 36340, HIGH_INT: 36341, FRAMEBUFFER: 36160, RENDERBUFFER: 36161, RGBA4: 32854, RGB5_A1: 32855, RGB565: 36194, DEPTH_COMPONENT16: 33189, STENCIL_INDEX8: 36168, DEPTH_STENCIL: 34041, RENDERBUFFER_WIDTH: 36162, RENDERBUFFER_HEIGHT: 36163, RENDERBUFFER_INTERNAL_FORMAT: 36164, RENDERBUFFER_RED_SIZE: 36176, RENDERBUFFER_GREEN_SIZE: 36177, RENDERBUFFER_BLUE_SIZE: 36178, RENDERBUFFER_ALPHA_SIZE: 36179, RENDERBUFFER_DEPTH_SIZE: 36180, RENDERBUFFER_STENCIL_SIZE: 36181, FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE: 36048, FRAMEBUFFER_ATTACHMENT_OBJECT_NAME: 36049, FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL: 36050, FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE: 36051, COLOR_ATTACHMENT0: 36064, DEPTH_ATTACHMENT: 36096, STENCIL_ATTACHMENT: 36128, DEPTH_STENCIL_ATTACHMENT: 33306, NONE: 0, FRAMEBUFFER_COMPLETE: 36053, FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 36054, FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: 36055, FRAMEBUFFER_INCOMPLETE_DIMENSIONS: 36057, FRAMEBUFFER_UNSUPPORTED: 36061, FRAMEBUFFER_BINDING: 36006, RENDERBUFFER_BINDING: 36007, MAX_RENDERBUFFER_SIZE: 34024, INVALID_FRAMEBUFFER_OPERATION: 1286, UNPACK_FLIP_Y_WEBGL: 37440, UNPACK_PREMULTIPLY_ALPHA_WEBGL: 37441, CONTEXT_LOST_WEBGL: 37442, UNPACK_COLORSPACE_CONVERSION_WEBGL: 37443, BROWSER_DEFAULT_WEBGL: 37444 }, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function e(t) {
      var i = t.type,
          n = t.name,
          r = t.size;a(this, e), this.className = "WebGLActiveInfo", this.type = i, this.name = n, this.size = r;
    };t.default = n, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLBuffer",
        u = function () {
      function e(t) {
        a(this, e), this.className = s, this.id = t;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLFrameBuffer",
        u = function () {
      function e(t) {
        a(this, e), this.className = s, this.id = t;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLRenderBuffer",
        u = function () {
      function e(t) {
        a(this, e), this.className = s, this.id = t;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLTexture",
        u = function () {
      function e(t, i) {
        a(this, e), this.className = s, this.id = t, this.type = i;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLProgram",
        u = function () {
      function e(t) {
        a(this, e), this.className = s, this.id = t;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLShader",
        u = function () {
      function e(t, i) {
        a(this, e), this.className = s, this.id = t, this.type = i;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function e(t) {
      var i = t.rangeMin,
          n = t.rangeMax,
          r = t.precision;a(this, e), this.className = "WebGLShaderPrecisionFormat", this.rangeMin = i, this.rangeMax = n, this.precision = r;
    };t.default = n, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }function n(e) {
      return (0, o.getTransferedObjectUUID)(s, e);
    }Object.defineProperty(t, "__esModule", { value: !0 }), t.default = void 0;var r = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        o = i(0),
        s = "WebGLUniformLocation",
        u = function () {
      function e(t, i) {
        a(this, e), this.className = s, this.id = t, this.type = i;
      }return r(e, [{ key: "uuid", value: function value() {
          return n(this.id);
        } }]), e;
    }();u.uuid = n, t.default = u, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      if (!(e instanceof t)) throw new TypeError("Cannot call a class as a function");
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function () {
      function e(e, t) {
        for (var i = 0; i < t.length; i++) {
          var a = t[i];a.enumerable = a.enumerable || !1, a.configurable = !0, "value" in a && (a.writable = !0), Object.defineProperty(e, a.key, a);
        }
      }return function (t, i, a) {
        return i && e(t.prototype, i), a && e(t, a), t;
      };
    }(),
        r = 1,
        o = function o() {},
        s = function () {
      function e() {
        a(this, e), this._id = r++, this._width = 0, this._height = 0, this._src = void 0, this._onload = o, this._onerror = o, this.complete = !1;
      }return n(e, [{ key: "addEventListener", value: function value(e, t) {
          "load" === e ? this.onload = t : "error" === e && (this.onerror = t);
        } }, { key: "removeEventListener", value: function value(e, t) {
          "load" === e ? this.onload = o : "error" === e && (this.onerror = o);
        } }, { key: "width", get: function get() {
          return this._width;
        }, set: function set(e) {
          this._width = e;
        } }, { key: "height", get: function get() {
          return this._height;
        }, set: function set(e) {
          this._height = e;
        } }, { key: "src", get: function get() {
          return this._src;
        }, set: function set(t) {
          var i = this;t.startsWith("//") && (t = "http:" + t), this._src = t, e.GBridge.perloadImage([this._src, this._id], function (e) {
            if ("string" == typeof e && (e = JSON.parse(e)), e.error) {
              var t = { type: "error", target: i };i.onerror(t);
            } else {
              i.complete = !0, i.width = "number" == typeof e.width ? e.width : 0, i.height = "number" == typeof e.height ? e.height : 0;var t = { type: "load", target: i };i.onload(t);
            }
          });
        } }, { key: "onload", get: function get() {
          return this._onload;
        }, set: function set(e) {
          this._onload = e;
        } }, { key: "onerror", get: function get() {
          return this._onerror;
        }, set: function set(e) {
          this._onerror = e;
        } }]), e;
    }();s.GBridge = null, t.default = s, e.exports = t.default;
  }, function (e, t, i) {
    "use strict";
    function a(e, t) {
      for (var i = "", a = 0; a < e.length; a++) {
        0 !== a && (i += t), i += e[a];
      }return i;
    }Object.defineProperty(t, "__esModule", { value: !0 });var n = function () {
      function e(e, t) {
        var i = [],
            a = !0,
            n = !1,
            r = void 0;try {
          for (var o, s = e[Symbol.iterator](); !(a = (o = s.next()).done) && (i.push(o.value), !t || i.length !== t); a = !0) {}
        } catch (e) {
          n = !0, r = e;
        } finally {
          try {
            !a && s.return && s.return();
          } finally {
            if (n) throw r;
          }
        }return i;
      }return function (t, i) {
        if (Array.isArray(t)) return t;if (Symbol.iterator in Object(t)) return e(t, i);throw new TypeError("Invalid attempt to destructure non-iterable instance");
      };
    }(),
        r = i(3),
        o = function (e) {
      return e && e.__esModule ? e : { default: e };
    }(r),
        s = $app_require$("@app-module/system.canvas"),
        u = !1,
        d = !1,
        c = function () {
      var e = [];Object.keys(o.default).forEach(function (t) {
        e[o.default[t]] = t;
      });var t = function t(_t) {
        return e[parseInt(_t)] || "NotFoundMethod";
      };return function (e, i) {
        var a = i.split(",")[0],
            n = t(a);console.log("=== callNative - componentId:" + e + "; method: " + n + "; cmds: " + i);
      };
    }(),
        l = {},
        f = { GCanvasModule: s, callEnable: function callEnable(e, t) {
        return l[e] = [], s.enable({ componentId: e, config: t });
      }, callEnableDebug: function callEnableDebug() {
        u = !0;
      }, callEnableDisableCombo: function callEnableDisableCombo() {
        d = !0;
      }, callSetContextType: function callSetContextType(e, t) {
        s.setContextType(t, e);
      }, callReset: function callReset(e) {
        s.resetComponent && canvasModule.resetComponent(componentId);
      }, render: function render(e) {
        return extendCallNative(e, 1073741825, "render");
      }, render2d: function render2d(e, t) {
        u && (console.log(">>> >>> render2d ==="), console.log(">>> commands: " + t)), extendCallNative(e, 536870913, t);
      }, callExtendCallNative: function callExtendCallNative(e, t) {
        throw "should not be here anymore " + t;
      }, flushNative: function flushNative(e) {
        var t = a(l[e], ";");l[e] = [], u && (console.log(">>> >>> flush native ==="), console.log(">>> commands: " + t));var i = extendCallNative(e, 1073741824, t);return u && console.log(">>> result: " + i), i;
      }, callNative: function callNative(e, t, i) {
        return u && c(e, t), l[e].push(t), !i || d ? f.flushNative(e) : void 0;
      }, texImage2D: function texImage2D(e) {
        for (var t = arguments.length, i = Array(t > 1 ? t - 1 : 0), a = 1; a < t; a++) {
          i[a - 1] = arguments[a];
        }if (6 === i.length) {
          var r = n(i, 6),
              o = r[0],
              u = r[1],
              d = r[2],
              c = r[3],
              l = r[4],
              f = r[5];s.texImage2D(e, o, u, d, c, l, f.src);
        } else if (9 === i.length) {
          var _ = n(i, 9),
              h = _[0],
              E = _[1],
              v = _[2],
              m = _[3],
              T = _[4],
              A = _[5],
              g = _[6],
              N = _[7],
              R = _[8];s.texImage2D(e, h, E, v, m, T, A, g, N, R ? R.src : 0);
        }
      }, texSubImage2D: function texSubImage2D(e, t, i, a, n, r, o, u) {
        s.texSubImage2D(e, t, i, a, n, r, o, u.src);
      }, bindImageTexture: function bindImageTexture(e, t, i) {
        s.bindImageTexture([t, i], e);
      }, perloadImage: function perloadImage(e, t) {
        var i = n(e, 2),
            a = i[0],
            r = i[1];s.preLoadImage([a, r], function (e) {
          e.url = a, e.id = r, t(e);
        });
      } };t.default = f, e.exports = t.default;
  }]);
});
/* WEBPACK VAR INJECTION */}.call(exports, __webpack_require__(16)(module)))

/***/ }),
/* 16 */
/***/ (function(module, exports) {

module.exports = function(module) {
	if(!module.webpackPolyfill) {
		module.deprecate = function() {};
		module.paths = [];
		// module.parent = undefined by default
		if(!module.children) module.children = [];
		Object.defineProperty(module, "loaded", {
			enumerable: true,
			get: function() {
				return module.l;
			}
		});
		Object.defineProperty(module, "id", {
			enumerable: true,
			get: function() {
				return module.i;
			}
		});
		module.webpackPolyfill = 1;
	}
	return module;
};


/***/ })
/******/ ]);
  };
  if (typeof window === "undefined") {
    return createPageHandler();
  }
  else {
    window.createPageHandler = createPageHandler
  }
})();
//# sourceMappingURL=2d.js.map