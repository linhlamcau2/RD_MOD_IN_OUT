/********************************************************************************************************
 * @file TipsActivity.java
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
package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;

/**
 * tips
 */
public class TipsActivity extends BaseActivity {
    public static final String INTENT_KEY_TIP_RES_ID = "TipsActivity.TIP_RES_ID";
    public static final String INTENT_KEY_TIP_STRING = "TipsActivity.TIP_STRING";
    public static final String INTENT_KEY_TIP_SUB_TITLE = "TipsActivity.INTENT_KEY_TIP_SUB_TITLE";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_share_tip);
        TextView tv_tip = findViewById(R.id.tv_tip);
        Intent intent = getIntent();
        if (intent.hasExtra(INTENT_KEY_TIP_STRING)) {
            tv_tip.setText(intent.getStringExtra(INTENT_KEY_TIP_STRING));
        } else {
            int tipResId = getIntent().getIntExtra(INTENT_KEY_TIP_RES_ID, R.string.share_tip);
            tv_tip.setText(tipResId);
        }
        
        String subTitle = intent.getStringExtra(INTENT_KEY_TIP_SUB_TITLE);
        setTitle("Tip", subTitle);

        enableBackNav(true);
    }

    public void onKnownClick(View view) {
        finish();
    }

}
