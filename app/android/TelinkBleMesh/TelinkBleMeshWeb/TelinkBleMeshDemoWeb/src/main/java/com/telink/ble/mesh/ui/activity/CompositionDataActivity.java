/********************************************************************************************************
 * @file CompositionDataActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.Toolbar;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.config.CompositionDataGetMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.Element;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.web.entity.MeshNode;

/**
 * show device composition data
 */

public class CompositionDataActivity extends BaseActivity implements EventListener<String> {

    private MeshNode nodeInfo;
    private int meshAddress;
    TextView tv_cps;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_composition_data);
        setTitle("Composition Data");
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        MenuItem menuItem = toolbar.getMenu().findItem(R.id.item_refresh);
        menuItem.setVisible(true);
        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_refresh) {
                MeshService.getInstance().sendMeshMessage(new CompositionDataGetMessage(meshAddress));
            }
            return false;
        });


        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            meshAddress = intent.getIntExtra("meshAddress", -1);
            nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
        }
        tv_cps = findViewById(R.id.tv_cps);
        TelinkMeshApplication.getInstance().addEventListener(CompositionDataStatusMessage.class.getName(), this);
        refreshCpsInfo();
    }

    private void refreshCpsInfo() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tv_cps.setText(formatCps());
            }
        });
    }

    private String formatCps() {
        CompositionData cps = nodeInfo.compositionData();
        if (cps == null) return "";


        StringBuilder sb = new StringBuilder("Composition-Data:\n");

        // basic
        sb.append(String.format("cid: 0x%04X", cps.cid)).append("\n");
        sb.append(String.format("pid: 0x%04X", cps.pid)).append("\n");
        sb.append(String.format("vid: 0x%04X", cps.vid)).append("\n");
        sb.append(String.format("crpl: 0x%04X", cps.crpl)).append("\n\n");

        // features
        sb.append(String.format("features: 0x%04X", cps.features)).append("\n");
        sb.append("\t").append("relay support: ").append(cps.relaySupport()).append("\n")
                .append("\t").append("proxy support: ").append(cps.proxySupport()).append("\n")
                .append("\t").append("friend support: ").append(cps.friendSupport()).append("\n")
                .append("\t").append("low power support: ").append(cps.lowPowerSupport()).append("\n\n");

        // elements
        sb.append("elements: (").append(cps.elements.size()).append(")").append("\n");
        int basicEleAdr = nodeInfo.address;
        for (Element ele : cps.elements) {
            sb.append("\t").append(String.format("element adr: 0x%04X", basicEleAdr)).append("\n");
            MeshSigModel meshSigModel;
            for (Integer sigModel : ele.sigModels) {
                sb.append("\t\t").append(String.format("SIG model -- 0x%04X", sigModel));
                meshSigModel = MeshSigModel.getById(sigModel);
                if (meshSigModel != null) {
                    sb.append(" -- ").append(meshSigModel.modelName);
                }
                sb.append("\n");
            }

            for (Integer vendorModel : ele.vendorModels) {
                sb.append("\t\t").append(String.format("vendor model -- 0x%06X", vendorModel)).append("\n");
            }
            basicEleAdr++;
        }


        return sb.toString();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(CompositionDataStatusMessage.class.getName())) {
            CompositionDataStatusMessage cpsStatusMsg = (CompositionDataStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            CompositionData compositionData = cpsStatusMsg.getCompositionData();
//            nodeInfo.compositionData = compositionData;
//            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(CompositionDataActivity.this, "Composition Data Refresh Success", Toast.LENGTH_SHORT).show();
                }
            });
            refreshCpsInfo();
        }
    }
}
