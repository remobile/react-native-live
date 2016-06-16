//
//  RCTLive.m
//  RCTLive
//
//  Created by fangyunjiang on 15/11/4.
//  Copyright (c) 2015年 remobile. All rights reserved.
//
#import "RCTViewManager.h"
#import "RCTBridge.h"
#import "RCTEventDispatcher.h"
#import "UIView+React.h"
#import "LiveVideoCoreSDK.h"
#import "RCTLive.h"

@interface RCTLiveManager : RCTViewManager

@end


@implementation RCTLiveManager

RCT_EXPORT_MODULE()

- (UIView *)view {
    RCTLive *view = [[RCTLive alloc]initWithBridge:self.bridge];

    return view;
}

@end
