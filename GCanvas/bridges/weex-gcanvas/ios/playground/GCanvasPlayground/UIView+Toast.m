//
//  UIView+Toast.m
//  GCanvasPlayground
//
//  Created by jiangweixing on 2018/7/9.
//  Copyright © 2018年 taobao. All rights reserved.
//

#import "UIView+Toast.h"

@implementation UIView(Toast)

- (void)toastMessage:(NSString*)message duration:(NSTimeInterval)duration{
    UIView *toast = [self toastViewForMessage:message];
    [self showToast:toast];
}

- (UIView*)toastViewForMessage:(NSString*)message{
    if (message ==nil) {
        return nil;
    }
    
    UILabel *messageLabel = [[UILabel alloc] init];
    messageLabel.numberOfLines = 0;
    messageLabel.font = [UIFont systemFontOfSize:16.0];
    messageLabel.textAlignment = NSTextAlignmentCenter;
    messageLabel.lineBreakMode = NSLineBreakByTruncatingTail;
    messageLabel.textColor = [UIColor whiteColor];
    messageLabel.backgroundColor = [UIColor clearColor];
    messageLabel.alpha = 1.0;
    messageLabel.text = message;
    
    CGSize maxSizeMessage = CGSizeMake((self.bounds.size.width * 0.8), self.bounds.size.height * 0.8);
    CGSize expectedSizeMessage = [messageLabel sizeThatFits:maxSizeMessage];
    // UILabel can return a size larger than the max size when the number of lines is 1
    expectedSizeMessage = CGSizeMake(MIN(maxSizeMessage.width, expectedSizeMessage.width), MIN(maxSizeMessage.height, expectedSizeMessage.height));
    messageLabel.frame = CGRectMake(0.0, 0.0, expectedSizeMessage.width, expectedSizeMessage.height);
    
    CGRect messageRect = CGRectZero;
    messageRect.origin.x = 10.0;
    messageRect.origin.y = 10.0;
    messageRect.size.width = messageLabel.bounds.size.width;
    messageRect.size.height = messageLabel.bounds.size.height;
    messageLabel.frame = messageRect;

    UIView *contentView = [[UIView alloc] init];
    contentView.autoresizingMask = (UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin);
    contentView.layer.cornerRadius = 2.0;
    contentView.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.75];
    CGFloat contentWidth = messageRect.origin.x * 2 + messageRect.size.width;
    CGFloat contentHeight = messageRect.origin.y * 2 + messageRect.size.height;
    contentView.frame = CGRectMake(0.0, 0.0, contentWidth, contentHeight);
    [contentView addSubview:messageLabel];
    
    UIView *maskView = [[UIView alloc] initWithFrame:self.bounds];
    maskView.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.1];
    contentView.center = CGPointMake(self.bounds.size.width*0.5, self.bounds.size.height *0.5);
    [maskView addSubview:contentView];
    
    return maskView;
}

- (void)showToast:(UIView *)toast{
    if (toast == nil) return;
    
    typeof(self)weakSelf = self;
    
    UITapGestureRecognizer *recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onHandleToastTapped:)];
    [toast addGestureRecognizer:recognizer];
    toast.userInteractionEnabled = YES;
    toast.exclusiveTouch = YES;
    toast.alpha = 0;
    
    [UIView animateWithDuration:0.3
                          delay:0
                        options:(UIViewAnimationOptionCurveEaseOut|UIViewAnimationOptionAllowUserInteraction)
                     animations:^{
                         toast.alpha = 1.0;
    } completion:^(BOOL finished) {
        [weakSelf addSubview:toast];
    }];
}

- (void)hideToast:(UIView*)toast{
    [UIView animateWithDuration:0.3
                          delay:0
                        options:(UIViewAnimationOptionCurveEaseOut|UIViewAnimationOptionAllowUserInteraction)
                     animations:^{
                         toast.alpha = 0;
                     } completion:^(BOOL finished) {
                         [toast removeFromSuperview];
                     }];
}

- (void)onHandleToastTapped:(UIGestureRecognizer*)recognizer{
    UIView *toast = recognizer.view;
    [self hideToast:toast];
}


@end
