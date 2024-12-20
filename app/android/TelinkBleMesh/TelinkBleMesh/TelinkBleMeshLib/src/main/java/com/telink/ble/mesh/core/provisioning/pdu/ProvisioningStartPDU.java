/********************************************************************************************************
 * @file ProvisioningStartPDU.java
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
package com.telink.ble.mesh.core.provisioning.pdu;

import com.telink.ble.mesh.core.provisioning.AuthenticationMethod;


/**
 * This class represents a Provisioning Start PDU (Protocol Data Unit).
 * It implements the ProvisioningStatePDU interface.
 * The Provisioning Start PDU is used to initiate the provisioning process.
 * It contains information about the algorithm used for provisioning, the public key used,
 * the authentication method used, the authentication action, and the authentication size.
 */
public class ProvisioningStartPDU implements ProvisioningStatePDU {
    private static final int LEN = 5;

    /**
     * The algorithm used for provisioning
     * 0x00: FIPS P-256 Elliptic Curve
     * 0x01–0xFF: Reserved for Future Use
     * <p>
     * update in epa
     * 0x00	BTM_ECDH_P256_CMAC_AES128_AES_CCM
     * 0x01	BTM_ECDH_P256_HMAC_SHA256_AES_CCM
     * 0x02–0xFF	Reserved for Future Use
     */
    public byte algorithm;

    /**
     * Public Key used
     * 0x00: No OOB Public Key is used
     * 0x01: OOB Public Key is used
     * 0x02–0xFF: Prohibited
     */
    public byte publicKey;

    /**
     * Authentication Method used
     * 0x00: No OOB authentication is used
     * 0x01: Static OOB authentication is used
     * 0x02: Output OOB authentication is used
     * 0x03: Input OOB authentication is used
     * 0x04–0xFF: Prohibited
     */
    public byte authenticationMethod;

    /**
     * Selected Output OOB Action or Input OOB Action or 0x00
     */
    public byte authenticationAction;

    /**
     * Size of the Output OOB used or size of the Input OOB used or 0x00
     */
    public byte authenticationSize;

    /**
     * Creates a simple ProvisioningStartPDU with default values.
     *
     * @param useOOBPublicKey    true if OOB Public Key is used, false otherwise
     * @param staticOOBSupported true if Static OOB authentication is supported, false otherwise
     * @return a ProvisioningStartPDU object
     */
    public static ProvisioningStartPDU getSimple(boolean useOOBPublicKey, boolean staticOOBSupported) {
        ProvisioningStartPDU startPDU = new ProvisioningStartPDU();
        startPDU.algorithm = 0;
        startPDU.publicKey = (byte) (useOOBPublicKey ? 1 : 0);
//        startPDU.publicKey = 0;
        startPDU.authenticationMethod = staticOOBSupported ? AuthenticationMethod.StaticOOB.value :
                AuthenticationMethod.NoOOB.value;
        startPDU.authenticationAction = 0;
        startPDU.authenticationSize = 0;
        return startPDU;
    }

    /**
     * Sets the algorithm used for provisioning.
     *
     * @param algorithm the algorithm byte value
     */
    public void setAlgorithm(byte algorithm) {
        this.algorithm = algorithm;
    }

    /**
     * Sets whether OOB Public Key is used or not.
     *
     * @param publicKeyEnable true if OOB Public Key is used, false otherwise
     */
    public void setPublicKey(boolean publicKeyEnable) {
        this.publicKey = (byte) (publicKeyEnable ? 1 : 0);
    }

    /**
     * Converts the ProvisioningStartPDU object to a byte array.
     *
     * @return a byte array representing the ProvisioningStartPDU
     */
    @Override
    public byte[] toBytes() {
        return new byte[]{
                algorithm,
                publicKey,
                authenticationMethod,
                authenticationAction,
                authenticationSize
        };
    }

    /**
     * Gets the state of the ProvisioningStartPDU.
     *
     * @return the state byte value
     */
    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_START;
    }
}


