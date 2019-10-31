/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas.util;

public final class GCanvasBase64 {

  public static byte[] decode(byte[] in) {
    return decode(in, in.length);
  }

  public static byte[] decode(byte[] in, int len) {
    // approximate output length
    int length = len / 4 * 3;

    // return an empty array on empty or short input without padding
    if (length == 0) {
      return new byte[0];
    }

    // temporary array
    byte[] out = new byte[length];

    // number of padding characters ('=')
    int pad = 0;
    byte chr;
    // compute the number of the padding characters
    // and adjust the length of the input
    for (; ; len--) {
      chr = in[len - 1];
      // skip the neutral characters
      if ((chr == '\n') || (chr == '\r') || (chr == ' ') || (chr == '\t')) {
        continue;
      }
      if (chr == '=') {
        pad++;
      } else {
        break;
      }
    }

    // index in the output array
    int outIndex = 0;

    // index in the input array
    int inIndex = 0;

    // holds the value of the input character
    int bits = 0;

    // holds the value of the input quantum
    int quantum = 0;

    for (int i = 0; i < len; i++) {
      chr = in[i];
      // skip the neutral characters
      if ((chr == '\n') || (chr == '\r') || (chr == ' ') || (chr == '\t')) {
        continue;
      }
      if ((chr >= 'A') && (chr <= 'Z')) {
        // char ASCII value
        //  A    65    0
        //  Z    90    25 (ASCII - 65)
        bits = chr - 65;
      } else if ((chr >= 'a') && (chr <= 'z')) {
        // char ASCII value
        //  a    97    26
        //  z    122   51 (ASCII - 71)
        bits = chr - 71;
      } else if ((chr >= '0') && (chr <= '9')) {
        // char ASCII value
        //  0    48    52
        //  9    57    61 (ASCII + 4)
        bits = chr + 4;
      } else if (chr == '+') {
        bits = 62;
      } else if (chr == '/') {
        bits = 63;
      } else {
        return null;
      }

      // append the value to the quantum
      quantum = (quantum << 6) | (byte) bits;

      if (inIndex % 4 == 3) {
        // 4 characters were read, so make the output:
        out[outIndex++] = (byte) (quantum >> 16);
        out[outIndex++] = (byte) (quantum >> 8);
        out[outIndex++] = (byte) quantum;
      }

      inIndex++;
    }

    if (pad > 0) {
      // adjust the quantum value according to the padding
      quantum = quantum << (6 * pad);

      // make output
      out[outIndex++] = (byte) (quantum >> 16);

      if (pad == 1) {
        out[outIndex++] = (byte) (quantum >> 8);
      }
    }

    // create the resulting array
    byte[] result = new byte[outIndex];
    System.arraycopy(out, 0, result, 0, outIndex);

    return result;
  }

}