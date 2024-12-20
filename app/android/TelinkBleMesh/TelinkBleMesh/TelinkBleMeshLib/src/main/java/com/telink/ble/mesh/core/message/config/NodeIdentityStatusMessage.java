/********************************************************************************************************
 * @file NodeIdentityStatusMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * The Config Node Identity Status is an unacknowledged message used to report the current Node Identity state for a subnet
 */
public class NodeIdentityStatusMessage extends StatusMessage implements Parcelable {

    /**
     * 1 byte
     */
    public int status;

    /**
     * 2 bytes
     */
    public int netKeyIndex;

    /**
     * 1 byte
     */
    public int identity;

    /**
     * Constructs a new NodeIdentityStatusMessage object.
     */
    public NodeIdentityStatusMessage() {
    }


    /**
     * Constructs a new NodeIdentityStatusMessage object from a Parcel.
     *
     * @param in the Parcel object containing the NodeIdentityStatusMessage data
     */
    protected NodeIdentityStatusMessage(Parcel in) {
        status = in.readInt();
        netKeyIndex = in.readInt();
        identity = in.readInt();
    }

    /**
     * A Creator object that generates instances of NodeIdentityStatusMessage from a Parcel.
     */
    public static final Creator<NodeIdentityStatusMessage> CREATOR = new Creator<NodeIdentityStatusMessage>() {
        @Override
        public NodeIdentityStatusMessage createFromParcel(Parcel in) {
            return new NodeIdentityStatusMessage(in);
        }

        @Override
        public NodeIdentityStatusMessage[] newArray(int size) {
            return new NodeIdentityStatusMessage[size];
        }
    };

    /**
     * Parses the parameters of the NodeIdentityStatusMessage from a byte array.
     *
     * @param params the byte array containing the parameters of the NodeIdentityStatusMessage
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++] & 0xFF;
        netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        identity = params[index] & 0xFF;
    }

    public int getStatus() {
        return status;
    }

    public int getNetKeyIndex() {
        return netKeyIndex;
    }

    public int getIdentity() {
        return identity;
    }
    /**
     * Describes the contents of the NodeIdentityStatusMessage object.
     *
     * @return an integer representing the contents of the NodeIdentityStatusMessage object
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the NodeIdentityStatusMessage object to a Parcel.
     *
     * @param dest  the Parcel object to write the NodeIdentityStatusMessage object to
     * @param flags additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(netKeyIndex);
        dest.writeInt(identity);
    }
}
