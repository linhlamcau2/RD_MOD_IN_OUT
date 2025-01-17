/********************************************************************************************************
 * @file LightnessStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.core.message.lighting;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a status message for the lightness of a device.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class LightnessStatusMessage extends StatusMessage implements Parcelable {
    private static final int DATA_LEN_COMPLETE = 5;

    private int presentLightness;

    /**
     * The target value of the Lightness state (optional).
     */
    private int targetLightness;

    private byte remainingTime;

    /**
     * tag of is complete message
     */
    private boolean isComplete = false;

    /**
     * Default constructor for the LightnessStatusMessage class.
     */
    public LightnessStatusMessage() {
    }

    /**
     * Constructor for the LightnessStatusMessage class that takes a Parcel as input.
     *
     * @param in The Parcel object to read the values from.
     */
    protected LightnessStatusMessage(Parcel in) {
        presentLightness = in.readInt();
        targetLightness = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * Creator object for the LightnessStatusMessage class, used to create new instances from a Parcel.
     */
    public static final Creator<LightnessStatusMessage> CREATOR = new Creator<LightnessStatusMessage>() {
        @Override
        public LightnessStatusMessage createFromParcel(Parcel in) {
            return new LightnessStatusMessage(in);
        }

        @Override
        public LightnessStatusMessage[] newArray(int size) {
            return new LightnessStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameters and sets the values accordingly.
     *
     * @param params The byte array containing the parameters.
     */
    @Override
    public void parse(byte[] params) {
        this.presentLightness = MeshUtils.bytes2Integer(params, 0, 2, ByteOrder.LITTLE_ENDIAN);
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetLightness = MeshUtils.bytes2Integer(params, 2, 2, ByteOrder.LITTLE_ENDIAN);
            this.remainingTime = params[4];
        }
    }

    /**
     * Returns the present lightness value.
     *
     * @return The present lightness value.
     */
    public int getPresentLightness() {
        return presentLightness;
    }

    /**
     * Returns the target lightness value.
     *
     * @return The target lightness value.
     */
    public int getTargetLightness() {
        return targetLightness;
    }

    /**
     * Returns the remaining time value.
     *
     * @return The remaining time value.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Returns whether the message is complete or not.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }

    /**
     * Returns the content description of the Parcelable object.
     *
     * @return The content description of the Parcelable object.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the values of the Parcelable object to a Parcel.
     *
     * @param dest  The Parcel object to write the values to.
     * @param flags Additional flags for writing the Parcel.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(presentLightness);
        dest.writeInt(targetLightness);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }
}