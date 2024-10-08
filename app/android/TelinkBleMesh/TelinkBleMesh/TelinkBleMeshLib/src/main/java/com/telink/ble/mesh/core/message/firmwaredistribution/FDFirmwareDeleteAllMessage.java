/********************************************************************************************************
 * @file FDFirmwareDeleteAllMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

/**
 * The Firmware Distribution Firmware Delete All message is an acknowledged message sent by a Firmware Distribution Client to delete all firmware images stored on a Firmware Distribution Server.
 * The response to a Firmware Distribution Firmware Delete All message is a Firmware Distribution Firmware Status message.
 */
public class FDFirmwareDeleteAllMessage extends UpdatingMessage {


    /**
     * Constructs a new FDFirmwareDeleteAllMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public FDFirmwareDeleteAllMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDFirmwareDeleteAllMessage with the specified destination address and application key index.
     * This method sets the response maximum to 1.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @return The created FDFirmwareDeleteAllMessage.
     */
    public static FDFirmwareDeleteAllMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDFirmwareDeleteAllMessage message = new FDFirmwareDeleteAllMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Gets the opcode for the FDFirmwareDeleteAllMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_FIRMWARE_DELETE_ALL.value;
    }

    /**
     * Gets the response opcode for the FDFirmwareDeleteAllMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FD_UPLOAD_STATUS.value;
    }
}