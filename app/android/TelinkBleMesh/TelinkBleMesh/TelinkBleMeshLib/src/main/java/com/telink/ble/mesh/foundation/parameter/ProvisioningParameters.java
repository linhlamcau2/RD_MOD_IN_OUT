/********************************************************************************************************
 * @file ProvisioningParameters.java
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
package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.entity.ProvisioningDevice;

/**
 * This is a class called ProvisioningParameters that extends the Parameters class.
 * used in {@link com.telink.ble.mesh.foundation.MeshService#startProvisioning(ProvisioningParameters)}
 */
public class ProvisioningParameters extends Parameters {

    /**
     * constructor that takes a ProvisioningDevice object as a parameter.
     *
     * @param device ProvisioningDevice
     */
    public ProvisioningParameters(ProvisioningDevice device) {
        this.setScanFilter(createScanFilter(UUIDInfo.SERVICE_PROVISION));
        this.set(ACTION_PROVISIONING_TARGET, device);
    }


}
