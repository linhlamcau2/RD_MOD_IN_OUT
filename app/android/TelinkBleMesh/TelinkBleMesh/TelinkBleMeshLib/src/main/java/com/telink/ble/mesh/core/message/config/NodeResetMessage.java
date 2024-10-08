/********************************************************************************************************
 * @file NodeResetMessage.java
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

import com.telink.ble.mesh.core.message.Opcode;

/**
 * The Config Node Reset is an acknowledged message used to reset a node (other than a Provisioner) and remove it from the network.
 * <p>
 * The response to a Config Node Reset message is a Config Node Reset Status message.
 */
public class NodeResetMessage extends ConfigMessage {

    public NodeResetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the NodeResetMessage, which is Opcode.NODE_RESET.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.NODE_RESET.value;
    }

    /**
     * Returns the parameters of the NodeResetMessage.
     *
     * @return null.
     */
    @Override
    public byte[] getParams() {
        return null;
    }

    /**
     * Returns the response opcode value for the NodeResetMessage, which is Opcode.NODE_RESET_STATUS.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.NODE_RESET_STATUS.value;
    }
}
