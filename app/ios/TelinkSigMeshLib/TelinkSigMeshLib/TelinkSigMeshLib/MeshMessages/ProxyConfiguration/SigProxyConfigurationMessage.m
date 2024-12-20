/********************************************************************************************************
 * @file     SigProxyConfigurationMessage.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/10/30
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

#import "SigProxyConfigurationMessage.h"

///more detail:  Mesh_v1.0.pdf section 6.4~6.7
/// @note   6.6 Proxy Server behavior
/// When a Proxy Client connects to a Proxy Server, an instance of a new bearer is connected
/// to the network layer via a network interface (see Section 3.4.5).
/// Upon connection, the Proxy Server shall initialize the proxy filter as a white list filter
/// and the white list shall be empty.
/// If the proxy filter is a white list filter, upon receiving a valid Mesh message from the Proxy Client,
/// the Proxy Server shall add the unicast address contained in the SRC field of the message to the white list.
/// If the proxy filter is a black list filter, upon receiving a valid Mesh message from the Proxy Client,
/// the Proxy Server shall remove the unicast address contained in the SRC field of the message from the black list.
/// Upon connection, the Proxy Server shall send a Secure Network Beacon for each known subnet to the Proxy Client.
/// Upon successfully processing a Secure Network Beacon with new values for the IV Index field or the Flags field,
/// the Proxy Server shall send this Secure Network Beacon to the Proxy Client.
/// When the Proxy Server is added to a new subnet,
/// it shall send a Secure Network Beacon for that subnet to the Proxy Client.
/// Upon receiving a Secure Network Beacon from the Proxy Client,
/// the Proxy Server shall process it as defined in Section 3.9.3.1.
/// When sending a proxy configuration message,
/// a Proxy Server shall set the SRC field to the unicast address of its primary element
/// and the SEQ field shall use the sequence number of its primary element.
/// If a Proxy Server receives a Set Filter Type message,
/// it shall set the proxy filter type as requested in the message parameter,
/// and it shall clear the proxy filter list.
/// The Proxy Server shall then respond with a Filter Status message.
/// If the Proxy Server receives an Add Addresses to Filter message,
/// then it shall add these addresses to the proxy filter list and shall respond with a Filter Status message.
/// If one or more addresses contained in the message are already in the list,
/// the Proxy Server shall not add these addresses.
/// If the Proxy Server runs out of space in the proxy filter list,
/// the Proxy Server shall not add these addresses.
/// If the AddressArray field contains the unassigned address,
/// the Proxy Server shall ignore that address.
/// If the Proxy Server receives a Remove Addresses from Filter message,
/// it shall remove these addresses from the proxy filter list and shall respond with a Filter Status message.
/// If one or more addresses contained in the message were not in the list,
/// the Proxy Server shall ignore these addresses.
/// If the AddressArray field contains the unassigned address,
/// the Proxy Server shall ignore that address.
/// Upon receiving a message with an unexpected value of the SAR field,
/// the Proxy Server shall disconnect.
/// Upon receiving a message with the Message Type field set to a value that is Reserved for Future Use,
/// the Proxy Server shall ignore this message.
/// The timeout for the SAR transfer is 20 seconds.
/// When the timeout expires, the Proxy Server shall disconnect.
/// @note   6.7 Proxy Client behavior
/// The Proxy Client may send proxy configuration messages to configure the proxy filter.
/// When sending a proxy configuration message,
/// the Proxy Client shall set the SRC field to the unicast address of its primary element
/// and the SEQ field shall use the sequence number of its primary element.
/// The Proxy Client can determine the state of the proxy filter list when it receives a Filter Status message.
/// Upon receiving a message with an unexpected value of the SAR field,
/// the Proxy Client shall disconnect.
/// Upon receiving a message with the Message
/// Type field set to a value that is Reserved for Future Use,
/// the Proxy Client shall ignore this message.
/// The timeout for the SAR transfer is 20 seconds.
/// When the timeout expires, the Proxy Client shall disconnect.
@implementation SigProxyConfigurationMessage

@end


/// A type of a Proxy Configuration message which opcode is known
/// during compilation time.
@implementation SigStaticProxyConfigurationMessage

@end


/// The base class for acknowledged messages.
///
/// An acknowledged message is transmitted and acknowledged by each
/// receiving element by responding to that message. The response is
/// typically a status message. If a response is not received within
/// an arbitrary time period, the message will be retransmitted
/// automatically until the timeout occurs.
@implementation SigAcknowledgedProxyConfigurationMessage

@end


@implementation SigStaticAcknowledgedProxyConfigurationMessage
- (UInt8)responseOpCode {
    return self.responseType.opCode;
}
@end


/// 6.5.1 Set Filter Type
/// - seeAlso: Mesh_v1.0.pdf (page.263)
/// @note The Set Filter Type message can be sent by a Proxy Client
/// to change the proxy filter type and clear the proxy filter list.
@implementation SigSetFilterType

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_setFilterType;
        self.responseType = [[SigFilterStatus alloc] init];
    }
    return self;
}

- (instancetype)initWithType:(SigProxyFilerType)type {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_setFilterType;
        self.responseType = [[SigFilterStatus alloc] init];
        _filterType = type;
    }
    return self;
}

/**
 * @brief   Initialize SigSetFilterType object.
 * @param   parameters    the hex data of SigSetFilterType.
 * @return  return `nil` when initialize SigSetFilterType object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _filterType = tem;
        self.opCode = SigProxyFilerOpcode_setFilterType;
        self.responseType = [[SigFilterStatus alloc] init];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _filterType;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    return temData;
}

@end


/// 6.5.2 Add Addresses to Filter
/// - seeAlso: Mesh_v1.0.pdf (page.264)
/// @note   The Add Addresses to Filter message is sent by a Proxy Client
/// to add destination addresses to the proxy filter list.
/// The AddressArray field contains a sequence of addresses to be added to the proxy filter list.
/// It may contain any combination of unicast addresses, virtual addresses, and group addresses.
/// @note   Each address in the AddressArray is a 16-bit value
/// and therefore the 16-bit virtual address and not the Label UUID is used.
@implementation SigAddAddressesToFilter

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_addAddressesToFilter;
        self.responseType = [[SigFilterStatus alloc] init];
        _addresses = [NSMutableArray array];
    }
    return self;
}

/// Creates the Add Addresses To Filter message.
///
/// - parameter addresses: The array of addresses to be added
///                        to the current filter.
- (instancetype)initWithAddresses:(NSArray <NSNumber *>*)addresses {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_addAddressesToFilter;
        self.responseType = [[SigFilterStatus alloc] init];
        _addresses = [NSMutableArray arrayWithArray:addresses];
    }
    return self;
}

/**
 * @brief   Initialize SigAddAddressesToFilter object.
 * @param   parameters    the hex data of SigAddAddressesToFilter.
 * @return  return `nil` when initialize SigAddAddressesToFilter object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length % 2 != 0) {
            return nil;
        }
        NSMutableArray *tem = [NSMutableArray array];
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        for (int i=0; 2*(i+1)>parameters.length; i++) {
            memcpy(&tem16, dataByte+2*i, 2);
            UInt16 bigTem = CFSwapInt16HostToBig(tem16);
//            UInt16 bigTem = tem16;
            [tem addObject:@(bigTem)];
        }
        _addresses = tem;
        self.opCode = SigProxyFilerOpcode_addAddressesToFilter;
        self.responseType = [[SigFilterStatus alloc] init];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSArray *addresses = [NSArray arrayWithArray:_addresses];
    for (NSNumber *bigNum in addresses) {
        UInt16 bigTem = CFSwapInt16HostToBig((UInt16)bigNum.intValue);
//        UInt16 bigTem = (UInt16)bigNum.intValue;
        NSData *temData = [NSData dataWithBytes:&bigTem length:2];
        [mData appendData:temData];
    }
    return mData;
}

@end


/// 6.5.3 Remove Addresses from Filter
/// - seeAlso: Mesh_v1.0.pdf (page.264)
/// @note   The Remove Addresses from Filter message is sent by a Proxy Client
/// to remove destination addresses from the proxy filter list.
/// The AddressArray field contains a sequence of addresses to be removed from the proxy filter list.
/// It may contain any combination of unicast addresses, virtual addresses, or group addresses.
/// @note   Each address in the AddressArray is a 16-bit value
/// and therefore the 16-bit virtual address and not the Label UUID is used.
@implementation SigRemoveAddressesFromFilter

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_removeAddressesFromFilter;
        self.responseType = [[SigFilterStatus alloc] init];
        _addresses = [NSMutableArray array];
    }
    return self;
}

/// Creates the Add Addresses To Filter message.
///
/// - parameter addresses: The array of addresses to be added
///                        to the current filter.
- (instancetype)initWithAddresses:(NSArray <NSNumber *>*)addresses {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_removeAddressesFromFilter;
        self.responseType = [[SigFilterStatus alloc] init];
        _addresses = [NSMutableArray arrayWithArray:addresses];
    }
    return self;
}

/**
 * @brief   Initialize SigRemoveAddressesFromFilter object.
 * @param   parameters    the hex data of SigRemoveAddressesFromFilter.
 * @return  return `nil` when initialize SigRemoveAddressesFromFilter object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length % 2 != 0) {
            return nil;
        }
        NSMutableArray *tem = [NSMutableArray array];
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        for (int i=0; 2*(i+1)>parameters.length; i++) {
            memcpy(&tem16, dataByte+2*i, 2);
            UInt16 bigTem = CFSwapInt16HostToBig(tem16);
//            UInt16 bigTem = tem16;
            [tem addObject:@(bigTem)];
        }
        _addresses = tem;
        self.opCode = SigProxyFilerOpcode_removeAddressesFromFilter;
        self.responseType = [[SigFilterStatus alloc] init];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSArray *addresses = [NSArray arrayWithArray:_addresses];
    for (NSNumber *bigNum in addresses) {
        UInt16 bigTem = CFSwapInt16HostToBig((UInt16)bigNum.intValue);
//        UInt16 bigTem = (UInt16)bigNum.intValue;
        NSData *temData = [NSData dataWithBytes:&bigTem length:2];
        [mData appendData:temData];
    }
    return mData;
}

@end


/// 6.5.4 Filter Status
/// - seeAlso: Mesh_v1.0.pdf (page.264)
/// @note   The Filter Status message is sent by a Proxy Server
/// to report the status of the proxy filter.
@implementation SigFilterStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_filterStatus;
    }
    return self;
}

/// Creates a new Filter Status message.
///
/// - parameter type: The current filter type.
/// - parameter listSize: Number of addresses in the proxy
///                       filter list.
- (instancetype)initWithType:(SigProxyFilerType)type listSize:(UInt16)listSize {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_filterStatus;
        _filterType = type;
        _listSize = listSize;
    }
    return self;
}

/**
 * @brief   Initialize SigFilterStatus object.
 * @param   parameters    the hex data of SigFilterStatus.
 * @return  return `nil` when initialize SigFilterStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        UInt8 tem = 0;
        UInt16 tem2 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        memcpy(&tem2, dataByte+1, 2);
        _filterType = tem;
        _listSize = CFSwapInt16HostToBig(tem2);
//        _listSize = tem2;
        self.opCode = SigProxyFilerOpcode_filterStatus;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _filterType;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    UInt16 temlistSize = CFSwapInt16HostToBig(_listSize);;
//    UInt16 temlistSize = _listSize;;
    NSData *temData2 = [NSData dataWithBytes:&temlistSize length:2];
    NSMutableData *mData = [NSMutableData dataWithData:temData];
    [mData appendData:temData2];
    return mData;
}

@end
