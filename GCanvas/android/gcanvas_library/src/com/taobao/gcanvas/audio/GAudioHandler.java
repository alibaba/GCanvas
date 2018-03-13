/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
package com.taobao.gcanvas.audio;

import android.app.Activity;
import android.content.Context;
import android.database.ContentObserver;
import android.media.AudioManager;
import android.os.Handler;
import android.provider.Settings;

import com.taobao.gcanvas.GCanvasResult;
import com.taobao.gcanvas.GCanvasWebView;
//import com.taobao.gcanvas.GUtil;
import com.taobao.gcanvas.util.GLog;

import org.json.JSONArray;
import org.json.JSONException;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashMap;


/**
 * This class called by CordovaActivity to play and record audio.
 * The file can be local or over a network using http.
 * <p>
 * Audio formats supported (tested): .mp3, .wav
 * <p>
 * Local audio files must reside in one of two places:
 * android_asset: 		file name must start with /android_asset/sound.mp3
 * sdcard:				file name is just sound.mp3
 */
public class GAudioHandler {

    public static String TAG = GAudioHandler.class.getName();

    private Activity activity;
    private GCanvasWebView webView;

    private HashMap<String, GAudioPlayer> players; // Audio player object
    private ArrayList<GAudioPlayer> pausedForPhone; // Audio players that were paused when phone call came in

    private VolumeObserver volumeObserver = null;

    public GAudioHandler() {
        this.players = new HashMap<String, GAudioPlayer>();
        this.pausedForPhone = new ArrayList<GAudioPlayer>();
    }

    public static String stripFileProtocol(String uriString) {
        if (uriString.startsWith("file://")) {
            uriString = uriString.substring(7);
        }
        return uriString;
    }

    public void initialize(Context context, GCanvasWebView webView) {
        if (context instanceof Activity) {
            activity = (Activity) context;
        }

        this.webView = webView;
    }

    public Activity getActivity() {
        return activity;
    }

    public GCanvasWebView getWebView() {
        return webView;
    }

    private HashMap<String, GAudioPlayer> getPlayers() {
        return players;
    }

    /**
     * Executes the request and returns PluginResult.
     *
     * @param action        The action to execute.
     * @param args          JSONArry of arguments for the plugin.
     * @param resultContext The callback context used when calling back into JavaScript.
     * @return A PluginResult object with a status and message.
     */
    public boolean execute(String action, JSONArray args, GCanvasResult resultContext) throws JSONException {
        if (action == null) {
            GLog.e("CANVAS", " ~ GAutioHandler ::: action is null");
            return false;
        }

        String result = "";

        if (action.equals("startRecordingAudio")) {
            startRecordingAudio(args.getString(0), stripFileProtocol(args.getString(1)), resultContext);
        } else if (action.equals("stopRecordingAudio")) {
            stopRecordingAudio(args.getString(0));
        } else if (action.equals("startPlayingAudio")) {
            startPlayingAudio(args.getString(0), stripFileProtocol(args.getString(1)), resultContext);
        } else if (action.equals("seekToAudio")) {
            seekToAudio(args.getString(0), args.getInt(1));
        } else if (action.equals("pausePlayingAudio")) {
            pausePlayingAudio(args.getString(0));
        } else if (action.equals("stopPlayingAudio")) {
            stopPlayingAudio(args.getString(0));
        } else if (action.equals("setVolume")) {
            try {
                this.setVolume(args.getString(0), Float.parseFloat(args.getString(1)));
            } catch (NumberFormatException nfe) {
                //no-op
            }
        } else if (action.equals("getCurrentPositionAudio")) {
            float f = this.getCurrentPositionAudio(args.getString(0));
            resultContext.success(f);
            return true;
        } else if (action.equals("getDurationAudio")) {
            float f = this.getDurationAudio(args.getString(0), args.getString(1), resultContext);
            resultContext.success(f);
            return true;
        } else if (action.equals("create")) {
            String id = args.getString(0);
            String src = stripFileProtocol(args.getString(1));
            GAudioPlayer audio = new GAudioPlayer(this, id, src, resultContext);
            this.players.put(id, audio);
        } else if (action.equals("release")) {
            boolean success = release(args.getString(0));
            resultContext.success(success);
            return true;
        } else if (action.equals("disable")) {
            onDestroy();
            return true;
        } else if (action.equals("setSourceAudio")) {
            String id = args.getString(0);
            String src = stripFileProtocol(args.getString(1));
            GAudioPlayer audio = this.players.get(id);
            if (audio == null || src.isEmpty()) {
                GLog.e("CANVAS", "audio's id error");
                return false;
            }
            audio.setSourceAudio(src);
        } else if (action.equals("startLoadingAudio")) {
            String id = args.getString(0);
            startLoadingAudio(id);
        } else { // Unrecognized action.
            return false;
        }

        resultContext.success(result);

        return true;
    }

    /**
     * Release the audio player instance to save memory.
     *
     * @param id The id of the audio player
     */
    private boolean release(String id) {
        if (!players.containsKey(id)) {
            return false;
        }

        GAudioPlayer audio = players.get(id);
        players.remove(id);
        audio.destroy();
        return true;
    }

    /**
     * Stop all audio players and recorders on navigate.
     */
    public void onReset() {
        onDestroy();
    }

    /**
     * Stop all audio players and recorders.
     */
    public void onDestroy() {
        for (GAudioPlayer audio : players.values()) {
            audio.destroy();
        }

        // Unregister volume observer 
        if (volumeObserver != null) {
            activity.getContentResolver().unregisterContentObserver(volumeObserver);
        }

        this.players.clear();
    }

    /**
     * Called when a message is sent to plugin.
     *
     * @param id   The message id
     * @param data The message data
     * @return Object to stop propagation or null
     */
    public Object onMessage(String id, Object data) {
        // If phone message
        if (id.equals("telephone")) {
            // If phone ringing, then pause playing
            if ("ringing".equals(data) || "offhook".equals(data)) {

                // Get all audio players and pause them
                for (GAudioPlayer audio : this.players.values()) {
                    if (audio.getState() == GAudioPlayer.STATE.MEDIA_RUNNING.ordinal()) {
                        this.pausedForPhone.add(audio);
                        audio.pausePlaying();
                    }
                }
            }
            // If phone idle, then resume playing those players we paused
            else if ("idle".equals(data)) {
                for (GAudioPlayer audio : this.pausedForPhone) {
                    audio.startPlaying(null);
                }
                this.pausedForPhone.clear();
            }
        }
        return null;
    }

    /**
     * Start recording and save the specified file.
     *
     * @param id   The id of the audio player
     * @param file The name of the file
     */
    public void startRecordingAudio(String id, String file, GCanvasResult resultContext) {
        GAudioPlayer audio = this.players.get(id);
        if (audio == null) {
            audio = new GAudioPlayer(this, id, file, resultContext);
            this.players.put(id, audio);
        }
        audio.startRecording(file);
    }

    /**
     * Stop recording and save to the file specified when recording started.
     *
     * @param id The id of the audio player
     */
    public void stopRecordingAudio(String id) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            audio.stopRecording();
        }
    }

    /**
     * Start or resume playing audio file.
     *
     * @param id   The id of the audio player
     * @param file The name of the audio file.
     */
    public void startPlayingAudio(String id, String file, GCanvasResult resultContext) {
        GAudioPlayer audio = this.players.get(id);
        if (audio == null) {
            audio = new GAudioPlayer(this, id, file, resultContext);
            this.players.put(id, audio);
        }

        audio.startPlaying(file);

        // Mute the audio based on the device settings
        int mode = Settings.System.getInt(activity.getContentResolver(), Settings.System.MODE_RINGER, 2);
        if (mode != 2) {
            audio.setVolume(0);
        } else {
            audio.setVolume(1);
        }
    }

    /**
     * Seek to a location.
     *
     * @param id           The id of the audio player
     * @param milliseconds int: number of milliseconds to skip 1000 = 1 second
     */
    public void seekToAudio(String id, int milliseconds) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            audio.seekToPlaying(milliseconds);
        }
    }

    /**
     * Pause playing.
     *
     * @param id The id of the audio player
     */
    public void pausePlayingAudio(String id) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            audio.pausePlaying();
        }
    }

    /**
     * Stop playing the audio file.
     *
     * @param id The id of the audio player
     */
    public void stopPlayingAudio(String id) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            audio.stopPlaying();
        }
    }

    public void startLoadingAudio(String id) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            audio.loadingAudio();
        }
    }

    /**
     * Get current position of playback.
     *
     * @param id The id of the audio player
     * @return position in msec
     */
    public float getCurrentPositionAudio(String id) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            return (audio.getCurrentPosition() / 1000.0f);
        }
        return -1;
    }

    /**
     * Get the duration of the audio file.
     *
     * @param id   The id of the audio player
     * @param file The name of the audio file.
     * @return The duration in msec.
     */
    public float getDurationAudio(String id, String file, GCanvasResult resultContext) {
        // Get audio file
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            return (audio.getDuration(file));
        }

        // If not already open, then open the file
        else {
            audio = new GAudioPlayer(this, id, file, resultContext);
            this.players.put(id, audio);
            return (audio.getDuration(file));
        }
    }

    /**
     * Get the audio device to be used for playback.
     *
     * @return 1=earpiece, 2=speaker
     */
    @SuppressWarnings("deprecation")
    public int getAudioOutputDevice() {
        AudioManager audiMgr = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
        if (audiMgr.getRouting(AudioManager.MODE_NORMAL) == AudioManager.ROUTE_EARPIECE) {
            return 1;
        } else if (audiMgr.getRouting(AudioManager.MODE_NORMAL) == AudioManager.ROUTE_SPEAKER) {
            return 2;
        } else {
            return -1;
        }
    }

    /**
     * Set the audio device to be used for playback.
     *
     * @param output 1=earpiece, 2=speaker
     */
    @SuppressWarnings("deprecation")
    public void setAudioOutputDevice(int output) {
        AudioManager audiMgr = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
        if (output == 2) {
            audiMgr.setRouting(AudioManager.MODE_NORMAL, AudioManager.ROUTE_SPEAKER, AudioManager.ROUTE_ALL);
        } else if (output == 1) {
            audiMgr.setRouting(AudioManager.MODE_NORMAL, AudioManager.ROUTE_EARPIECE, AudioManager.ROUTE_ALL);
        } else {
            GLog.w("AudioHandler setAudioOutputDevice Error: Unknown output device.");
        }
    }

    /**
     * Set the volume for an audio device
     *
     * @param id     The id of the audio player
     * @param volume Volume to adjust to 0.0f - 1.0f
     */
    public void setVolume(String id, float volume) {
        GAudioPlayer audio = this.players.get(id);
        if (audio != null) {
            audio.setVolume(volume);
        } else {
            GLog.w("AudioHandler setVolume Error: Unknown Audio Player " + id);
        }
    }

    private static class VolumeObserver extends ContentObserver {

        private final WeakReference<GAudioHandler> mGAudioHandlerRef;

        public VolumeObserver(GAudioHandler gAudioHandler, Handler handler) {
            super(handler);
            mGAudioHandlerRef = new WeakReference<GAudioHandler>(gAudioHandler);
        }

        @SuppressWarnings("deprecation")
        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);
            GAudioHandler audioHandler = mGAudioHandlerRef.get();
            if (audioHandler != null) {
                int mode = Settings.System.getInt(audioHandler.getActivity().getContentResolver(), Settings.System.MODE_RINGER, 2);
                if (mode != 2) { // mute or virbate
                    for (String i : audioHandler.getPlayers().keySet()) {
                        audioHandler.getPlayers().get(i).setVolume(0);
                    }
                } else {
                    // Volume values are raw scalars in range 0.0 to 1.0.
                    for (String i : audioHandler.getPlayers().keySet()) {
                        audioHandler.getPlayers().get(i).setVolume(1);
                    }
                }
            }
        }
    }

}
