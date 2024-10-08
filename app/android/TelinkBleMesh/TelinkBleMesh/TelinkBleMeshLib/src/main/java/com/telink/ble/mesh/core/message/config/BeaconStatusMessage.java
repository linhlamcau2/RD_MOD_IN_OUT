/********************************************************************************************************
 * @file BeaconStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * The Config Beacon Status is an unacknowledged message used to report the current Secure Network Beacon state of a node
 * <p>
 * response of {@link BeaconGetMessage} {@link BeaconSetMessage}
 * <p>
 * {@link com.telink.ble.mesh.core.message.Opcode#CFG_BEACON_STATUS}
 */
public class BeaconStatusMessage extends StatusMessage implements Parcelable {


    public byte beacon;


    /**
     * Constructs a new BeaconStatusMessage object.
     */
    public BeaconStatusMessage() {
    }

    /**
     * A Creator object that generates instances of BeaconStatusMessage from a Parcel.
     */
    protected BeaconStatusMessage(Parcel in) {
        beacon = in.readByte();
    }

    /**
     * A Creator object that generates instances of BeaconStatusMessage from a Parcel.
     */
    public static final Creator<BeaconStatusMessage> CREATOR = new Creator<BeaconStatusMessage>() {
        @Override
        public BeaconStatusMessage createFromParcel(Parcel in) {
            return new BeaconStatusMessage(in);
        }

        @Override
        public BeaconStatusMessage[] newArray(int size) {
            return new BeaconStatusMessage[size];
        }
    };

    /**
     * Parses the parameters of the BeaconStatusMessage from a byte array.
     *
     * @param params the byte array containing the parameters of the BeaconStatusMessage
     */
    @Override
    public void parse(byte[] params) {
        beacon = params[0];
    }

    /**
     * Describes the contents of the BeaconStatusMessage object.
     *
     * @return an integer representing the contents of the BeaconStatusMessage object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the BeaconStatusMessage object to a Parcel.
     *
     * @param dest  the Parcel object to write the BeaconStatusMessage object to
     * @param flags additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(beacon);
    }
}
