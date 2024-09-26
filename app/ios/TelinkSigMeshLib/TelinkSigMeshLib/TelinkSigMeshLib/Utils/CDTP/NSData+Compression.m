/********************************************************************************************************
 * @file     NSData+Compression.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/10/9
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "NSData+Compression.h"
#import <zlib.h>

/*
 ExampleData: decompressionData <=> compressionData

 decompressionData:
 7B0D0A20202224736368656D61223A22687474703A2F2F6A736F6E2D736368656D612E6F72672F64726166742D30342F736368656D6123222C0D0A2020226964223A22687474703A2F2F7777772E626C7565746F6F74682E636F6D2F73706563696669636174696F6E732F61737369676E65642D6E756D626572732F6D6573682D70726F66696C652F6364622D736368656D612E6A736F6E23222C0D0A20202276657273696F6E223A22312E302E30222C0D0A2020226D65736855554944223A2262303337643563662D326137652D323239352D373633362D373162623566666237356165222C0D0A2020226D6573684E616D65223A224D65736820436F6E66696720436C69656E74204E6574776F726B222C0D0A2020226E6F646573223A5B0D0A202020207B0D0A2020202020202255554944223A2235393530313966392D313064642D626633642D306133362D393830376566353838633663222C0D0A202020202020227365637572697479223A22736563757265222C0D0A202020202020226578636C75646564223A66616C73652C0D0A20202020202022636F6E666967436F6D706C657465223A66616C73652C0D0A20202020202022656C656D656E7473223A5B0D0A20202020202020207B0D0A20202020202020202020226D6F64656C73223A5B0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2230303030222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D2C0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2230303031222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D0D0A202020202020202020205D2C0D0A2020202020202020202022696E646578223A302C0D0A20202020202020202020226C6F636174696F6E223A2230303030220D0A20202020202020207D0D0A2020202020205D2C0D0A202020202020226E65744B657973223A5B0D0A20202020202020207B0D0A2020202020202020202022696E646578223A302C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D2C0D0A20202020202020207B0D0A2020202020202020202022696E646578223A33342C0D0A202020202020202020202275706461746564223A747275650D0A20202020202020207D0D0A2020202020205D2C0D0A202020202020226170704B657973223A5B0D0A2020202020205D2C0D0A20202020202022756E696361737441646472657373223A2230303031222C0D0A20202020202022636964223A2230356631222C0D0A20202020202022706964223A2230303032222C0D0A20202020202022766964223A2230303031222C0D0A202020202020226665617475726573223A7B0D0A20202020202020202270726F7879223A322C0D0A2020202020202020226C6F77506F776572223A320D0A2020202020207D2C0D0A202020202020226372706C223A2237666666222C0D0A202020202020226465766963654B6579223A226664346563396362396465626339323865633765343430303030643461353264220D0A202020207D2C0D0A202020207B0D0A2020202020202255554944223A2230393037306630322D306130362D306430332D306230382D303130353061306330653034222C0D0A202020202020227365637572697479223A22736563757265222C0D0A202020202020226578636C75646564223A66616C73652C0D0A20202020202022636F6E666967436F6D706C657465223A66616C73652C0D0A20202020202022656C656D656E7473223A5B0D0A20202020202020207B0D0A20202020202020202020226D6F64656C73223A5B0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2230303030222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D2C0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2231303030222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D2C0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A223035663130303031222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020202020340D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020202020223161316237373166346135343738396162633434393037346637353832643337220D0A20202020202020202020202020205D0D0A2020202020202020202020207D0D0A202020202020202020205D2C0D0A2020202020202020202022696E646578223A302C0D0A20202020202020202020226C6F636174696F6E223A2230303030220D0A20202020202020207D2C0D0A20202020202020207B0D0A20202020202020202020226D6F64656C73223A5B0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A202231303031222C0D0A20202020202020202020202020202262696E64223A205B0D0A2020202020202020202020202020202032310D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A205B0D0A202020202020202020202020202020202263303230220D0A20202020202020202020202020205D2C0D0A2020202020202020202020202020227075626C697368223A207B0D0A202020202020202020202020202020202261646472657373223A202263303430222C0D0A2020202020202020202020202020202022696E646578223A2032312C0D0A202020202020202020202020202020202274746C223A203132372C0D0A202020202020202020202020202020202263726564656E7469616C73223A20302C0D0A202020202020202020202020202020202272657472616E736D6974223A207B0D0A20202020202020202020202020202020202022636F756E74223A20332C0D0A20202020202020202020202020202020202022696E74657276616C223A203335300D0A202020202020202020202020202020207D2C0D0A2020202020202020202020202020202022706572696F64223A207B0D0A202020202020202020202020202020202020226E756D6265724F665374657073223A2036332C0D0A202020202020202020202020202020202020227265736F6C7574696F6E223A203130300D0A202020202020202020202020202020207D0D0A20202020202020202020202020207D0D0A2020202020202020202020207D0D0A202020202020202020205D2C0D0A2020202020202020202022696E646578223A312C0D0A20202020202020202020226C6F636174696F6E223A2230303030220D0A20202020202020207D0D0A2020202020205D2C0D0A202020202020226E65744B657973223A5B0D0A20202020202020207B0D0A2020202020202020202022696E646578223A302C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D2C0D0A20202020202020207B0D0A2020202020202020202022696E646578223A33342C0D0A202020202020202020202275706461746564223A747275650D0A20202020202020207D0D0A2020202020205D2C0D0A202020202020226170704B657973223A5B0D0A20202020202020207B0D0A2020202020202020202022696E646578223A32312C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D2C0D0A20202020202020207B0D0A2020202020202020202022696E646578223A322C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D2C0D0A20202020202020207B0D0A2020202020202020202022696E646578223A342C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D0D0A2020202020205D2C0D0A20202020202022756E696361737441646472657373223A2230306161222C0D0A20202020202022636964223A2230356631222C0D0A20202020202022706964223A2230303031222C0D0A20202020202022766964223A2230303031222C0D0A202020202020226665617475726573223A7B0D0A20202020202020202270726F7879223A322C0D0A2020202020202020226C6F77506F776572223A322C0D0A202020202020202022667269656E64223A302C0D0A20202020202020202272656C6179223A300D0A2020202020207D2C0D0A202020202020226372706C223A2230303634222C0D0A202020202020226E6574776F726B5472616E736D6974223A7B0D0A202020202020202022636F756E74223A302C0D0A202020202020202022696E74657276616C223A3130300D0A2020202020207D2C0D0A2020202020202264656661756C7454544C223A3132372C0D0A2020202020202272656C617952657472616E736D6974223A7B0D0A202020202020202022636F756E74223A372C0D0A202020202020202022696E74657276616C223A3331300D0A2020202020207D2C0D0A202020202020226465766963654B6579223A223762353161303031353061636534616365323835313336636231373964373338220D0A202020207D2C0D0A202020207B0D0A2020202020202255554944223A2230383031303730642D306130612D303630622D306630352D306330653033303430393032222C0D0A202020202020227365637572697479223A22736563757265222C0D0A202020202020226578636C75646564223A66616C73652C0D0A20202020202022636F6E666967436F6D706C657465223A66616C73652C0D0A20202020202022656C656D656E7473223A5B0D0A20202020202020207B0D0A20202020202020202020226D6F64656C73223A5B0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2230303030222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D2C0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2231303030222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D2C0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A223035663130303031222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D0D0A202020202020202020205D2C0D0A2020202020202020202022696E646578223A302C0D0A20202020202020202020226C6F636174696F6E223A2230303030220D0A20202020202020207D2C0D0A20202020202020207B0D0A20202020202020202020226D6F64656C73223A5B0D0A2020202020202020202020207B0D0A2020202020202020202020202020226D6F64656C4964223A2231303031222C0D0A20202020202020202020202020202262696E64223A5B0D0A20202020202020202020202020205D2C0D0A202020202020202020202020202022737562736372696265223A5B0D0A20202020202020202020202020205D0D0A2020202020202020202020207D0D0A202020202020202020205D2C0D0A2020202020202020202022696E646578223A312C0D0A20202020202020202020226C6F636174696F6E223A2230303030220D0A20202020202020207D0D0A2020202020205D2C0D0A202020202020226E65744B657973223A5B0D0A20202020202020207B0D0A2020202020202020202022696E646578223A302C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D2C0D0A20202020202020207B0D0A2020202020202020202022696E646578223A33342C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D0D0A2020202020205D2C0D0A202020202020226170704B657973223A5B0D0A20202020202020207B0D0A2020202020202020202022696E646578223A32312C0D0A202020202020202020202275706461746564223A66616C73650D0A20202020202020207D0D0A2020202020205D2C0D0A20202020202022756E696361737441646472657373223A2230306163222C0D0A20202020202022636964223A2230356631222C0D0A20202020202022706964223A2230303031222C0D0A20202020202022766964223A2230303031222C0D0A202020202020226665617475726573223A7B0D0A20202020202020202270726F7879223A322C0D0A2020202020202020226C6F77506F776572223A320D0A2020202020207D2C0D0A202020202020226372706C223A2230303634222C0D0A202020202020227365637572654E6574776F726B426561636F6E223A747275652C0D0A202020202020226465766963654B6579223A226237316263663362323134633237636430393033353863663932326664666166220D0A202020207D0D0A20205D2C0D0A20202270726F766973696F6E657273223A5B0D0A202020207B0D0A2020202020202270726F766973696F6E65724E616D65223A22426C75655A206D6573682D636667636C69656E74222C0D0A2020202020202255554944223A2235393530313966392D313064642D626633642D306133362D393830376566353838633663222C0D0A20202020202022616C6C6F6361746564556E696361737452616E6765223A5B0D0A20202020202020207B0D0A20202020202020202020226C6F7741646472657373223A2230306161222C0D0A20202020202020202020226869676841646472657373223A2237666666220D0A20202020202020207D0D0A2020202020205D2C0D0A20202020202022616C6C6F636174656447726F757052616E6765223A5B0D0A20202020202020207B0D0A20202020202020202020226C6F7741646472657373223A2263303030222C0D0A20202020202020202020226869676841646472657373223A2266656666220D0A20202020202020207D0D0A2020202020205D2C0D0A20202020202022616C6C6F63617465645363656E6552616E6765223A5B0D0A20202020202020207B0D0A202020202020202020202266697273745363656E65223A202230303031222C0D0A20202020202020202020226C6173745363656E65223A202230303430220D0A20202020202020207D0D0A2020202020205D0D0A202020207D0D0A20205D2C0D0A2020226E65744B657973223A5B0D0A202020207B0D0A202020202020226E616D65223A225375626E65742030303030222C0D0A20202020202022696E646578223A302C0D0A202020202020226D696E5365637572697479223A22736563757265222C0D0A2020202020202274696D657374616D70223A22323032302D30352D32375431383A30363A30355A222C0D0A202020202020227068617365223A302C0D0A202020202020226B6579223A226663326461333434656634303764663736356364356461626332663466656238220D0A202020207D2C0D0A202020207B0D0A202020202020226E616D65223A225375626E65742030303232222C0D0A20202020202022696E646578223A33342C0D0A202020202020226D696E5365637572697479223A22736563757265222C0D0A2020202020202274696D657374616D70223A22323032302D30352D32375431383A30363A33345A222C0D0A202020202020227068617365223A312C0D0A202020202020226B6579223A223836326265323664623162306262646562636466613335346534393866613636222C0D0A202020202020226F6C644B6579223A226366396135633062336164313031636437663864646561373733306235393239220D0A202020207D0D0A20205D2C0D0A2020226170704B657973223A5B0D0A202020207B0D0A202020202020226E616D65223A224170704B65792030303031222C0D0A20202020202022626F756E644E65744B6579223A302C0D0A20202020202022696E646578223A312C0D0A202020202020226B6579223A226232333432303333356364323033316131373162343238303164323436333563220D0A202020207D2C0D0A202020207B0D0A202020202020226E616D65223A224170704B65792030303032222C0D0A20202020202022626F756E644E65744B6579223A302C0D0A20202020202022696E646578223A322C0D0A202020202020226B6579223A223138333739356537346535306466623339383661346262333638356565306263220D0A202020207D2C0D0A202020207B0D0A202020202020226E616D65223A224170704B65792030303034222C0D0A20202020202022626F756E644E65744B6579223A33342C0D0A20202020202022696E646578223A342C0D0A202020202020226B6579223A226462313963393038373763343134356162623433353064653863343166326434220D0A202020207D2C0D0A202020207B0D0A202020202020226E616D65223A224170704B65792030303135222C0D0A20202020202022626F756E644E65744B6579223A33342C0D0A20202020202022696E646578223A32312C0D0A202020202020226B6579223A226130303761376665303637656635666661333530396365646531666634303239222C0D0A202020202020226F6C644B6579223A226130303761376665303637656635666661333530396365646531666634303239220D0A202020207D0D0A20205D2C0D0A20202267726F757073223A5B0D0A202020207B0D0A202020202020226E616D65223A2247726F75705F39333831222C0D0A2020202020202261646472657373223A223161316237373166346135343738396162633434393037346637353832643337222C0D0A20202020202022706172656E7441646472657373223A2230303030220D0A202020207D2C0D0A202020207B0D0A202020202020226E616D65223A2247726F75705F63303230222C0D0A2020202020202261646472657373223A2263303230222C0D0A20202020202022706172656E7441646472657373223A2230303030220D0A202020207D2C0D0A202020207B0D0A202020202020226E616D65223A2247726F75705F63303430222C0D0A2020202020202261646472657373223A2263303430222C0D0A20202020202022706172656E7441646472657373223A2230303030220D0A202020207D0D0A20205D2C0D0A20202274696D657374616D70223A22323032302D30352D32375431383A35383A33305A222C0D0A2020227363656E6573223A5B0D0A20205D2C0D0A2020227061727469616C223A66616C73650D0A7D0D0A

 compressionData:
 789CED58DB8EDB36107D2FD07F10D43E466B92A244C96F490A1441DBB4C86E5E5214052F435B8D2C09BAEC6E10E4DF3B927C912D796D27DB3408B2C0C236399C391CCE1C0EE7FDF7DF398EFB63A597B092EEDC5DD675319FCDFEA9F2CCEB07AFF2723133A5B4B547F8AC1FFBC17DD2AD4BCC6EC9DDDDDD954A1BA8F3BC5E5EE97C35AB0AD0894DB4AC933CAB66B2AA924506C6CB9A9582B29AADA05A7A4599DB248599366A63B035BEB1708B82B81ACDD02B7245D6A3EDCAD7AF5FFC84C38AF8C204DA7A4C0AF0188B034F847EE809AA5460AD128184C1AA977205B8EA37FCEA3CCF339B2C9CE7690259EDBC84FA2E2FDFAE65B3DC40E5CEFF6C7F38CEFBFE03C7D75683382034B6B14789319EB2BEF18844AB714404D8208A74A87B4DDDAA0A745326F53B5CD97D85C11CDCEBB431809EB432AD6037A13B78CFF35591420DA369486185B87720F78076222BDC44BA2F3012DA09BE680F93E0DF0EDC5640259919E9719CBFC69255A32A5D260AA6C4F7073E3CB90817FD6CB8863FF755B9680FEEDD39D91F4DF33EC8370EDC4D6E75EDF4B819D4BFC0BB870E6EDA4A5318596F036560E3C9093D3E3FA2A82E1B7818AB2C8A03AC83C926C3E4AEEAA7C6945055A3537275C70F24B0C3C122599F271B0CDE26E343762DC81A7305150F76E52261DC6322B1C196D0FF777FE47750E2F066F4C3004659A4A85D586B07DA0DDC261A707338650D071D6B151B503A661168019CB74769B80C98591FE846E7880E484C04B184210990D02386F81E5124F20825010E6902847FA38347A403FA85E26A63FD32AA721CFE8970719A4AAA84A01683958B28964A738E21C9AD0822667CE19EDAE26332DE5136BA3CFE9CF6A08F3BD399F005A397B9734A87AB0923639F8DF5148D4A936A895A469B68B973C38BAD423E11AF3BEF22ECA9D9BA46E27228135393BA0483199FC8D6A70E991229A12E6556AD927A1A62472B4DD6CEFA13EB3B7C3594B7B285E107642C72981CDDA202CA2437474DF6F5DFEFF6BA86A2851E1EB18DCECBD3A68F32070361C2FAE1D0878BE39A9E1DD75FDD4D7EC4C641287E3C58F6487A8EEDF950CF79158A94675728F43FA95086C3B6C4B787D98F0F8CFB54E2FA6DBC8F4B1942C2613D91F54F979B6DAE0FD1AC137CCFC22EAB0769353063C0CA26AD6F6E7E458901FBF4D05E0D5865C29298B6E4D3694BBB2A4CA8804A7430964C1A38FEB328A07EA81515B1117E74B20A8BB0DE12A47D8A11E99190280F8BB2C0EBEA2F1F1938DEAB39BF5561BDF8B72AEC8B7A303E62F9F470F5F4F99DF095DCB567DC3B8F7FD99E79B9E9FFF9723BFBCEEA2977DD747B0652B771D05631D37783125469EB2B46B966421BE4723F88B48D19B3C64ABBB91BDA8FDE452DD2DBA46D1E4239D5C91B4CAF9B82CFD206DE385D5752DB85EEDA8203C41FD7FA936917E6605EF7A7F54A660B78202ED097C7AA956E7E992C963B81AEA771221437007E2EF3A6B8C8BC1EF1FB81790BE79BBFD690C129F33629ABBA936C5F4D630E7753B937CFA759631C0C87ECB18B83AC3FFCEB46A18CB37FD58E29C55D25D9F50385439D60FCD47255E024C357A487F5071337349A93704E8237C34C5CCA0AF674BFED9B519A19E9730E961361AC08036D02836F7A66B90575BC0C3ADC0963E39D0C48ED93B6E2F389ADD0C3AD442153C042A3A8224AB5CD35CC553FE0C0E3C8CA301CA8C853D3673AE6B40C3451BE3494506D848D8C0129844F5410B37822D30FD976E492A79D807340700AEB55F3B20B8CBD6318DD56EBDD28E673467C1FCF033FA8A4C8499C61D169180F71F4D4C1EC50B0F351B0431434F2451C80E0101063951F47A1E44AF96114001075010A7E0CC53048466FB0350C3CD358C724124273CA03A914F7111044F8D332C3CF8641830B60B0D1A1E08341486181842DFBDA36BC48ACC100B5983F182E13217672CD38C4162D793E10611DB9FE1DFBD130C0B60DA0D30DBA412EC912EF9DBD9E3A39E5CCDE7AD7B59AB27E30F12916F8310BFC5C0B03A71E6798209AFB64CD306ED5F2FDC6F79BCB5D966DE76B5B27A1DA7F01BD9E21B2
 */

/*
 Compression Algorithm: rfc1951
 Reference: DEFLATE Compressed Data Format Specification, RFC 1951, version 1.3  (https://specificationrefs.bluetooth.com/ext-ref/IETF/RFC1951.pdf)
 */
@implementation NSData (Compression)

/// This method will apply the gzip deflate algorithm and return the compressed data.
/// The compression level is a floating point value between 0.0 and 1.0,
///with 0.0 meaning no compression and 1.0 meaning maximum compression.
///A value of 0.1 will provide the fastest possible compression. If you supply a negative value,
///this will apply the default compression level, which is equivalent to a value of around 0.7.
- (NSData *)compressionDataWithCompressionLevel:(float)level {
    if (self.length == 0) {
        return self;
    }

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = (uint)self.length;
    stream.next_in = (Bytef *)(void *)self.bytes;
    stream.total_out = 0;
    stream.avail_out = 0;

    static const NSUInteger ChunkSize = 16384;

    NSMutableData *output = nil;
    int compression = (level < 0.0f)? Z_DEFAULT_COMPRESSION: (int)(roundf(level * 9));
    if (deflateInit(&stream, compression) == Z_OK) {
        output = [NSMutableData dataWithLength:ChunkSize];
        while (stream.avail_out == 0) {
            if (stream.total_out >= output.length) {
                output.length += ChunkSize;
            }
            stream.next_out = (uint8_t *)output.mutableBytes + stream.total_out;
            stream.avail_out = (uInt)(output.length - stream.total_out);
            deflate(&stream, Z_FINISH);
        }
        deflateEnd(&stream);
        output.length = stream.total_out;
    }
    return output;
}

/// This method is equivalent to calling `compressionDataWithCompressionLevel:` with the default compression level.
- (NSData *)compressionData {
    return [self compressionDataWithCompressionLevel:-1.0f];
}

/// This method will unzip data that was compressed using the deflate algorithm and return the result.
- (NSData *)decompressionData {
    if (self.length == 0) {
        return self;
    }

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.avail_in = (uint)self.length;
    stream.next_in = (Bytef *)self.bytes;
    stream.total_out = 0;
    stream.avail_out = 0;

    NSMutableData *output = nil;
    if (inflateInit2(&stream, 47) == Z_OK) {
        int status = Z_OK;
        output = [NSMutableData dataWithCapacity:self.length * 2];
        while (status == Z_OK) {
            if (stream.total_out >= output.length) {
                output.length += self.length / 2;
            }
            stream.next_out = (uint8_t *)output.mutableBytes + stream.total_out;
            stream.avail_out = (uInt)(output.length - stream.total_out);
            status = inflate (&stream, Z_SYNC_FLUSH);
        }
        if (inflateEnd(&stream) == Z_OK) {
            if (status == Z_STREAM_END) {
                output.length = stream.total_out;
            }
        }
    }

    return output;
}

@end
