/********************************************************************************************************
 * @file TimeMessage.java
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
package com.telink.ble.mesh.core.message.time;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

/**
 * This class represents a time-based message in the mesh network.
 */
public abstract class TimeMessage extends MeshMessage {
    /**
     * Constructs a TimeMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The address of the destination node for this message.
     * @param appKeyIndex        The index of the application key to be used for encryption and decryption.
     */
    public TimeMessage(int destinationAddress, int appKeyIndex) {
        this.destinationAddress = destinationAddress;
        this.appKeyIndex = appKeyIndex;
        this.accessType = AccessType.APPLICATION;
    }
}
