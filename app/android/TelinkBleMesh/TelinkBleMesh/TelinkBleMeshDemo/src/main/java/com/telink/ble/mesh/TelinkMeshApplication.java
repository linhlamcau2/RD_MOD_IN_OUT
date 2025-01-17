/********************************************************************************************************
 * @file TelinkMeshApplication.java
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
package com.telink.ble.mesh;

import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.core.message.generic.LevelStatusMessage;
import com.telink.ble.mesh.core.message.generic.OnOffStatusMessage;
import com.telink.ble.mesh.core.message.lighting.CtlStatusMessage;
import com.telink.ble.mesh.core.message.lighting.CtlTemperatureStatusMessage;
import com.telink.ble.mesh.core.message.lighting.LightnessStatusMessage;
import com.telink.ble.mesh.core.message.sensor.SensorStatusMessage;
import com.telink.ble.mesh.entity.OnlineStatusInfo;
import com.telink.ble.mesh.foundation.MeshApplication;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.foundation.event.OnlineStatusEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.db.ObjectBox;
import com.telink.ble.mesh.util.MeshLogger;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.List;

/**
 * Telinkg Mesh App
 * Created by kee on 2017/8/17.
 */
public class TelinkMeshApplication extends MeshApplication {

    private final String TAG = "Telink-APP";
    private static TelinkMeshApplication mThis;

    private MeshInfo meshInfo;

    private NodeSortType sortType= NodeSortType.ADDRESS_ASC;

    private Handler mOfflineCheckHandler;

    @Override
    public void onCreate() {
        super.onCreate();
        mThis = this;
        HandlerThread offlineCheckThread = new HandlerThread("offline check thread");
        offlineCheckThread.start();
        mOfflineCheckHandler = new Handler(offlineCheckThread.getLooper());
        MeshLogger.enableRecord(SharedPreferenceHelper.isLogEnable(this));
        AppCrashHandler.init(this);
        closePErrorDialog();
    }

    /**
     * check and load database
     * 1. init local storage service;
     * 2. if exist load the last selected mesh network;
     * 3. if not exist, create a new mesh network
     * 4. setup the mesh network
     */
    public void initMeshInfo() {
        MeshInfoService.getInstance().init(ObjectBox.get());
        MeshInfo meshInfo = null;
        long id = SharedPreferenceHelper.getSelectedMeshId(this);
        if (id != -1) {
            // exists
            meshInfo = MeshInfoService.getInstance().getById(id);
        }
        if (meshInfo == null) {
            meshInfo = MeshInfo.createNewMesh(this, "Default Mesh");
        }
        MeshInfoService.getInstance().addMeshInfo(meshInfo);
        SharedPreferenceHelper.setSelectedMeshId(this, meshInfo.id);

        loadSortType();
    }

    private void closePErrorDialog() {
        if (Build.VERSION.SDK_INT <= 27) {
            return;
        }
        try {
            Class aClass = Class.forName("android.content.pm.PackageParser$Package");
            Constructor declaredConstructor = aClass.getDeclaredConstructor(String.class);
            declaredConstructor.setAccessible(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            Class cls = Class.forName("android.app.ActivityThread");
            Method declaredMethod = cls.getDeclaredMethod("currentActivityThread");
            declaredMethod.setAccessible(true);
            Object activityThread = declaredMethod.invoke(null);
            Field mHiddenApiWarningShown = cls.getDeclaredField("mHiddenApiWarningShown");
            mHiddenApiWarningShown.setAccessible(true);
            mHiddenApiWarningShown.setBoolean(activityThread, true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public Handler getOfflineCheckHandler() {
        return mOfflineCheckHandler;
    }

    public void setupMesh(MeshInfo mesh) {
        SharedPreferenceHelper.setSelectedMeshId(this, mesh.id);

        MeshLogger.d("setup mesh info: " + mesh.toString());
        if (mesh.extendGroups.size() == 0) {
            if (SharedPreferenceHelper.isLevelServiceEnable(this)) {
                mesh.addExtendGroups();
            }
        }
        this.meshInfo = mesh;
        MeshService.getInstance().setupMeshNetwork(mesh.convertToConfiguration());
        dispatchEvent(new MeshEvent(this, MeshEvent.EVENT_TYPE_MESH_RESET, "mesh reset"));
    }

    public MeshInfo getMeshInfo() {
        return meshInfo;
    }

    public static TelinkMeshApplication getInstance() {
        return mThis;
    }

    public NodeSortType getSortType() {
        return sortType;
    }

    private void loadSortType() {
        int type = SharedPreferenceHelper.getNodeSortType(this);
        sortType = NodeSortType.values()[type];
        if (sortType == null) {
            sortType = NodeSortType.NAME_ASC;
        }
    }

    public void resetSortType(NodeSortType sortType) {
        this.sortType = sortType;
        SharedPreferenceHelper.setNodeSortType(this, sortType);
    }

    @Override
    protected void onMeshEvent(MeshEvent autoConnectEvent) {
        String eventType = autoConnectEvent.getType();
        if (MeshEvent.EVENT_TYPE_DISCONNECTED.equals(eventType)) {
            AppSettings.ONLINE_STATUS_ENABLE = false;
            for (NodeInfo nodeInfo : meshInfo.nodes) {
                nodeInfo.setOnlineState(OnlineState.OFFLINE);
            }
        }
    }

    @Override
    protected void onStatusNotificationEvent(StatusNotificationEvent statusNotificationEvent) {
        NotificationMessage message = statusNotificationEvent.getNotificationMessage();

        StatusMessage statusMessage = message.getStatusMessage();
        if (statusMessage != null) {
            NodeInfo statusChangedNode = null;
            if (message.getStatusMessage() instanceof OnOffStatusMessage) {
                OnOffStatusMessage onOffStatusMessage = (OnOffStatusMessage) statusMessage;
                int onOff = onOffStatusMessage.isComplete() ? onOffStatusMessage.getTargetOnOff() : onOffStatusMessage.getPresentOnOff();
                for (NodeInfo nodeInfo : meshInfo.nodes) {
                    if (nodeInfo.meshAddress == message.getSrc()) {
                        if (nodeInfo.getOnlineState().st != onOff) {
                            statusChangedNode = nodeInfo;
                        }
                        nodeInfo.setOnlineState(OnlineState.getBySt(onOff));
                        break;
                    }
                }
            } else if (message.getStatusMessage() instanceof LevelStatusMessage) {
                LevelStatusMessage levelStatusMessage = (LevelStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                int level = levelStatusMessage.isComplete() ? levelStatusMessage.getTargetLevel() : levelStatusMessage.getPresentLevel();
                int tarVal = UnitConvert.level2lum((short) level);
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.compositionData == null) {
                        continue;
                    }
                    int lightnessEleAdr = onlineDevice.getTargetEleAdr(MeshSigModel.SIG_MD_LIGHTNESS_S.modelId);
                    if (lightnessEleAdr == srcAdr) {
                        if (onLumStatus(onlineDevice, tarVal)) {
                            statusChangedNode = onlineDevice;
                        }

                    } else {
                        int tempEleAdr = onlineDevice.getTargetEleAdr(MeshSigModel.SIG_MD_LIGHT_CTL_TEMP_S.modelId);
                        if (tempEleAdr == srcAdr) {
                            if (onlineDevice.temp != tarVal) {
                                statusChangedNode = onlineDevice;
                                onlineDevice.temp = tarVal;
                            }
                        }
                    }
                }
            } else if (message.getStatusMessage() instanceof CtlStatusMessage) {
                CtlStatusMessage ctlStatusMessage = (CtlStatusMessage) statusMessage;
                MeshLogger.d("ctl : " + ctlStatusMessage.toString());
                int srcAdr = message.getSrc();
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.meshAddress == srcAdr) {
                        int lum = ctlStatusMessage.isComplete() ? ctlStatusMessage.getTargetLightness() : ctlStatusMessage.getPresentLightness();
                        if (onLumStatus(onlineDevice, UnitConvert.lightness2lum(lum))) {
                            statusChangedNode = onlineDevice;
                        }

                        int temp = ctlStatusMessage.isComplete() ? ctlStatusMessage.getTargetTemperature() : ctlStatusMessage.getPresentTemperature();
                        if (onTempStatus(onlineDevice, UnitConvert.tempToTemp100(temp))) {
                            statusChangedNode = onlineDevice;
                        }
                        break;
                    }
                }
            } else if (message.getStatusMessage() instanceof LightnessStatusMessage) {
                LightnessStatusMessage lightnessStatusMessage = (LightnessStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.meshAddress == srcAdr) {
                        int lum = lightnessStatusMessage.isComplete() ? lightnessStatusMessage.getTargetLightness() : lightnessStatusMessage.getPresentLightness();
                        if (onLumStatus(onlineDevice, UnitConvert.lightness2lum(lum))) {
                            statusChangedNode = onlineDevice;
                        }
                        break;
                    }
                }
            } else if (message.getStatusMessage() instanceof CtlTemperatureStatusMessage) {
                CtlTemperatureStatusMessage ctlTemp = (CtlTemperatureStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.meshAddress == srcAdr) {
                        int temp = ctlTemp.isComplete() ? ctlTemp.getTargetTemperature() : ctlTemp.getPresentTemperature();
                        if (onTempStatus(onlineDevice, UnitConvert.lightness2lum(temp))) {
                            statusChangedNode = onlineDevice;
                        }
                        break;
                    }
                }
            } else if (message.getStatusMessage() instanceof SensorStatusMessage) {
                SensorStatusMessage sensorStatus = (SensorStatusMessage) statusMessage;
                int srcAdr = message.getSrc();
                for (NodeInfo onlineDevice : meshInfo.nodes) {
                    if (onlineDevice.meshAddress == srcAdr) {
                        if (onlineDevice.updateSensorState(sensorStatus.sensorData)) {
                            statusChangedNode = onlineDevice;
                        }
                        break;
                    }
                }
            }

            if (statusChangedNode != null) {
                onNodeInfoStatusChanged(statusChangedNode);
            }
        }
    }


    private boolean onLumStatus(NodeInfo nodeInfo, int lum) {
        boolean statusChanged = false;
        int tarOnOff = lum > 0 ? 1 : 0;
        if (nodeInfo.getOnlineState().st != tarOnOff) {
            statusChanged = true;
        }
        nodeInfo.setOnlineState(OnlineState.getBySt(tarOnOff));
        if (nodeInfo.lum != lum) {
            statusChanged = true;
            nodeInfo.lum = lum;
        }
        return statusChanged;
    }

    private boolean onTempStatus(NodeInfo nodeInfo, int temp) {
        boolean statusChanged = false;
        if (nodeInfo.temp != temp) {
            statusChanged = true;
            nodeInfo.temp = temp;
        }
        return statusChanged;
    }


    /**
     * node info status changed for UI refresh
     */
    private void onNodeInfoStatusChanged(NodeInfo nodeInfo) {
        dispatchEvent(new NodeStatusChangedEvent(this, NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, nodeInfo));
    }

    @Override
    protected void onOnlineStatusEvent(OnlineStatusEvent onlineStatusEvent) {
        List<OnlineStatusInfo> infoList = onlineStatusEvent.getOnlineStatusInfoList();
        if (infoList != null && meshInfo != null) {
            NodeInfo statusChangedNode = null;
            for (OnlineStatusInfo onlineStatusInfo : infoList) {
                if (onlineStatusInfo.status == null || onlineStatusInfo.status.length < 3) break;
                NodeInfo deviceInfo = meshInfo.getDeviceByMeshAddress(onlineStatusInfo.address);
                if (deviceInfo == null) continue;
                int onOff;
                if (onlineStatusInfo.sn == 0) {
                    onOff = -1;
                } else {
                    if (onlineStatusInfo.status[0] == 0) {
                        onOff = 0;
                    } else {
                        onOff = 1;
                    }
                }
                /*if (deviceInfo.getOnOff() != onOff){

                }*/
                if (deviceInfo.getOnlineState().st != onOff) {
                    statusChangedNode = deviceInfo;
                }
                deviceInfo.setOnlineState(OnlineState.getBySt(onOff));
                if (deviceInfo.lum != onlineStatusInfo.status[0]) {
                    statusChangedNode = deviceInfo;
                    deviceInfo.lum = onlineStatusInfo.status[0];
                }

                if (deviceInfo.temp != onlineStatusInfo.status[1]) {
                    statusChangedNode = deviceInfo;
                    deviceInfo.temp = onlineStatusInfo.status[1];
                }
            }
            if (statusChangedNode != null) {
                onNodeInfoStatusChanged(statusChangedNode);
            }
        }
    }

    /**
     * save sequence number and iv index when mesh info updated
     */
    protected void onNetworkInfoUpdate(NetworkInfoUpdateEvent networkInfoUpdateEvent) {
        MeshLogger.d(String.format("mesh info update from local sequenceNumber-%06X ivIndex-%08X to sequenceNumber-%06X ivIndex-%08X",
                meshInfo.sequenceNumber, meshInfo.ivIndex,
                networkInfoUpdateEvent.getSequenceNumber(), networkInfoUpdateEvent.getIvIndex()));
        this.meshInfo.ivIndex = networkInfoUpdateEvent.getIvIndex();
        this.meshInfo.sequenceNumber = networkInfoUpdateEvent.getSequenceNumber();
        MeshInfoService.getInstance().updateMeshInfo(this.meshInfo);
//        this.meshInfo.saveOrUpdate(this);
    }


}
