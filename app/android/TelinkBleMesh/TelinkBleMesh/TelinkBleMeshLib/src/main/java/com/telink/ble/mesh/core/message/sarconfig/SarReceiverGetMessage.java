/********************************************************************************************************
 * @file SarReceiverGetMessage.java
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
package com.telink.ble.mesh.core.message.sarconfig;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;


/**
 * A SAR_RECEIVER_GET message is an acknowledged message used to get the current SAR Receiver state of a node
 */
public class SarReceiverGetMessage extends ConfigMessage {

    /**
     * Constructs a SarReceiverGetMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address to send the message to.
     */
    public SarReceiverGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode .
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_SAR_RECEIVER_GET.value;
    }

    /**
     * Returns the response opcode .
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_SAR_RECEIVER_STATUS.value;
    }

    /**
     * Returns the parameters .
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return null;
    }


}
